#pragma once

#include <Effekseer.h>

namespace Effekseer
{
class MaterialFile;
} // namespace Effekseer

namespace EffekseerGodot
{

class MaterialLoader : public ::Effekseer::MaterialLoader
{
public:
	MaterialLoader() = default;

	virtual ~MaterialLoader() = default;

	::Effekseer::MaterialRef Load(const char16_t* path) override;

	::Effekseer::MaterialRef Load(const void* data, int32_t size, Effekseer::MaterialFileType fileType) override;

	void Unload(::Effekseer::MaterialRef data) override;

private:
	::Effekseer::MaterialRef LoadAcutually(const ::Effekseer::MaterialFile& materialFile);
};

} // namespace EffekseerGodot
