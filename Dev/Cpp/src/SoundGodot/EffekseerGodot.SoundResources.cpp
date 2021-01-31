#include "EffekseerGodot.SoundResources.h"

namespace EffekseerGodot
{

SoundData::SoundData(godot::Ref<godot::AudioStream> stream)
	: stream_(stream)
{
}

SoundData::~SoundData()
{
}

} // namespace EffekseerGodot
