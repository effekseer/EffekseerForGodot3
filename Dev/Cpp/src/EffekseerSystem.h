#pragma once

#include <Godot.hpp>
#include <World.hpp>
#include <Camera.hpp>
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

	void _process(float delta);

	void draw(Camera* camera, Effekseer::Handle handle);

	void stop_all_effects();

	void set_paused_to_all_effects(bool paused);

	int get_total_instance_count() const;

	const Effekseer::ManagerRef& get_manager() { return m_manager; }

private:
	static EffekseerSystem* s_instance;

	Effekseer::ManagerRef m_manager;
	EffekseerGodot::RendererRef m_renderer;
	bool m_shouldResetState = true;
};

}
