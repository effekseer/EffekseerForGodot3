
const char code[] = 
#if !LIGHTING
R"(
render_mode unshaded;
)"
#endif

R"(
uniform mat4 ModelViewMatrix;
)"

#if DISTORTION
R"(
uniform float DistortionIntensity;
uniform sampler2D Texture0 : hint_normal;
)"
#elif LIGHTING
R"(
uniform sampler2D Texture0 : hint_albedo;
uniform sampler2D Texture1 : hint_normal;
)"
#else
R"(
uniform sampler2D Texture0 : hint_albedo;
)"
#endif

#if SOFT_PARTICLE
R"(
uniform vec4 SoftParticleParams;
uniform vec4 SoftParticleReco;
)"
#endif

#include "CommonLibrary.inl"

R"(
void vertex() {
	MODELVIEW_MATRIX = ModelViewMatrix;
}
)"

R"(
void fragment() {
)"
#if DISTORTION
R"(
	vec2 distortionUV = DistortionMap(Texture0, UV, DistortionIntensity, COLOR.xy, TANGENT, BINORMAL);
	vec4 color = ColorMap(SCREEN_TEXTURE, SCREEN_UV + distortionUV, vec4(1.0, 1.0, 1.0, COLOR.a));
	ALBEDO = color.rgb; ALPHA = color.a;
)"
#elif LIGHTING
R"(
	NORMAL = NormalMap(Texture1, UV, NORMAL, TANGENT, BINORMAL);
	vec4 color = ColorMap(Texture0, UV, COLOR);
	ALBEDO = color.rgb; ALPHA = color.a;
)"
#else
R"(
	vec4 color = ColorMap(Texture0, UV, COLOR);
	ALBEDO = color.rgb; ALPHA = color.a;
)"
#endif

#if SOFT_PARTICLE
R"(
	vec4 reconstruct2 = vec4(PROJECTION_MATRIX[2][2], PROJECTION_MATRIX[3][2], PROJECTION_MATRIX[2][3], PROJECTION_MATRIX[3][3]);
	ALPHA *= SoftParticle(DEPTH_TEXTURE, SCREEN_UV, FRAGCOORD.z, SoftParticleParams, SoftParticleReco, reconstruct2);
)"
#endif

R"(
}
)";

const Shader::ParamDecl decl[] = {
	{ "ModelViewMatrix", Shader::ParamType::Matrix44, 0, 0 },
#if DISTORTION
	{ "DistortionIntensity", Shader::ParamType::Float, 1, 48 },
	#if SOFT_PARTICLE
		{ "SoftParticleParams", Shader::ParamType::Vector4, 1, offsetof(EffekseerRenderer::PixelConstantBufferDistortion, SoftParticleParam) + 0 },
		{ "SoftParticleReco",   Shader::ParamType::Vector4, 1, offsetof(EffekseerRenderer::PixelConstantBufferDistortion, SoftParticleParam) + 16 },
	#endif
#else
	#if SOFT_PARTICLE
		{ "SoftParticleParams", Shader::ParamType::Vector4, 1, offsetof(EffekseerRenderer::PixelConstantBuffer, SoftParticleParam) + 0 },
		{ "SoftParticleReco",   Shader::ParamType::Vector4, 1, offsetof(EffekseerRenderer::PixelConstantBuffer, SoftParticleParam) + 16 },
	#endif
#endif
};
