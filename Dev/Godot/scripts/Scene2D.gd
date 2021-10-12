extends Node2D

func _ready():
	$GUI/ResourceName.text = $Effect.effect.resource_path
	$GUI/EffectMenu.connect("effect_choosed", self, "_on_effect_choosed")
	$GUI/PlayButton.connect("pressed", self, "_on_play_button_pressed")
	$GUI/StopButton.connect("pressed", self, "_on_stop_button_pressed")
	$GUI/PauseButton.connect("pressed", self, "_on_pause_button_pressed")
	
	$Effect.target_position = Vector2(512, 300)

func _on_effect_choosed(effect_path: String):
	$GUI/ResourceName.text = effect_path
	$Effect.effect = load(effect_path)

func _on_play_button_pressed():
	$Effect.play()

func _on_stop_button_pressed():
	$Effect.stop()

func _on_pause_button_pressed():
	get_tree().paused = !get_tree().paused

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
