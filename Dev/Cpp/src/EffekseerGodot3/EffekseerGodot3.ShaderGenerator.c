#pragma once

#include <assert.h>
#include <iostream>
#undef min

namespace Effekseer
{
namespace GodotShader
{

static const char g_material_src_vertex_pre[] =  R"(

void vertex()
{
	vec2 uv1 = UV;
	vec2 uv2 = UV2;
	vec4 vcolor = COLOR;

	mat3 normalMatrix = mat3(ModelMatrix);
	vec3 worldNormal = normalMatrix * NORMAL;
	vec3 worldTangent = normalMatrix * TANGENT;
	vec3 worldBinormal = normalMatrix * BINORMAL;

	v_WorldN_PX.xyz = worldNormal;
	v_WorldB_PY.xyz = worldBinormal;
	v_WorldT_PZ.xyz = worldTangent;
	vec3 pixelNormalDir = worldNormal;
	vec3 objectScale = vec3(1.0, 1.0, 1.0);

	vec2 screenUV = vec2(0.0);
	float meshZ = 0.0;

	MODELVIEW_MATRIX = ModelViewMatrix;
)";

static const char g_material_src_vertex_post[] = R"(

	vec4 worldPos = ModelMatrix * vec4(VERTEX, 1.0);
	v_WorldN_PX.w = worldPos.x;
	v_WorldB_PY.w = worldPos.y;
	v_WorldT_PZ.w = worldPos.z;
}
)";

static const char g_material_src_fragment_pre[] = R"(

void fragment()
{
	vec2 uv1 = UV;
	vec2 uv2 = UV2;
	vec4 vcolor = COLOR;

	vec3 worldPos = vec3(v_WorldN_PX.w, v_WorldB_PY.w, v_WorldT_PZ.w);
	vec3 worldNormal = v_WorldN_PX.xyz;
	vec3 worldTangent = v_WorldT_PZ.xyz;
	vec3 worldBinormal = v_WorldB_PY.xyz;
	vec3 pixelNormalDir = worldNormal;
	vec3 objectScale = vec3(1.0, 1.0, 1.0);

	vec2 screenUV = SCREEN_UV;
	float meshZ = FRAGCOORD.z;
)";

static const char g_material_src_fragment_post[] = R"(

	ALBEDO = baseColor;
	EMISSION = emissive;
	METALLIC = metallic;
	ROUGHNESS = roughness;
	AO = ambientOcclusion;
	ALPHA = opacity;

	if (opacityMask <= 0.0) discard;
	if (opacity <= 0.0) discard;
}
)";

static const char g_material_uniforms[] = R"(

varying mediump vec4 v_WorldN_PX;
varying mediump vec4 v_WorldB_PY;
varying mediump vec4 v_WorldT_PZ;

uniform mat4 ModelViewMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform vec4 ModelUV;
uniform vec4 ModelColor : hint_color;

uniform vec4 BuiltinData;
uniform vec4 CustomData1;
uniform vec4 CustomData2;
uniform vec4 ReconstructionParam1;
uniform vec4 ReconstructionParam2;

)";

struct ShaderData
{
	std::string Code;
};

class ShaderGenerator
{
	bool useUniformBlock_ = false;
	bool useSet_ = false;
	int32_t textureBindingOffset_ = 0;

	std::string Replace(std::string target, std::string from_, std::string to_)
	{
		std::string::size_type Pos(target.find(from_));

		while (Pos != std::string::npos)
		{
			target.replace(Pos, from_.length(), to_);
			Pos = target.find(from_, Pos + to_.length());
		}

		return target;
	}

	std::string GetType(int32_t i)
	{
		if (i == 1)
			return "float";
		if (i == 2)
			return "vec2";
		if (i == 3)
			return "vec3";
		if (i == 4)
			return "vec4";
		if (i == 16)
			return "mat4";
		assert(0);
		return "";
	}

	std::string GetElement(int32_t i)
	{
		if (i == 1)
			return ".x";
		if (i == 2)
			return ".xy";
		if (i == 3)
			return ".xyz";
		if (i == 4)
			return ".xyzw";
		assert(0);
		return "";
	}

	void ExportUniform(std::ostringstream& maincode, int32_t type, const char* name)
	{
		maincode << "uniform " << GetType(type) << " " << name << ";" << std::endl;
	}

	void ExportTexture(std::ostringstream& maincode, const char* name)
	{
		maincode << "uniform sampler2D " << name << ";" << std::endl;
	}

	void ExportHeader(std::ostringstream& maincode, Effekseer::MaterialFile* materialFile, int stage, bool isSprite, bool isOutputDefined, bool is450)
	{
	}

	void ExportDefaultUniform(std::ostringstream& maincode, Effekseer::MaterialFile* materialFile, int stage, bool isSprite)
	{
		maincode << g_material_uniforms;
	}

