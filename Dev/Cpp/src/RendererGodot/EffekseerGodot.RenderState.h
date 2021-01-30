#pragma once

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRenderer.RenderStateBase.h"
#include "EffekseerGodot.Base.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerGodot
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class RenderState : public ::EffekseerRenderer::RenderStateBase
{
private:
	static const int32_t DepthTestCount = 2;
	static const int32_t DepthWriteCount = 2;
	static const int32_t CulTypeCount = 3;
	static const int32_t AlphaTypeCount = 5;
	static const int32_t TextureFilterCount = 2;
	static const int32_t TextureWrapCount = 2;

public:
	RenderState();
	virtual ~RenderState();

	void Update(bool forced);
};

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerGodot
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
