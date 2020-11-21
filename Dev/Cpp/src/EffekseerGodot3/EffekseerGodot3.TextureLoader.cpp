#include <Image.hpp>
#include <ImageTexture.hpp>
#include <ResourceLoader.hpp>
#include "EffekseerGodot3.TextureLoader.h"
#include "EffekseerGodot3.Utils.h"

namespace EffekseerGodot3
{

Effekseer::TextureData* TextureLoader::Load(const char16_t* path, Effekseer::TextureType textureType)
{
	std::u16string pathstr = (const char16_t*)path;

	// find it from resource table and if it exists, it is reused.
	auto it = resources.find(pathstr);
	if (it != resources.end())
	{
		it->second->ReferenceCount++;
		return it->second.get();
	}

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

	auto added = resources.emplace(pathstr, new TextureResource());
	auto& res = added.first->second;
	res->Name = std::move(pathstr);
	res->Width = (int32_t)texture->get_width();
	res->Height = (int32_t)texture->get_height();
	res->GodotTexture = resource;

	//switch (texture->get_format())
	//{
	//case godot::Image::FORMAT_RGBA8: res->TextureFormat = Effekseer::TextureFormatType::ABGR8; break;
	//case godot::Image::FORMAT_DXT1:  res->TextureFormat = Effekseer::TextureFormatType::BC1; break;
	//case godot::Image::FORMAT_DXT3:  res->TextureFormat = Effekseer::TextureFormatType::BC2; break;
	//case godot::Image::FORMAT_DXT5:  res->TextureFormat = Effekseer::TextureFormatType::BC3; break;
	//default: printf("Unknown format.\n");
	//}
	
	res->UserID = Convert::RIDToInt64(resource->get_rid());

	printf("TextureLoader::Load path=%s (%dx%d)\n", path8, res->Width, res->Height);

	return res.get();
}

void TextureLoader::Unload(Effekseer::TextureData* textureData)
{
	if (textureData == nullptr)
	{
		return;
	}

	TextureResource* res = (TextureResource*)textureData;
	// if refrercen count is zero, it is released
	res->ReferenceCount--;
	if (res->ReferenceCount <= 0)
	{
		// Unload by Godot
		resources.erase(res->Name);
	}
}

} // namespace EffekseerGodot3