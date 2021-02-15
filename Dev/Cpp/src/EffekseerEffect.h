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

	void resolve_dependencies();
	
	void setup();

	String get_data_path() const { return m_data_path; }

	void set_data_path(String path) { m_data_path = path; }

	Dictionary get_subresources() const { return m_subresources; }

	void set_subresources(Dictionary subresources) { m_subresources = subresources; }

	PoolByteArray get_data_bytes() const { return m_data_bytes; }

	void set_data_bytes(PoolByteArray bytes) { m_data_bytes = bytes; }

	float get_scale() const { return m_scale; }

	void set_scale(float scale) { m_scale = scale; }

	Effekseer::EffectRef& get_native() { return m_native; }

private:
	void get_material_path(char16_t* path, size_t path_size);

	String m_data_path;
	PoolByteArray m_data_bytes;
	Dictionary m_subresources;
	float m_scale = 1.0f;
	Effekseer::EffectRef m_native;
};

}
