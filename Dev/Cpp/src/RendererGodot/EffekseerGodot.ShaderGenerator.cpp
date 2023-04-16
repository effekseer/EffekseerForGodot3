#include <iostream>
#include <iomanip>
#include "EffekseerGodot.ShaderGenerator.h"

namespace EffekseerGodot
{

static const char* g_material_src_common_atan2 = R"(
float atan2(float y, float x) {
	return x == 0.0 ? sign(y)* 3.141592 / 2.0 : atan(y, x);
}
)";

static const char* g_material_src_common_calcdepthfade = R"(
float CalcDepthFade(sampler2D depthTex, vec2 screenUV, float meshZ, float softParticleParam)
{
	float backgroundZ = texture(depthTex, screenUV).x;

	float distance = softParticleParam * PredefinedData.y;
	vec2 rescale = ReconstructionParam1.xy;
	vec4 params = ReconstructionParam2;

	vec2 zs = vec2(backgroundZ * rescale.x + rescale.y, meshZ);

	vec2 depth = (zs * params.w - params.y) / (params.x - zs * params.z);
	float dir = sign(depth.x);
	depth *= dir;
	return min(max((depth.x - depth.y) / distance, 0.0), 1.0);
}
)";

static const char* g_material_src_common_calcdepthfade_caller = 
	"CalcDepthFade(screenUV, meshZ, temp_0)";

static const char* g_material_src_common_srgb_to_linear = R"(
vec3 SRGBToLinear(vec3 c) {
	return c * (c * (c * 0.305306011 + 0.682171111) + 0.012522878);
}
)";

static const char* g_material_src_common_gradient = R"(

struct Gradient
{
	int colorCount;
	int alphaCount;
	vec4 colors[8];
	vec2 alphas[8];
};

vec4 SampleGradient(Gradient gradient, float t)
{
	vec3 color = gradient.colors[0].xyz;
	for(int i = 1; i < 8; i++)
	{
		float a = clamp((t - gradient.colors[i-1].w) / (gradient.colors[i].w - gradient.colors[i-1].w), 0.0, 1.0) * step(float(i), float(gradient.colorCount-1));
		color = mix(color, gradient.colors[i].xyz, a);
	}

	float alpha = gradient.alphas[0].x;
	for(int i = 1; i < 8; i++)
	{
		float a = clamp((t - gradient.alphas[i-1].y) / (gradient.alphas[i].y - gradient.alphas[i-1].y), 0.0, 1.0) * step(float(i), float(gradient.alphaCount-1));
		alpha = mix(alpha, gradient.alphas[i].x, a);
	}

	return vec4(color, alpha);
}

Gradient GradientParameter(vec4 param_v, vec4 param_c1, vec4 param_c2, vec4 param_c3, vec4 param_c4, vec4 param_c5, vec4 param_c6, vec4 param_c7, vec4 param_c8, vec4 param_a1, vec4 param_a2, vec4 param_a3, vec4 param_a4)
{
	Gradient g;
	g.colorCount = int(param_v.x);
	g.alphaCount = int(param_v.y);
	g.colors[0] = param_c1;
	g.colors[1] = param_c2;
	g.colors[2] = param_c3;
	g.colors[3] = param_c4;
	g.colors[4] = param_c5;
	g.colors[5] = param_c6;
	g.colors[6] = param_c7;
	g.colors[7] = param_c8;
	g.alphas[0].xy = param_a1.xy;
	g.alphas[1].xy = param_a1.zw;
	g.alphas[2].xy = param_a2.xy;
	g.alphas[3].xy = param_a2.zw;
	g.alphas[4].xy = param_a3.xy;
	g.alphas[5].xy = param_a3.zw;
	g.alphas[6].xy = param_a4.xy;
	g.alphas[7].xy = param_a4.zw;
	return g;
}

)";

