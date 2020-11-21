//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <File.hpp>
#include "EffekseerGodot3.Resources.h"
#include "EffekseerGodot3.ModelLoader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerGodot3
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelLoader::ModelLoader()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelLoader::~ModelLoader()
{
}

Effekseer::Model* ModelLoader::Load(const char16_t* path)
{
	char path8[1024];
	Effekseer::ConvertUtf16ToUtf8((int8_t*)path8, sizeof(path8), (const int16_t*)path);

	godot::Ref<godot::File> file = godot::File::_new();
	if (file->open(path8, godot::File::READ) != godot::Error::OK)
	{
		printf("ModelLoader::Load path=%s Failed.\n", path8);
		return nullptr;
	}

	auto buffer = file->get_buffer(file->get_len());
	auto bufferReader = buffer.read();

	printf("ModelLoader::Load path=%s\n", path8);

	return Load(bufferReader.ptr(), buffer.size());
}

Effekseer::Model* ModelLoader::Load(const void* data, int32_t size)
{
	return new ModelResource(data, size);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void ModelLoader::Unload(Effekseer::Model* data)
{
	if (data != NULL)
	{
		auto model = (Effekseer::Model*)data;
		delete model;
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerGodot3
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
