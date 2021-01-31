extends Reference

var playback_count := 1
var playbacks := []

func load_sound(path) -> Resource:
	return load(path)

func play(params: Dictionary) -> int:
	#print(params)
	var player
	if params.mode_3d:
		player = AudioStreamPlayer3D.new()
		player.unit_db = 20.0 * log(params.volume) / log(10.0)
		player.pitch_scale = pow(2.0, params.pitch)
		player.unit_size = params.distance
		player.translation = params.position
	else:
		player = AudioStreamPlayer.new()
		player.volume_db = 20.0 * log(params.volume) / log(10.0)
		player.pitch_scale = pow(2.0, params.pitch)

	params.emitter.add_child(player)
	player.stream = params.stream
	player.play()
	
	var handle = playback_count
	playback_count = playback_count + 1
	playbacks.append({ "handle": handle, "tag": params.tag, "player": weakref(player) })
	player.connect("finished", self, "_player_finished", [handle]);
	return handle

func _player_finished(handle):
	for playback in playbacks:
		if handle == playback.handle:
			playbacks.erase(playback)
			var player = playback.player.get_ref()
			if player:
				player.queue_free()
			return

func stop(handle: int):
	for playback in playbacks:
		if handle == playback.handle:
			var player = playback.player.get_ref()
			if player:
				player.stop()
				player.queue_free()
			return

func pause(handle: int, paused: bool):
	for playback in playbacks:
		if handle == playback.handle:
			var player = playback.player.get_ref()
			if player:
				player.stream_paused = paused

func check_playing(handle: int):
	for playback in playbacks:
		if handle == playback.handle:
			var player = playback.player.get_ref()
			if player:
				return player.playing
			break
	return false

func stop_tag(tag: int):
	for playback in playbacks:
		if playback.tag == tag:
			var player = playback.player.get_ref()
			if  player:
				player.stop()
				player.queue_free()
				playbacks.erase(playback)

func pause_tag(tag: int, paused: bool):
	for playback in playbacks:
		if playback.tag == tag:
			var player = playback.player.get_ref()
			if  player:
				player.stream_paused = paused

func check_playing_tag(tag: int):
	for playback in playbacks:
		if playback.tag == tag:
			var player = playback.player.get_ref()
			if  player and player.playing:
				return true
	return false

func stop_all():
	for playback in playbacks:
		var player = playback.player.get_ref()
		if  player:
			player.stop()
			player.queue_free()
			playbacks.erase(playback)