inline std::string GetFixedGradient(const char* name, const Effekseer::Gradient& gradient)
{
	std::stringstream ss;

	ss << "Gradient " << name << "() {" << std::endl;
	ss << "Gradient g;" << std::endl;
	ss << "g.colorCount = " << gradient.ColorCount << ";" << std::endl;
	ss << "g.alphaCount = " << gradient.AlphaCount << ";" << std::endl;

	ss << std::fixed << std::setprecision(7);

	// glsl must fill all variables in some environments
	for (int32_t i = 0; i < gradient.Colors.size(); i++)
	{
		ss << "g.colors[" << i << "] = vec4("
			<< gradient.Colors[i].Color[0] * gradient.Colors[i].Intensity << ", "
			<< gradient.Colors[i].Color[1] * gradient.Colors[i].Intensity << ", "
			<< gradient.Colors[i].Color[2] * gradient.Colors[i].Intensity << ", "
			<< gradient.Colors[i].Position << ");" << std::endl;
	}

	for (int32_t i = 0; i < gradient.Alphas.size(); i++)
	{
		ss << "g.alphas[" << i << "] = vec2(" 
			<< gradient.Alphas[i].Alpha << ", "
			<< gradient.Alphas[i].Position << ");" << std::endl;
	}

	ss << "return g; }" << std::endl;

	return ss.str();
}

static const char* g_material_src_common_noise = R"(
float Rand2(vec2 n) { 
	return fract(sin(dot(n, vec2(12.9898, 78.233))) * 43758.5453123);
}

float SimpleNoise_Block(vec2 p) {
	ivec2 i = ivec2(floor(p));
	vec2 f = fract(p);
	f = f * f * (3.0 - 2.0 * f);
	
	float x0 = mix(Rand2(vec2(i+ivec2(0,0))), Rand2(vec2(i+ivec2(1,0))), f.x);
	float x1 = mix(Rand2(vec2(i+ivec2(0,1))), Rand2(vec2(i+ivec2(1,1))), f.x);
	return mix(x0, x1, f.y);
}

float SimpleNoise(vec2 uv, float scale) {
	const int loop = 3;
    float ret = 0.0;
	for(int i = 0; i < loop; i++) {
	    float freq = pow(2.0, float(i));
		float intensity = pow(0.5, float(loop-i));
	    ret += SimpleNoise_Block(uv * scale / freq) * intensity;
	}

	return ret;
}
)";

static const char* g_material_src_common_light = R"(
vec3 GetLightDirection() {
	return vec3(0.0);
}
vec3 GetLightColor() {
	return vec3(0.0);
}
vec3 GetLightAmbientColor() {
	return vec3(0.0);
}
)";

static const char g_material_src_spatial_vertex_sprite_pre[] =  R"(
void vertex()
{
	vec3 worldNormal = NORMAL;
	vec3 worldTangent = TANGENT;
	vec3 worldBinormal = BINORMAL;
	vec3 worldPos = VERTEX;
)";

static const char g_material_src_spatial_vertex_model_pre[] =  R"(
void vertex()
{
	UV = (UV.xy * ModelUV.zw) + ModelUV.xy;
	COLOR = COLOR * ModelColor;

	mat3 normalMatrix = mat3(ModelMatrix);
	vec3 worldNormal = normalize(normalMatrix * NORMAL);
	vec3 worldTangent = normalize(normalMatrix * TANGENT);
	vec3 worldBinormal = normalize(normalMatrix * BINORMAL);
	vec3 worldPos = (ModelMatrix * vec4(VERTEX, 1.0)).xyz;
)";

static const char g_material_src_spatial_vertex_common[] =  R"(
	vec2 uv1 = UV;
	vec2 uv2 = UV2;
	vec4 vcolor = COLOR;

	v_WorldN_PX.xyz = worldNormal;
	v_WorldB_PY.xyz = worldBinormal;
	v_WorldT_PZ.xyz = worldTangent;
	vec3 pixelNormalDir = worldNormal;
	vec3 objectScale = vec3(1.0, 1.0, 1.0);

	vec2 screenUV = vec2(0.0);
	float meshZ = 0.0;
)";

