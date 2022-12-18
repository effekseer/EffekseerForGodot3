#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.ModelRendererBase.h"
#include "EffekseerGodot.Renderer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerGodot
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
typedef ::Effekseer::ModelRenderer::NodeParameter efkModelNodeParam;
typedef ::Effekseer::ModelRenderer::InstanceParameter efkModelInstanceParam;
typedef ::Effekseer::SIMD::Vec3f efkVector3D;

class ModelRenderer;
using ModelRendererRef = Effekseer::RefPtr<ModelRenderer>;

class ModelRenderer : public ::EffekseerRenderer::ModelRendererBase
{
private:
	Renderer* m_renderer = nullptr;
	std::array<std::unique_ptr<Shader>, 6> m_shaders;

	ModelRenderer(Renderer* renderer);

public:
	virtual ~ModelRenderer();

	static ModelRendererRef Create(Renderer* renderer);

public:
	void BeginRendering(const efkModelNodeParam& parameter, int32_t count, void* userData) override;

	void Rendering(const efkModelNodeParam& parameter, const InstanceParameter& instanceParameter, void* userData) override;

	void EndRendering(const efkModelNodeParam& parameter, void* userData) override;

	Shader* GetShader(::EffekseerRenderer::RendererShaderType type);
};
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerGodot
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

