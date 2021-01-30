
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <VisualServer.hpp>
#include <World.hpp>
#include <Mesh.hpp>
#include <ImageTexture.hpp>
#include "EffekseerGodot.Renderer.h"
#include "EffekseerGodot.RenderState.h"
#include "EffekseerGodot.RendererImplemented.h"

#include "EffekseerGodot.IndexBuffer.h"
#include "EffekseerGodot.Shader.h"
#include "EffekseerGodot.VertexBuffer.h"
#include "EffekseerGodot.ModelRenderer.h"
#include "EffekseerGodot.RenderResources.h"
#include "EffekseerGodot.Utils.h"

#include "EffekseerRenderer.Renderer_Impl.h"
#include "EffekseerRenderer.RibbonRendererBase.h"
#include "EffekseerRenderer.RingRendererBase.h"
#include "EffekseerRenderer.SpriteRendererBase.h"
#include "EffekseerRenderer.TrackRendererBase.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerGodot
{

namespace StandardShaders
{

namespace Unlit
{
#define DISTORTION 0
#define LIGHTING 0
namespace Lightweight
{
#define SOFT_PARTICLE 0
#include "Shaders/Particle.inl"
#undef SOFT_PARTICLE
}
namespace SoftParticle
{
#define SOFT_PARTICLE 1
#include "Shaders/Particle.inl"
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
#include "Shaders/Particle.inl"
#undef SOFT_PARTICLE
	}
	namespace SoftParticle
	{
#define SOFT_PARTICLE 1
#include "Shaders/Particle.inl"
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
#include "Shaders/Particle.inl"
#undef SOFT_PARTICLE
}
namespace SoftParticle
{
#define SOFT_PARTICLE 1
#include "Shaders/Particle.inl"
#undef SOFT_PARTICLE
}
#undef LIGHTING
#undef DISTORTION
}

}

RenderCommand::RenderCommand()
{
	auto vs = godot::VisualServer::get_singleton();
	m_immediate = vs->immediate_create();
	m_instance = vs->instance_create();
	m_material = vs->material_create();
	vs->instance_geometry_set_material_override(m_instance, m_material);
}

RenderCommand::~RenderCommand()
{
	auto vs = godot::VisualServer::get_singleton();
	vs->free_rid(m_instance);
	vs->free_rid(m_immediate);
	vs->free_rid(m_material);
}

void RenderCommand::Reset()
{
	auto vs = godot::VisualServer::get_singleton();
	vs->immediate_clear(m_immediate);
	vs->instance_set_base(m_instance, godot::RID());
}

inline godot::Color ConvertColor(const EffekseerRenderer::VertexColor& color)
{
	return godot::Color(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f);
}

inline godot::Vector2 ConvertUV(const float uv[])
{
	return godot::Vector2(uv[0], uv[1]);
}

inline godot::Vector3 ConvertVector3(const EffekseerRenderer::VertexFloat3& v)
{
	return godot::Vector3(v.X, v.Y, v.Z);
}

inline EffekseerRenderer::VertexFloat3 ConvertPackedVector3(const EffekseerRenderer::VertexColor& v)
{
	Effekseer::Vector3D result;
	result.X = v.R / 255.0f * 2.0f - 1.0f;
	result.Y = v.G / 255.0f * 2.0f - 1.0f;
	result.Z = v.B / 255.0f * 2.0f - 1.0f;
	return result;
}

inline EffekseerRenderer::VertexFloat3 Normalize(const EffekseerRenderer::VertexFloat3& v)
{
	float length = sqrtf(v.X * v.X + v.Y * v.Y + v.Z * v.Z);
	EffekseerRenderer::VertexFloat3 result;
	result.X = v.X / length;
	result.Y = v.Y / length;
	result.Z = v.Z / length;
	return result;
}

inline float Dot(const EffekseerRenderer::VertexFloat3& lhs, const EffekseerRenderer::VertexFloat3& rhs)
{
	return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z;
}

inline EffekseerRenderer::VertexFloat3 Cross(const EffekseerRenderer::VertexFloat3& lhs, const EffekseerRenderer::VertexFloat3& rhs)
{
	EffekseerRenderer::VertexFloat3 result;
	result.X = lhs.Y * rhs.Z - lhs.Z * rhs.Y;
	result.Y = lhs.Z * rhs.X - lhs.X * rhs.Z;
	result.Z = lhs.X * rhs.Y - lhs.Y * rhs.X;
	return result;
}

inline godot::Plane ConvertTangent(const EffekseerRenderer::VertexFloat3& t)
{
	return godot::Plane(t.X, t.Y, t.Z, 1.0f);
}

void RenderCommand::DrawSprites(godot::World* world, 
	const void* vertexData, const void* indexData, int32_t stride, int32_t spriteCount, 
	EffekseerRenderer::RendererShaderType shaderType, int32_t priority)
{
	auto vs = godot::VisualServer::get_singleton();

	const uint16_t* indeces = (const uint16_t*)indexData;

	vs->instance_set_base(m_instance, m_immediate);
	vs->instance_set_scenario(m_instance, world->get_scenario());
	vs->material_set_render_priority(m_material, priority);

	vs->immediate_begin(m_immediate, godot::Mesh::PRIMITIVE_TRIANGLES);

	using namespace EffekseerRenderer;

	if (shaderType == RendererShaderType::Unlit)
	{
		const SimpleVertex* vertices = (const SimpleVertex*)vertexData;
		for (int32_t i = 0; i < spriteCount; i++)
		{
			for (int32_t j = 0; j < 6; j++)
			{
				auto& v = vertices[indeces[i * 6 + j]];
				vs->immediate_color(m_immediate, ConvertColor(v.Col));
				vs->immediate_uv(m_immediate, ConvertUV(v.UV));
				vs->immediate_vertex(m_immediate, ConvertVector3(v.Pos));
			}
		}
	}
	else if (shaderType == RendererShaderType::BackDistortion)
	{
		const DynamicVertex* vertices = (const DynamicVertex*)vertexData;
		for (int32_t i = 0; i < spriteCount; i++)
		{
			for (int32_t j = 0; j < 6; j++)
			{
				auto& v = vertices[indeces[i * 6 + j]];
				vs->immediate_color(m_immediate, ConvertColor(v.Col));
				vs->immediate_uv(m_immediate, ConvertUV(v.UV));

				VertexFloat3 normal = Normalize(UnpackVector3DF(v.Normal));
				VertexFloat3 tangent = Normalize(UnpackVector3DF(v.Tangent));
				vs->immediate_normal(m_immediate, ConvertVector3(normal));
				vs->immediate_tangent(m_immediate, ConvertTangent(tangent));
				vs->immediate_vertex(m_immediate, ConvertVector3(v.Pos));
			}
		}
	}
	else if (shaderType == RendererShaderType::Lit)
	{
		const LightingVertex* vertices = (const LightingVertex*)vertexData;
		for (int32_t i = 0; i < spriteCount; i++)
		{
			for (int32_t j = 0; j < 6; j++)
			{
				auto& v = vertices[indeces[i * 6 + j]];
				vs->immediate_color(m_immediate, ConvertColor(v.Col));
				vs->immediate_uv(m_immediate, ConvertUV(v.UV));
				
				VertexFloat3 normal = Normalize(UnpackVector3DF(v.Normal));
				VertexFloat3 tangent = Normalize(UnpackVector3DF(v.Tangent));
				vs->immediate_normal(m_immediate, ConvertVector3(normal));
				vs->immediate_tangent(m_immediate, ConvertTangent(tangent));
				vs->immediate_vertex(m_immediate, ConvertVector3(v.Pos));
			}
		}
	}
	else if (shaderType == RendererShaderType::Material)
	{
		for (int32_t i = 0; i < spriteCount; i++)
		{
			for (int32_t j = 0; j < 6; j++)
			{
				auto& v = *(DynamicVertex*)((uint8_t*)vertexData + indeces[i * 6 + j] * stride);
				vs->immediate_color(m_immediate, ConvertColor(v.Col));
				vs->immediate_uv(m_immediate, ConvertUV(v.UV));

				VertexFloat3 normal = Normalize(UnpackVector3DF(v.Normal));
				VertexFloat3 tangent = Normalize(UnpackVector3DF(v.Tangent));
				vs->immediate_normal(m_immediate, ConvertVector3(normal));
				vs->immediate_tangent(m_immediate, ConvertTangent(tangent));
				vs->immediate_vertex(m_immediate, ConvertVector3(v.Pos));
			}
		}
	}

	vs->immediate_end(m_immediate);
}

void RenderCommand::DrawModel(godot::World* world, godot::RID mesh, int32_t priority)
{
	auto vs = godot::VisualServer::get_singleton();

	vs->instance_set_base(m_instance, mesh);
	vs->instance_set_scenario(m_instance, world->get_scenario());
	vs->material_set_render_priority(m_material, priority);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererRef Renderer::Create(int32_t squareMaxCount, int32_t drawMaxCount)
{
	auto renderer = Effekseer::MakeRefPtr<RendererImplemented>(squareMaxCount);
	if (renderer->Initialize(drawMaxCount))
	{
		return renderer;
	}
	return nullptr;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::RendererImplemented(int32_t squareMaxCount)
	: m_squareMaxCount(squareMaxCount)
{
	// dummy
	m_background = Effekseer::MakeRefPtr<Texture>();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
RendererImplemented::~RendererImplemented()
{
	GetImpl()->DeleteProxyTextures(this);

	assert(GetRef() == 0);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::Initialize(int32_t drawMaxCount)
{
	m_renderState.reset(new RenderState());

	// generate a vertex buffer
	{
		m_vertexBuffer = VertexBuffer::Create(this, EffekseerRenderer::GetMaximumVertexSizeInAllTypes() * m_squareMaxCount * 4, true);
		if (m_vertexBuffer == nullptr)
			return false;
	}

	// generate an index buffer
	{
		m_indexBuffer = IndexBuffer::Create(this, m_squareMaxCount * 6, false);
		if (m_indexBuffer == nullptr)
			return false;

		m_indexBuffer->Lock();

		// ( 標準設定で　DirectX 時計周りが表, OpenGLは反時計回りが表 )
		for (int i = 0; i < m_squareMaxCount; i++)
		{
			uint16_t* buf = (uint16_t*)m_indexBuffer->GetBufferDirect(6);
			buf[0] = 3 + 4 * i;
			buf[1] = 1 + 4 * i;
			buf[2] = 0 + 4 * i;
			buf[3] = 3 + 4 * i;
			buf[4] = 0 + 4 * i;
			buf[5] = 2 + 4 * i;
		}

		m_indexBuffer->Unlock();
	}

	{
		using namespace EffekseerGodot::StandardShaders;
		using namespace EffekseerRenderer;

		m_lightweightShaders[(size_t)RendererShaderType::Unlit] = Shader::Create("Sprite_Basic_Unlit_Lightweight", 
			Unlit::Lightweight::code, RendererShaderType::Unlit, Unlit::Lightweight::decl);
		m_lightweightShaders[(size_t)RendererShaderType::Unlit]->SetVertexConstantBufferSize(sizeof(StandardRendererVertexBuffer));
		m_lightweightShaders[(size_t)RendererShaderType::Unlit]->SetPixelConstantBufferSize(sizeof(PixelConstantBuffer));

		m_softparticleShaders[(size_t)RendererShaderType::Unlit] = Shader::Create("Sprite_Basic_Unlit_SoftParticle", 
			Unlit::SoftParticle::code, RendererShaderType::Unlit, Unlit::SoftParticle::decl);
		m_softparticleShaders[(size_t)RendererShaderType::Unlit]->SetVertexConstantBufferSize(sizeof(StandardRendererVertexBuffer));
		m_softparticleShaders[(size_t)RendererShaderType::Unlit]->SetPixelConstantBufferSize(sizeof(PixelConstantBuffer));

		m_lightweightShaders[(size_t)RendererShaderType::Lit] = Shader::Create("Sprite_Basic_Lighting_Lightweight", 
			Lighting::Lightweight::code, RendererShaderType::Lit, Lighting::Lightweight::decl);
		m_lightweightShaders[(size_t)RendererShaderType::Lit]->SetVertexConstantBufferSize(sizeof(StandardRendererVertexBuffer));
		m_lightweightShaders[(size_t)RendererShaderType::Lit]->SetPixelConstantBufferSize(sizeof(PixelConstantBuffer));

		m_softparticleShaders[(size_t)RendererShaderType::Lit] = Shader::Create("Sprite_Basic_Lighting_SoftParticle",
			Lighting::SoftParticle::code, RendererShaderType::Lit, Lighting::SoftParticle::decl);
		m_softparticleShaders[(size_t)RendererShaderType::Lit]->SetVertexConstantBufferSize(sizeof(StandardRendererVertexBuffer));
		m_softparticleShaders[(size_t)RendererShaderType::Lit]->SetPixelConstantBufferSize(sizeof(PixelConstantBuffer));

		m_lightweightShaders[(size_t)RendererShaderType::BackDistortion] = Shader::Create("Sprite_Basic_Distortion_Lightweight",
			Distortion::Lightweight::code, RendererShaderType::BackDistortion, Distortion::Lightweight::decl);
		m_lightweightShaders[(size_t)RendererShaderType::BackDistortion]->SetVertexConstantBufferSize(sizeof(StandardRendererVertexBuffer));
		m_lightweightShaders[(size_t)RendererShaderType::BackDistortion]->SetPixelConstantBufferSize(sizeof(PixelConstantBufferDistortion));

		m_softparticleShaders[(size_t)RendererShaderType::BackDistortion] = Shader::Create("Sprite_Basic_Distortion_SoftParticle",
			Distortion::SoftParticle::code, RendererShaderType::BackDistortion, Distortion::SoftParticle::decl);
		m_softparticleShaders[(size_t)RendererShaderType::BackDistortion]->SetVertexConstantBufferSize(sizeof(StandardRendererVertexBuffer));
		m_softparticleShaders[(size_t)RendererShaderType::BackDistortion]->SetPixelConstantBufferSize(sizeof(PixelConstantBufferDistortion));

	}

	m_renderCommands.resize((size_t)drawMaxCount);

	m_standardRenderer.reset(new StandardRenderer(this));

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::Destroy()
{
	m_renderCommands.clear();
}

void RendererImplemented::ResetState()
{
	m_renderCount = 0;

	for (auto& batch : m_renderCommands)
	{
		batch.Reset();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::BeginRendering()
{
	impl->CalculateCameraProjectionMatrix();

	// ステート初期設定
	m_renderState->GetActiveState().Reset();
	m_renderState->Update(true);

	// レンダラーリセット
	m_standardRenderer->ResetAndRenderingIfRequired();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
bool RendererImplemented::EndRendering()
{
	// レンダラーリセット
	m_standardRenderer->ResetAndRenderingIfRequired();

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
VertexBuffer* RendererImplemented::GetVertexBuffer()
{
	return m_vertexBuffer.Get();
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
IndexBuffer* RendererImplemented::GetIndexBuffer()
{
	if (GetRenderMode() == ::Effekseer::RenderMode::Wireframe)
	{
		return m_indexBufferForWireframe.Get();
	}
	else
	{
		return m_indexBuffer.Get();
	}
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::SpriteRendererRef RendererImplemented::CreateSpriteRenderer()
{
	return ::Effekseer::SpriteRendererRef(new ::EffekseerRenderer::SpriteRendererBase<RendererImplemented, false>(this));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RibbonRendererRef RendererImplemented::CreateRibbonRenderer()
{
	return ::Effekseer::RibbonRendererRef(new ::EffekseerRenderer::RibbonRendererBase<RendererImplemented, false>(this));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::RingRendererRef RendererImplemented::CreateRingRenderer()
{
	return ::Effekseer::RingRendererRef(new ::EffekseerRenderer::RingRendererBase<RendererImplemented, false>(this));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::ModelRendererRef RendererImplemented::CreateModelRenderer()
{
	return ModelRenderer::Create(this);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
::Effekseer::TrackRendererRef RendererImplemented::CreateTrackRenderer()
{
	return ::Effekseer::TrackRendererRef(new ::EffekseerRenderer::TrackRendererBase<RendererImplemented, false>(this));
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
const Effekseer::Backend::TextureRef& RendererImplemented::GetBackground()
{
	return m_background;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
EffekseerRenderer::DistortingCallback* RendererImplemented::GetDistortingCallback()
{
	return nullptr;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetLayout(Shader* shader)
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::DrawSprites(int32_t spriteCount, int32_t vertexOffset)
{
	assert(m_currentShader != nullptr);

	if (m_renderCount >= m_renderCommands.size())
	{
		return;
	}

	const void* vertexData = GetVertexBuffer()->Refer();
	const void* indexData = GetIndexBuffer()->Refer();

	if (m_currentShader->GetShaderType() == EffekseerRenderer::RendererShaderType::Material)
	{
		const auto& state = m_standardRenderer->GetState();
		const float* v = (const float*)((const uint8_t*)vertexData + sizeof(EffekseerRenderer::DynamicVertex));
		int32_t offset = m_currentShader->GetVertexConstantBufferSize() - (state.CustomData1Count + state.CustomData2Count) * 4;
	
		if (state.CustomData1Count > 0)
		{
			SetVertexBufferToShader(v, state.CustomData1Count * 4, offset);
			v += state.CustomData1Count;
			offset += state.CustomData1Count * 4;
		}
		if (state.CustomData2Count > 0)
		{
			SetVertexBufferToShader(v, state.CustomData2Count * 4, offset);
			v += state.CustomData2Count;
			offset += state.CustomData2Count * 4;
		}
	}

	m_currentShader->ApplyToMaterial(
		m_renderCommands[m_renderCount].GetMaterial(), 
		m_renderState->GetActiveState());

	m_renderCommands[m_renderCount].DrawSprites(m_world, vertexData, indexData, 
		m_standardRenderer->CalculateCurrentStride(), spriteCount, 
		m_currentShader->GetShaderType(), (int32_t)m_renderCount);
	m_renderCount++;

	impl->drawcallCount++;
	impl->drawvertexCount += spriteCount * 4;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetModel(Effekseer::ModelRef model)
{
	m_currentModel = model;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::DrawPolygon(int32_t vertexCount, int32_t indexCount)
{
	assert(m_currentShader != nullptr);
	assert(m_currentModel != nullptr);

	if (m_renderCount >= m_renderCommands.size())
	{
		return;
	}

	m_currentShader->ApplyToMaterial(
		m_renderCommands[m_renderCount].GetMaterial(), 
		m_renderState->GetActiveState());

	
	auto mesh = m_currentModel.DownCast<Model>()->GetRID();
	m_renderCommands[m_renderCount].DrawModel(m_world, mesh, (int32_t)m_renderCount);
	m_renderCount++;

	impl->drawcallCount++;
	impl->drawvertexCount += vertexCount;
}

void RendererImplemented::DrawPolygonInstanced(int32_t vertexCount, int32_t indexCount, int32_t instanceCount)
{
	assert(m_currentModel != nullptr);
	impl->drawcallCount++;
	impl->drawvertexCount += vertexCount * instanceCount;
}

Shader* RendererImplemented::GetShader(::EffekseerRenderer::RendererShaderType type) const
{
	const auto& state = m_standardRenderer->GetState();
	const bool softparticleEnabled = !(
		state.SoftParticleDistanceFar == 0.0f &&
		state.SoftParticleDistanceNear == 0.0f &&
		state.SoftParticleDistanceNearOffset == 0.0f);

	if ((size_t)type >= (size_t)EffekseerRenderer::RendererShaderType::Unlit && 
		(size_t)type <= (size_t)EffekseerRenderer::RendererShaderType::AdvancedBackDistortion)
	{
		if (softparticleEnabled)
		{
			return m_softparticleShaders[(size_t)type].get();
		}
		else
		{
			return m_lightweightShaders[(size_t)type].get();
		}
	}

	assert(0);
	return nullptr;
}

void RendererImplemented::BeginShader(Shader* shader)
{
	m_currentShader = shader;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::EndShader(Shader* shader)
{
	m_currentShader = nullptr;
}

void RendererImplemented::SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset)
{
	assert(m_currentShader != nullptr);
	assert(m_currentShader->GetVertexConstantBufferSize() >= size + dstOffset);

	auto p = static_cast<uint8_t*>(m_currentShader->GetVertexConstantBuffer()) + dstOffset;
	memcpy(p, data, size);
}

void RendererImplemented::SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset)
{
	assert(m_currentShader != nullptr);
	assert(m_currentShader->GetPixelConstantBufferSize() >= size + dstOffset);

	auto p = static_cast<uint8_t*>(m_currentShader->GetPixelConstantBuffer()) + dstOffset;
	memcpy(p, data, size);
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetTextures(Shader* shader, Effekseer::Backend::TextureRef* textures, int32_t count)
{
	auto& state = m_renderState->GetActiveState();
	
	state.TextureIDs.fill(0);
	for (int32_t i = 0; i < count; i++)
	{
		state.TextureIDs[i] = (textures[i] != nullptr) ? 
			Convert::RIDToInt64(textures[i].DownCast<Texture>()->GetRID()) : 0;
	}
}

void RendererImplemented::ResetRenderState()
{
	m_renderState->GetActiveState().Reset();
	m_renderState->Update(true);
}

Effekseer::Backend::TextureRef RendererImplemented::CreateProxyTexture(EffekseerRenderer::ProxyTextureType type)
{
	return nullptr;
}

void RendererImplemented::DeleteProxyTexture(Effekseer::Backend::TextureRef& texture)
{
	texture = nullptr;
}

} // namespace EffekseerGodot
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
