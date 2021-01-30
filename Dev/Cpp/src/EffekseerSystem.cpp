#include <ProjectSettings.hpp>
#include <Viewport.hpp>
#include <Camera.hpp>
#include <Transform.hpp>
#include "GDLibrary.h"
#include "EffekseerEffect.h"
#include "Effekseer.h"
#include "RendererGodot/EffekseerGodot.Utils.h"
#include "RendererGodot/EffekseerGodot.Renderer.h"
#include "LoaderGodot/EffekseerGodot.TextureLoader.h"
#include "LoaderGodot/EffekseerGodot.ModelLoader.h"
#include "LoaderGodot/EffekseerGodot.MaterialLoader.h"
#include "EffekseerSystem.h"

namespace godot {

EffekseerSystem* EffekseerSystem::s_instance = nullptr;

void EffekseerSystem::_register_methods()
{
	register_method("_init", &EffekseerSystem::_init);
	register_method("_process", &EffekseerSystem::_process);
	register_method("play", &EffekseerSystem::play);
	register_method("stop", &EffekseerSystem::stop);
	register_method("exists", &EffekseerSystem::exists);
	register_method("get_total_instance_count", &EffekseerSystem::get_total_instance_count);
}

EffekseerSystem::EffekseerSystem()
{
	s_instance = this;

	int32_t instanceMaxCount = 2000;
	int32_t squareMaxCount = 8000;
	int32_t drawMaxCount = 128;

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

	m_manager = Effekseer::Manager::Create(instanceMaxCount);
	m_manager->LaunchWorkerThreads(2);
	m_manager->SetTextureLoader(Effekseer::MakeRefPtr<EffekseerGodot::TextureLoader>());
	m_manager->SetModelLoader(Effekseer::MakeRefPtr<EffekseerGodot::ModelLoader>());
	m_manager->SetMaterialLoader(Effekseer::MakeRefPtr<EffekseerGodot::MaterialLoader>());

	m_renderer = EffekseerGodot::Renderer::Create(squareMaxCount, drawMaxCount);
	m_renderer->SetProjectionMatrix(Effekseer::Matrix44().Indentity());

	m_manager->SetSpriteRenderer(m_renderer->CreateSpriteRenderer());
	m_manager->SetRibbonRenderer(m_renderer->CreateRibbonRenderer());
	m_manager->SetTrackRenderer(m_renderer->CreateTrackRenderer());
	m_manager->SetRingRenderer(m_renderer->CreateRingRenderer());
	m_manager->SetModelRenderer(m_renderer->CreateModelRenderer());
}

EffekseerSystem::~EffekseerSystem()
{
	s_instance = nullptr;
}

void EffekseerSystem::_init()
{
	set_process_priority(100);
}

void EffekseerSystem::_process(float delta)
{
	m_manager->Update(delta * 60.0f);

	if (m_manager->GetTotalInstanceCount() == 0) {
		m_renderer->ResetState();
	} else {
		m_shouldResetState = true;
	}

	m_renderer->SetTime(m_renderer->GetTime() + delta);
}

void EffekseerSystem::draw(Camera* camera, Effekseer::Handle handle)
{
	if (m_shouldResetState) {
		m_renderer->ResetState();
	}

	auto camera_transform = camera->get_camera_transform().inverse();

	m_renderer->SetWorld(camera->get_world().ptr());
	m_renderer->SetCameraMatrix(EffekseerGodot::Convert::Matrix44(camera_transform));

	m_renderer->BeginRendering();
	m_manager->DrawHandle(handle);
	m_renderer->EndRendering();
}

Effekseer::Handle EffekseerSystem::play(godot::Ref<EffekseerEffect> effect, const Transform& transform)
{
	if (effect == nullptr) {
		Godot::print_error("Effect is null", __FUNCTION__, __FILE__, __LINE__);
		return -1;
	}

	Effekseer::Handle handle = m_manager->Play(effect->get_native(), EffekseerGodot::Convert::Vector3(transform.origin));
	if (handle >= 0) {
		auto rotation = transform.basis.get_euler_xyz();
		auto scale = transform.basis.get_scale();
		m_manager->SetRotation(handle, rotation.x, rotation.y, rotation.z);
		m_manager->SetScale(handle, scale.x, scale.y, scale.z);
	}
	return handle;
}

void EffekseerSystem::stop(Effekseer::Handle handle)
{
	m_manager->StopEffect(handle);
}

void EffekseerSystem::stop_root(Effekseer::Handle handle)
{
	m_manager->StopRoot(handle);
}

void EffekseerSystem::set_paused(Effekseer::Handle handle, bool paused)
{
	m_manager->SetPaused(handle, paused);
}

bool EffekseerSystem::exists(Effekseer::Handle handle)
{
	return m_manager->Exists(handle);
}

int EffekseerSystem::get_total_instance_count() const
{
	return m_manager->GetTotalInstanceCount();
}

}
