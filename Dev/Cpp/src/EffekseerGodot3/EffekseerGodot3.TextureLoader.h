#pragma once

#include <unordered_map>
#include <string>
#include <Effekseer.h>
#include "EffekseerGodot3.Resources.h"

namespace EffekseerGodot3
{

class TextureLoader : public Effekseer::TextureLoader
{
protected:
	std::unordered_map<std::u16string, std::unique_ptr<TextureResource>> resources;

public:
	TextureLoader() {}
	virtual ~TextureLoader() {}
	Effekseer::TextureData* Load(const char16_t* path, Effekseer::TextureType textureType) override;
	void Unload(Effekseer::TextureData* texture) override;
};

} // namespace EffekseerGodot3