static const char g_material_src_spatial_vertex_sprite_post[] = R"(
	worldPos += worldPositionOffset;
	v_WorldN_PX.w = worldPos.x;
	v_WorldB_PY.w = worldPos.y;
	v_WorldT_PZ.w = worldPos.z;
	POSITION = PROJECTION_MATRIX * ViewMatrix * vec4(worldPos, 1.0);
}
)";

static const char g_material_src_spatial_vertex_model_post[] = R"(
	worldPos += worldPositionOffset;
	v_WorldN_PX.w = worldPos.x;
	v_WorldB_PY.w = worldPos.y;
	v_WorldT_PZ.w = worldPos.z;
	POSITION = PROJECTION_MATRIX * ViewMatrix * vec4(worldPos, 1.0);
}
)";

static const char g_material_src_canvasitem_vertex_sprite_pre[] =  R"(
void vertex()
{
	vec4 uvTangent = texture(UVTangentTexture, UV);
	vec3 worldNormal = vec3(0.0, 0.0, 1.0);
	vec3 worldTangent = vec3(uvTangent.zw, 0.0);
	vec3 worldBinormal = cross(worldNormal, worldTangent);
	vec3 worldPos = vec3(VERTEX, 0.0);
	vec2 uv1 = uvTangent.xy;
	vec2 uv2 = vec2(0.0);
	vec4 vcolor = COLOR;
)";

static const char g_material_src_canvasitem_vertex_model_pre[] =  R"(
void vertex()
{
	UV = (UV * ModelUV.zw) + ModelUV.xy;
	COLOR = COLOR * ModelColor;

	vec3 worldNormal = vec3(0.0, 0.0, 1.0);
	vec3 worldTangent = vec3(1.0, 0.0, 0.0);
	vec3 worldBinormal = vec3(0.0, 1.0, 0.0);
	vec3 worldPos = vec3(VERTEX, 0.0);
	vec2 uv1 = UV;
	vec2 uv2 = vec2(0.0);
	vec4 vcolor = COLOR;
)";

static const char g_material_src_canvasitem_vertex_common[] =  R"(
	v_WorldN_PX.xyz = worldNormal;
	v_WorldB_PY.xyz = worldBinormal;
	v_WorldT_PZ.xyz = worldTangent;
	vec3 pixelNormalDir = worldNormal;
	vec3 objectScale = vec3(1.0, 1.0, 1.0);

	vec2 screenUV = vec2(0.0);
	float meshZ = 0.0;
)";

static const char g_material_src_canvasitem_vertex_sprite_post[] = R"(
	worldPos += worldPositionOffset;
	v_WorldN_PX.w = worldPos.x;
	v_WorldB_PY.w = worldPos.y;
	v_WorldT_PZ.w = worldPos.z;
	VERTEX = worldPos.xy;
	UV = uvTangent.xy;
}
)";

static const char g_material_src_canvasitem_vertex_model_post[] = R"(
	worldPos += worldPositionOffset;
	v_WorldN_PX.w = worldPos.x;
	v_WorldB_PY.w = worldPos.y;
	v_WorldT_PZ.w = worldPos.z;
	VERTEX = worldPos.xy;
}
)";

static const char g_material_src_spatial_fragment_pre[] = R"(
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

static const char g_material_src_spatial_fragment_lit_post[] = R"(
	ALBEDO = SRGBToLinear(baseColor);
	EMISSION = SRGBToLinear(emissive);
	METALLIC = metallic;
	ROUGHNESS = roughness;
	AO = ambientOcclusion;
	ALPHA = clamp(opacity, 0.0, 1.0);
	
	if (opacityMask <= 0.0) discard;
	if (opacity <= 0.0) discard;
}
)";

static const char g_material_src_spatial_fragment_unlit_post[] = R"(
	ALBEDO = SRGBToLinear(emissive);
	ALPHA = clamp(opacity, 0.0, 1.0);
	
	if (opacityMask <= 0.0) discard;
	if (opacity <= 0.0) discard;
}
)";

