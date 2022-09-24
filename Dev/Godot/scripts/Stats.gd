extends Control

func _ready():
	pass

func _process(delta: float):
	$FPS.text = "FPS: %.3f" % Performance.get_monitor(Performance.TIME_FPS)
	$DrawCalls3D.text = "3D DrawCalls: %.0f" % Performance.get_monitor(Performance.RENDER_DRAW_CALLS_IN_FRAME)
	$Objects3D.text = "3D Objects: %.0f" % Performance.get_monitor(Performance.RENDER_OBJECTS_IN_FRAME)
	$Vertices3D.text = "3D Vertices: %.0f" % Performance.get_monitor(Performance.RENDER_VERTICES_IN_FRAME)
	$DrawCalls2D.text = "2D DrawCalls: %.0f" % Performance.get_monitor(Performance.RENDER_2D_DRAW_CALLS_IN_FRAME)
	$Items2D.text = "2D Items: %.0f" % Performance.get_monitor(Performance.RENDER_2D_ITEMS_IN_FRAME)
