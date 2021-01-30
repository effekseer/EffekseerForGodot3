#pragma once

#include <Godot.hpp>
#include <Resource.hpp>
#include <Effekseer.h>

namespace godot {

class EffekseerResource : public Resource
{
	GODOT_CLASS(EffekseerResource, Resource)

public:
	static void _register_methods();

	EffekseerResource();
	~EffekseerResource();

	void _init();

	void load(String path);

	const PoolByteArray& get_data_ref() const { return m_data_bytes; }

	PoolByteArray get_data_bytes() const { return m_data_bytes; }

	void set_data_bytes(PoolByteArray bytes) { m_data_bytes = bytes; }

private:
	PoolByteArray m_data_bytes;
};

}
