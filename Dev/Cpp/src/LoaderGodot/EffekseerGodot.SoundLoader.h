#pragma once

#include <Reference.hpp>
#include <Effekseer.h>

namespace EffekseerGodot
{

class SoundLoader : public Effekseer::SoundLoader
{
public:
	SoundLoader(godot::Ref<godot::Reference> soundContext);

	virtual ~SoundLoader() = default;

	Effekseer::SoundDataRef Load(const char16_t* path) override;

	void Unload(Effekseer::SoundDataRef texture) override;

private:
	godot::Ref<godot::Reference> soundContext_;
};

} // namespace EffekseerGodot
