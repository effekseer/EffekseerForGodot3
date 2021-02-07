#include <VisualServer.hpp>
#include <Texture.hpp>
#include "EffekseerGodot.Shader.h"
#include "../Utils/EffekseerGodot.Utils.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerGodot
{

static const char* ShaderType = 
	"shader_type spatial;\n";

static const char* BlendMode[] = {
	"",
	"render_mode blend_mix;\n",
	"render_mode blend_add;\n",
	"render_mode blend_sub;\n",
	"render_mode blend_mul;\n",
};
static const char* CullMode[] = {
	"render_mode cull_back;\n",
	"render_mode cull_front;\n",
	"render_mode cull_disabled;\n",
};
static const char* DepthTestMode[] = {
	"render_mode depth_test_disable;\n",
	"",
};
static const char* DepthWriteMode[] = {
	"render_mode depth_draw_never;\n",
	"render_mode depth_draw_always;\n",
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
std::unique_ptr<Shader> Shader::Create(const char* name, const char* code, 
	EffekseerRenderer::RendererShaderType shaderType,
	std::vector<ParamDecl>&& paramDecls)
{
	return std::unique_ptr<Shader>(new Shader(name, code, shaderType, std::move(paramDecls)));
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Shader::Shader(const char* name, const char* code, 
	EffekseerRenderer::RendererShaderType shaderType,
	std::vector<ParamDecl>&& paramDecls)
{
	m_name = name;
	m_paramDecls = std::move(paramDecls);
	m_shaderType = shaderType;

	auto vs = godot::VisualServer::get_singleton();

	godot::String baseCode = code;

#define COUNT_OF(list) (sizeof(list) / sizeof(list[0]))
	for (size_t dwm = 0; dwm < COUNT_OF(DepthWriteMode); dwm++)
	{
		for (size_t dtm = 0; dtm < COUNT_OF(DepthTestMode); dtm++)
		{
			for (size_t cm = 0; cm < COUNT_OF(CullMode); cm++)
			{
				for (size_t bm = 0; bm < COUNT_OF(BlendMode); bm++)
				{
					godot::String fullCode = ShaderType;
					fullCode += DepthWriteMode[dwm];
					fullCode += DepthTestMode[dtm];
					fullCode += CullMode[cm];
					fullCode += BlendMode[bm];
					fullCode += baseCode;

					m_rid[dwm][dtm][cm][bm] = vs->shader_create();
					vs->shader_set_code(m_rid[dwm][dtm][cm][bm], fullCode);
				}
			}
		}
	}
#undef COUNT_OF
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Shader::~Shader()
{
	auto vs = godot::VisualServer::get_singleton();

#define COUNT_OF(list) (sizeof(list) / sizeof(list[0]))
	for (size_t dwm = 0; dwm < COUNT_OF(DepthWriteMode); dwm++)
	{
		for (size_t dtm = 0; dtm < COUNT_OF(DepthTestMode); dtm++)
		{
			for (size_t cm = 0; cm < COUNT_OF(CullMode); cm++)
			{
				for (size_t bm = 0; bm < COUNT_OF(BlendMode); bm++)
				{
					vs->free_rid(m_rid[dwm][dtm][cm][bm]);
				}
			}
		}
	}
#undef COUNT_OF
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void Shader::ApplyToMaterial(godot::RID material, EffekseerRenderer::RenderStateBase::State& state)
{
	auto vs = godot::VisualServer::get_singleton();

	const size_t bm = (size_t)state.AlphaBlend;
	const size_t cm = (size_t)state.CullingType;
	const size_t dtm = (size_t)state.DepthTest;
	const size_t dwm = (size_t)state.DepthWrite;

	vs->material_set_shader(material, m_rid[dwm][dtm][cm][bm]);

	for (size_t i = 0; i < m_paramDecls.size(); i++)
	{
		const auto& decl = m_paramDecls[i];

		if (decl.type == ParamType::Int)
		{
			auto value = *(const int32_t*)&m_constantBuffers[decl.slot][decl.offset];
			vs->material_set_param(material, decl.name, value);
		}
		else if (decl.type == ParamType::Float)
		{
			auto value = *(const float*)&m_constantBuffers[decl.slot][decl.offset];
			vs->material_set_param(material, decl.name, value);
		}
		else if (decl.type == ParamType::Vector2)
		{
			auto& vector = *(const godot::Vector2*)&m_constantBuffers[decl.slot][decl.offset];
			vs->material_set_param(material, decl.name, vector);
		}
		else if (decl.type == ParamType::Vector3)
		{
			auto& vector = *(const godot::Vector3*)&m_constantBuffers[decl.slot][decl.offset];
			vs->material_set_param(material, decl.name, vector);
		}
		else if (decl.type == ParamType::Vector4)
		{
			auto& vector = *(const godot::Quat*)&m_constantBuffers[decl.slot][decl.offset];
			//auto& vector = *(const godot::Color*)&m_constantBuffers[decl.slot][decl.offset];
			vs->material_set_param(material, decl.name, vector);
		}
		else if (decl.type == ParamType::Color)
		{
			auto& vector = *(const godot::Color*)&m_constantBuffers[decl.slot][decl.offset];
			vs->material_set_param(material, decl.name, vector);
		}
		else if (decl.type == ParamType::Matrix44)
		{
			auto& matrix = *(const Effekseer::Matrix44*)&m_constantBuffers[decl.slot][decl.offset];
			vs->material_set_param(material, decl.name, ToGdMatrix(matrix));
		}
		else if (decl.type == ParamType::Texture)
		{
			godot::RID texture = Int64ToRID((int64_t)state.TextureIDs[decl.slot]);
			vs->texture_set_flags(texture, godot::Texture::FLAG_MIPMAPS | 
				((state.TextureFilterTypes[decl.slot] == Effekseer::TextureFilterType::Linear) ? godot::Texture::FLAG_FILTER : 0) | 
				((state.TextureWrapTypes[decl.slot] == Effekseer::TextureWrapType::Repeat) ? godot::Texture::FLAG_REPEAT : 0));
			vs->material_set_param(material, decl.name, texture);
		}
	}
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerGodot
