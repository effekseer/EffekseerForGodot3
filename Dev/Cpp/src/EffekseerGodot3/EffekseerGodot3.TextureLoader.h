#pragma once

#include <unordered_map>
#include <string>
#include <Effekseer.h>
#include "EffekseerGodot3.Resources.h"

namespace EffekseerGodot3
{

class TextureLoader : public Effekseer::TextureLoader
{
public:
	TextureLoader() {}
	virtual ~TextureLoader() {}
	Effekseer::TextureRef Load(const char16_t* path, Effekseer::TextureType textureType) override;
	void Unload(Effekseer::TextureRef texture) override;
};

} // namespace EffekseerGodot3
