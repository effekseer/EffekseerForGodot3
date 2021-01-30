#include <File.hpp>
#include "EffekseerSystem.h"
#include "EffekseerResource.h"

namespace godot {

void EffekseerResource::_register_methods()
{
	register_method("_init", &EffekseerResource::_init);
	register_method("load", &EffekseerResource::load);
	register_property<EffekseerResource, PoolByteArray>("data_bytes", 
		&EffekseerResource::set_data_bytes, &EffekseerResource::get_data_bytes, {});
}

EffekseerResource::EffekseerResource()
{
}

EffekseerResource::~EffekseerResource()
{
}

void EffekseerResource::_init()
{
}

void EffekseerResource::load(String path)
{
	godot::Ref<godot::File> file = godot::File::_new();
	if (file->open(path, godot::File::READ) != godot::Error::OK) {
		return;
	}

	int64_t size = file->get_len();
	m_data_bytes = file->get_buffer(size);
	file->close();
}

}
