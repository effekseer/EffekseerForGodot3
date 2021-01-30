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

	String get_data_path() const { return m_data_path; }

	void set_data_path(String path) { m_data_path = path; }

	Array get_subresources() const { return m_subresources; }

	void set_subresources(Array resources) { m_subresources = resources; }

	PoolByteArray get_data_bytes() const { return m_data_bytes; }

	void set_data_bytes(PoolByteArray bytes);

	Effekseer::EffectRef& get_native() { return m_native; }

private:
	void load_in_editor();
	void load_in_runtime();
	void get_material_path(char16_t* path, size_t path_size);

	String m_data_path;
	PoolByteArray m_data_bytes;
	Array m_subresources;
	Effekseer::EffectRef m_native;
};

}
