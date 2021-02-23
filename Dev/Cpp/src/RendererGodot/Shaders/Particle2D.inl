
const char code[] = 
#if !LIGHTING
R"(
render_mode unshaded;
)"
#endif

R"(
render_mode skip_vertex_transform;
uniform mat4 EffectMatrix;
)"

#if DISTORTION || LIGHTING
R"(
varying vec4 v_UVTangent;
uniform sampler2D UVTangentTexture : hint_normal;
)"
#endif

#if DISTORTION
R"(
uniform float DistortionIntensity;
uniform sampler2D DistortionTexture : hint_normal;
)"
#elif LIGHTING
R"(
uniform sampler2D ColorTexture : hint_albedo;
uniform sampler2D NormalTexture : hint_normal;
)"
#else
R"(
uniform sampler2D ColorTexture : hint_albedo;
)"
#endif

#include "Common2D.inl"

R"(
void vertex() {
	VERTEX = (WORLD_MATRIX * (EffectMatrix * vec4(VERTEX, 0.0, 1.0))).xy;
)"
#if DISTORTION || LIGHTING
R"(
	v_UVTangent = texture(UVTangentTexture, UV);
)"
#endif
R"(
}
)"

R"(
void fragment() {
)"
#if DISTORTION
R"(
	vec2 distortionUV = DistortionMap(DistortionTexture, v_UVTangent.xy, DistortionIntensity, COLOR.xy, v_UVTangent.zw);
	vec4 color = ColorMap(SCREEN_TEXTURE, SCREEN_UV + distortionUV, vec4(1.0, 1.0, 1.0, COLOR.a));
	COLOR = color;
)"
#elif LIGHTING
R"(
	NORMAL = NormalMap(NormalTexture, v_UVTangent.xy, v_UVTangent.zw);
	vec4 color = ColorMap(ColorTexture, v_UVTangent.xy, COLOR);
	COLOR = color;
)"
#else
R"(
	vec4 color = ColorMap(ColorTexture, UV, COLOR);
	COLOR = color;
)"
#endif
R"(
}
)";

const Shader::ParamDecl decl[] = {
	{ "EffectMatrix", Shader::ParamType::Matrix44, 0, 0 },
#if DISTORTION
	{ "DistortionIntensity", Shader::ParamType::Float, 1, 48 },
	{ "DistortionTexture", Shader::ParamType::Texture, 0, 0 },
	{ "UVTangentTexture", Shader::ParamType::Texture, 1, 0 },
#elif LIGHTING
	{ "ColorTexture",  Shader::ParamType::Texture, 0, 0 },
	{ "NormalTexture", Shader::ParamType::Texture, 1, 0 },
#else
	{ "ColorTexture",  Shader::ParamType::Texture, 0, 0 },
#endif
};