static const char g_material_src_canvasitem_fragment_pre[] = R"(
void fragment()
{
	vec2 uv1 = UV;
	vec2 uv2 = vec2(0.0);
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

static const char g_material_src_canvasitem_fragment_lit_post[] = R"(
	COLOR = vec4(SRGBToLinear(baseColor + emissive), clamp(opacity, 0.0, 1.0));

	if (opacityMask <= 0.0) discard;
}
)";

static const char g_material_src_canvasitem_fragment_unlit_post[] = R"(
	COLOR = vec4(emissive, clamp(opacity, 0.0, 1.0));
	
	if (opacityMask <= 0.0) discard;
}
)";

static const char g_material_varying_common[] = R"(
varying mediump vec4 v_WorldN_PX;
varying mediump vec4 v_WorldB_PY;
varying mediump vec4 v_WorldT_PZ;
)";

static const char g_material_uniforms_common[] = R"(
uniform vec4 PredefinedData;
uniform vec3 CameraPosition;
uniform vec4 ReconstructionParam1;
uniform vec4 ReconstructionParam2;
)";

static const char g_material_uniforms_sprite[] = R"(
uniform mat4 ViewMatrix;
)";

static const char g_material_uniforms_model[] = R"(
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform vec4 ModelUV;
uniform vec4 ModelColor;
)";

static inline constexpr size_t GradientElements = 13;

static void Replace(std::string& target, const std::string& from, const std::string& to)
{
	auto pos = target.find(from);

	while (pos != std::string::npos)
	{
		target.replace(pos, from.length(), to);
		pos = target.find(from, pos + to.length());
	}
}

static bool Contains(const std::string& target, const std::string& str)
{
	return target.find(str) != std::string::npos;
}

static const char* GetType(int32_t i)
{
	if (i == 1) return "float";
	if (i == 2) return "vec2";
	if (i == 3) return "vec3";
	if (i == 4) return "vec4";
	if (i == 16) return "mat4";
	assert(0);
	return "";
}

static const char* GetElement(int32_t i)
{
	if (i == 1) return ".x";
	if (i == 2) return ".xy";
	if (i == 3) return ".xyz";
	if (i == 4) return ".xyzw";
	assert(0);
	return "";
}

