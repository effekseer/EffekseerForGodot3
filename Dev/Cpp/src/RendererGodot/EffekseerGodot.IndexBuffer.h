#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.IndexBufferBase.h"
#include "EffekseerGodot.Base.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerGodot
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
	IndexBuffer(Renderer* renderer, int maxCount, bool isDynamic);

	virtual ~IndexBuffer();

	static Effekseer::RefPtr<IndexBuffer> Create(Renderer* renderer, int maxCount, bool isDynamic);

public:
	void Lock() override;
	void Unlock() override;

	const uint8_t* Refer() const { return m_buffer.data(); }
};
using IndexBufferRef = Effekseer::RefPtr<IndexBuffer>;

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerGodot
  //-----------------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------------