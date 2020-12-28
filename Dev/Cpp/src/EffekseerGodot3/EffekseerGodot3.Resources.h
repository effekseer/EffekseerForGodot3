#pragma once

#include <stdint.h>
#include <Effekseer.h>
#include <Resource.hpp>

namespace EffekseerGodot3
{
	
class Texture : public Effekseer::Backend::Texture
{
public:
	godot::RID GetRID() const { return textureRid_; }

private:
	friend class TextureLoader;

	godot::Ref<godot::Resource> godotTexture_;
	godot::RID textureRid_;
};

class Model : public Effekseer::Model
{
public:
	Model(const void* data, int32_t size);
	~Model();
	godot::RID GetRID() const { return meshRid_; }

private:
	friend class ModelLoader;

	godot::RID meshRid_;
};

}
