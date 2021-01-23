#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.RenderStateBase.h"
#include "EffekseerRenderer.StandardRenderer.h"
#include "EffekseerGodot3.Base.h"
#include "EffekseerGodot3.Renderer.h"
#include "EffekseerGodot3.RenderState.h"
#include "EffekseerGodot3.VertexBuffer.h"
#include "EffekseerGodot3.IndexBuffer.h"

namespace EffekseerGodot3
{

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	描画コマンド
*/
class RenderCommand {
public:
	RenderCommand();
	~RenderCommand();
	void Reset();
	void DrawSprites(godot::World* world, 
		const void* vertexData, const void* indexData, int32_t spriteCount, 
		EffekseerRenderer::RendererShaderType shaderType, int32_t priority);
	void DrawModel(godot::World* world, godot::RID mesh, int32_t priority);

	godot::RID GetImmediate() { return m_immediate; }
	godot::RID GetInstance() { return m_instance; }
	godot::RID GetMaterial() { return m_material; }

private:
	godot::RID m_immediate;
	godot::RID m_instance;
	godot::RID m_material;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class RendererImplemented;
using RendererImplementedRef = Effekseer::RefPtr<RendererImplemented>;

/**
	@brief	描画クラス
*/
class RendererImplemented
	: public Renderer
	, public Effekseer::ReferenceObject
{
	using StandardRenderer = EffekseerRenderer::StandardRenderer<RendererImplemented, Shader>;

private:
	VertexBufferRef m_vertexBuffer;
	IndexBufferRef m_indexBuffer;
	IndexBufferRef m_indexBufferForWireframe;
	int32_t m_squareMaxCount = 0;

	std::array<std::unique_ptr<Shader>, 6> m_lightweightShaders;
	std::array<std::unique_ptr<Shader>, 6> m_softparticleShaders;

	Shader* m_currentShader = nullptr;
	godot::World* m_world = nullptr;

	std::vector<RenderCommand> m_renderCommands;
	size_t m_renderCount = 0;

	Effekseer::ModelRef m_currentModel = nullptr;
	
	std::unique_ptr<StandardRenderer> m_standardRenderer;
	std::unique_ptr<RenderState> m_renderState;

	Effekseer::Backend::TextureRef m_background;

public:
	/**
		@brief	コンストラクタ
	*/
	RendererImplemented(int32_t squareMaxCount);

	/**
		@brief	デストラクタ
	*/
	~RendererImplemented();

	void OnLostDevice() override {}

	void OnResetDevice() override {}

	/**
		@brief	初期化
	*/
	bool Initialize(int32_t drawMaxCount);

	void Destroy();

	void SetWorld(godot::World* world) { m_world = world; }

	/**
	@brief	ステートを復帰するかどうかのフラグを設定する。
	*/
	void SetRestorationOfStatesFlag(bool flag) {}

	/**
		@brief	状態リセット
	*/
	void ResetState();

	/**
		@brief	描画開始
	*/
	bool BeginRendering();

	/**
		@brief	描画終了
	*/
	bool EndRendering();

	/**
		@brief	頂点バッファ取得
	*/
	VertexBuffer* GetVertexBuffer();

	/**
		@brief	インデックスバッファ取得
	*/
	IndexBuffer* GetIndexBuffer();

	/**
		@brief	最大描画スプライト数
	*/
	int32_t GetSquareMaxCount() const { return m_squareMaxCount; }

	::EffekseerRenderer::RenderStateBase* GetRenderState() { return m_renderState.get(); } 

	/**
		@brief	スプライトレンダラーを生成する。
	*/
	::Effekseer::SpriteRendererRef CreateSpriteRenderer();

	/**
		@brief	リボンレンダラーを生成する。
	*/
	::Effekseer::RibbonRendererRef CreateRibbonRenderer();

	/**
		@brief	リングレンダラーを生成する。
	*/
	::Effekseer::RingRendererRef CreateRingRenderer();

	/**
		@brief	モデルレンダラーを生成する。
	*/
	::Effekseer::ModelRendererRef CreateModelRenderer();

	/**
		@brief	軌跡レンダラーを生成する。
	*/
	::Effekseer::TrackRendererRef CreateTrackRenderer();

	::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::FileInterface* fileInterface = NULL) override { return nullptr; }

	::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::FileInterface* fileInterface = NULL) override { return nullptr; }

	::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::FileInterface* fileInterface = nullptr) override  { return nullptr; }

	/**
		@brief	背景を取得する。
	*/
	const Effekseer::Backend::TextureRef& GetBackground() override;

	EffekseerRenderer::DistortingCallback* GetDistortingCallback() override;

	void SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback) override;

	StandardRenderer* GetStandardRenderer() { return m_standardRenderer.get(); }

	void SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t size) {}
	void SetIndexBuffer(IndexBuffer* indexBuffer) {}

	void SetVertexBuffer(Effekseer::Backend::VertexBufferRef vertexBuffer, int32_t size) {}
	void SetIndexBuffer(Effekseer::Backend::IndexBufferRef indexBuffer) {}

	void SetLayout(Shader* shader);
	void DrawSprites(int32_t spriteCount, int32_t vertexOffset);
	void SetModel(Effekseer::ModelRef model);
	void DrawPolygon(int32_t vertexCount, int32_t indexCount);
	void DrawPolygonInstanced(int32_t vertexCount, int32_t indexCount, int32_t instanceCount);

	Shader* GetShader(::EffekseerRenderer::RendererShaderType type) const;
	void BeginShader(Shader* shader);
	void EndShader(Shader* shader);

	void SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset);
	void SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset);
	void SetTextures(Shader* shader, Effekseer::Backend::TextureRef* textures, int32_t count);
	void ResetRenderState();

	Effekseer::Backend::TextureRef CreateProxyTexture(EffekseerRenderer::ProxyTextureType type) override;

	void DeleteProxyTexture(Effekseer::Backend::TextureRef& texture) override;

	virtual int GetRef() { return Effekseer::ReferenceObject::GetRef(); }
	virtual int AddRef() { return Effekseer::ReferenceObject::AddRef(); }
	virtual int Release() { return Effekseer::ReferenceObject::Release();; }
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerGodot3
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
