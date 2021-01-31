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

	Effekseer::Handle play(godot::Ref<EffekseerEffect> effect, Spatial* node);

	void stop(Effekseer::Handle handle);
	
	void stop_root(Effekseer::Handle handle);
	
	void set_paused(Effekseer::Handle handle, bool paused);

	bool exists(Effekseer::Handle handle);

	int get_total_instance_count() const;

	const Effekseer::ManagerRef& get_manager() { return m_manager; }

private:
	static EffekseerSystem* s_instance;

	Effekseer::ManagerRef m_manager;
	EffekseerGodot::RendererRef m_renderer;
	EffekseerGodot::SoundPlayerRef m_soundPlayer;
	bool m_shouldResetState = true;
};

}
