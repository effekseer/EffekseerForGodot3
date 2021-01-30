
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerGodot.VertexBuffer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerGodot
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBuffer::VertexBuffer(RendererImplemented* renderer, int size, bool isDynamic)
	: VertexBufferBase(size, isDynamic)
	, m_buffer((size_t)size)
	, m_vertexRingOffset(0)
	, m_ringBufferLock(false)
	, m_ringLockedOffset(0)
	, m_ringLockedSize(0)
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
Effekseer::RefPtr<VertexBuffer> VertexBuffer::Create(RendererImplemented* renderer, int size, bool isDynamic)
{
	return VertexBufferRef(new VertexBuffer(renderer, size, isDynamic));
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::Lock()
{
	assert(!m_isLock);
	assert(!m_ringBufferLock);

	m_isLock = true;
	m_resource = m_buffer.data();
	m_offset = 0;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
bool VertexBuffer::RingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment)
{
	assert(!m_isLock);
	assert(!m_ringBufferLock);
	
	m_ringBufferLock = true;
	data = m_resource = m_buffer.data();
	offset = m_offset = 0;

	return true;
}

bool VertexBuffer::TryRingBufferLock(int32_t size, int32_t& offset, void*& data, int32_t alignment)
{
	return RingBufferLock(size, offset, data, alignment);
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexBuffer::Unlock()
{
	assert(m_isLock || m_ringBufferLock);

	m_resource = NULL;
	m_isLock = false;
	m_ringBufferLock = false;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerGodot
  //-----------------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------------