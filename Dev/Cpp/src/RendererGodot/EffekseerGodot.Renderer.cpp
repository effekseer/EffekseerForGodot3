
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <VisualServer.hpp>
#include <World.hpp>
#include <Node.hpp>
#include <Viewport.hpp>
#include <Mesh.hpp>
#include <Image.hpp>
#include "../Utils/EffekseerGodot.Utils.h"

#include "EffekseerGodot.Renderer.h"
#include "EffekseerGodot.RenderState.h"
#include "EffekseerGodot.RendererImplemented.h"

#include "EffekseerGodot.IndexBuffer.h"
#include "EffekseerGodot.Shader.h"
#include "EffekseerGodot.VertexBuffer.h"
#include "EffekseerGodot.ModelRenderer.h"
#include "EffekseerGodot.RenderResources.h"

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

static constexpr int32_t CUSTOM_DATA_TEXTURE_WIDTH = 256;
static constexpr int32_t CUSTOM_DATA_TEXTURE_HEIGHT = 256;

DynamicTexture::DynamicTexture()
{
}

DynamicTexture::~DynamicTexture()
{
	auto vs = godot::VisualServer::get_singleton();
	vs->free_rid(m_imageTexture);
}

void DynamicTexture::Init(int32_t width, int32_t height)
{
	auto vs = godot::VisualServer::get_singleton();
	godot::Ref<godot::Image> image;
	image.instance();
	image->create(CUSTOM_DATA_TEXTURE_WIDTH, CUSTOM_DATA_TEXTURE_HEIGHT, false, godot::Image::FORMAT_RGBAF);
	m_imageTexture = vs->texture_create_from_image(image, 0);
}

const DynamicTexture::LockedRect* DynamicTexture::Lock(int32_t x, int32_t y, int32_t width, int32_t height)
{
	assert(m_lockedRect.ptr == nullptr);
	assert(m_lockedRect.width == 0 && m_lockedRect.height == 0);

	m_rectData.resize(width * height * sizeof(godot::Color));
	m_lockedRect.ptr = (float*)m_rectData.write().ptr();
	m_lockedRect.pitch = width * sizeof(godot::Color);
	m_lockedRect.x = x;
	m_lockedRect.y = y;
	m_lockedRect.width = width;
	m_lockedRect.height = height;
	return &m_lockedRect;
}