std::string ShaderGenerator::GenerateShaderCode(const Effekseer::MaterialFile& materialFile, bool isSprite, bool isRefrection, bool isSpatial)
{
	const int32_t actualTextureCount = std::min(Effekseer::UserTextureSlotMax, materialFile.GetTextureCount());
	const int32_t customData1Count = materialFile.GetCustomData1Count();
	const int32_t customData2Count = materialFile.GetCustomData2Count();

	std::ostringstream maincode;

	if (materialFile.GetShadingModel() == Effekseer::ShadingModelType::Unlit)
	{
		maincode << "render_mode unshaded;\n";
	}

	// Output builtin varyings
	maincode << g_material_varying_common;

	if (customData1Count > 0)
	{
		maincode << "varying " << GetType(customData1Count) << " v_CustomData1;\n";
	}

	if (customData2Count > 0)
	{
		maincode << "varying " << GetType(customData2Count) << " v_CustomData2;\n";
	}

	// Output builtin uniforms
	{
		maincode << g_material_uniforms_common;

		if (isSprite)
		{
			if (materialFile.GetCustomData1Count() > 0)
			{
				maincode << "uniform sampler2D CustomData1;\n";
			}
			if (materialFile.GetCustomData2Count() > 0)
			{
				maincode << "uniform sampler2D CustomData2;\n";
			}
			maincode << g_material_uniforms_sprite;
		}
		else
		{
			if (materialFile.GetCustomData1Count() > 0)
			{
				maincode << "uniform " << GetType(materialFile.GetCustomData1Count()) << " CustomData1;\n";
			}
			if (materialFile.GetCustomData2Count() > 0)
			{
				maincode << "uniform " << GetType(materialFile.GetCustomData2Count()) << " CustomData2;\n";
			}
			maincode << g_material_uniforms_model;
		}
		if (!isSpatial)
		{
			maincode << "uniform sampler2D UVTangentTexture;\n";
		}
	}

	// Output user uniforms
	for (int32_t i = 0; i < materialFile.GetUniformCount(); i++)
	{
		auto uniformName = materialFile.GetUniformName(i);
		maincode << "uniform " << GetType(4) << " " << uniformName << ";" << std::endl;
	}

	for (size_t i = 0; i < materialFile.Gradients.size(); i++)
	{
		for (size_t j = 0; j < GradientElements; j++)
		{
			maincode << "uniform " << GetType(4) << " " << materialFile.Gradients[i].Name << "_" << j << ";" << std::endl;
		}
	}

	// Output user textures
	for (int32_t i = 0; i < actualTextureCount; i++)
	{
		auto textureName = materialFile.GetTextureName(i);
		auto textureHint = ": hint_black";
		maincode << "uniform sampler2D " << textureName << textureHint << ";" << std::endl;
	}

	// Output builtin functions
	maincode << g_material_src_common_srgb_to_linear;

	auto isRequired = [&materialFile](Effekseer::MaterialFile::RequiredPredefinedMethodType type){
		return std::find(materialFile.RequiredMethods.begin(), materialFile.RequiredMethods.end(), type) != materialFile.RequiredMethods.end();
	};

	if (isRequired(Effekseer::MaterialFile::RequiredPredefinedMethodType::Noise))
	{
		maincode << g_material_src_common_noise;
	}

	if (isRequired(Effekseer::MaterialFile::RequiredPredefinedMethodType::Light))
	{
		maincode << g_material_src_common_light;
	}

	if (isRequired(Effekseer::MaterialFile::RequiredPredefinedMethodType::Gradient))
	{
		maincode << g_material_src_common_gradient;
	}

	for (const auto& gradient : materialFile.FixedGradients)
	{
		maincode << GetFixedGradient(gradient.Name.c_str(), gradient.Data);
	}

	// Output user code
	auto baseCode = std::string(materialFile.GetGenericCode());
	Replace(baseCode, "$F1$", "float");
	Replace(baseCode, "$F2$", "vec2");
	Replace(baseCode, "$F3$", "vec3");
	Replace(baseCode, "$F4$", "vec4");
	Replace(baseCode, "$TIME$", "PredefinedData.x");
	Replace(baseCode, "$EFFECTSCALE$", "PredefinedData.y");
	Replace(baseCode, "$LOCALTIME$", "PredefinedData.w");
	Replace(baseCode, "$UV$", "uv");
	Replace(baseCode, "MOD", "mod");
	Replace(baseCode, "FRAC", "fract");
	Replace(baseCode, "LERP", "mix");
	Replace(baseCode, "cameraPosition", "CameraPosition");
	if (Contains(baseCode, "atan2("))
		maincode << g_material_src_common_atan2;
	if (Contains(baseCode, "CalcDepthFade("))
		maincode << g_material_src_common_calcdepthfade;

	// replace textures
	for (int32_t i = 0; i < actualTextureCount; i++)
	{
		auto textureIndex = materialFile.GetTextureIndex(i);
		auto textureName = std::string(materialFile.GetTextureName(i));

		std::string keyP = "$TEX_P" + std::to_string(textureIndex) + "$";
		std::string keyS = "$TEX_S" + std::to_string(textureIndex) + "$";

		Replace(baseCode, keyP, "texture(" + textureName + ",");
		Replace(baseCode, keyS, ")");
	}

	// invalid texture
	for (int32_t i = actualTextureCount; i < materialFile.GetTextureCount(); i++)
	{
		auto textureIndex = materialFile.GetTextureIndex(i);
		auto textureName = std::string(materialFile.GetTextureName(i));

		std::string keyP = "$TEX_P" + std::to_string(textureIndex) + "$";
		std::string keyS = "$TEX_S" + std::to_string(textureIndex) + "$";

		Replace(baseCode, keyP, "vec4(");
		Replace(baseCode, keyS, ",0.0,1.0)");
	}

	if (isSpatial)
	{
		// Vertex shader (Spatial)
		if (isSprite)
		{
			maincode << g_material_src_spatial_vertex_sprite_pre;
		}
		else
		{
			maincode << g_material_src_spatial_vertex_model_pre;
		}
		
		maincode << g_material_src_spatial_vertex_common;

		if (isSprite)
		{
			if (customData1Count > 0) maincode << "\t" << GetType(customData1Count) << " customData1 = texture(CustomData1, UV2)" << GetElement(customData1Count) << ";\n";
			if (customData2Count > 0) maincode << "\t" << GetType(customData2Count) << " customData2 = texture(CustomData2, UV2)" << GetElement(customData2Count) << ";\n";
		}
		else
		{
			if (customData1Count > 0) maincode << "\t" << GetType(customData1Count) << " customData1 = CustomData1" << GetElement(customData1Count) << ";\n";
			if (customData2Count > 0) maincode << "\t" << GetType(customData2Count) << " customData2 = CustomData2" << GetElement(customData2Count) << ";\n";
		}

		std::string vertCode = baseCode;

		Replace(vertCode, g_material_src_common_calcdepthfade_caller, "1.0");

		maincode << vertCode;

		if (customData1Count > 0) maincode << "\t" << "v_CustomData1 = customData1;\n";
		if (customData2Count > 0) maincode << "\t" << "v_CustomData2 = customData2;\n";

		if (isSprite)
		{
			maincode << g_material_src_spatial_vertex_sprite_post;
		}
		else
		{
			maincode << g_material_src_spatial_vertex_model_post;
		}
	}
	else
	{
		// Vertex shader (CanvasItem)
		if (isSprite)
		{
			maincode << g_material_src_canvasitem_vertex_sprite_pre;
		}
		else
		{
			maincode << g_material_src_canvasitem_vertex_model_pre;
		}

		maincode << g_material_src_canvasitem_vertex_common;

		if (isSprite)
		{
			if (customData1Count > 0) maincode << "\t" << GetType(customData1Count) << " customData1 = texture(CustomData1, UV)" << GetElement(customData1Count) << ";\n";
			if (customData2Count > 0) maincode << "\t" << GetType(customData2Count) << " customData2 = texture(CustomData2, UV)" << GetElement(customData2Count) << ";\n";
		}
		else
		{
			if (customData1Count > 0) maincode << "\t" << GetType(customData1Count) << " customData1 = CustomData1" << GetElement(customData1Count) << ";\n";
			if (customData2Count > 0) maincode << "\t" << GetType(customData2Count) << " customData2 = CustomData2" << GetElement(customData2Count) << ";\n";
		}

		std::string vertCode = baseCode;

		Replace(vertCode, g_material_src_common_calcdepthfade_caller, "1.0");

		maincode << vertCode;

		if (customData1Count > 0) maincode << "\t" << "v_CustomData1 = customData1;\n";
		if (customData2Count > 0) maincode << "\t" << "v_CustomData2 = customData2;\n";

		if (isSprite)
		{
			maincode << g_material_src_canvasitem_vertex_sprite_post;
		}
		else
		{
			maincode << g_material_src_canvasitem_vertex_model_post;
		}
	}

	if (isSpatial)
	{
		// Fragment shader (Spatial)
		maincode << g_material_src_spatial_fragment_pre;

		if (customData1Count > 0)
		{
			maincode << "\t" << GetType(customData1Count) << " customData1 = v_CustomData1;\n";
		}

		if (customData2Count > 0)
		{
			maincode << "\t" << GetType(customData2Count) << " customData2 = v_CustomData2;\n";
		}

		std::string fragCode = baseCode;
		
		Replace(fragCode, g_material_src_common_calcdepthfade_caller, 
			"CalcDepthFade(DEPTH_TEXTURE, screenUV, meshZ, temp_0)");
		
		maincode << fragCode;

		if (materialFile.GetShadingModel() == Effekseer::ShadingModelType::Lit)
		{
			maincode << g_material_src_spatial_fragment_lit_post;
		}
		else
		{
			maincode << g_material_src_spatial_fragment_unlit_post;
		}
	}
	else
	{
		// Fragment shader (CanvasItem)
		maincode << g_material_src_canvasitem_fragment_pre;

		if (customData1Count > 0)
		{
			maincode << "\t" << GetType(customData1Count) << " customData1 = v_CustomData1;\n";
		}

		if (customData2Count > 0)
		{
			maincode << "\t" << GetType(customData2Count) << " customData2 = v_CustomData2;\n";
		}

		std::string fragCode = baseCode;

		Replace(fragCode, g_material_src_common_calcdepthfade_caller, "1.0");

		maincode << fragCode;

		if (materialFile.GetShadingModel() == Effekseer::ShadingModelType::Lit)
		{
			maincode << g_material_src_canvasitem_fragment_lit_post;
		}
		else
		{
			maincode << g_material_src_canvasitem_fragment_unlit_post;
		}
	}

	std::string code = maincode.str();
	//puts(code.c_str());
	return code;
}

