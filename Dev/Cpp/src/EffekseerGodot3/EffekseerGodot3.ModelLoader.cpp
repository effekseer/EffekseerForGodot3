#include <ResourceLoader.hpp>
#include "EffekseerGodot3.Resources.h"
#include "EffekseerGodot3.ModelLoader.h"
#include "EffekseerGodot3.Utils.h"
#include "../EffekseerResource.h"

namespace EffekseerGodot3
{

ModelLoader::ModelLoader()
{
}

ModelLoader::~ModelLoader()
{
}

Effekseer::ModelRef ModelLoader::Load(const char16_t* path)
{
	// Load by Godot
	auto loader = godot::ResourceLoader::get_singleton();
	auto resource = loader->load(Convert::String16(path), "");
	if (!resource.is_valid())
	{
		return nullptr;
	}

	auto efkres = godot::as<godot::EffekseerResource>(resource.ptr());
	auto& data = efkres->get_data_ref();

	return Load(data.read().ptr(), data.size());
}

Effekseer::ModelRef ModelLoader::Load(const void* data, int32_t size)
{
	return Effekseer::MakeRefPtr<Model>(data, size);
}

void ModelLoader::Unload(Effekseer::ModelRef data)
{
}

} // namespace EffekseerGodot3

