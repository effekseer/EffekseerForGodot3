extends Node2D

func _ready():
	$GUI/Controller/ResourceName.text = $Effect.effect.resource_path
	$GUI/Controller/EffectMenu.connect("effect_choosed", self, "_on_effect_choosed")
	$GUI/Controller/PlayButton.connect("pressed", self, "_on_play_button_pressed")
	$GUI/Controller/StopButton.connect("pressed", self, "_on_stop_button_pressed")
	$GUI/Controller/PauseButton.connect("pressed", self, "_on_pause_button_pressed")
	for i in range(4):
		$GUI/Controller/Triggers/Buttons.get_child(i).connect("pressed", self, "_on_trigger_button_pressed", [i])
	
	$Effect.target_position = Vector2(512, 300)

func _on_effect_choosed(effect_path: String):
	$GUI/Controller/ResourceName.text = effect_path
	$Effect.effect = load(effect_path)

func _on_play_button_pressed():
	$Effect.play()

func _on_stop_button_pressed():
	$Effect.stop()

func _on_pause_button_pressed():
	get_tree().paused = !get_tree().paused

func _on_trigger_button_pressed(index: int):
	$Effect.send_trigger(index)

func _process(delta: float):
	if Input.is_action_pressed("act_move_left"):
		$Effect.position += Vector2(-100, 0) * delta
	if Input.is_action_pressed("act_move_right"):
		$Effect.position += Vector2(100, 0) * delta
	if Input.is_action_pressed("act_move_up"):
		$Effect.position += Vector2(0, -100) * delta
	if Input.is_action_pressed("act_move_down"):
		$Effect.position += Vector2(0, 100) * delta
	if Input.is_action_pressed("act_rot_left"):
		$Effect.rotation_degrees -= 60 * delta
	if Input.is_action_pressed("act_rot_right"):
		$Effect.rotation_degrees += 60 * delta
