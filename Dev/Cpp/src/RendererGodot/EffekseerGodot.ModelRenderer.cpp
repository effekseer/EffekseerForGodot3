
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerGodot.RenderState.h"
#include "EffekseerGodot.RendererImplemented.h"

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
#undef LIGHTING
#undef DISTORTION
}

}

ModelRenderer::ModelRenderer(RendererImplemented* renderer)
	: m_renderer(renderer)
{
	using namespace EffekseerGodot::ModelShaders;
	using namespace EffekseerRenderer;

	m_lightweightShaders[(size_t)RendererShaderType::Unlit] = Shader::Create("Model_Basic_Unlit_Lightweight",
		Unlit::Lightweight::code, Shader::RenderType::Spatial, RendererShaderType::Unlit, Unlit::Lightweight::decl);
	m_lightweightShaders[(size_t)RendererShaderType::Unlit]->SetVertexConstantBufferSize(sizeof(ModelRendererVertexConstantBuffer<40>));
	m_lightweightShaders[(size_t)RendererShaderType::Unlit]->SetPixelConstantBufferSize(sizeof(PixelConstantBuffer));

	m_softparticleShaders[(size_t)RendererShaderType::Unlit] = Shader::Create("Model_Basic_Unlit_SoftParticle",
		Unlit::SoftParticle::code, Shader::RenderType::Spatial, RendererShaderType::Unlit, Unlit::SoftParticle::decl);
	m_softparticleShaders[(size_t)RendererShaderType::Unlit]->SetVertexConstantBufferSize(sizeof(ModelRendererVertexConstantBuffer<40>));
	m_softparticleShaders[(size_t)RendererShaderType::Unlit]->SetPixelConstantBufferSize(sizeof(PixelConstantBuffer));

	m_lightweightShaders[(size_t)RendererShaderType::Lit] = Shader::Create("Model_Basic_Lighting_Lightweight",
		Lighting::Lightweight::code, Shader::RenderType::Spatial, RendererShaderType::Lit, Lighting::Lightweight::decl);
	m_lightweightShaders[(size_t)RendererShaderType::Lit]->SetVertexConstantBufferSize(sizeof(ModelRendererVertexConstantBuffer<40>));
	m_lightweightShaders[(size_t)RendererShaderType::Lit]->SetPixelConstantBufferSize(sizeof(PixelConstantBuffer));

	m_softparticleShaders[(size_t)RendererShaderType::Lit] = Shader::Create("Model_Basic_Lighting_SoftParticle",
		Lighting::SoftParticle::code, Shader::RenderType::Spatial, RendererShaderType::Lit, Lighting::SoftParticle::decl);
	m_softparticleShaders[(size_t)RendererShaderType::Lit]->SetVertexConstantBufferSize(sizeof(ModelRendererVertexConstantBuffer<40>));
	m_softparticleShaders[(size_t)RendererShaderType::Lit]->SetPixelConstantBufferSize(sizeof(PixelConstantBuffer));

	m_lightweightShaders[(size_t)RendererShaderType::BackDistortion] = Shader::Create("Model_Basic_Distortion_Lightweight",
		Distortion::Lightweight::code, Shader::RenderType::Spatial, RendererShaderType::BackDistortion, Distortion::Lightweight::decl);
	m_lightweightShaders[(size_t)RendererShaderType::BackDistortion]->SetVertexConstantBufferSize(sizeof(ModelRendererVertexConstantBuffer<40>));
	m_lightweightShaders[(size_t)RendererShaderType::BackDistortion]->SetPixelConstantBufferSize(sizeof(PixelConstantBufferDistortion));

	m_softparticleShaders[(size_t)RendererShaderType::BackDistortion] = Shader::Create("Model_Basic_Distortion_SoftParticle",
		Distortion::SoftParticle::code, Shader::RenderType::Spatial, RendererShaderType::BackDistortion, Distortion::SoftParticle::decl);
	m_softparticleShaders[(size_t)RendererShaderType::BackDistortion]->SetVertexConstantBufferSize(sizeof(ModelRendererVertexConstantBuffer<40>));
	m_softparticleShaders[(size_t)RendererShaderType::BackDistortion]->SetPixelConstantBufferSize(sizeof(PixelConstantBufferDistortion));
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

	Effekseer::ModelRef model;

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

	const bool softparticleEnabled = !(
		parameter.BasicParameterPtr->SoftParticleDistanceFar == 0.0f &&
		parameter.BasicParameterPtr->SoftParticleDistanceFar == 0.0f &&
		parameter.BasicParameterPtr->SoftParticleDistanceFar == 0.0f);
	auto& shaders = (softparticleEnabled) ? m_softparticleShaders : m_lightweightShaders;

	using namespace EffekseerRenderer;

	EndRendering_<
		RendererImplemented,
		Shader,
		Effekseer::Model,
		false,
		1>(
		m_renderer,
		shaders[(size_t)RendererShaderType::AdvancedLit].get(),
		shaders[(size_t)RendererShaderType::AdvancedUnlit].get(),
		shaders[(size_t)RendererShaderType::AdvancedBackDistortion].get(),
		shaders[(size_t)RendererShaderType::Lit].get(),
		shaders[(size_t)RendererShaderType::Unlit].get(),
		shaders[(size_t)RendererShaderType::BackDistortion].get(),
		parameter, userData);

	m_renderer->SetModel(nullptr);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerGodot
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
