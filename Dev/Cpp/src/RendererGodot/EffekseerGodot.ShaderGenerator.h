#pragma once

#include <vector>
#include "EffekseerGodot.Shader.h"

namespace EffekseerGodot
{

struct ShaderData
{
	std::string CodeSpatial;
	std::string CodeCanvasItem;
	std::vector<Shader::ParamDecl> ParamDecls;
	int32_t VertexConstantBufferSize;
	int32_t PixelConstantBufferSize;
};

class ShaderGenerator
{
public:
	static constexpr size_t ShaderMax = static_cast<size_t>(Effekseer::MaterialShaderType::Max);
	std::array<ShaderData, ShaderMax> Generate(const Effekseer::MaterialFile& materialFile);

private:
	ShaderData GenerateShaderData(const Effekseer::MaterialFile& materialFile, Effekseer::MaterialShaderType shaderType);
	std::string GenerateShaderCode(const Effekseer::MaterialFile& materialFile, bool isSprite, bool isRefrection, bool isSpatial);
	void GenerateParamDecls(ShaderData& shaderData, const Effekseer::MaterialFile& materialFile, bool isSprite, bool isRefrection);
};

} // namespace Effekseer
