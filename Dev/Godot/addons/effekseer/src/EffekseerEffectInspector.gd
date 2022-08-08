tool
extends VBoxContainer

var editor: EditorInterface
var effect
onready var viewport_container: ViewportContainer = $Previewer
onready var viewport: Viewport = $Previewer/Viewport
onready var preview3d := $Previewer/Viewport/Preview3D
onready var emitter3d := $Previewer/Viewport/Preview3D/EffekseerEmitter
onready var camera3d := $Previewer/Viewport/Preview3D/Camera
onready var preview2d := $Previewer/Viewport/Preview2D
onready var emitter2d := $Previewer/Viewport/Preview2D/EffekseerEmitter2D
onready var camera2d := $Previewer/Viewport/Preview2D/Camera2D

var popup_menu: PopupMenu
var view_menu: PopupMenu
var view_mode: int = 0

func _enter_tree():
	if editor:
		view_mode = editor.get_editor_settings().get_setting("effekseer/preview_mode") as int
	
	$Previewer.connect("gui_input", self, "_previewer_gui_input")
	$OpenEditorButton.connect("pressed", self, "_open_button_pressed")
	$Controller/PlayButton.connect("pressed", self, "_play_button_pressed")
	$Controller/StopButton.connect("pressed", self, "_stop_button_pressed")
	for i in range(4):
		$Triggers.get_child(i).connect("pressed", self, "_trigger_button_pressed", [i])

func _exit_tree():
	if editor:
		editor.get_editor_settings().set_setting("effekseer/preview_mode", view_mode)

func _ready():
	preview3d.visible = view_mode == 0
	preview2d.visible = view_mode == 1	
	_setup_option_menu()
	
	if emitter3d: emitter3d.effect = effect
	if emitter2d: emitter2d.effect = effect
		
	if view_mode == 0:
		if emitter3d: emitter3d.play()
	else:
		if emitter2d: emitter2d.play()

func _process(delta: float):
	if view_mode == 0:
		if emitter3d and emitter3d.is_playing():
			_update_camera_3d()
	else:
		if emitter2d and emitter2d.is_playing():
			_update_camera_2d()

func _open_button_pressed():
	if editor:
		var editor_path = editor.get_editor_settings().get_setting("effekseer/editor_path") as String
		var file_path = ProjectSettings.globalize_path(effect.get_path())
		if editor_path.empty():
			printerr("Effekseer editor path is not specified.")
			printerr("Please specify editor path at Editor -> Editor settings -> Effekseer.")
		else:
			var result = OS.execute(editor_path, [file_path], false)
			if result < 0:
				printerr("Failed to execute Effekseer editor")
				printerr("Please correct specify editor path at Editor -> Editor settings -> Effekseer.")

func _play_button_pressed():
	if view_mode == 0:
		emitter3d.play()
	else:
		emitter2d.play()

func _stop_button_pressed():
	if view_mode == 0:
		emitter3d.stop()
	else:
		emitter2d.stop()

func _trigger_button_pressed(index: int):
	if view_mode == 0:
		emitter3d.send_trigger(index)
	else:
		emitter2d.send_trigger(index)


var target_pos3d := Vector3(0.0, 0.0, 0.0)
var target_pos2d := Vector2(0.0, 0.0)
var zoom3d := 0.0
var zoom2d := 0.0
var azimuth := 45.0
var elevation := 20.0
var drag_button := 0
var drag_mouse_pos := Vector2.ZERO
var drag_target_pos3d := Vector3(0.0, 0.0, 0.0)
var drag_target_pos2d := Vector2(0.0, 0.0)
var drag_azimuth := 45.0
var drag_elevation := 20.0

func _previewer_gui_input(event: InputEvent):
	if view_mode == 0:
		_input_3d(event)
	else:
		_input_2d(event)

