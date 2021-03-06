extends Node2D


func _process(delta: float):
	if Input.is_key_pressed(KEY_A):
		position += Vector2(-100, 0) * delta
	if Input.is_key_pressed(KEY_D):
		position += Vector2(100, 0) * delta
	if Input.is_key_pressed(KEY_W):
		position += Vector2(0, -100) * delta
	if Input.is_key_pressed(KEY_S):
		position += Vector2(0, 100) * delta
	if Input.is_key_pressed(KEY_Q):
		rotation_degrees -= 30 * delta
	if Input.is_key_pressed(KEY_E):
		rotation_degrees += 30 * delta
	
	if Input.is_action_just_pressed("ui_accept"):
		$Effect.play()

	if Input.is_action_just_pressed("ui_cancel"):
		$Effect.stop()
