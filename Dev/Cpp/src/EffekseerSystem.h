#pragma once

#include <Godot.hpp>
#include <Node.hpp>
#include <Effekseer.h>
#include "EffekseerRenderer.CommonUtils.h"
#include "RendererGodot/EffekseerGodot.Renderer.h"
#include "SoundGodot/EffekseerGodot.SoundPlayer.h"

namespace EffekseerGodot
{
class Renderer;
}

namespace godot {

class EffekseerEffect;
class EffekseerEmitter;
class EffekseerEmitter2D;

constexpr int32_t EFFEKSEER_INVALID_LAYER = -1;

class EffekseerSystem : public Node
{
	GODOT_CLASS(EffekseerSystem, Node)

public:
	enum class LayerType {
		_3D,
		_2D,
	};

public:
	static void _register_methods();

	static EffekseerSystem* get_instance() { return s_instance; }

	static std::tuple<EffekseerSystem*, Effekseer::ManagerRef> get_instance_manager()
	{
		if (s_instance == nullptr)
		{
			return std::make_tuple(nullptr, nullptr);
		}
		return std::make_tuple(s_instance, s_instance->get_manager());
	}

	EffekseerSystem();

	~EffekseerSystem();

	void _init();

	void _enter_tree();

	void _exit_tree();

	void _ready();

	void _process(float delta);

	void _update_draw();

	int32_t attach_layer(Viewport* viewport, LayerType layer_type);

	void detach_layer(Viewport* viewport, LayerType layer_type);

	void stop_all_effects();

	void set_paused_to_all_effects(bool paused);

	int get_total_instance_count() const;

	const Effekseer::ManagerRef& get_manager() { return m_manager; }

	EffekseerGodot::Shader* get_builtin_shader(bool is_model, EffekseerRenderer::RendererShaderType shader_type);

	enum class ShaderLoadType
	{
		CanvasItem,
		SpatialStandard,
		SpatialModel,
	};

	void load_shader(ShaderLoadType load_type, godot::RID shader_rid);

	void clear_shader_load_count();

	int get_shader_load_count() const;

	int get_shader_load_progress() const;

	void complete_all_shader_loads();

private:
	void _process_shader_loader();

private:
	static EffekseerSystem* s_instance;

	Effekseer::ManagerRef m_manager;
	EffekseerGodot::RendererRef m_renderer;

	struct RenderLayer {
		Viewport* viewport = nullptr;
		LayerType layer_type = LayerType::_3D;
		int32_t ref_count = 0;
	};
	std::array<RenderLayer, 30> m_render_layers;

	struct ShaderLoadRequest {
		ShaderLoadType load_type;
		godot::RID shader_rid;
	};
	std::queue<ShaderLoadRequest> m_shader_load_queue;
	bool m_should_complete_all_shader_loads = false;
	int m_shader_load_count = 0;
	int m_shader_load_progress = 0;

	struct ShaderLoader {
		ShaderLoadType load_type;
		godot::RID matarial;
		godot::RID mesh;
		godot::RID instance;
	};
	std::vector<ShaderLoader> m_shader_loaders;
};

}
