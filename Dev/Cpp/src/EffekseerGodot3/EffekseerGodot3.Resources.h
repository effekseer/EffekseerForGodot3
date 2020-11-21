#pragma once

#include <stdint.h>
#include <Effekseer.h>
#include <Resource.hpp>

namespace EffekseerGodot3
{
	
struct TextureResource : public Effekseer::TextureData
{
	int ReferenceCount = 1;
	std::u16string Name;
	godot::Ref<godot::Resource> GodotTexture;
};

class ModelResource : public Effekseer::Model
{
public:
	ModelResource(const void* data, int32_t size);
	~ModelResource();
	godot::RID GetRID() const { return GodotMesh; }

private:
	int ReferenceCount = 1;
	std::u16string Name;
	godot::RID GodotMesh;
};

}
