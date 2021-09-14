#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.RenderStateBase.h"
#include "EffekseerRenderer.StandardRenderer.h"
#include "EffekseerGodot.Base.h"
#include "EffekseerGodot.Renderer.h"
#include "EffekseerGodot.RenderState.h"
#include "EffekseerGodot.VertexBuffer.h"
#include "EffekseerGodot.IndexBuffer.h"

namespace godot
{
class Node2D;
}

namespace EffekseerGodot
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
	void DrawSprites(godot::World* world, int32_t priority);
	void DrawModel(godot::World* world, godot::RID mesh, int32_t priority);

	godot::RID GetImmediate() { return m_immediate; }
	godot::RID GetInstance() { return m_instance; }
	godot::RID GetMaterial() { return m_material; }

private:
	godot::RID m_immediate;
	godot::RID m_instance;
	godot::RID m_material;
};

/**
	@brief	2D描画コマンド
*/
class RenderCommand2D {
public:
	RenderCommand2D();
	~RenderCommand2D();

	void Reset();
	void DrawSprites(godot::Node2D* parent);
	void DrawModel(godot::Node2D* parent, godot::RID mesh);

	godot::RID GetCanvasItem() { return m_canvasItem; }
	godot::RID GetMaterial() { return m_material; }

private:
	godot::RID m_canvasItem;
	godot::RID m_material;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------

/**
	@brief	ダイナミックテクスチャ
*/
class DynamicTexture
{
public:
	struct LockedRect {
		float* ptr;
		size_t pitch;
		int32_t x;
		int32_t y;
		int32_t width;
		int32_t height;
	};

	DynamicTexture();
	~DynamicTexture();
	void Init(int32_t width, int32_t height);
	const LockedRect* Lock(int32_t x, int32_t y, int32_t width, int32_t height);
	void Unlock();

	godot::RID GetRID() { return m_imageTexture; }

private:
	godot::RID m_imageTexture;
	godot::PoolByteArray m_rectData;
	LockedRect m_lockedRect{};
};

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
	int32_t m_vertexStride = 0;

	std::array<std::unique_ptr<Shader>, 6> m_shaders;

	Shader* m_currentShader = nullptr;
	godot::World* m_world = nullptr;

	std::vector<RenderCommand> m_renderCommands;
	size_t m_renderCount = 0;
	std::vector<RenderCommand2D> m_renderCommand2Ds;
	size_t m_renderCount2D = 0;

	struct ModelRenderState {
		Effekseer::ModelRef model = nullptr;
		bool softparticleEnabled = false;
	};
	ModelRenderState m_modelRenderState;

	DynamicTexture m_customData1Texture;
	DynamicTexture m_customData2Texture;
	DynamicTexture m_uvTangentTexture;
	int32_t m_vertexTextureOffset = 0;

	std::unique_ptr<StandardRenderer> m_standardRenderer;
	std::unique_ptr<RenderState> m_renderState;

	Effekseer::Backend::TextureRef m_background;
	Effekseer::Backend::TextureRef m_depth;

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

	/**
	@brief	ステートを復帰するかどうかのフラグを設定する。
	*/
	void SetRestorationOfStatesFlag(bool flag) override {}

	/**
		@brief	状態リセット
	*/
	void ResetState() override;

	/**
		@brief	描画開始
	*/
	bool BeginRendering() override;

	/**
		@brief	描画終了
	*/
	bool EndRendering() override;

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
	int32_t GetSquareMaxCount() const override { return m_squareMaxCount; }

	::EffekseerRenderer::RenderStateBase* GetRenderState() { return m_renderState.get(); } 

	/**
		@brief	スプライトレンダラーを生成する。
	*/
	::Effekseer::SpriteRendererRef CreateSpriteRenderer() override;

	/**
		@brief	リボンレンダラーを生成する。
	*/
	::Effekseer::RibbonRendererRef CreateRibbonRenderer() override;

	/**
		@brief	リングレンダラーを生成する。
	*/
	::Effekseer::RingRendererRef CreateRingRenderer() override;

	/**
		@brief	モデルレンダラーを生成する。
	*/
	::Effekseer::ModelRendererRef CreateModelRenderer() override;

	/**
		@brief	軌跡レンダラーを生成する。
	*/
	::Effekseer::TrackRendererRef CreateTrackRenderer() override;

	::Effekseer::TextureLoaderRef CreateTextureLoader(::Effekseer::FileInterface* fileInterface = NULL) override { return nullptr; }

	::Effekseer::ModelLoaderRef CreateModelLoader(::Effekseer::FileInterface* fileInterface = NULL) override { return nullptr; }

	::Effekseer::MaterialLoaderRef CreateMaterialLoader(::Effekseer::FileInterface* fileInterface = nullptr) override { return nullptr; }

	/**
		@brief	背景を取得する。
	*/
	const Effekseer::Backend::TextureRef& GetBackground() override;

	EffekseerRenderer::DistortingCallback* GetDistortingCallback() override;

	void SetDistortingCallback(EffekseerRenderer::DistortingCallback* callback) override;

	StandardRenderer* GetStandardRenderer() { return m_standardRenderer.get(); }

	void SetVertexBuffer(VertexBuffer* vertexBuffer, int32_t size);
	void SetIndexBuffer(IndexBuffer* indexBuffer);

	void SetVertexBuffer(Effekseer::Backend::VertexBufferRef vertexBuffer, int32_t size) {}
	void SetIndexBuffer(Effekseer::Backend::IndexBufferRef indexBuffer) {}

	void SetLayout(Shader* shader);
	void DrawSprites(int32_t spriteCount, int32_t vertexOffset);
	void DrawPolygon(int32_t vertexCount, int32_t indexCount);
	void DrawPolygonInstanced(int32_t vertexCount, int32_t indexCount, int32_t instanceCount);
	void BeginModelRendering(Effekseer::ModelRef model, bool softparticleEnabled);
	void EndModelRendering();

	Shader* GetShader(::EffekseerRenderer::RendererShaderType type);
	void BeginShader(Shader* shader);
	void EndShader(Shader* shader);

	void SetVertexBufferToShader(const void* data, int32_t size, int32_t dstOffset);
	void SetPixelBufferToShader(const void* data, int32_t size, int32_t dstOffset);
	void SetTextures(Shader* shader, Effekseer::Backend::TextureRef* textures, int32_t count);
	void ResetRenderState() override;

	Effekseer::Backend::TextureRef CreateProxyTexture(EffekseerRenderer::ProxyTextureType type) override;

	void DeleteProxyTexture(Effekseer::Backend::TextureRef& texture) override;

	virtual int GetRef() override { return Effekseer::ReferenceObject::GetRef(); }
	virtual int AddRef() override { return Effekseer::ReferenceObject::AddRef(); }
	virtual int Release() override { return Effekseer::ReferenceObject::Release(); }

private:
	void TransferVertexToImmediate3D(godot::RID immediate, 
		const void* vertexData, int32_t spriteCount);

	void TransferVertexToCanvasItem2D(godot::RID canvas_item, 
		const void* vertexData, int32_t spriteCount, godot::Vector2 baseScale);

	void TransferModelToCanvasItem2D(godot::RID canvas_item, Effekseer::Model* model, 
		godot::Vector2 baseScale, bool flipPolygon,
		Effekseer::CullingType cullingType);
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerGodot
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
