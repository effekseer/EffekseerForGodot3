
const char code[] = 
#if !LIGHTING
R"(
render_mode unshaded;
)"
#endif

R"(
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform vec4 ModelUV;
uniform vec4 ModelColor : hint_color;
)"

#if DISTORTION

R"(
uniform float DistortionIntensity;
uniform sampler2D DistortionTexture : hint_normal;
)"
#elif LIGHTING
R"(
uniform float EmissiveScale;
uniform sampler2D ColorTexture : hint_albedo;
uniform sampler2D NormalTexture : hint_normal;
)"
#else
R"(
uniform float EmissiveScale;
uniform sampler2D ColorTexture : hint_albedo;
)"
#endif

#include "Common2D.inl"

R"(
void vertex() {
    UV = (UV.xy * ModelUV.zw) + ModelUV.xy;
	COLOR = COLOR * ModelColor;
}
)"

R"(
void fragment() {
)"
#if DISTORTION
R"(
	vec2 distortionUV = DistortionMap(DistortionTexture, UV, DistortionIntensity, COLOR.xy, vec2(1.0, 0.0));
	COLOR = ColorMap(SCREEN_TEXTURE, SCREEN_UV + distortionUV, vec4(1.0, 1.0, 1.0, COLOR.a));
)"
#elif LIGHTING
R"(
	NORMALMAP = NormalMap(NormalTexture, UV, vec2(1.0, 0.0));
	COLOR = ColorMap(ColorTexture, UV, COLOR);
	COLOR.rgb *= EmissiveScale;
)"
#else
R"(
	COLOR = ColorMap(ColorTexture, UV, COLOR);
	COLOR.rgb *= EmissiveScale;
)"
#endif
R"(
}
)";

const Shader::ParamDecl decl[] = {
	//{ "ViewMatrix",  Shader::ParamType::Matrix44, 0,   0 },
	//{ "ModelMatrix", Shader::ParamType::Matrix44, 0,  64 },
	{ "ModelUV",     Shader::ParamType::Vector4,  0, 128 },
	{ "ModelColor",  Shader::ParamType::Vector4,  0, 144 },

#if DISTORTION
	{ "DistortionIntensity", Shader::ParamType::Float, 1, 48 },
	{ "DistortionTexture", Shader::ParamType::Texture, 0, 0 },
#elif LIGHTING
	{ "EmissiveScale", Shader::ParamType::Float, 1, offsetof(EffekseerRenderer::PixelConstantBuffer, EmmisiveParam) },
	{ "ColorTexture",  Shader::ParamType::Texture, 0, 0 },
	{ "NormalTexture", Shader::ParamType::Texture, 1, 0 },
#else
	{ "EmissiveScale", Shader::ParamType::Float, 1, offsetof(EffekseerRenderer::PixelConstantBuffer, EmmisiveParam) },
	{ "ColorTexture",  Shader::ParamType::Texture, 0, 0 },
#endif
};