func _input_3d(event: InputEvent):
	if event is InputEventMouseButton:
		if event.button_index == BUTTON_MIDDLE:
			# Move
			if event.pressed and drag_button == 0:
				drag_button = BUTTON_MIDDLE
				drag_mouse_pos = event.position
				drag_target_pos3d = target_pos3d
			elif drag_button == BUTTON_MIDDLE:
				drag_button = 0
		elif event.button_index == BUTTON_RIGHT:
			# Rotation
			if event.pressed and drag_button == 0:
				drag_button = BUTTON_RIGHT
				drag_mouse_pos = event.position
				drag_azimuth = azimuth
				drag_elevation = elevation
			elif drag_button == BUTTON_RIGHT:
				drag_button = 0
		elif event.button_index == BUTTON_WHEEL_UP:
			# Zoom up
			zoom3d = max(zoom3d - 1, -16)
		elif event.button_index == BUTTON_WHEEL_DOWN:
			# Zoom down
			zoom3d = min(zoom3d + 1, 16)
		_update_camera_3d()
	elif event is InputEventMouseMotion:
		if drag_button == BUTTON_MIDDLE:
			var diff = event.position - drag_mouse_pos
			target_pos3d = drag_target_pos3d - (camera3d.transform.basis.x * diff.x - camera3d.transform.basis.y * diff.y) * 0.05
			_update_camera_3d()
		elif drag_button == BUTTON_RIGHT:
			var diff = event.position - drag_mouse_pos
			azimuth = drag_azimuth + diff.x * 0.2
			elevation = clamp(drag_elevation + diff.y * 0.2, -85, 89.5)
			_update_camera_3d()

func _input_2d(event: InputEvent):
	if event is InputEventMouseButton:
		if event.button_index == BUTTON_MIDDLE or event.button_index == BUTTON_RIGHT:
			# Move
			if event.pressed and drag_button == 0:
				drag_button = BUTTON_MIDDLE
				drag_mouse_pos = event.position
				drag_target_pos2d = target_pos2d
			elif drag_button == BUTTON_MIDDLE:
				drag_button = 0
		elif event.button_index == BUTTON_WHEEL_UP:
			# Zoom up
			zoom2d = max(zoom2d - 1, -16)
		elif event.button_index == BUTTON_WHEEL_DOWN:
			# Zoom down
			zoom2d = min(zoom2d + 1, 16)
		_update_camera_2d()
	elif event is InputEventMouseMotion:
		if drag_button == BUTTON_MIDDLE or drag_button == BUTTON_RIGHT:
			var diff = event.position - drag_mouse_pos
			target_pos2d = drag_target_pos2d - diff
			_update_camera_2d()

func _update_camera_3d():
	if camera3d:
		var distance := 16 * pow(1.125, zoom3d)
		camera3d.transform.origin = target_pos3d + Vector3(cos(deg2rad(azimuth)) * cos(deg2rad(elevation)), sin(deg2rad(elevation)), sin(deg2rad(azimuth)) * cos(deg2rad(elevation))) * distance
		camera3d.look_at(target_pos3d, Vector3.UP)

func _update_camera_2d():
	if camera2d:
		var scale := 10 / pow(1.125, zoom2d)
		# Camera2D not working in editor
		#camera2d.position = target_pos2d
		#camera2d.zoom = Vector2(distance, distance)
		viewport.canvas_transform.origin = viewport.size * 0.5 - target_pos2d
		viewport.canvas_transform.x = Vector2(scale, 0)
		viewport.canvas_transform.y = Vector2(0, scale)

func _setup_option_menu():
	popup_menu = $Controller/OptionsButton.get_popup()
	popup_menu.clear()
	popup_menu.connect("id_pressed", self, "_menu_pressed")
	popup_menu.add_item("Reset Camera", 0)
	popup_menu.add_separator()
	
	view_menu = PopupMenu.new()
	view_menu.name = "View Mode"
	view_menu.add_radio_check_item("3D", 0)
	view_menu.add_radio_check_item("2D", 1)
	view_menu.set_item_checked(0, view_mode == 0)
	view_menu.set_item_checked(1, view_mode == 1)
	view_menu.connect("id_pressed", self, "_view_mode_changed")
	popup_menu.add_submenu_item("View Mode", "View Mode")
	popup_menu.add_child(view_menu)

func _menu_pressed(id: int):
	if id == 0:
		if view_mode == 0:
			target_pos3d = Vector3.ZERO
			zoom3d = 0
			azimuth = 45.0
			elevation = 20.0
			_update_camera_3d()
		else:
			target_pos2d = Vector2.ZERO
			zoom2d = 0
			_update_camera_2d()

func _view_mode_changed(id: int):
	print("_view_mode_changed")
	view_mode = id
	view_menu.set_item_checked(0, id == 0)
	view_menu.set_item_checked(1, id == 1)
	preview3d.visible = view_mode == 0
	preview2d.visible = view_mode == 1
	
	if view_mode == 0:
		camera3d.make_current()
		_update_camera_3d()
	else:
		camera2d.make_current()
		_update_camera_2d()
