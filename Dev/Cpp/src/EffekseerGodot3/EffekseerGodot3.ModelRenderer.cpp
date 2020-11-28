
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerGodot3.RenderState.h"
#include "EffekseerGodot3.RendererImplemented.h"

#include "EffekseerGodot3.IndexBuffer.h"
#include "EffekseerGodot3.ModelRenderer.h"
#include "EffekseerGodot3.Shader.h"
#include "EffekseerGodot3.VertexBuffer.h"

namespace EffekseerGodot3
{
	
namespace ModelShaders
{

namespace Unlit
{
const char code[] = R"(
render_mode unshaded;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform vec4 ModelUV;
uniform vec4 ModelColor : hint_color;
uniform sampler2D Texture0 : hint_albedo;
void vertex() {
	MODELVIEW_MATRIX = ViewMatrix * ModelMatrix;
    UV.x = (UV.x * ModelUV.z) + ModelUV.x;
    UV.y = (UV.y * ModelUV.w) + ModelUV.y;
	COLOR = COLOR * ModelColor;
}
void fragment() {
	vec4 texel = texture(Texture0, UV);
	ALBEDO = texel.rgb * COLOR.rgb;
	ALPHA = texel.a * COLOR.a;
}
)";
const Shader::ParamDecl decl[] = {
	{ "ViewMatrix",  Shader::ParamType::Matrix44, 0,   0 },
	{ "ModelMatrix", Shader::ParamType::Matrix44, 0,  64 },
	{ "ModelUV",     Shader::ParamType::Vector4,  0, 128 },
	{ "ModelColor",  Shader::ParamType::Vector4,  0, 144 },
};
}

namespace Distortion
{
const char code[] = R"(
render_mode unshaded;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform vec4 ModelUV;
uniform vec4 ModelColor : hint_color;
uniform float DistortionIntensity;
uniform sampler2D Texture0 : hint_normal;
uniform sampler2D Texture1 : hint_albedo;
void vertex() {
	MODELVIEW_MATRIX = ViewMatrix * ModelMatrix;
    UV.x = (UV.x * ModelUV.z) + ModelUV.x;
    UV.y = (UV.y * ModelUV.w) + ModelUV.y;
	COLOR = COLOR * ModelColor;
}
void fragment() {
	vec4 texel = texture(Texture0, UV);
    vec2 posU = BINORMAL.xy;
    vec2 posR = TANGENT.xy;
    float xscale = (((texel.x * 2.0) - 1.0) * COLOR.x) * DistortionIntensity * 4.0;
    float yscale = (((texel.y * 2.0) - 1.0) * COLOR.y) * DistortionIntensity * 4.0;
    vec2 uv = posR * xscale + posU * yscale;
    ALBEDO = texture(SCREEN_TEXTURE, SCREEN_UV + uv).xyz;
	ALPHA = COLOR.a * texel.a;
}
)";
const Shader::ParamDecl decl[] = {
	{ "ViewMatrix",          Shader::ParamType::Matrix44, 0,   0 },
	{ "ModelMatrix",         Shader::ParamType::Matrix44, 0,  64 },
	{ "ModelUV",             Shader::ParamType::Vector4,  0, 128 },
	{ "ModelColor",          Shader::ParamType::Vector4,  0, 144 },
	{ "DistortionIntensity", Shader::ParamType::Float,    1,  48 },
};
}

namespace Lighting
{
const char code[] = R"(
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform vec4 ModelUV;
uniform vec4 ModelColor : hint_color;
uniform sampler2D Texture0 : hint_albedo;
void vertex() {
	MODELVIEW_MATRIX = ViewMatrix * ModelMatrix;
    UV.x = (UV.x * ModelUV.z) + ModelUV.x;
    UV.y = (UV.y * ModelUV.w) + ModelUV.y;
	COLOR = COLOR * ModelColor;
}
void fragment() {
	vec4 texel = texture(Texture0, UV);
	ALBEDO = texel.rgb * COLOR.rgb;
	ALPHA = texel.a * COLOR.a;
}
)";
const Shader::ParamDecl decl[] = {
	{ "ViewMatrix",  Shader::ParamType::Matrix44, 0,   0 },
	{ "ModelMatrix", Shader::ParamType::Matrix44, 0,  64 },
	{ "ModelUV",     Shader::ParamType::Vector4,  0, 128 },
	{ "ModelColor",  Shader::ParamType::Vector4,  0, 144 },
};
}

}

