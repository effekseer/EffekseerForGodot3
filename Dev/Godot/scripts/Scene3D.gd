extends Spatial

func _ready():
	$GUI/ResourceName.text = $Effect.effect.resource_path
	$GUI/EffectMenu.connect("effect_choosed", self, "_on_effect_choosed")
	$GUI/PlayButton.connect("pressed", self, "_on_play_button_pressed")
	$GUI/StopButton.connect("pressed", self, "_on_stop_button_pressed")
	$GUI/PauseButton.connect("pressed", self, "_on_pause_button_pressed")
	for i in range(4):
		$GUI/Triggers/Buttons.get_child(i).connect("pressed", self, "_on_trigger_button_pressed", [i])
	
	$Effect.target_position = $Effect.global_transform.origin + Vector3(0, 15, 0)

func _on_effect_choosed(effect_path: String):
	$GUI/ResourceName.text = effect_path
	$Effect.effect = load(effect_path)

func _on_play_button_pressed():
	$Effect.play()

func _on_stop_button_pressed():
	$Effect.stop()

func _on_trigger_button_pressed(index: int):
	$Effect.send_trigger(index)

func _on_pause_button_pressed():
	get_tree().paused = !get_tree().paused

func _process(delta: float):
	if Input.is_action_pressed("act_move_left"):
		$Effect.transform.origin += Vector3(-5, 0, 0) * delta
	if Input.is_action_pressed("act_move_right"):
		$Effect.transform.origin += Vector3(5, 0, 0) * delta
	if Input.is_action_pressed("act_move_up"):
		$Effect.transform.origin += Vector3(0, 0, -5) * delta
	if Input.is_action_pressed("act_move_down"):
		$Effect.transform.origin += Vector3(0, 0, 5) * delta
	if Input.is_action_pressed("act_rot_left"):
		$Effect.rotate_y(deg2rad(-60) * delta)
	if Input.is_action_pressed("act_rot_right"):
		$Effect.rotate_y(deg2rad(60) * delta)
