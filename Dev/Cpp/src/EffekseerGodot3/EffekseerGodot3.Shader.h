#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.ShaderBase.h"
#include "EffekseerGodot3.RendererImplemented.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerGodot3
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Shader : public ::EffekseerRenderer::ShaderBase
{
public:
	enum class ParamType
	{
		Int,
		Float,
		Vector2,
		Vector3,
		Vector4,
		Matrix44,
	};

	struct ParamDecl
	{
		const char* name;
		ParamType type;
		uint16_t slot;
		uint16_t offset;
	};

	virtual ~Shader();

	static std::unique_ptr<Shader> Create(const char* name, const char* code, 
		EffekseerRenderer::RendererShaderType shaderType,
		const ParamDecl* paramDecls, uint32_t paramCount);

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

	const char* m_name = nullptr;
	const ParamDecl* m_paramDecls = nullptr;
	uint32_t m_paramCount = 0;
	EffekseerRenderer::RendererShaderType m_shaderType = EffekseerRenderer::RendererShaderType::Unlit;
	godot::RID m_rid[2][2][3][5];;

	Shader(const char* name, const char* code, 
		EffekseerRenderer::RendererShaderType shaderType, 
		const ParamDecl* paramDecls, uint32_t paramCount);

};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerGodot3
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
