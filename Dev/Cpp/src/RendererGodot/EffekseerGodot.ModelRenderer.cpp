
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerGodot.RenderState.h"
#include "EffekseerGodot.Renderer.h"

#include "EffekseerGodot.IndexBuffer.h"
#include "EffekseerGodot.ModelRenderer.h"
#include "EffekseerGodot.Shader.h"
#include "EffekseerGodot.VertexBuffer.h"

namespace EffekseerGodot
{
	
namespace ModelShaders
{

namespace Unlit
{
#define DISTORTION 0
#define LIGHTING 0
namespace Lightweight
{
#define SOFT_PARTICLE 0
#include "Shaders/Model.inl"
#undef SOFT_PARTICLE
}
namespace SoftParticle
{
#define SOFT_PARTICLE 1
#include "Shaders/Model.inl"
#undef SOFT_PARTICLE
}
namespace CanvasItem
{
#include "Shaders/Model2D.inl"
}
#undef LIGHTING
#undef DISTORTION
}

namespace Lighting
{
#define DISTORTION 0
#define LIGHTING 1
namespace Lightweight
{
#define SOFT_PARTICLE 0
#include "Shaders/Model.inl"
#undef SOFT_PARTICLE
}
namespace SoftParticle
{
#define SOFT_PARTICLE 1
#include "Shaders/Model.inl"
#undef SOFT_PARTICLE
}
namespace CanvasItem
{
#include "Shaders/Model2D.inl"
}
#undef LIGHTING
#undef DISTORTION
}

namespace Distortion
{
#define DISTORTION 1
#define LIGHTING 0
namespace Lightweight
{
#define SOFT_PARTICLE 0
#include "Shaders/Model.inl"
#undef SOFT_PARTICLE
}
namespace SoftParticle
{
#define SOFT_PARTICLE 1
#include "Shaders/Model.inl"
#undef SOFT_PARTICLE
}
namespace CanvasItem
{
#include "Shaders/Model2D.inl"
}
#undef LIGHTING
#undef DISTORTION
}

}

ModelRenderer::ModelRenderer(Renderer* renderer)
	: m_renderer(renderer)
{
	using namespace EffekseerRenderer;
	using namespace EffekseerGodot::ModelShaders;

	m_shaders[(size_t)RendererShaderType::Unlit] = Shader::Create("Model_Basic_Unlit", RendererShaderType::Unlit);
	m_shaders[(size_t)RendererShaderType::Unlit]->SetVertexConstantBufferSize(sizeof(ModelRendererVertexConstantBuffer<40>));
	m_shaders[(size_t)RendererShaderType::Unlit]->SetPixelConstantBufferSize(sizeof(PixelConstantBuffer));
	m_shaders[(size_t)RendererShaderType::Unlit]->SetCode(Shader::RenderType::SpatialLightweight, Unlit::Lightweight::code, Unlit::Lightweight::decl);
	m_shaders[(size_t)RendererShaderType::Unlit]->SetCode(Shader::RenderType::SpatialDepthFade, Unlit::SoftParticle::code, Unlit::SoftParticle::decl);
	m_shaders[(size_t)RendererShaderType::Unlit]->SetCode(Shader::RenderType::CanvasItem, Unlit::CanvasItem::code, Unlit::CanvasItem::decl);

	m_shaders[(size_t)RendererShaderType::Lit] = Shader::Create("Model_Basic_Lighting", RendererShaderType::Lit);
	m_shaders[(size_t)RendererShaderType::Lit]->SetVertexConstantBufferSize(sizeof(ModelRendererVertexConstantBuffer<40>));
	m_shaders[(size_t)RendererShaderType::Lit]->SetPixelConstantBufferSize(sizeof(PixelConstantBuffer));
	m_shaders[(size_t)RendererShaderType::Lit]->SetCode(Shader::RenderType::SpatialLightweight, Lighting::Lightweight::code, Lighting::Lightweight::decl);
	m_shaders[(size_t)RendererShaderType::Lit]->SetCode(Shader::RenderType::SpatialDepthFade, Lighting::SoftParticle::code, Lighting::SoftParticle::decl);
	m_shaders[(size_t)RendererShaderType::Lit]->SetCode(Shader::RenderType::CanvasItem, Lighting::CanvasItem::code, Lighting::CanvasItem::decl);

	m_shaders[(size_t)RendererShaderType::BackDistortion] = Shader::Create("Model_Basic_Distortion", RendererShaderType::BackDistortion);
	m_shaders[(size_t)RendererShaderType::BackDistortion]->SetVertexConstantBufferSize(sizeof(ModelRendererVertexConstantBuffer<40>));
	m_shaders[(size_t)RendererShaderType::BackDistortion]->SetPixelConstantBufferSize(sizeof(PixelConstantBuffer));
	m_shaders[(size_t)RendererShaderType::BackDistortion]->SetCode(Shader::RenderType::SpatialLightweight, Distortion::Lightweight::code, Distortion::Lightweight::decl);
	m_shaders[(size_t)RendererShaderType::BackDistortion]->SetCode(Shader::RenderType::SpatialDepthFade, Distortion::SoftParticle::code, Distortion::SoftParticle::decl);
	m_shaders[(size_t)RendererShaderType::BackDistortion]->SetCode(Shader::RenderType::CanvasItem, Distortion::CanvasItem::code, Distortion::CanvasItem::decl);
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
ModelRendererRef ModelRenderer::Create(Renderer* renderer)
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
	Rendering_<Renderer>(m_renderer, parameter, instanceParameter, userData);
}

void ModelRenderer::EndRendering(const efkModelNodeParam& parameter, void* userData)
{
	if (parameter.ModelIndex < 0)
	{
		return;
	}

	Effekseer::ModelRef model;

	if (parameter.IsProceduralMode)
	{
		model = parameter.EffectPointer->GetProceduralModel(parameter.ModelIndex);
	}
	else
	{
		model = parameter.EffectPointer->GetModel(parameter.ModelIndex);
	}

	if (model == nullptr)
	{
		return;
	}

	const bool softparticleEnabled = 
		!(parameter.BasicParameterPtr->SoftParticleDistanceFar == 0.0f &&
		parameter.BasicParameterPtr->SoftParticleDistanceNear == 0.0f &&
		parameter.BasicParameterPtr->SoftParticleDistanceNearOffset == 0.0f) &&
		parameter.BasicParameterPtr->MaterialType != Effekseer::RendererMaterialType::File;

	m_renderer->BeginModelRendering(model, softparticleEnabled);

	using namespace EffekseerRenderer;

	EndRendering_<
		Renderer,
		Shader,
		Effekseer::Model,
		false,
		1>(
		m_renderer,
		nullptr, nullptr, nullptr, 
		m_shaders[(size_t)RendererShaderType::Lit].get(),
		m_shaders[(size_t)RendererShaderType::Unlit].get(),
		m_shaders[(size_t)RendererShaderType::BackDistortion].get(),
		parameter, userData);

	m_renderer->EndModelRendering();
}

Shader* ModelRenderer::GetShader(::EffekseerRenderer::RendererShaderType type)
{
	return m_shaders[(size_t)type].get();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerGodot
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
