#pragma once

#include <stdint.h>
#include <Effekseer.h>
#include <AudioStream.hpp>

namespace EffekseerGodot
{

class SoundData : public Effekseer::SoundData
{
public:
	SoundData(godot::Ref<godot::AudioStream> stream);
	
	virtual ~SoundData();
	
	godot::Ref<godot::AudioStream> GetStream() const { return stream_; }

private:
	godot::Ref<godot::AudioStream> stream_;
};

}
