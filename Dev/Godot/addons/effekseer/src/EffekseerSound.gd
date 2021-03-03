extends Reference

const ATTENUATION_FACTOR_2D = 100
var playbacks := []

func load_sound(path) -> Resource:
	return load(path)

func play(params: Dictionary):
	#print(params)

	var volume_db = 20.0 * log(params.volume) / log(10.0)
	var pitch_scale = pow(2.0, params.pitch)
	
	var player
	if params.positional:
		if params.emitter is Spatial:
			player = AudioStreamPlayer3D.new()
			player.unit_db = volume_db
			player.pitch_scale = pitch_scale
			player.unit_size = params.distance
			player.translation = params.position
		elif params.emitter is Node2D:
			player = AudioStreamPlayer2D.new()
			player.volume_db = volume_db
			player.pitch_scale = pitch_scale
			player.max_distance = params.distance * ATTENUATION_FACTOR_2D
			player.position = Vector2(params.position.x, params.position.y)
	else:
		player = AudioStreamPlayer.new()
		player.volume_db = volume_db
		player.pitch_scale = pitch_scale

	params.emitter.add_child(player)
	player.stream = params.stream
	player.play()
	
	playbacks.append({ "handle": params.handle, "tag": params.tag, "player": weakref(player) })
	player.connect("finished", self, "_player_finished", [params.handle]);

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
