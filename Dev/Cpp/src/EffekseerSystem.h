#pragma once

#include <Godot.hpp>
#include <Node.hpp>
#include <Effekseer.h>
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

	EffekseerSystem();

	~EffekseerSystem();

	void _init();

	void _enter_tree();

	void _exit_tree();

	void _process(float delta);

	void _update_draw();

	int32_t attach_layer(Viewport* viewport, LayerType layer_type);

	void detach_layer(Viewport* viewport, LayerType layer_type);

	void stop_all_effects();

	void set_paused_to_all_effects(bool paused);

	int get_total_instance_count() const;

	const Effekseer::ManagerRef& get_manager() { return m_manager; }

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
};

}
