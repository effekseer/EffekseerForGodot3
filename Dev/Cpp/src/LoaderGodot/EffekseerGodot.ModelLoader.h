#pragma once

#include <Effekseer.h>

namespace EffekseerGodot
{

class ModelLoader : public ::Effekseer::ModelLoader
{
public:
	ModelLoader() = default;

	virtual ~ModelLoader() = default;

	Effekseer::ModelRef Load(const char16_t* path) override;

	Effekseer::ModelRef Load(const void* data, int32_t size) override;

	void Unload(Effekseer::ModelRef data) override;
};

} // namespace EffekseerGodot

