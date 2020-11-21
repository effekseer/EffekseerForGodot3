#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.IndexBufferBase.h"
#include "EffekseerGodot3.Base.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerGodot3
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class IndexBuffer
	: public EffekseerRenderer::IndexBufferBase
	, public Effekseer::ReferenceObject
{
private:
	std::vector<uint8_t> m_buffer;

public:
	IndexBuffer(RendererImplemented* renderer, int maxCount, bool isDynamic);

	virtual ~IndexBuffer();

	static Effekseer::RefPtr<IndexBuffer> Create(RendererImplemented* renderer, int maxCount, bool isDynamic);

public:
	void Lock();
	void Unlock();

	const uint8_t* Refer() const { return m_buffer.data(); }
};
using IndexBufferRef = Effekseer::RefPtr<IndexBuffer>;

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerGodot3
  //-----------------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------------