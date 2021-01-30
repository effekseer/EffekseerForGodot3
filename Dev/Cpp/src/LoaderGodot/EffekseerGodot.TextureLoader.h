#pragma once

#include <Effekseer.h>

namespace EffekseerGodot
{

class TextureLoader : public Effekseer::TextureLoader
{
public:
	TextureLoader() = default;
	virtual ~TextureLoader() = default;
	Effekseer::TextureRef Load(const char16_t* path, Effekseer::TextureType textureType) override;
	void Unload(Effekseer::TextureRef texture) override;
};

} // namespace EffekseerGodot
