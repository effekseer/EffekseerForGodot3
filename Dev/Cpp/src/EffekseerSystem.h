#pragma once

#include <Godot.hpp>
#include <World.hpp>
#include <Camera.hpp>
#include <Camera2D.hpp>
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

class EffekseerSystem : public Node
{
	GODOT_CLASS(EffekseerSystem, Node)

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

	void draw3D(Effekseer::Handle handle, const Transform& camera_transform);

	void draw2D(Effekseer::Handle handle, const Transform2D& camera_transform);

	void stop_all_effects();

	void set_paused_to_all_effects(bool paused);

	int get_total_instance_count() const;

	const Effekseer::ManagerRef& get_manager() { return m_manager; }

private:
	static EffekseerSystem* s_instance;

	Effekseer::ManagerRef m_manager;
	EffekseerGodot::RendererRef m_renderer;
};

}