	void ExportMain(std::ostringstream& maincode,
					Effekseer::MaterialFile* materialFile,
					bool isSprite,
					Effekseer::MaterialShaderType shaderType,
					const std::string& baseCode,
					bool useUniformBlock)
	{
		const int32_t customData1Count = materialFile->GetCustomData1Count();
		const int32_t customData2Count = materialFile->GetCustomData2Count();
		
		{
			// Vertex shader
			maincode << g_material_src_vertex_pre;

			if (customData1Count > 0)
			{
				maincode << "\t" << GetType(customData1Count) + " customData1 = CustomData1" + GetElement(customData1Count) + ";\n";
			}

			if (customData2Count > 0)
			{
				maincode << "\t" << GetType(customData2Count) + " customData2 = CustomData2" + GetElement(customData2Count) + ";\n";
			}

			maincode << baseCode;

			maincode << g_material_src_vertex_post;
		}
		{
			// Fragment shader
			maincode << g_material_src_fragment_pre;

			if (customData1Count > 0)
			{
				maincode << "\t" << GetType(customData1Count) + " customData1 = CustomData1" + GetElement(customData1Count) + ";\n";
			}

			if (customData2Count > 0)
			{
				maincode << "\t" << GetType(customData2Count) + " customData2 = CustomData2" + GetElement(customData2Count) + ";\n";
			}

			maincode << baseCode;

			maincode << g_material_src_fragment_post;
		}
	}

public:
	ShaderData GenerateShader(Effekseer::MaterialFile* materialFile,
							  Effekseer::MaterialShaderType shaderType,
							  int32_t maximumTextureCount,
							  bool useUniformBlock,
							  bool isOutputDefined,
							  bool is450,
							  bool useSet,
							  int textureBindingOffset,
							  bool isYInverted,
							  int instanceCount)
	{
		useUniformBlock_ = useUniformBlock;
		useSet_ = useSet;
		textureBindingOffset_ = textureBindingOffset;

		bool isSprite = shaderType == MaterialShaderType::Standard || shaderType == MaterialShaderType::Refraction;
		bool isRefrection = materialFile->GetHasRefraction() &&
							(shaderType == MaterialShaderType::Refraction || shaderType == MaterialShaderType::RefractionModel);

		ShaderData shaderData;

		std::ostringstream maincode;

		int32_t actualTextureCount = std::min(maximumTextureCount, materialFile->GetTextureCount());

		ExportHeader(maincode, materialFile, 1, isSprite, isOutputDefined, is450);

		ExportDefaultUniform(maincode, materialFile, 1, isSprite);

		for (int32_t i = 0; i < actualTextureCount; i++)
		{
			auto textureIndex = materialFile->GetTextureIndex(i);
			auto textureName = materialFile->GetTextureName(i);

			ExportTexture(maincode, textureName);
		}

		if (materialFile->GetShadingModel() == ::Effekseer::ShadingModelType::Lit)
		{
		}
		else if (materialFile->GetShadingModel() == ::Effekseer::ShadingModelType::Unlit)
		{
		}

		for (int32_t i = 0; i < materialFile->GetUniformCount(); i++)
		{
			auto uniformIndex = materialFile->GetUniformIndex(i);
			auto uniformName = materialFile->GetUniformName(i);

			ExportUniform(maincode, 4, uniformName);
		}


		auto baseCode = std::string(materialFile->GetGenericCode());
		baseCode = Replace(baseCode, "$F1$", "float");
		baseCode = Replace(baseCode, "$F2$", "vec2");
		baseCode = Replace(baseCode, "$F3$", "vec3");
		baseCode = Replace(baseCode, "$F4$", "vec4");
		baseCode = Replace(baseCode, "$TIME$", "BuiltinData.x");
		baseCode = Replace(baseCode, "$EFFECTSCALE$", "BuiltinData.y");
		baseCode = Replace(baseCode, "$UV$", "uv");
		baseCode = Replace(baseCode, "$MOD", "mod");


		// replace textures
		for (int32_t i = 0; i < actualTextureCount; i++)
		{
			auto textureIndex = materialFile->GetTextureIndex(i);
			auto textureName = std::string(materialFile->GetTextureName(i));

			std::string keyP = "$TEX_P" + std::to_string(textureIndex) + "$";
			std::string keyS = "$TEX_S" + std::to_string(textureIndex) + "$";

			baseCode = Replace(baseCode, keyP, "texture(" + textureName + ",");
			baseCode = Replace(baseCode, keyS, ")");
		}

		// invalid texture
		for (int32_t i = actualTextureCount; i < materialFile->GetTextureCount(); i++)
		{
			auto textureIndex = materialFile->GetTextureIndex(i);
			auto textureName = std::string(materialFile->GetTextureName(i));

			std::string keyP = "$TEX_P" + std::to_string(textureIndex) + "$";
			std::string keyS = "$TEX_S" + std::to_string(textureIndex) + "$";

			baseCode = Replace(baseCode, keyP, "vec4(");
			baseCode = Replace(baseCode, keyS, ",0.0,1.0)");
		}

		ExportMain(maincode, materialFile, isSprite, shaderType, baseCode, useUniformBlock);

		shaderData.Code = maincode.str();

		return shaderData;
	}
};

} // namespace GodotShader

} // namespace Effekseer
