#include <AudioServer.hpp>
#include "EffekseerGodot.SoundPlayer.h"
#include "EffekseerGodot.SoundResources.h"
#include "../Utils/EffekseerGodot.Utils.h"

namespace EffekseerGodot
{

SoundPlayer::SoundPlayer(godot::Ref<godot::Reference> soundContext)
	: soundContext_(soundContext)
{
}

SoundPlayer::~SoundPlayer()
{
}

Effekseer::SoundHandle SoundPlayer::Play(Effekseer::SoundTag tag, const InstanceParameter& parameter)
{
	auto data = (SoundData*)parameter.Data.Get();

	godot::Dictionary args;
	args["tag"] = reinterpret_cast<int64_t>(tag);
	args["emitter"] = reinterpret_cast<godot::Object*>(parameter.UserData);
	args["stream"] = data->GetStream();
	args["volume"] = parameter.Volume;
	args["pitch"] = parameter.Pitch;
	args["pan"] = parameter.Pan;
	args["mode_3d"] = parameter.Mode3D;
	args["position"] = ToGdVector3(parameter.Position);
	args["distance"] = parameter.Distance;

	auto result = (size_t)(int64_t)soundContext_->call("play", args);
	return reinterpret_cast<Effekseer::SoundHandle>((size_t)result);
}

void SoundPlayer::Stop(Effekseer::SoundHandle handle, Effekseer::SoundTag tag)
{
	soundContext_->call("stop", reinterpret_cast<int64_t>(handle));
}

void SoundPlayer::Pause(Effekseer::SoundHandle handle, Effekseer::SoundTag tag, bool pause)
{
	soundContext_->call("pause", reinterpret_cast<int64_t>(handle), pause);
}

bool SoundPlayer::CheckPlaying(Effekseer::SoundHandle handle, Effekseer::SoundTag tag)
{
	return (bool)soundContext_->call("check_playing", reinterpret_cast<int64_t>(handle));
}

void SoundPlayer::StopTag(Effekseer::SoundTag tag)
{
	soundContext_->call("stop_tag", reinterpret_cast<int64_t>(tag));
}

void SoundPlayer::PauseTag(Effekseer::SoundTag tag, bool pause)
{
	soundContext_->call("pause_tag", reinterpret_cast<int64_t>(tag), pause);
}

bool SoundPlayer::CheckPlayingTag(Effekseer::SoundTag tag)
{
	return (bool)soundContext_->call("check_playing_tag", reinterpret_cast<int64_t>(tag));
}

void SoundPlayer::StopAll()
{
	soundContext_->call("stop_all");
}

} // namespace EffekseerGodot