ModelRenderer::ModelRenderer(RendererImplemented* renderer)
	: m_renderer(renderer)
{
	#define COUNT_OF(list) (sizeof(list) / sizeof(list[0]))

	using namespace EffekseerGodot3::ModelShaders;
	using namespace EffekseerRenderer;

	m_shader_unlit = Shader::Create("ModelUnlit", Unlit::code, 
		RendererShaderType::Unlit, Unlit::decl, COUNT_OF(Unlit::decl));
	m_shader_unlit->SetVertexConstantBufferSize(sizeof(ModelRendererVertexConstantBuffer<40>));
	m_shader_unlit->SetPixelConstantBufferSize(sizeof(ModelRendererPixelConstantBuffer));

	m_shader_distortion = Shader::Create("ModelDistortion", Distortion::code, 
		RendererShaderType::BackDistortion, Distortion::decl, COUNT_OF(Distortion::decl));
	m_shader_distortion->SetVertexConstantBufferSize(sizeof(ModelRendererVertexConstantBuffer<40>));
	m_shader_distortion->SetPixelConstantBufferSize(sizeof(ModelRendererDistortionPixelConstantBuffer));

	m_shader_lit = Shader::Create("ModelLighting", Lighting::code, 
		RendererShaderType::Lit, Lighting::decl, COUNT_OF(Lighting::decl));
	m_shader_lit->SetVertexConstantBufferSize(sizeof(ModelRendererVertexConstantBuffer<40>));
	m_shader_lit->SetPixelConstantBufferSize(sizeof(ModelRendererPixelConstantBuffer));

	//m_shader_advanced_lit = Shader::Create("ModelAdvancedLighting", Unlit::code, 
	//	RendererShaderType::AdvancedLit, Unlit::decl, COUNT_OF(Unlit::decl));
	//m_shader_advanced_lit->SetVertexConstantBufferSize(sizeof(ModelRendererAdvancedVertexConstantBuffer<40>));
	//m_shader_advanced_lit->SetPixelConstantBufferSize(sizeof(ModelRendererAdvancedPixelConstantBuffer));
	//
	//m_shader_advanced_unlit = Shader::Create("ModelAdvancedUnlit", Unlit::code, 
	//	RendererShaderType::AdvancedUnlit, Unlit::decl, COUNT_OF(Unlit::decl));
	//m_shader_advanced_unlit->SetVertexConstantBufferSize(sizeof(ModelRendererAdvancedVertexConstantBuffer<40>));
	//m_shader_advanced_unlit->SetPixelConstantBufferSize(sizeof(ModelRendererAdvancedPixelConstantBuffer));
	//
	//m_shader_advanced_distortion = Shader::Create("ModelAdvancedDistortion", Unlit::code, 
	//	RendererShaderType::AdvancedBackDistortion, Unlit::decl, COUNT_OF(Unlit::decl));
	//m_shader_advanced_distortion->SetVertexConstantBufferSize(sizeof(ModelRendererAdvancedVertexConstantBuffer<40>));
	//m_shader_advanced_distortion->SetPixelConstantBufferSize(sizeof(ModelRendererDistortionPixelConstantBuffer));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRenderer::~ModelRenderer()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
ModelRendererRef ModelRenderer::Create(RendererImplemented* renderer)
{
	assert(renderer != NULL);

	return ModelRendererRef(new ModelRenderer(renderer));
}

void ModelRenderer::BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData)
{
	BeginRendering_(m_renderer, parameter, count, userData);
}

void ModelRenderer::Rendering(const efkModelNodeParam& parameter, const InstanceParameter& instanceParameter, void* userData)
{
	Rendering_<RendererImplemented>(m_renderer, parameter, instanceParameter, userData);
}

void ModelRenderer::EndRendering(const efkModelNodeParam& parameter, void* userData)
{
	if (parameter.ModelIndex < 0)
	{
		return;
	}

	Effekseer::Model* model = nullptr;

	if (parameter.IsProcedualMode)
	{
		model = parameter.EffectPointer->GetProcedualModel(parameter.ModelIndex);
	}
	else
	{
		model = parameter.EffectPointer->GetModel(parameter.ModelIndex);
	}

	if (model == nullptr)
	{
		return;
	}

	m_renderer->SetModel(model);

	EndRendering_<
		RendererImplemented,
		Shader,
		Effekseer::Model,
		false,
		1>(
		m_renderer,
		m_shader_advanced_lit.get(),
		m_shader_advanced_unlit.get(),
		m_shader_advanced_distortion.get(),
		m_shader_lit.get(),
		m_shader_unlit.get(),
		m_shader_distortion.get(),
		parameter);

	m_renderer->SetModel(nullptr);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerGodot3
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
