
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <VisualServer.hpp>
#include <World.hpp>
#include <Mesh.hpp>
#include <ImageTexture.hpp>
#include "EffekseerGodot3.Renderer.h"
#include "EffekseerGodot3.RenderState.h"
#include "EffekseerGodot3.RendererImplemented.h"

#include "EffekseerGodot3.IndexBuffer.h"
#include "EffekseerGodot3.Shader.h"
#include "EffekseerGodot3.VertexBuffer.h"
#include "EffekseerGodot3.ModelRenderer.h"
#include "EffekseerGodot3.MaterialLoader.h"
#include "EffekseerGodot3.ModelLoader.h"
#include "EffekseerGodot3.Resources.h"
#include "EffekseerGodot3.Utils.h"

#include "EffekseerRenderer.Renderer_Impl.h"
#include "EffekseerRenderer.RibbonRendererBase.h"
#include "EffekseerRenderer.RingRendererBase.h"
#include "EffekseerRenderer.SpriteRendererBase.h"
#include "EffekseerRenderer.TrackRendererBase.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerGodot3
{

namespace StandardShaders
{

namespace Unlit
{
const char code[] = R"(
render_mode unshaded;
uniform mat4 ModelViewMatrix;
uniform sampler2D Texture0 : hint_albedo;
void vertex() {
	MODELVIEW_MATRIX = ModelViewMatrix;
}
void fragment() {
	vec4 texel = texture(Texture0, UV);
	ALBEDO = texel.rgb * COLOR.rgb;
	ALPHA = texel.a * COLOR.a;
}
)";
const Shader::ParamDecl decl[] = {
	{ "ModelViewMatrix", Shader::ParamType::Matrix44, 0, 0 },
};
}

namespace Distortion
{
const char code[] = R"(
render_mode unshaded;
uniform mat4 ModelViewMatrix;
uniform float DistortionIntensity;
uniform sampler2D Texture0 : hint_normal;
uniform sampler2D Texture1 : hint_albedo;
void vertex() {
	MODELVIEW_MATRIX = ModelViewMatrix;
}
void fragment() {
	vec4 texel = texture(Texture0, UV);
    vec2 posU = BINORMAL.xy;
    vec2 posR = TANGENT.xy;
    float xscale = (((texel.x * 2.0) - 1.0) * COLOR.x) * DistortionIntensity * 4.0;
    float yscale = (((texel.y * 2.0) - 1.0) * COLOR.y) * DistortionIntensity * 4.0;
    vec2 uv = posR * xscale + posU * yscale;
    ALBEDO = texture(SCREEN_TEXTURE, SCREEN_UV + uv).xyz;
	ALPHA = COLOR.a * texel.a;
}
)";
const Shader::ParamDecl decl[] = {
	{ "ModelViewMatrix",     Shader::ParamType::Matrix44, 0,  0 },
	{ "DistortionIntensity", Shader::ParamType::Float,    1, 48 },
};
}

namespace Lighting
{
const char code[] = R"(
uniform mat4 ModelViewMatrix;
uniform sampler2D Texture0 : hint_albedo;
void vertex() {
	MODELVIEW_MATRIX = ModelViewMatrix;
}
void fragment() {
	vec4 texel = texture(Texture0, UV);
	ALBEDO = texel.rgb * COLOR.rgb;
	ALPHA = texel.a * COLOR.a;
}
)";
const Shader::ParamDecl decl[] = {
	{ "ModelViewMatrix", Shader::ParamType::Matrix44, 0, 0 },
};
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

inline godot::Plane ConvertTangent(const EffekseerRenderer::VertexFloat3& n, const EffekseerRenderer::VertexFloat3& t)
{
	return godot::Plane(t.X, t.Y, t.Z, 1.0f);
}

