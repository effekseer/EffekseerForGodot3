#include <ResourceLoader.hpp>
#include "EffekseerGodot.MaterialLoader.h"
#include "../RendererGodot/EffekseerGodot.ShaderGenerator.h"
#include "../RendererGodot/EffekseerGodot.Shader.h"
#include "../RendererGodot/EffekseerGodot.RenderResources.h"
#include "../Utils/EffekseerGodot.Utils.h"
#include "../EffekseerResource.h"

namespace EffekseerGodot
{

::Effekseer::MaterialRef MaterialLoader::Load(const char16_t* path)
{
	// Load by Godot
	auto loader = godot::ResourceLoader::get_singleton();
	auto resource = loader->load(ToGdString(path), "");
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
		auto shader = Shader::Create("Custom_Sprite", RendererShaderType::Material);
		shader->SetVertexConstantBufferSize(shaderDataList[0].VertexConstantBufferSize);
		shader->SetPixelConstantBufferSize(shaderDataList[0].PixelConstantBufferSize);
		shader->Compile(Shader::RenderType::SpatialLightweight, shaderDataList[0].Code.c_str(), std::move(shaderDataList[0].ParamDecls));
		shader->Compile(Shader::RenderType::CanvasItem, shaderDataList[0].Code.c_str(), std::move(shaderDataList[0].ParamDecls));
		material->UserPtr = shader.release();
	}
	{
		auto shader = Shader::Create("Model_Custom", RendererShaderType::Material);
		shader->SetVertexConstantBufferSize(shaderDataList[1].VertexConstantBufferSize);
		shader->SetPixelConstantBufferSize(shaderDataList[1].PixelConstantBufferSize);
		shader->Compile(Shader::RenderType::SpatialLightweight, shaderDataList[1].Code.c_str(), std::move(shaderDataList[1].ParamDecls));
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

} // namespace EffekseerGodot
