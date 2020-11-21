extends Node

func _ready():
	pass

func _process(_delta: float):
	if Input.is_action_just_pressed("ui_accept"):
		if $Effect.is_playing():
			$Effect.stop()
		else:
			$Effect.play()
