#pragma once

#include <stdint.h>
#include <Effekseer.h>
#include <Reference.hpp>

namespace EffekseerGodot
{
	
class SoundPlayer;
using SoundPlayerRef = Effekseer::RefPtr<SoundPlayer>;

class SoundPlayer : public Effekseer::SoundPlayer
{
public:
	SoundPlayer(godot::Ref<godot::Reference> soundContext);

	virtual ~SoundPlayer();

	virtual Effekseer::SoundHandle Play( Effekseer::SoundTag tag, const InstanceParameter& parameter ) override;

	virtual void Stop( Effekseer::SoundHandle handle, Effekseer::SoundTag tag ) override;

	virtual void Pause( Effekseer::SoundHandle handle, Effekseer::SoundTag tag, bool pause ) override;

	virtual bool CheckPlaying( Effekseer::SoundHandle handle, Effekseer::SoundTag tag ) override;

	virtual void StopTag( Effekseer::SoundTag tag ) override;

	virtual void PauseTag( Effekseer::SoundTag tag, bool pause ) override;

	virtual bool CheckPlayingTag( Effekseer::SoundTag tag ) override;

	virtual void StopAll() override;

private:
	godot::Ref<godot::Reference> soundContext_;
	int64_t playbackCount_ = 0;
};

}
