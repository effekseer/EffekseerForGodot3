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
};

}
