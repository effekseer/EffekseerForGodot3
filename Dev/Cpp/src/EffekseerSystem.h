#pragma once

#include <Godot.hpp>
#include <World.hpp>
#include <Camera.hpp>
#include <Node.hpp>
#include <Effekseer.h>

namespace EffekseerGodot3
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

	Effekseer::Handle play(godot::Ref<EffekseerEffect> effect, const Transform& transform);

	void stop(Effekseer::Handle handle);
	
	void stop_root(Effekseer::Handle handle);
	
	void set_paused(Effekseer::Handle handle, bool paused);

	bool exists(Effekseer::Handle handle);

	int get_total_instance_count() const;

	Effekseer::Manager* get_manager() const { return m_manager; }

private:
	static EffekseerSystem* s_instance;

	Effekseer::Manager* m_manager = nullptr;
	EffekseerGodot3::Renderer* m_renderer = nullptr;
	bool m_shouldResetState = true;
};

}