void ShaderGenerator::GenerateParamDecls(ShaderData& shaderData, const Effekseer::MaterialFile& materialFile, bool isSprite, bool isRefrection)
{
	auto appendDecls = [](std::vector<Shader::ParamDecl>& decls, const char* name, Shader::ParamType type, uint16_t slot, uint16_t offset)
	{
		Shader::ParamDecl decl = {};
		std::char_traits<char>::copy(decl.name, name, sizeof(decl.name));
		decl.type = type;
		decl.slot = slot;
		decl.offset = offset;
		decls.emplace_back(decl);
	};
	auto appendCustomDataDecls = [appendDecls](std::vector<Shader::ParamDecl>& decls, const ::Effekseer::MaterialFile& materialFile, 
		const EffekseerRenderer::MaterialShaderParameterGenerator& parameterGenerator, bool isSprite)
	{
		if (!isSprite)
		{
			if (materialFile.GetCustomData1Count() > 0)
			{
				Shader::ParamType type = (Shader::ParamType)((size_t)Shader::ParamType::Float + materialFile.GetCustomData1Count() - 1);
				appendDecls(decls, "CustomData1", type, 0, parameterGenerator.VertexModelCustomData1Offset);
			}
			if (materialFile.GetCustomData2Count() > 0)
			{
				Shader::ParamType type = (Shader::ParamType)((size_t)Shader::ParamType::Float + materialFile.GetCustomData2Count() - 1);
				appendDecls(decls, "CustomData2", type, 0, parameterGenerator.VertexModelCustomData2Offset);
			}
		}
	};
	auto appendUserUniformDecls = [appendDecls](std::vector<Shader::ParamDecl>& decls, const ::Effekseer::MaterialFile& materialFile, 
		const EffekseerRenderer::MaterialShaderParameterGenerator& parameterGenerator)
	{
		uint16_t offset = (uint16_t)parameterGenerator.VertexUserUniformOffset;
		for (int32_t i = 0; i < materialFile.GetUniformCount(); i++)
		{
			const char* uniformName = materialFile.GetUniformName(i);
			appendDecls(decls, uniformName, Shader::ParamType::Vector4, 0, offset);
			offset += 4 * sizeof(float);
		}
		for (size_t i = 0; i < materialFile.Gradients.size(); i++)
		{
			for (size_t j = 0; j < GradientElements; j++)
			{
				char uniformName[128];
				snprintf(uniformName, sizeof(uniformName), "%s_%u", materialFile.Gradients[i].Name.c_str(), static_cast<uint32_t>(j));
				appendDecls(decls, uniformName, Shader::ParamType::Vector4, 0, offset);
				offset += 4 * sizeof(float);
			}
		}
	};
	auto appendTextureDecls = [appendDecls](std::vector<Shader::ParamDecl>& decls, const ::Effekseer::MaterialFile& materialFile)
	{
		for (int32_t i = 0; i < materialFile.GetTextureCount(); i++)
		{
			uint16_t textureIndex = (uint16_t)materialFile.GetTextureIndex(i);
			const char* textureName = materialFile.GetTextureName(i);
			appendDecls(decls, textureName, Shader::ParamType::Texture, textureIndex, 0);
		}
	};

	auto parameterGenerator = EffekseerRenderer::MaterialShaderParameterGenerator(materialFile, !isSprite, 0, 1);

	if (isSprite)
	{
		std::vector<Shader::ParamDecl> decls;
		appendDecls(decls, "ViewMatrix",  Shader::ParamType::Matrix44, 0, parameterGenerator.VertexCameraMatrixOffset);
		appendDecls(decls, "PredefinedData", Shader::ParamType::Vector4, 1, parameterGenerator.PixelPredefinedOffset);
		appendDecls(decls, "CameraPosition", Shader::ParamType::Vector3, 1, parameterGenerator.PixelCameraPositionOffset);
		//appendCustomDataDecls(decls, materialFile, parameterGenerator, isSprite);
		appendUserUniformDecls(decls, materialFile, parameterGenerator);
		appendTextureDecls(decls, materialFile);

		shaderData.ParamDecls = std::move(decls);
	}
	else
	{
		std::vector<Shader::ParamDecl> decls;
		appendDecls(decls, "ViewMatrix",  Shader::ParamType::Matrix44, 0, parameterGenerator.VertexProjectionMatrixOffset);
		appendDecls(decls, "ModelMatrix", Shader::ParamType::Matrix44, 0, parameterGenerator.VertexModelMatrixOffset);
		appendDecls(decls, "ModelUV",     Shader::ParamType::Vector4,  0, parameterGenerator.VertexModelUVOffset);
		appendDecls(decls, "ModelColor",  Shader::ParamType::Vector4,  0, parameterGenerator.VertexModelColorOffset);
		appendDecls(decls, "PredefinedData", Shader::ParamType::Vector4, 1, parameterGenerator.PixelPredefinedOffset);
		appendDecls(decls, "CameraPosition", Shader::ParamType::Vector3, 1, parameterGenerator.PixelCameraPositionOffset);
		appendCustomDataDecls(decls, materialFile, parameterGenerator, isSprite);
		appendUserUniformDecls(decls, materialFile, parameterGenerator);
		appendTextureDecls(decls, materialFile);

		shaderData.ParamDecls = std::move(decls);
	}
	
	shaderData.VertexConstantBufferSize = parameterGenerator.VertexShaderUniformBufferSize;
	shaderData.PixelConstantBufferSize = parameterGenerator.PixelShaderUniformBufferSize;
}

