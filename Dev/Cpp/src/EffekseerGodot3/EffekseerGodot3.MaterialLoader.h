
#ifndef __EFFEKSEERRENDERER_GODOT3_MATERIALLOADER_H__
#define __EFFEKSEERRENDERER_GODOT3_MATERIALLOADER_H__

#include "EffekseerGodot3.RendererImplemented.h"
#include <memory>

namespace Effekseer
{
class Material;
class CompiledMaterialBinary;
} // namespace Effekseer

namespace EffekseerGodot3
{

class MaterialLoader : public ::Effekseer::MaterialLoader
{
private:
	Renderer* renderer_ = nullptr;
	::Effekseer::FileInterface* fileInterface_ = nullptr;
	::Effekseer::DefaultFileInterface defaultFileInterface_;

	::Effekseer::MaterialRef LoadAcutually(::Effekseer::Material& material, ::Effekseer::CompiledMaterialBinary* binary);

public:
	MaterialLoader(Renderer* renderer, ::Effekseer::FileInterface* fileInterface);
	virtual ~MaterialLoader();

	::Effekseer::MaterialRef Load(const EFK_CHAR* path) override;

	::Effekseer::MaterialRef Load(const void* data, int32_t size, Effekseer::MaterialFileType fileType) override;

	void Unload(::Effekseer::MaterialRef data) override;
};

} // namespace EffekseerGodot3

#endif // __EFFEKSEERRENDERER_GL_MODELLOADER_H__
