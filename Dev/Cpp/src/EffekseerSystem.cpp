#include <assert.h>
#include <algorithm>
#include <ProjectSettings.hpp>
#include <ResourceLoader.hpp>
#include <Viewport.hpp>
#include <Camera.hpp>
#include <Transform.hpp>
#include <GDScript.hpp>
#include <VisualServer.hpp>
#include <World.hpp>
#include <World2D.hpp>
#include <Camera.hpp>
#include <Camera2D.hpp>

#include "RendererGodot/EffekseerGodot.Renderer.h"
#include "RendererGodot/EffekseerGodot.ModelRenderer.h"
#include "LoaderGodot/EffekseerGodot.TextureLoader.h"
#include "LoaderGodot/EffekseerGodot.ModelLoader.h"
#include "LoaderGodot/EffekseerGodot.MaterialLoader.h"
#include "LoaderGodot/EffekseerGodot.CurveLoader.h"
#include "LoaderGodot/EffekseerGodot.SoundLoader.h"
#include "LoaderGodot/EffekseerGodot.ProceduralModelGenerator.h"
#include "SoundGodot/EffekseerGodot.SoundPlayer.h"
#include "Utils/EffekseerGodot.Utils.h"
#include "EffekseerSystem.h"
#include "EffekseerEffect.h"
#include "EffekseerEmitter.h"
#include "EffekseerEmitter2D.h"