ShaderData ShaderGenerator::GenerateShaderData(
	const Effekseer::MaterialFile& materialFile, Effekseer::MaterialShaderType shaderType)
{
	const bool isSprite = shaderType == Effekseer::MaterialShaderType::Standard || shaderType == Effekseer::MaterialShaderType::Refraction;
	const bool isRefrection = materialFile.GetHasRefraction() &&
		(shaderType == Effekseer::MaterialShaderType::Refraction || shaderType == Effekseer::MaterialShaderType::RefractionModel);
	
	ShaderData shaderData;
	shaderData.CodeSpatial = GenerateShaderCode(materialFile, isSprite, isRefrection, true);
	shaderData.CodeCanvasItem = GenerateShaderCode(materialFile, isSprite, isRefrection, false);
	GenerateParamDecls(shaderData, materialFile, isSprite, isRefrection);
	return shaderData;
}

std::array<ShaderData, ShaderGenerator::ShaderMax> ShaderGenerator::Generate(const Effekseer::MaterialFile& materialFile)
{
	std::array<ShaderData, ShaderMax> list;

	using namespace Effekseer;

	list[(size_t)MaterialShaderType::Standard] = GenerateShaderData(materialFile, MaterialShaderType::Standard);
	list[(size_t)MaterialShaderType::Model] = GenerateShaderData(materialFile, MaterialShaderType::Model);

	//if (materialFile.GetHasRefraction())
	//{
	//	list[(size_t)MaterialShaderType::Refraction] = GenerateShaderData(MaterialShaderType::Refraction);
	//	list[(size_t)MaterialShaderType::RefractionModel] = GenerateShaderData(MaterialShaderType::RefractionModel);
	//}

	return list;
}

} // namespace Effekseer
