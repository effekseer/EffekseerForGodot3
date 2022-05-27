#pragma once

#include <stdint.h>
#include <Effekseer.h>
#include <Resource.hpp>

namespace EffekseerGodot
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
	Model(const Effekseer::CustomVector<Vertex>& vertecies, const Effekseer::CustomVector<Face>& faces);
	Model(const void* data, int32_t size);
	~Model();
	godot::RID GetRID() const { return meshRid_; }

private:
	void UploadToEngine();

	godot::RID meshRid_;
};

}
