#pragma once

#include <Godot.hpp>
#include <Node2D.hpp>
#include "EffekseerEffect.h"

namespace godot {

class EffekseerEmitter2D : public Node2D
{
	GODOT_CLASS(EffekseerEmitter2D, Node2D)

public:
	static void _register_methods();

	EffekseerEmitter2D();

	~EffekseerEmitter2D();

	void _init();

	void _ready();

	void _enter_tree();

	void _exit_tree();

	void _process(float delta);

	void _update_draw();

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

	void set_orientation(Vector3 orientation) { m_orientation = orientation; }

	Vector3 get_orientation() const { return m_orientation; }

	void set_flip_h(bool flip_h) { m_flip_h = flip_h; }

	bool get_flip_h() const { return m_flip_h; }

	void set_flip_v(bool flip_v) { m_flip_v = flip_v; }

	bool get_flip_v() const { return m_flip_v; }

	void set_effect(Ref<EffekseerEffect> effect);

	Ref<EffekseerEffect> get_effect() const { return m_effect; }

	void set_autoplay(bool autoplay) { m_autoplay = autoplay; }

	bool is_autoplay() const { return m_autoplay; }

private:
	Ref<EffekseerEffect> m_effect;
	bool m_autoplay = true;
	Array m_handles;
	bool m_paused = false;
	float m_speed = 1.0f;
	Effekseer::Color m_color = {255, 255, 255, 255};
	Vector3 m_orientation;
	bool m_flip_h = false;
	bool m_flip_v = true;
};

}
