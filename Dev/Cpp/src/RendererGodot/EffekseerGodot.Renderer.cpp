
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Godot.hpp>
#include <VisualServer.hpp>
#include <World.hpp>
#include <Node2D.hpp>
#include <Spatial.hpp>
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
namespace CanvasItem
{
#include "Shaders/Particle2D.inl"
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
namespace CanvasItem
{
#include "Shaders/Particle2D.inl"
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
namespace CanvasItem
{
#include "Shaders/Particle2D.inl"
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
	image->create(width, height, false, godot::Image::FORMAT_RGBAF);
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

inline godot::Color ConvertColor(const EffekseerRenderer::VertexColor& color)
{
	return godot::Color(color.R / 255.0f, color.G / 255.0f, color.B / 255.0f, color.A / 255.0f);
}

inline godot::Vector2 ConvertUV(const float uv[])
{
	return godot::Vector2(uv[0], uv[1]);
}

inline godot::Vector2 ConvertUV(const Effekseer::Vector2D& uv)
{
	return godot::Vector2(uv.X, uv.Y);
}

inline godot::Vector3 ConvertVector3(const EffekseerRenderer::VertexFloat3& v)
{
	return godot::Vector3(v.X, v.Y, v.Z);
}

inline godot::Vector2 ConvertVector2(const EffekseerRenderer::VertexFloat3& v)
{
	return godot::Vector2(v.X, -v.Y); // invert Y
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

EffekseerGodot::RenderCommand2D::RenderCommand2D()
{
	auto vs = godot::VisualServer::get_singleton();
	m_canvasItem = vs->canvas_item_create();
	m_material = vs->material_create();
}

EffekseerGodot::RenderCommand2D::~RenderCommand2D()
{
	auto vs = godot::VisualServer::get_singleton();
	vs->free_rid(m_canvasItem);
	vs->free_rid(m_material);
}

void EffekseerGodot::RenderCommand2D::Reset()
{
	auto vs = godot::VisualServer::get_singleton();
	vs->canvas_item_clear(m_canvasItem);
	vs->canvas_item_set_parent(m_canvasItem, godot::RID());
}

void EffekseerGodot::RenderCommand2D::DrawSprites(godot::RID parentCanvasItem)
{
	auto vs = godot::VisualServer::get_singleton();

	vs->canvas_item_set_parent(m_canvasItem, parentCanvasItem);
	vs->canvas_item_set_material(m_canvasItem, m_material);
}

void RenderCommand2D::DrawModel(godot::RID parentCanvasItem, godot::RID mesh)
{
	auto vs = godot::VisualServer::get_singleton();

	vs->canvas_item_set_parent(m_canvasItem, parentCanvasItem);
	vs->canvas_item_add_mesh(m_canvasItem, mesh);
	vs->canvas_item_set_material(m_canvasItem, m_material);
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
	{
		using namespace EffekseerRenderer;
		using namespace EffekseerGodot::StandardShaders;

		m_shaders[(size_t)RendererShaderType::Unlit] = Shader::Create("Sprite_Basic_Unlit", RendererShaderType::Unlit);
		m_shaders[(size_t)RendererShaderType::Unlit]->SetVertexConstantBufferSize(sizeof(StandardRendererVertexBuffer));
		m_shaders[(size_t)RendererShaderType::Unlit]->SetPixelConstantBufferSize(sizeof(PixelConstantBuffer));
		m_shaders[(size_t)RendererShaderType::Unlit]->Compile(Shader::RenderType::SpatialLightweight, Unlit::Lightweight::code, Unlit::Lightweight::decl);
		m_shaders[(size_t)RendererShaderType::Unlit]->Compile(Shader::RenderType::SpatialDepthFade, Unlit::SoftParticle::code, Unlit::SoftParticle::decl);
		m_shaders[(size_t)RendererShaderType::Unlit]->Compile(Shader::RenderType::CanvasItem, Unlit::CanvasItem::code, Unlit::CanvasItem::decl);

		m_shaders[(size_t)RendererShaderType::Lit] = Shader::Create("Sprite_Basic_Lighting", RendererShaderType::Lit);
		m_shaders[(size_t)RendererShaderType::Lit]->SetVertexConstantBufferSize(sizeof(StandardRendererVertexBuffer));
		m_shaders[(size_t)RendererShaderType::Lit]->SetPixelConstantBufferSize(sizeof(PixelConstantBuffer));
		m_shaders[(size_t)RendererShaderType::Lit]->Compile(Shader::RenderType::SpatialLightweight, Lighting::Lightweight::code, Lighting::Lightweight::decl);
		m_shaders[(size_t)RendererShaderType::Lit]->Compile(Shader::RenderType::SpatialDepthFade, Lighting::SoftParticle::code, Lighting::SoftParticle::decl);
		m_shaders[(size_t)RendererShaderType::Lit]->Compile(Shader::RenderType::CanvasItem, Lighting::CanvasItem::code, Lighting::CanvasItem::decl);

		m_shaders[(size_t)RendererShaderType::BackDistortion] = Shader::Create("Sprite_Basic_Distortion", RendererShaderType::BackDistortion);
		m_shaders[(size_t)RendererShaderType::BackDistortion]->SetVertexConstantBufferSize(sizeof(StandardRendererVertexBuffer));
		m_shaders[(size_t)RendererShaderType::BackDistortion]->SetPixelConstantBufferSize(sizeof(PixelConstantBuffer));
		m_shaders[(size_t)RendererShaderType::BackDistortion]->Compile(Shader::RenderType::SpatialLightweight, Distortion::Lightweight::code, Distortion::Lightweight::decl);
		m_shaders[(size_t)RendererShaderType::BackDistortion]->Compile(Shader::RenderType::SpatialDepthFade, Distortion::SoftParticle::code, Distortion::SoftParticle::decl);
		m_shaders[(size_t)RendererShaderType::BackDistortion]->Compile(Shader::RenderType::CanvasItem, Distortion::CanvasItem::code, Distortion::CanvasItem::decl);
	}

	m_renderCommands.resize((size_t)drawMaxCount);
	m_renderCommand2Ds.resize((size_t)drawMaxCount);

	m_standardRenderer.reset(new StandardRenderer(this));

	m_customData1Texture.Init(CUSTOM_DATA_TEXTURE_WIDTH, CUSTOM_DATA_TEXTURE_HEIGHT);
	m_customData2Texture.Init(CUSTOM_DATA_TEXTURE_WIDTH, CUSTOM_DATA_TEXTURE_HEIGHT);
	m_uvTangentTexture.Init(CUSTOM_DATA_TEXTURE_WIDTH, CUSTOM_DATA_TEXTURE_HEIGHT);

	return true;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::Destroy()
{
	m_renderCommands.clear();
	m_renderCommand2Ds.clear();
}

void RendererImplemented::ResetState()
{
	for (size_t i = 0; i < m_renderCount; i++)
	{
		m_renderCommands[i].Reset();
	}
	m_renderCount = 0;
	m_customDataCount = 0;

	for (size_t i = 0; i < m_renderCount2D; i++)
	{
		m_renderCommand2Ds[i].Reset();
	}
	m_renderCount2D = 0;
	m_uvTangentCount = 0;
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
	auto ringRenderer = Effekseer::MakeRefPtr<::EffekseerRenderer::RingRendererBase<RendererImplemented, false>>(this);
	ringRenderer->SetFasterSngleRingModeEnabled(false);
	return ringRenderer;
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

	auto vs = godot::VisualServer::get_singleton();

	const auto& state = m_standardRenderer->GetState();
	godot::Object* godotObj = reinterpret_cast<godot::Object*>(GetImpl()->CurrentHandleUserData);
	
	if (auto node3d = godot::Object::cast_to<godot::Spatial>(godotObj)) {
		if (m_renderCount >= m_renderCommands.size()) return;

		const bool softparticleEnabled = !(
			state.SoftParticleDistanceFar == 0.0f &&
			state.SoftParticleDistanceNear == 0.0f &&
			state.SoftParticleDistanceNearOffset == 0.0f);
		const Shader::RenderType renderType = (softparticleEnabled) ? 
			Shader::RenderType::SpatialDepthFade : Shader::RenderType::SpatialLightweight;

		auto& command = m_renderCommands[m_renderCount];

		// Transfer vertex data
		TransferVertexToImmediate3D(command.GetImmediate(), GetVertexBuffer()->Refer(), spriteCount, state);

		// Setup material
		m_currentShader->ApplyToMaterial(renderType, command.GetMaterial(), m_renderState->GetActiveState());

		if (state.CustomData1Count > 0)
		{
			vs->material_set_param(command.GetMaterial(), "CustomData1", m_customData1Texture.GetRID());
		}
		if (state.CustomData2Count > 0)
		{
			vs->material_set_param(command.GetMaterial(), "CustomData2", m_customData2Texture.GetRID());
		}

		command.DrawSprites(node3d->get_world().ptr(), (int32_t)m_renderCount);
		m_renderCount++;

	} else if (auto node2d = godot::Object::cast_to<godot::Node2D>(godotObj)) {
		if (m_renderCount2D >= m_renderCommand2Ds.size()) return;

		auto& command = m_renderCommand2Ds[m_renderCount2D];

		// Transfer vertex data
		TransferVertexToCanvasItem2D(command.GetCanvasItem(), GetVertexBuffer()->Refer(), spriteCount, state);

		// Setup material
		m_currentShader->ApplyToMaterial(Shader::RenderType::CanvasItem, command.GetMaterial(), m_renderState->GetActiveState());

		if (m_currentShader->GetShaderType() == EffekseerRenderer::RendererShaderType::Lit || 
			m_currentShader->GetShaderType() == EffekseerRenderer::RendererShaderType::BackDistortion)
		{
			vs->material_set_param(command.GetMaterial(), "UVTangentTexture", m_uvTangentTexture.GetRID());
		}

		command.DrawSprites(node2d->get_canvas_item());
		m_renderCount2D++;
	}

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

	auto vs = godot::VisualServer::get_singleton();

	const auto& state = m_standardRenderer->GetState();
	godot::Object* godotObj = reinterpret_cast<godot::Object*>(GetImpl()->CurrentHandleUserData);

	if (auto node3d = godot::Object::cast_to<godot::Spatial>(godotObj)) {
		if (m_renderCount >= m_renderCommands.size()) return;

		const bool softparticleEnabled = !(
			state.SoftParticleDistanceFar == 0.0f &&
			state.SoftParticleDistanceNear == 0.0f &&
			state.SoftParticleDistanceNearOffset == 0.0f);
		const Shader::RenderType renderType = (softparticleEnabled) ? 
			Shader::RenderType::SpatialDepthFade : Shader::RenderType::SpatialLightweight;

		auto& command = m_renderCommands[m_renderCount];

		// Setup material
		m_currentShader->ApplyToMaterial(renderType, command.GetMaterial(), m_renderState->GetActiveState());

		auto mesh = m_currentModel.DownCast<Model>()->GetRID();
		command.DrawModel(node3d->get_world().ptr(), mesh, (int32_t)m_renderCount);
		m_renderCount++;

	} else if (auto node2d = godot::Object::cast_to<godot::Node2D>(godotObj)) {
		if (m_renderCount2D >= m_renderCommand2Ds.size()) return;

		auto& command = m_renderCommand2Ds[m_renderCount2D];

		// Transfer vertex data
		TransferModelToCanvasItem2D(command.GetCanvasItem(), m_currentModel.Get(), state);
		
		// Setup material
		m_currentShader->ApplyToMaterial(Shader::RenderType::CanvasItem, command.GetMaterial(), m_renderState->GetActiveState());

		//auto mesh = m_currentModel.DownCast<Model>()->GetRID();
		//command.DrawModel(node2d->get_canvas_item(), mesh);
		command.DrawSprites(node2d->get_canvas_item());
		m_renderCount2D++;
	}

	impl->drawcallCount++;
	impl->drawvertexCount += vertexCount;
}

void RendererImplemented::DrawPolygonInstanced(int32_t vertexCount, int32_t indexCount, int32_t instanceCount)
{
	assert(m_currentModel != nullptr);
	impl->drawcallCount++;
	impl->drawvertexCount += vertexCount * instanceCount;
}

Shader* RendererImplemented::GetShader(::EffekseerRenderer::RendererShaderType type)
{
	if ((size_t)type >= (size_t)EffekseerRenderer::RendererShaderType::Unlit && 
		(size_t)type <= (size_t)EffekseerRenderer::RendererShaderType::AdvancedBackDistortion)
	{
		return m_shaders[(size_t)type].get();
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

void RendererImplemented::TransferVertexToImmediate3D(godot::RID immediate, 
	const void* vertexData, int32_t spriteCount, const EffekseerRenderer::StandardRendererState& state)
{
	using namespace EffekseerRenderer;

	auto vs = godot::VisualServer::get_singleton();

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
		const int32_t stride = sizeof(DynamicVertex) + (state.CustomData1Count + state.CustomData2Count) * sizeof(float);
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

					if (customData1Count)
					{
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
					}
					if (customData2Count > 0)
					{
						const float* customData2VertexPtr = (const float*)(vertexPtr + sizeof(DynamicVertex)) + customData1Count;
						for (int32_t i = 0; i < customData2Count; i++)
						{
							customData2TexPtr[i] = customData2VertexPtr[i];
						}
						for (int32_t i = customData2Count; i < 4; i++)
						{
							customData2TexPtr[i] = 0.0f;
						}
						customData2TexPtr += 4;
					}

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
}

void RendererImplemented::TransferVertexToCanvasItem2D(godot::RID canvas_item, 
	const void* vertexData, int32_t spriteCount, 
	const EffekseerRenderer::StandardRendererState& state)
{
	using namespace EffekseerRenderer;

	auto vs = godot::VisualServer::get_singleton();

	godot::PoolIntArray indexArray;
	godot::PoolVector2Array pointArray;
	godot::PoolColorArray colorArray;
	godot::PoolVector2Array uvArray;

	indexArray.resize(spriteCount * 6);
	pointArray.resize(spriteCount * 4);
	colorArray.resize(spriteCount * 4);
	uvArray.resize(spriteCount * 4);

	{
		int* indices = indexArray.write().ptr();

		for (int32_t i = 0; i < spriteCount; i++)
		{
			indices[i * 6 + 0] = i * 4 + 0;
			indices[i * 6 + 1] = i * 4 + 1;
			indices[i * 6 + 2] = i * 4 + 2;
			indices[i * 6 + 3] = i * 4 + 3;
			indices[i * 6 + 4] = i * 4 + 2;
			indices[i * 6 + 5] = i * 4 + 1;
		}
	}

	RendererShaderType shaderType = m_currentShader->GetShaderType();

	if (shaderType == RendererShaderType::Unlit)
	{
		godot::Vector2* points = pointArray.write().ptr();
		godot::Color* colors = colorArray.write().ptr();
		godot::Vector2* uvs = uvArray.write().ptr();

		const SimpleVertex* vertices = (const SimpleVertex*)vertexData;
		for (int32_t i = 0; i < spriteCount; i++)
		{
			for (int32_t j = 0; j < 4; j++)
			{
				auto& v = vertices[i * 4 + j];
				points[i * 4 + j] = ConvertVector2(v.Pos);
				colors[i * 4 + j] = ConvertColor(v.Col);
				uvs[i * 4 + j] = ConvertUV(v.UV);
			}
		}
	}
	else if (shaderType == RendererShaderType::BackDistortion || shaderType == RendererShaderType::Lit)
	{
		godot::Vector2* points = pointArray.write().ptr();
		godot::Color* colors = colorArray.write().ptr();
		godot::Vector2* uvs = uvArray.write().ptr();

		const int32_t width = CUSTOM_DATA_TEXTURE_WIDTH;
		const int32_t height = (spriteCount * 4 + width - 1) / width;
		float* uvtTexPtr = m_uvTangentTexture.Lock(0, m_uvTangentCount / width, width, height)->ptr;

		const LightingVertex* vertices = (const LightingVertex*)vertexData;
		for (int32_t i = 0; i < spriteCount; i++)
		{
			for (int32_t j = 0; j < 4; j++)
			{
				const godot::Vector2 uv2(
					((float)(m_uvTangentCount % width) + 0.5f) / width, 
					((float)(m_uvTangentCount / width) + 0.5f) / width);

				auto& v = vertices[i * 4 + j];
				points[i * 4 + j] = ConvertVector2(v.Pos);
				colors[i * 4 + j] = ConvertColor(v.Col);
				uvs[i * 4 + j] = uv2;

				auto tangent = UnpackVector3DF(v.Tangent);
				uvtTexPtr[0] = v.UV[0];
				uvtTexPtr[1] = v.UV[1];
				uvtTexPtr[2] = tangent.X;
				uvtTexPtr[3] = -tangent.Y;
				uvtTexPtr += 4;
				m_uvTangentCount++;
			}
		}

		m_uvTangentTexture.Unlock();
		m_uvTangentCount = (m_uvTangentCount + width - 1) / width * width;
	}
	else if (shaderType == RendererShaderType::Material)
	{
	}

	vs->canvas_item_add_triangle_array(canvas_item, indexArray, pointArray, colorArray, uvArray);
}

void RendererImplemented::TransferModelToCanvasItem2D(godot::RID canvas_item, 
	Effekseer::Model* model, const EffekseerRenderer::StandardRendererState& state)
{
	using namespace EffekseerRenderer;

	auto vs = godot::VisualServer::get_singleton();

	const int32_t vertexCount = model->GetVertexCount();
	const Effekseer::Model::Vertex* vertexData = model->GetVertexes();

	const int32_t faceCount = model->GetFaceCount();
	const Effekseer::Model::Face* faceData = model->GetFaces();

	godot::PoolIntArray indexArray;
	godot::PoolVector2Array pointArray;
	godot::PoolColorArray colorArray;
	godot::PoolVector2Array uvArray;

	indexArray.resize(faceCount * 3);
	pointArray.resize(vertexCount);
	colorArray.resize(vertexCount);
	uvArray.resize(vertexCount);

	{
		int* indices = indexArray.write().ptr();

		for (int32_t i = 0; i < faceCount; i++)
		{
			indices[i * 3 + 0] = faceData[i].Indexes[0];
			indices[i * 3 + 1] = faceData[i].Indexes[1];
			indices[i * 3 + 2] = faceData[i].Indexes[2];
		}
	}

	const uint8_t* constantBuffer = (const uint8_t*)m_currentShader->GetVertexConstantBuffer();
	const Effekseer::Matrix44 worldMatrix = *(Effekseer::Matrix44*)(constantBuffer + 64);

	//RendererShaderType shaderType = m_currentShader->GetShaderType();

	//if (shaderType == RendererShaderType::Unlit)
	{
		godot::Vector2* points = pointArray.write().ptr();
		godot::Color* colors = colorArray.write().ptr();
		godot::Vector2* uvs = uvArray.write().ptr();

		for (int32_t i = 0; i < vertexCount; i++)
		{
			auto& v = vertexData[i];
			Effekseer::Vector3D pos;
			Effekseer::Vector3D::Transform(pos, v.Position, worldMatrix);
			points[i] = ConvertVector2(pos);
			colors[i] = ConvertColor(v.VColor);
			uvs[i] = ConvertUV(v.UV);
		}
	}

	vs->canvas_item_add_triangle_array(canvas_item, indexArray, pointArray, colorArray, uvArray);
}

} // namespace EffekseerGodot
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
