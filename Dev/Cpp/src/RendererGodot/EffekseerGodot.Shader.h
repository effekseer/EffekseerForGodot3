#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.ShaderBase.h"
#include "EffekseerGodot.RendererImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerGodot
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Shader : public ::EffekseerRenderer::ShaderBase
{
public:
	enum class ParamType : uint8_t
	{
		Int,
		Float,
		Vector2,
		Vector3,
		Vector4,
		Matrix44,
		Color,
		Texture,
	};

	struct ParamDecl
	{
		char name[24];
		ParamType type;
		uint16_t slot;
		uint16_t offset;
	};

	virtual ~Shader();

	static std::unique_ptr<Shader> Create(const char* name, const char* code, 
		EffekseerRenderer::RendererShaderType shaderType,
		std::vector<ParamDecl>&& paramDecls);

	template <size_t N>
	static std::unique_ptr<Shader> Create(const char* name, const char* code, 
		EffekseerRenderer::RendererShaderType shaderType,
		const ParamDecl (&paramDecls)[N])
	{
		std::vector<ParamDecl> v(N);
		v.assign(paramDecls, paramDecls + N);
		return Create(name, code, shaderType, std::move(v));
	}

	void SetVertexConstantBufferSize(int32_t size)
	{
		m_constantBuffers[0].resize((size_t)size);
	}
	void SetPixelConstantBufferSize(int32_t size)
	{
		m_constantBuffers[1].resize((size_t)size);
	}
	int32_t GetVertexConstantBufferSize() const
	{
		return (int32_t)m_constantBuffers[0].size();
	}
	int32_t GetPixelConstantBufferSize() const
	{
		return (int32_t)m_constantBuffers[1].size();
	}

	void* GetVertexConstantBuffer()
	{
		return m_constantBuffers[0].data();
	}
	void* GetPixelConstantBuffer()
	{
		return m_constantBuffers[1].data();
	}

	void SetConstantBuffer() {}

	void ApplyToMaterial(godot::RID material, EffekseerRenderer::RenderStateBase::State& state);

	EffekseerRenderer::RendererShaderType GetShaderType() { return m_shaderType; }

private:
	std::vector<uint8_t> m_constantBuffers[2];

	std::string m_name;
	std::vector<ParamDecl> m_paramDecls;
	EffekseerRenderer::RendererShaderType m_shaderType = EffekseerRenderer::RendererShaderType::Unlit;
	godot::RID m_rid[2][2][3][5];;

	Shader(const char* name, const char* code, 
		EffekseerRenderer::RendererShaderType shaderType, 
		std::vector<ParamDecl>&& paramDecls);

};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerGodot
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
