#include <ProjectSettings.hpp>
#include <ResourceLoader.hpp>
#include <Viewport.hpp>
#include <Camera.hpp>
#include <Transform.hpp>
#include <GDScript.hpp>
#include <VisualServer.hpp>

#include "RendererGodot/EffekseerGodot.Renderer.h"
#include "LoaderGodot/EffekseerGodot.TextureLoader.h"
#include "LoaderGodot/EffekseerGodot.ModelLoader.h"
#include "LoaderGodot/EffekseerGodot.MaterialLoader.h"
#include "LoaderGodot/EffekseerGodot.CurveLoader.h"
#include "LoaderGodot/EffekseerGodot.SoundLoader.h"
#include "SoundGodot/EffekseerGodot.SoundPlayer.h"
#include "Utils/EffekseerGodot.Utils.h"
#include "EffekseerSystem.h"
#include "EffekseerEffect.h"

namespace godot {

EffekseerSystem* EffekseerSystem::s_instance = nullptr;

void EffekseerSystem::_register_methods()
{
	register_method("_init", &EffekseerSystem::_init);
	register_method("_enter_tree", &EffekseerSystem::_enter_tree);
	register_method("_exit_tree", &EffekseerSystem::_exit_tree);
	register_method("_process", &EffekseerSystem::_process);
	register_method("_update_draw", &EffekseerSystem::_update_draw);
	register_method("stop_all_effects", &EffekseerSystem::stop_all_effects);
	register_method("set_paused_to_all_effects", &EffekseerSystem::set_paused_to_all_effects);
	register_method("get_total_instance_count", &EffekseerSystem::get_total_instance_count);
}

EffekseerSystem::EffekseerSystem()
{
	s_instance = this;

	int32_t instanceMaxCount = 2000;
	int32_t squareMaxCount = 8000;
	int32_t drawMaxCount = 128;
	Ref<Script> soundScript;

	auto settings = ProjectSettings::get_singleton();

	if (settings->has_setting("effekseer/instance_max_count")) {
		instanceMaxCount = (int32_t)settings->get_setting("effekseer/instance_max_count");
	}
	if (settings->has_setting("effekseer/square_max_count")) {
		squareMaxCount = (int32_t)settings->get_setting("effekseer/square_max_count");
	}
	if (settings->has_setting("effekseer/draw_max_count")) {
		drawMaxCount = (int32_t)settings->get_setting("effekseer/draw_max_count");
	}
	if (settings->has_setting("effekseer/sound_script")) {
		soundScript = Ref<Script>(settings->get_setting("effekseer/sound_script"));
	} else {
		soundScript = ResourceLoader::get_singleton()->load("res://addons/effekseer/src/EffekseerSound.gd", "");
	}
	Ref<Reference> sound = EffekseerGodot::ScriptNew(soundScript);
	
	m_manager = Effekseer::Manager::Create(instanceMaxCount);
#ifndef __EMSCRIPTEN__
	m_manager->LaunchWorkerThreads(2);
#endif
	m_manager->SetTextureLoader(Effekseer::MakeRefPtr<EffekseerGodot::TextureLoader>());
	m_manager->SetModelLoader(Effekseer::MakeRefPtr<EffekseerGodot::ModelLoader>());
	m_manager->SetMaterialLoader(Effekseer::MakeRefPtr<EffekseerGodot::MaterialLoader>());
	m_manager->SetCurveLoader(Effekseer::MakeRefPtr<EffekseerGodot::CurveLoader>());
	m_manager->SetSoundLoader(Effekseer::MakeRefPtr<EffekseerGodot::SoundLoader>(sound));

	m_renderer = EffekseerGodot::Renderer::Create(squareMaxCount, drawMaxCount);
	m_renderer->SetProjectionMatrix(Effekseer::Matrix44().Indentity());

	m_manager->SetSpriteRenderer(m_renderer->CreateSpriteRenderer());
	m_manager->SetRibbonRenderer(m_renderer->CreateRibbonRenderer());
	m_manager->SetTrackRenderer(m_renderer->CreateTrackRenderer());
	m_manager->SetRingRenderer(m_renderer->CreateRingRenderer());
	m_manager->SetModelRenderer(m_renderer->CreateModelRenderer());
	m_manager->SetSoundPlayer(Effekseer::MakeRefPtr<EffekseerGodot::SoundPlayer>(sound));
}

EffekseerSystem::~EffekseerSystem()
{
	s_instance = nullptr;
}

void EffekseerSystem::_init()
{
}

void EffekseerSystem::_enter_tree()
{
	set_process_priority(100);
	VisualServer::get_singleton()->connect("frame_pre_draw", this, "_update_draw");
}

void EffekseerSystem::_exit_tree()
{
	VisualServer::get_singleton()->disconnect("frame_pre_draw", this, "_update_draw");
}

void EffekseerSystem::_process(float delta)
{
	// Stabilize in a variable frame environment
	float deltaFrames = delta * 60.0f;
	int iterations = (int)roundf(deltaFrames);
	float advance = deltaFrames / iterations;
	for (int i = 0; i < iterations; i++) {
		m_manager->Update(advance);
	}
	m_renderer->SetTime(m_renderer->GetTime() + delta);
}

void EffekseerSystem::_update_draw()
{
	m_renderer->ResetState();
}

void EffekseerSystem::draw3D(Effekseer::Handle handle, const Transform& camera_transform)
{
	Effekseer:: Matrix44 matrix = EffekseerGodot::ToEfkMatrix44(camera_transform.inverse());
	m_renderer->SetCameraMatrix(matrix);

	m_renderer->BeginRendering();
	m_manager->DrawHandle(handle);
	m_renderer->EndRendering();
}

void EffekseerSystem::draw2D(Effekseer::Handle handle, const Transform2D& camera_transform)
{
	Effekseer:: Matrix44 matrix = EffekseerGodot::ToEfkMatrix44(camera_transform.inverse());
	matrix.Values[3][2] = -1.0f; // Z offset
	m_renderer->SetCameraMatrix(matrix);

	m_renderer->BeginRendering();
	m_manager->DrawHandle(handle);
	m_renderer->EndRendering();
}

void EffekseerSystem::stop_all_effects()
{
	m_manager->StopAllEffects();
}

void EffekseerSystem::set_paused_to_all_effects(bool paused)
{
	m_manager->SetPausedToAllEffects(paused);
}

int EffekseerSystem::get_total_instance_count() const
{
	return m_manager->GetTotalInstanceCount();
}

}
