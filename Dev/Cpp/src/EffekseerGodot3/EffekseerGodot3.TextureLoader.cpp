#include <Image.hpp>
#include <ImageTexture.hpp>
#include <ResourceLoader.hpp>
#include "EffekseerGodot3.TextureLoader.h"
#include "EffekseerGodot3.Utils.h"

namespace EffekseerGodot3
{

Effekseer::TextureRef TextureLoader::Load(const char16_t* path, Effekseer::TextureType textureType)
{
	char path8[1024];
	Effekseer::ConvertUtf16ToUtf8((int8_t*)path8, sizeof(path8), (const int16_t*)path);

	// Load by Godot
	auto loader = godot::ResourceLoader::get_singleton();
	auto resource = loader->load(path8);
	if (!resource.is_valid())
	{
		printf("TextureLoader::Load path=%s Failed.\n", path8);
		return nullptr;
	}

	auto texture = (godot::ImageTexture*)resource.ptr();
	texture->set_flags(texture->get_flags() | godot::Texture::FLAG_MIPMAPS | godot::Texture::FLAG_FILTER | godot::Texture::FLAG_REPEAT);

	auto backend = Effekseer::MakeRefPtr<Texture>();
	backend->size_[0] = (int32_t)texture->get_width();
	backend->size_[1] = (int32_t)texture->get_height();
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

	printf("TextureLoader::Load path=%s (%dx%d)\n", path8, backend->GetSize()[0], backend->GetSize()[1]);

	auto result = Effekseer::MakeRefPtr<Effekseer::Texture>();
	result->SetBackend(backend);
	return result;
}

void TextureLoader::Unload(Effekseer::TextureRef textureData)
{
}

} // namespace EffekseerGodot3