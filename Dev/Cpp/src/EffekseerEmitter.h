#pragma once

#include <Godot.hpp>
#include <Spatial.hpp>
#include "EffekseerEffect.h"

namespace godot {

class EffekseerEmitter : public Spatial
{
	GODOT_CLASS(EffekseerEmitter, Spatial)

public:
	static void _register_methods();

	EffekseerEmitter();

	~EffekseerEmitter();

	void _init();

	void _ready();

	void _enter_tree();

	void _exit_tree();

	void _notification(int what);

	void _update_transform();

	void _update_visibility();

	void _update_paused();

	void remove_handle(Effekseer::Handle handle);

	void play();

	void stop();

	void stop_root();

	bool is_playing();

	void set_paused(bool paused);

	bool is_paused() const;

	void set_speed(float speed);

	float get_speed() const;

	void set_color(Color color);

	Color get_color() const;

	void set_target_position(Vector3 position);

	Vector3 get_target_position() const;

	void set_effect(Ref<EffekseerEffect> effect);

	Ref<EffekseerEffect> get_effect() const { return m_effect; }

	void set_autoplay(bool autoplay) { m_autoplay = autoplay; }

	bool is_autoplay() const { return m_autoplay; }

	void set_dynamic_input(int index, float value);

	void send_trigger(int index);

private:
	Ref<EffekseerEffect> m_effect;
	int32_t m_layer = -1;
	bool m_autoplay = true;
	Array m_handles;
	bool m_paused = false;
	float m_speed = 1.0f;
	Effekseer::Color m_color = {255, 255, 255, 255};
	Vector3 m_target_position = {};
};

}
