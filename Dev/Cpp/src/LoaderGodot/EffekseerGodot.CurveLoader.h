#pragma once

#include <Effekseer.h>

namespace EffekseerGodot
{

class CurveLoader : public ::Effekseer::CurveLoader
{
public:
	CurveLoader() = default;

	virtual ~CurveLoader() = default;

	Effekseer::CurveRef Load(const char16_t* path) override;

	Effekseer::CurveRef Load(const void* data, int32_t size) override;

	void Unload(Effekseer::CurveRef data) override;
};

} // namespace EffekseerGodot

