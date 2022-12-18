#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.ShaderBase.h"
#include "EffekseerGodot.Renderer.h"

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
	enum class RenderType : uint8_t
	{
		SpatialLightweight,
		SpatialDepthFade,
		CanvasItem,
		Max
	};

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

	static std::unique_ptr<Shader> Create(const char* name, EffekseerRenderer::RendererShaderType shaderType);

	template <size_t N>
	void SetCode(RenderType renderType, const char* code, const ParamDecl (&paramDecls)[N])
	{
		std::vector<ParamDecl> v(N);
		v.assign(paramDecls, paramDecls + N);
		SetCode(renderType, code, std::move(v));
	}

	void SetCode(RenderType renderType, const char* code, std::vector<ParamDecl>&& paramDecls);

	bool HasRID(RenderType renderType, bool depthTest, bool depthWrite, ::Effekseer::AlphaBlendType blendType, ::Effekseer::CullingType cullingType);

	godot::RID GetRID(RenderType renderType, bool depthTest, bool depthWrite, ::Effekseer::AlphaBlendType blendType, ::Effekseer::CullingType cullingType);

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

	void SetCustomData1Count(int32_t count) { m_customData1 = (int8_t)count; }
	void SetCustomData2Count(int32_t count) { m_customData2 = (int8_t)count; }
	int32_t GetCustomData1Count() const { return m_customData1; }
	int32_t GetCustomData2Count() const { return m_customData2; }

	void ApplyToMaterial(RenderType renderType, godot::RID material, EffekseerRenderer::RenderStateBase::State& state);

	EffekseerRenderer::RendererShaderType GetShaderType() { return m_shaderType; }

private:
	std::vector<uint8_t> m_constantBuffers[2];

	std::string m_name;
	EffekseerRenderer::RendererShaderType m_shaderType = EffekseerRenderer::RendererShaderType::Unlit;

	struct InternalShader {
		godot::String baseCode;
		godot::RID rid[2][2][3][5];
		std::vector<ParamDecl> paramDecls;
	};
	InternalShader m_internals[(size_t)RenderType::Max];
	int8_t m_customData1 = 0;
	int8_t m_customData2 = 0;

	Shader(const char* name, EffekseerRenderer::RendererShaderType shaderType);
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerGodot
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