namespace godot {

EffekseerSystem* EffekseerSystem::s_instance = nullptr;

void EffekseerSystem::_register_methods()
{
	register_method("_init", &EffekseerSystem::_init);
	register_method("_enter_tree", &EffekseerSystem::_enter_tree);
	register_method("_exit_tree", &EffekseerSystem::_exit_tree);
	register_method("_ready", &EffekseerSystem::_ready);
	register_method("_process", &EffekseerSystem::_process);
	register_method("_update_draw", &EffekseerSystem::_update_draw);
	register_method("stop_all_effects", &EffekseerSystem::stop_all_effects);
	register_method("set_paused_to_all_effects", &EffekseerSystem::set_paused_to_all_effects);
	register_method("get_total_instance_count", &EffekseerSystem::get_total_instance_count);
	register_method("clear_shader_load_count", &EffekseerSystem::clear_shader_load_count);
	register_method("get_shader_load_count", &EffekseerSystem::get_shader_load_count);
	register_method("get_shader_load_progress", &EffekseerSystem::get_shader_load_progress);
	register_method("complete_all_shader_loads", &EffekseerSystem::complete_all_shader_loads);
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

	auto effekseerSettings = Effekseer::Setting::Create();

	effekseerSettings->SetTextureLoader(Effekseer::MakeRefPtr<EffekseerGodot::TextureLoader>());
	effekseerSettings->SetModelLoader(Effekseer::MakeRefPtr<EffekseerGodot::ModelLoader>());
	effekseerSettings->SetMaterialLoader(Effekseer::MakeRefPtr<EffekseerGodot::MaterialLoader>());
	effekseerSettings->SetCurveLoader(Effekseer::MakeRefPtr<EffekseerGodot::CurveLoader>());
	effekseerSettings->SetSoundLoader(Effekseer::MakeRefPtr<EffekseerGodot::SoundLoader>(sound));
	effekseerSettings->SetProceduralMeshGenerator(Effekseer::MakeRefPtr<EffekseerGodot::ProceduralModelGenerator>());

	m_manager->SetSetting(effekseerSettings);

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
	set_pause_mode(PAUSE_MODE_PROCESS);
	VisualServer::get_singleton()->connect("frame_pre_draw", this, "_update_draw");
}

void EffekseerSystem::_exit_tree()
{
	VisualServer::get_singleton()->disconnect("frame_pre_draw", this, "_update_draw");
}

void EffekseerSystem::_ready()
{
}

void EffekseerSystem::_process(float delta)
{
	for (size_t i = 0; i < m_render_layers.size(); i++) {
		auto& layer = m_render_layers[i];
		if (layer.viewport == nullptr) {
			continue;
		}

		if (layer.layer_type == LayerType::_3D) {
			if (Camera* camera = layer.viewport->get_camera()) {
				Effekseer::Manager::LayerParameter layerParams;
				layerParams.ViewerPosition = EffekseerGodot::ToEfkVector3(camera->get_camera_transform().get_origin());
				m_manager->SetLayerParameter((int32_t)i, layerParams);
			}
		}
	}

	// Stabilize in a variable frame environment
	float deltaFrames = delta * 60.0f;
	int iterations = std::max(1, (int)roundf(deltaFrames));
	float advance = deltaFrames / iterations;
	for (int i = 0; i < iterations; i++) {
		m_manager->Update(advance);
	}
	m_renderer->SetTime(m_renderer->GetTime() + delta);

	// Shader loading process
	_process_shader_loader();
}

void EffekseerSystem::_update_draw()
{
	m_renderer->ResetState();

	for (size_t i = 0; i < m_render_layers.size(); i++) {
		auto& layer = m_render_layers[i];
		if (layer.viewport == nullptr) {
			continue;
		}

		Effekseer::Manager::DrawParameter params{};
		params.CameraCullingMask = (int32_t)(1 << i);

		if (layer.layer_type == LayerType::_3D) {
			if (Camera* camera = layer.viewport->get_camera()) {
				Transform camera_transform = camera->get_camera_transform();
				Effekseer:: Matrix44 matrix = EffekseerGodot::ToEfkMatrix44(camera_transform.inverse());
				m_renderer->SetCameraMatrix(matrix);
			}
		} else if (layer.layer_type == LayerType::_2D) {
			Transform2D camera_transform = layer.viewport->get_canvas_transform();
			Effekseer:: Matrix44 matrix = EffekseerGodot::ToEfkMatrix44(camera_transform.inverse());
			matrix.Values[3][2] = -1.0f; // Z offset
			m_renderer->SetCameraMatrix(matrix);
		}

		m_renderer->BeginRendering();
		m_manager->Draw(params);
		m_renderer->EndRendering();
	}
}

int32_t EffekseerSystem::attach_layer(Viewport* viewport, LayerType layer_type)
{
	auto it = std::find_if(m_render_layers.begin(), m_render_layers.end(), 
		[viewport, layer_type](const RenderLayer& layer){ return layer.viewport == viewport && layer.layer_type == layer_type; });
	if (it != m_render_layers.end()) {
		// Existing layer
		it->ref_count++;
		return (int32_t)std::distance(m_render_layers.begin(), it);
	} else {
		// Add a new layer
		it = std::find_if(m_render_layers.begin(), m_render_layers.end(), 
			[](const RenderLayer& layer){ return layer.viewport == nullptr; });
		if (it == m_render_layers.end()) {
			Godot::print_error("Cannot draw in more than 30 viewports", 
				"EffekseerSystem::attach_layer", "EffekseerSystem.cpp", __LINE__);
			return EFFEKSEER_INVALID_LAYER;
		}
		it->viewport = viewport;
		it->layer_type = layer_type;
		it->ref_count = 1;
		return (int32_t)std::distance(m_render_layers.begin(), it);
	}
}

void EffekseerSystem::detach_layer(Viewport* viewport, LayerType layer_type)
{
	auto it = std::find_if(m_render_layers.begin(), m_render_layers.end(), 
		[viewport, layer_type](const RenderLayer& layer){ return layer.viewport == viewport && layer.layer_type == layer_type; });
	if (it != m_render_layers.end()) {
		if (--it->ref_count <= 0) {
			// Delete the layer
			it->viewport = nullptr;
			it->ref_count = 0;
		}
	}
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

EffekseerGodot::Shader* EffekseerSystem::get_builtin_shader(bool is_model, EffekseerRenderer::RendererShaderType shader_type)
{
	if (is_model) {
		return m_manager->GetModelRenderer().DownCast<EffekseerGodot::ModelRenderer>()->GetShader(shader_type);
	}
	else {
		return m_renderer->GetShader(shader_type);
	}
}

void EffekseerSystem::load_shader(ShaderLoadType load_type, godot::RID shader_rid)
{
	m_shader_load_queue.push({ load_type, shader_rid });
	m_shader_load_count++;
}

void EffekseerSystem::clear_shader_load_count()
{
	m_shader_load_count -= m_shader_load_progress;
	m_shader_load_progress = 0;
}

int EffekseerSystem::get_shader_load_count() const
{
	return m_shader_load_count;
}

int EffekseerSystem::get_shader_load_progress() const
{
	return m_shader_load_progress;
}

void EffekseerSystem::complete_all_shader_loads()
{
	m_should_complete_all_shader_loads = true;
}

void EffekseerSystem::_process_shader_loader()
{
	auto vs = VisualServer::get_singleton();

	// Destroy all completed loaders
	for (auto& loader : m_shader_loaders) {
		vs->free_rid(loader.matarial);
		vs->free_rid(loader.mesh);
		vs->free_rid(loader.instance);
	}
	m_shader_loaders.clear();
	
	// Start loader if queued load request
	const size_t load_count_in_a_frame = 1;
	size_t loaded_count = 0;
	while (loaded_count < load_count_in_a_frame || m_should_complete_all_shader_loads) {
		if (m_shader_load_queue.size() == 0) {
			break;
		}

		auto request = m_shader_load_queue.front();
		m_shader_load_queue.pop();

		ShaderLoader loader;
		loader.load_type = request.load_type;
		loader.matarial = vs->material_create();
		vs->material_set_shader(loader.matarial, request.shader_rid);

		if (loader.load_type == ShaderLoadType::CanvasItem) {
			loader.instance = vs->canvas_item_create();
			vs->canvas_item_set_material(loader.instance, loader.matarial);
			vs->canvas_item_add_rect(loader.instance, Rect2(0.0f, 0.0f, 0.0f, 0.0f), Color());
			vs->canvas_item_set_parent(loader.instance, get_viewport()->find_world_2d()->get_canvas());
			m_shader_loaders.push_back(loader);
		}
		else {
			loader.mesh = vs->mesh_create();
			loader.instance = vs->instance_create();

			godot::PoolVector3Array positions;
			positions.resize(1);

			godot::Array arrays;
			arrays.resize(VisualServer::ARRAY_MAX);
			arrays[VisualServer::ARRAY_VERTEX] = positions;

			int64_t format = VisualServer::ARRAY_FORMAT_VERTEX;
			if (request.load_type == ShaderLoadType::SpatialModel) {
				format |= VisualServer::ARRAY_FLAG_USE_OCTAHEDRAL_COMPRESSION;
			}
			vs->mesh_add_surface_from_arrays(loader.mesh, VisualServer::PRIMITIVE_POINTS, arrays, {}, format);
			vs->mesh_surface_set_material(loader.mesh, 0, loader.matarial);
			vs->instance_set_base(loader.instance, loader.mesh);
			vs->instance_set_scenario(loader.instance, get_viewport()->find_world()->get_scenario());
		}

		m_shader_loaders.push_back(loader);
		loaded_count++;
	}

	m_shader_load_progress += loaded_count;
	m_should_complete_all_shader_loads = false;
}

}
