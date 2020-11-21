#pragma once

#include <Godot.hpp>
#include <Resource.hpp>
#include <Effekseer.h>

namespace godot {

class EffekseerEffect : public Resource
{
	GODOT_CLASS(EffekseerEffect, Resource)

public:
	static void _register_methods();

	EffekseerEffect();
	~EffekseerEffect();

	void _init();

	void load(String path);

	void release();

	String get_load_path() const { return m_load_path; }

	Effekseer::EffectRef& get_native() { return m_native; }

private:
	String m_load_path;
	Effekseer::EffectRef m_native;
};

}
