#include <ResourceLoader.hpp>
#include "EffekseerGodot3.MaterialLoader.h"
#include "EffekseerGodot3.ShaderGenerator.h"
#include "EffekseerGodot3.ModelRenderer.h"
#include "EffekseerGodot3.Shader.h"
#include "EffekseerGodot3.Utils.h"
#include "../EffekseerResource.h"

namespace EffekseerGodot3
{

MaterialLoader::MaterialLoader(const RendererImplementedRef& renderer, ::Effekseer::FileInterface* fileInterface)
	: renderer_(renderer)
	, fileInterface_(fileInterface)
{
	if (fileInterface == nullptr)
	{
		fileInterface_ = &defaultFileInterface_;
	}
}

MaterialLoader ::~MaterialLoader()
{
}

::Effekseer::MaterialRef MaterialLoader::Load(const char16_t* path)
{
	// Load by Godot
	auto loader = godot::ResourceLoader::get_singleton();
	auto resource = loader->load(Convert::String16(path), "");
	if (!resource.is_valid())
	{
		return nullptr;
	}

	auto efkres = godot::as<godot::EffekseerResource>(resource.ptr());
	auto& data = efkres->get_data_ref();

	return Load(data.read().ptr(), data.size(), Effekseer::MaterialFileType::Code);
}

::Effekseer::MaterialRef MaterialLoader::LoadAcutually(const ::Effekseer::MaterialFile& materialFile)
{
	using namespace EffekseerRenderer;

	auto material = ::Effekseer::MakeRefPtr<::Effekseer::Material>();
	material->IsSimpleVertex = materialFile.GetIsSimpleVertex();
	material->IsRefractionRequired = materialFile.GetHasRefraction();

	ShaderGenerator shaderGenerator;
	auto shaderDataList = shaderGenerator.Generate(materialFile);

	{
		auto shader = Shader::Create("CustomShader_Sprite", shaderDataList[0].Code.c_str(), 
			RendererShaderType::Material, std::move(shaderDataList[0].ParamDecls));
		shader->SetVertexConstantBufferSize(shaderDataList[0].VertexConstantBufferSize);
		shader->SetPixelConstantBufferSize(shaderDataList[0].PixelConstantBufferSize);
		material->UserPtr = shader.release();
	}
	{
		auto shader = Shader::Create("CustomShader_Model", shaderDataList[1].Code.c_str(), 
			RendererShaderType::Material, std::move(shaderDataList[1].ParamDecls));
		shader->SetVertexConstantBufferSize(shaderDataList[1].VertexConstantBufferSize);
		shader->SetPixelConstantBufferSize(shaderDataList[1].PixelConstantBufferSize);
		material->ModelUserPtr = shader.release();
	}

	if (materialFile.GetHasRefraction())
	{
	}

	material->CustomData1 = materialFile.GetCustomData1Count();
	material->CustomData2 = materialFile.GetCustomData2Count();
	material->TextureCount = std::min(materialFile.GetTextureCount(), Effekseer::UserTextureSlotMax);
	material->UniformCount = materialFile.GetUniformCount();
	material->ShadingModel = materialFile.GetShadingModel();

	for (int32_t i = 0; i < material->TextureCount; i++)
	{
		material->TextureWrapTypes.at(i) = materialFile.GetTextureWrap(i);
	}

	return material;
}

::Effekseer::MaterialRef MaterialLoader::Load(const void* data, int32_t size, Effekseer::MaterialFileType fileType)
{
	Effekseer::MaterialFile materialFile;

	if (materialFile.Load((const uint8_t*)data, size))
	{
		return LoadAcutually(materialFile);
	}

	return nullptr;
}

void MaterialLoader::Unload(::Effekseer::MaterialRef data)
{
	if (data == nullptr)
		return;

	auto shader = reinterpret_cast<Shader*>(data->UserPtr);
	auto modelShader = reinterpret_cast<Shader*>(data->ModelUserPtr);
	auto refractionShader = reinterpret_cast<Shader*>(data->RefractionUserPtr);
	auto refractionModelShader = reinterpret_cast<Shader*>(data->RefractionModelUserPtr);

	ES_SAFE_DELETE(shader);
	ES_SAFE_DELETE(modelShader);
	ES_SAFE_DELETE(refractionShader);
	ES_SAFE_DELETE(refractionModelShader);

	data->UserPtr = nullptr;
	data->ModelUserPtr = nullptr;
	data->RefractionUserPtr = nullptr;
	data->RefractionModelUserPtr = nullptr;
}

} // namespace EffekseerGodot3