void DynamicTexture::Unlock()
{
	assert(m_lockedRect.ptr != nullptr);
	assert(m_lockedRect.width > 0 && m_lockedRect.height > 0);

	godot::Ref<godot::Image> image;
	image.instance();
	image->create_from_data(m_lockedRect.width, m_lockedRect.height, 
		false, godot::Image::FORMAT_RGBAF, m_rectData);

	auto vs = godot::VisualServer::get_singleton();
	vs->texture_set_data_partial(m_imageTexture, image, 
		0, 0, m_lockedRect.width, m_lockedRect.height, 
		m_lockedRect.x, m_lockedRect.y, 0, 0);

	m_rectData.resize(0);
	m_lockedRect = {};
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

void RenderCommand::DrawSprites(godot::World* world, int32_t priority)
{
	auto vs = godot::VisualServer::get_singleton();

	vs->instance_set_base(m_instance, m_immediate);
	vs->instance_set_scenario(m_instance, world->get_scenario());
	vs->material_set_render_priority(m_material, priority);
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

	m_customData1Texture.Init(CUSTOM_DATA_TEXTURE_WIDTH, CUSTOM_DATA_TEXTURE_HEIGHT);
	m_customData2Texture.Init(CUSTOM_DATA_TEXTURE_WIDTH, CUSTOM_DATA_TEXTURE_HEIGHT);

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
	m_customDataCount = 0;

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
	using namespace EffekseerRenderer;

	assert(m_currentShader != nullptr);

	if (m_renderCount >= m_renderCommands.size())
	{
		return;
	}

	auto vs = godot::VisualServer::get_singleton();
	godot::Node* node = reinterpret_cast<godot::Node*>(GetImpl()->CurrentHandleUserData);
	godot::Viewport* viewport = node->get_viewport();
	if (viewport == nullptr) return;
	godot::World* world = viewport->get_world().ptr();
	if (world == nullptr) return;

	const auto& state = m_standardRenderer->GetState();
	auto& command = m_renderCommands[m_renderCount];

	// Transfer vertex data
	const void* vertexData = GetVertexBuffer()->Refer();
	const void* indexData = GetIndexBuffer()->Refer();

	godot::RID immediate = command.GetImmediate();

	vs->immediate_begin(immediate, godot::Mesh::PRIMITIVE_TRIANGLE_STRIP);

	RendererShaderType shaderType = m_currentShader->GetShaderType();

	if (shaderType == RendererShaderType::Unlit)
	{
		const SimpleVertex* vertices = (const SimpleVertex*)vertexData;
		for (int32_t i = 0; i < spriteCount; i++)
		{
			vs->immediate_color(immediate, godot::Color());
			vs->immediate_uv(immediate, godot::Vector2());
			vs->immediate_vertex(immediate, ConvertVector3(vertices[i * 4 + 0].Pos));

			for (int32_t j = 0; j < 4; j++)
			{
				auto& v = vertices[i * 4 + j];
				vs->immediate_color(immediate, ConvertColor(v.Col));
				vs->immediate_uv(immediate, ConvertUV(v.UV));
				vs->immediate_vertex(immediate, ConvertVector3(v.Pos));
			}

			vs->immediate_color(immediate, godot::Color());
			vs->immediate_uv(immediate, godot::Vector2());
			vs->immediate_vertex(immediate, ConvertVector3(vertices[i * 4 + 3].Pos));
		}
	}
	else if (shaderType == RendererShaderType::BackDistortion || shaderType == RendererShaderType::Lit)
	{
		const LightingVertex* vertices = (const LightingVertex*)vertexData;
		for (int32_t i = 0; i < spriteCount; i++)
		{
			vs->immediate_color(immediate, godot::Color());
			vs->immediate_uv(immediate, godot::Vector2());
			vs->immediate_normal(immediate, godot::Vector3());
			vs->immediate_tangent(immediate, godot::Plane());
			vs->immediate_vertex(immediate, ConvertVector3(vertices[i * 4 + 0].Pos));

			for (int32_t j = 0; j < 4; j++)
			{
				auto& v = vertices[i * 4 + j];
				vs->immediate_color(immediate, ConvertColor(v.Col));
				vs->immediate_uv(immediate, ConvertUV(v.UV));
				vs->immediate_normal(immediate, ConvertVector3(Normalize(UnpackVector3DF(v.Normal))));
				vs->immediate_tangent(immediate, ConvertTangent(Normalize(UnpackVector3DF(v.Tangent))));
				vs->immediate_vertex(immediate, ConvertVector3(v.Pos));
			}

			vs->immediate_color(immediate, godot::Color());
			vs->immediate_uv(immediate, godot::Vector2());
			vs->immediate_normal(immediate, godot::Vector3());
			vs->immediate_tangent(immediate, godot::Plane());
			vs->immediate_vertex(immediate, ConvertVector3(vertices[i * 4 + 3].Pos));
		}
	}
	else if (shaderType == RendererShaderType::Material)
	{
		const int32_t stride = m_standardRenderer->CalculateCurrentStride();
		const int32_t customData1Count = state.CustomData1Count;
		const int32_t customData2Count = state.CustomData2Count;

		if (customData1Count > 0 || customData2Count > 0)
		{
			const int32_t width = CUSTOM_DATA_TEXTURE_WIDTH;
			const int32_t height = (spriteCount * 4 + width - 1) / width;
			float* customData1TexPtr = nullptr;
			float* customData2TexPtr = nullptr;

			if (customData1Count > 0)
			{
				customData1TexPtr = m_customData1Texture.Lock(0, m_customDataCount / width, width, height)->ptr;
			}
			if (customData2Count > 0)
			{
				customData2TexPtr = m_customData2Texture.Lock(0, m_customDataCount / width, width, height)->ptr;
			}

			for (int32_t i = 0; i < spriteCount; i++)
			{
				const uint8_t* vertexPtr = (const uint8_t*)vertexData + i * 4 * stride;

				vs->immediate_color(immediate, godot::Color());
				vs->immediate_uv(immediate, godot::Vector2());
				vs->immediate_uv2(immediate, godot::Vector2());
				vs->immediate_normal(immediate, godot::Vector3());
				vs->immediate_tangent(immediate, godot::Plane());
				vs->immediate_vertex(immediate, ConvertVector3((*(const DynamicVertex*)(vertexPtr)).Pos));

				for (int32_t j = 0; j < 4; j++)
				{
					const godot::Vector2 uv2(
						((float)(m_customDataCount % width) + 0.5f) / width, 
						((float)(m_customDataCount / width) + 0.5f) / width);
					
					auto& v = *(const DynamicVertex*)vertexPtr;
					vs->immediate_color(immediate, ConvertColor(v.Col));
					vs->immediate_uv(immediate, ConvertUV(v.UV));
					vs->immediate_uv2(immediate, uv2);
					vs->immediate_normal(immediate, ConvertVector3(Normalize(UnpackVector3DF(v.Normal))));
					vs->immediate_tangent(immediate, ConvertTangent(Normalize(UnpackVector3DF(v.Tangent))));
					vs->immediate_vertex(immediate, ConvertVector3(v.Pos));

					const float* customData1VertexPtr = (const float*)(vertexPtr + sizeof(DynamicVertex));
					for (int32_t i = 0; i < customData1Count; i++)
					{
						customData1TexPtr[i] = customData1VertexPtr[i];
					}
					for (int32_t i = customData1Count; i < 4; i++)
					{
						customData1TexPtr[i] = 0.0f;
					}
					customData1TexPtr += 4;

					const float* customData2VertexPtr = customData1VertexPtr + customData1Count;
					for (int32_t i = 0; i < customData2Count; i++)
					{
						customData2TexPtr[i] = customData2VertexPtr[i];
					}
					for (int32_t i = customData2Count; i < 4; i++)
					{
						customData2TexPtr[i] = 0.0f;
					}
					customData2TexPtr += 4;

					vertexPtr += stride;
					m_customDataCount++;
				}

				vs->immediate_color(immediate, godot::Color());
				vs->immediate_uv(immediate, godot::Vector2());
				vs->immediate_uv2(immediate, godot::Vector2());
				vs->immediate_normal(immediate, godot::Vector3());
				vs->immediate_tangent(immediate, godot::Plane());
				vs->immediate_vertex(immediate, ConvertVector3((*(const DynamicVertex*)(vertexPtr - stride)).Pos));
			}

			if (customData1Count > 0)
			{
				m_customData1Texture.Unlock();
			}
			if (customData2Count > 0)
			{
				m_customData2Texture.Unlock();
			}
			m_customDataCount = (m_customDataCount + width - 1) / width * width;
		}
		else
		{
			for (int32_t i = 0; i < spriteCount; i++)
			{
				const uint8_t* vertices = (const uint8_t*)vertexData + i * 4 * stride;

				vs->immediate_color(immediate, godot::Color());
				vs->immediate_uv(immediate, godot::Vector2());
				vs->immediate_normal(immediate, godot::Vector3());
				vs->immediate_tangent(immediate, godot::Plane());
				vs->immediate_vertex(immediate, ConvertVector3((*(const DynamicVertex*)(vertices + 0 * stride)).Pos));

				for (int32_t j = 0; j < 4; j++)
				{
					auto& v = *(const DynamicVertex*)(vertices + j * stride);
					vs->immediate_color(immediate, ConvertColor(v.Col));
					vs->immediate_uv(immediate, ConvertUV(v.UV));
					vs->immediate_normal(immediate, ConvertVector3(Normalize(UnpackVector3DF(v.Normal))));
					vs->immediate_tangent(immediate, ConvertTangent(Normalize(UnpackVector3DF(v.Tangent))));
					vs->immediate_vertex(immediate, ConvertVector3(v.Pos));
				}

				vs->immediate_color(immediate, godot::Color());
				vs->immediate_uv(immediate, godot::Vector2());
				vs->immediate_normal(immediate, godot::Vector3());
				vs->immediate_tangent(immediate, godot::Plane());
				vs->immediate_vertex(immediate, ConvertVector3((*(const DynamicVertex*)(vertices + 3 * stride)).Pos));
			}
		}
	}

	vs->immediate_end(immediate);

	// Setup material
	m_currentShader->ApplyToMaterial(command.GetMaterial(), m_renderState->GetActiveState());

	if (state.CustomData1Count > 0)
	{
		vs->material_set_param(command.GetMaterial(), "CustomData1", m_customData1Texture.GetRID());
	}
	if (state.CustomData2Count > 0)
	{
		vs->material_set_param(command.GetMaterial(), "CustomData2", m_customData2Texture.GetRID());
	}

	command.DrawSprites(world, (int32_t)m_renderCount);
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

	godot::Node* node = reinterpret_cast<godot::Node*>(GetImpl()->CurrentHandleUserData);
	godot::Viewport* viewport = node->get_viewport();
	if (viewport == nullptr) return;
	godot::World* world = viewport->get_world().ptr();
	if (world == nullptr) return;

	auto& command = m_renderCommands[m_renderCount];

	m_currentShader->ApplyToMaterial(command.GetMaterial(), m_renderState->GetActiveState());

	auto mesh = m_currentModel.DownCast<Model>()->GetRID();
	command.DrawModel(world, mesh, (int32_t)m_renderCount);
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
			RIDToInt64(textures[i].DownCast<Texture>()->GetRID()) : 0;
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
