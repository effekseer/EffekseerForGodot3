#include <Image.hpp>
#include <ImageTexture.hpp>
#include <ResourceLoader.hpp>
#include "EffekseerGodot.TextureLoader.h"
#include "../RendererGodot/EffekseerGodot.RenderResources.h"
#include "../Utils/EffekseerGodot.Utils.h"

namespace EffekseerGodot
{

Effekseer::TextureRef TextureLoader::Load(const char16_t* path, Effekseer::TextureType textureType)
{
	godot::String gdpath = ToGdString(path);

	// Load by Godot
	auto loader = godot::ResourceLoader::get_singleton();
	auto resource = loader->load(gdpath);
	if (!resource.is_valid())
	{
		return nullptr;
	}

	auto texture = (godot::ImageTexture*)resource.ptr();
	texture->set_flags(godot::Texture::FLAG_MIPMAPS);

	auto backend = Effekseer::MakeRefPtr<Texture>();
	backend->param_.Size[0] = (int32_t)texture->get_width();
	backend->param_.Size[1] = (int32_t)texture->get_height();
	backend->godotTexture_ = resource;
	backend->textureRid_ = resource->get_rid();

	//auto format = texture->get_format();
	//switch (format)
	//{
	//case godot::Image::FORMAT_RGBA8: backend->format_ = Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM; break;
	//case godot::Image::FORMAT_DXT1:  backend->format_ = Effekseer::Backend::TextureFormatType::BC1; break;
	//case godot::Image::FORMAT_DXT3:  backend->format_ = Effekseer::Backend::TextureFormatType::BC2; break;
	//case godot::Image::FORMAT_DXT5:  backend->format_ = Effekseer::Backend::TextureFormatType::BC3; break;
	//default: printf("Unknown format.\n");
	//}

	//printf("TextureLoader::Load path=%s (%dx%d)\n", path8, backend->GetSize()[0], backend->GetSize()[1]);

	auto result = Effekseer::MakeRefPtr<Effekseer::Texture>();
	result->SetBackend(backend);
	return result;
}

void TextureLoader::Unload(Effekseer::TextureRef textureData)
{
}

} // namespace EffekseerGodot