void RenderCommand::DrawSprites(godot::World* world, 
	const void* vertexData, const void* indexData, int32_t spriteCount, 
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
		const VertexDistortion* vertices = (const VertexDistortion*)vertexData;
		for (int32_t i = 0; i < spriteCount; i++)
		{
			for (int32_t j = 0; j < 6; j++)
			{
				auto& v = vertices[indeces[i * 6 + j]];
				vs->immediate_color(m_immediate, ConvertColor(v.Col));
				vs->immediate_uv(m_immediate, ConvertUV(v.UV));

				VertexFloat3 tangent = Normalize(v.Tangent);
				VertexFloat3 binormal = Normalize(v.Binormal);
				VertexFloat3 normal = Normalize(Cross(tangent, binormal));
				vs->immediate_normal(m_immediate, ConvertVector3(normal));
				vs->immediate_tangent(m_immediate, ConvertTangent(normal, tangent));
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

				VertexFloat3 normal = ConvertPackedVector3(v.Normal);
				VertexFloat3 tangent = ConvertPackedVector3(v.Tangent);
				vs->immediate_normal(m_immediate, ConvertVector3(normal));
				vs->immediate_tangent(m_immediate, ConvertTangent(normal, tangent));
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
#define COUNT_OF(list) (sizeof(list) / sizeof(list[0]))
		using namespace EffekseerGodot3::StandardShaders;
		using namespace EffekseerRenderer;

		m_shader_unlit = Shader::Create("Unlit", Unlit::code, 
			RendererShaderType::Unlit, Unlit::decl, COUNT_OF(Unlit::decl));
		m_shader_unlit->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
		m_shader_unlit->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererPixelBuffer));

		m_shader_distortion = Shader::Create("Distortion", Distortion::code, 
			RendererShaderType::BackDistortion, Distortion::decl, COUNT_OF(Distortion::decl));
		m_shader_distortion->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
		m_shader_distortion->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererDistortionPixelBuffer));

		m_shader_lighting = Shader::Create("Lighting", Lighting::code, 
			RendererShaderType::Lit, Lighting::decl, COUNT_OF(Lighting::decl));
		m_shader_lighting->SetVertexConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererVertexBuffer));
		m_shader_lighting->SetPixelConstantBufferSize(sizeof(EffekseerRenderer::StandardRendererLitPixelBuffer));

		//m_shader_advanced_unlit = Shader::Create("Advanced Unlit", Unlit::code, 
		//	RendererShaderType::AdvancedUnlit, Unlit::decl, COUNT_OF(Unlit::decl));
		//m_shader_advanced_unlit->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4 + sizeof(float) * 4);
		//m_shader_advanced_unlit->SetPixelConstantBufferSize(sizeof(float) * 4 * 6);
		//
		//m_shader_advanced_distortion = Shader::Create("Advanced Distortion", Unlit::code, 
		//	RendererShaderType::AdvancedBackDistortion, Unlit::decl, COUNT_OF(Unlit::decl));
		//m_shader_advanced_distortion->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4 + sizeof(float) * 4);
		//m_shader_advanced_distortion->SetPixelConstantBufferSize(sizeof(float) * 4 * 5);
		//
		//m_shader_advanced_lighting = Shader::Create("Advanced Lighting", Unlit::code, 
		//	RendererShaderType::AdvancedLit, Unlit::decl, COUNT_OF(Unlit::decl));
		//m_shader_advanced_lighting->SetVertexConstantBufferSize(sizeof(Effekseer::Matrix44) * 2 + sizeof(float) * 4 + sizeof(float) * 4);
		//m_shader_advanced_lighting->SetPixelConstantBufferSize(sizeof(float) * 4 * 9);
#undef COUNT_OF
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
Effekseer::TextureData* RendererImplemented::GetBackground()
{
	return &m_background;
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

	m_currentShader->ApplyToMaterial(
		m_renderCommands[m_renderCount].GetMaterial(), 
		m_renderState->GetActiveState());

	const void* vertexData = GetVertexBuffer()->Refer();
	const void* indexData = GetIndexBuffer()->Refer();

	m_renderCommands[m_renderCount].DrawSprites(m_world, vertexData, indexData, 
		spriteCount, m_currentShader->GetShaderType(), (int32_t)m_renderCount);
	m_renderCount++;

	impl->drawcallCount++;
	impl->drawvertexCount += spriteCount * 4;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void RendererImplemented::SetModel(Effekseer::Model* model)
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

	auto mesh = ((const ModelResource*)m_currentModel)->GetRID();
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
	if (type == ::EffekseerRenderer::RendererShaderType::AdvancedBackDistortion)
	{
		return m_shader_advanced_distortion.get();
	}
	else if (type == ::EffekseerRenderer::RendererShaderType::AdvancedLit)
	{
		return m_shader_advanced_lighting.get();
	}
	else if (type == ::EffekseerRenderer::RendererShaderType::AdvancedUnlit)
	{
		return m_shader_advanced_unlit.get();
	}
	else if (type == ::EffekseerRenderer::RendererShaderType::BackDistortion)
	{
		return m_shader_distortion.get();
	}
	else if (type == ::EffekseerRenderer::RendererShaderType::Lit)
	{
		return m_shader_lighting.get();
	}
	else if (type == ::EffekseerRenderer::RendererShaderType::Unlit)
	{
		return m_shader_unlit.get();
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
void RendererImplemented::SetTextures(Shader* shader, Effekseer::TextureData** textures, int32_t count)
{
	auto& state = m_renderState->GetActiveState();
	
	state.TextureIDs.fill(0);
	for (int32_t i = 0; i < count; i++)
	{
		state.TextureIDs[i] = (textures[i]) ? (uint64_t)textures[i]->UserID : 0;
	}
}

void RendererImplemented::ResetRenderState()
{
	m_renderState->GetActiveState().Reset();
	m_renderState->Update(true);
}

Effekseer::TextureData* RendererImplemented::CreateProxyTexture(EffekseerRenderer::ProxyTextureType type)
{
	return nullptr;
}

void RendererImplemented::DeleteProxyTexture(Effekseer::TextureData* data)
{
}

} // namespace EffekseerGodot3
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
