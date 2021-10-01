tool
extends EditorPlugin

var effect_import_plugin
var effect_inspector_plugin
var resource_import_plugin
var emitter_gizmo_plugin
const plugin_path = "res://addons/effekseer"
const plugin_source_path = "res://addons/effekseer/src"

func _enter_tree():
	add_project_setting("effekseer/instance_max_count", 2000, TYPE_INT, PROPERTY_HINT_RANGE, "40,8000")
	add_project_setting("effekseer/square_max_count", 8000, TYPE_INT, PROPERTY_HINT_RANGE, "80,32000")
	add_project_setting("effekseer/draw_max_count", 128, TYPE_INT, PROPERTY_HINT_RANGE, "16,1024")
	add_project_setting("effekseer/sound_script", load(plugin_source_path + "/EffekseerSound.gd"), TYPE_OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "Script")
	add_editor_setting("effekseer/editor_path", "", TYPE_STRING, PROPERTY_HINT_GLOBAL_FILE, get_editor_file_name())
	add_editor_setting("effekseer/preview_mode", "", TYPE_INT, PROPERTY_HINT_ENUM, "3D,2D")
	
	add_autoload_singleton("EffekseerSystem", plugin_source_path + "/EffekseerSystem.gdns")
	var icon = load(plugin_path + "/icon16.png") as Texture
	#add_custom_type("EffekseerEffect", "Resource", load(plugin_source_path + "/EffekseerEffect.gdns"), icon)
	add_custom_type("EffekseerEmitter", "Spatial", load(plugin_source_path + "/EffekseerEmitter.gdns"), icon)
	add_custom_type("EffekseerEmitter2D", "Node2D", load(plugin_source_path + "/EffekseerEmitter2D.gdns"), icon)
	
	effect_import_plugin = load(plugin_source_path + "/EffekseerEffectImportPlugin.gd").new()
	effect_inspector_plugin = load(plugin_source_path + "/EffekseerEffectInspectorPlugin.gd").new(get_editor_interface())
	resource_import_plugin = load(plugin_source_path + "/EffekseerResourceImportPlugin.gd").new()
	emitter_gizmo_plugin = load(plugin_source_path + "/EffekseerEmitterGizmoPlugin.gd").new()
	add_import_plugin(effect_import_plugin)
	add_inspector_plugin(effect_inspector_plugin)
	add_import_plugin(resource_import_plugin)
	add_spatial_gizmo_plugin(emitter_gizmo_plugin)
	

func _exit_tree():
	remove_spatial_gizmo_plugin(emitter_gizmo_plugin)
	remove_import_plugin(resource_import_plugin)
	remove_inspector_plugin(effect_inspector_plugin)
	remove_import_plugin(effect_import_plugin)
	
	remove_custom_type("EffekseerEmitter2D")
	remove_custom_type("EffekseerEmitter")
	#remove_custom_type("EffekseerEffect")
	remove_autoload_singleton("EffekseerSystem")
	
	remove_editor_setting("effekseer/editor_path")
	remove_editor_setting("effekseer/preview_mode")
	remove_project_setting("effekseer/sound_script")
	remove_project_setting("effekseer/draw_max_count")
	remove_project_setting("effekseer/square_max_count")
	remove_project_setting("effekseer/instance_max_count")


func add_project_setting(name: String, initial_value, type: int, hint: int, hint_string: String) -> void:
	if ProjectSettings.has_setting(name):
		return

	var property_info: Dictionary = {
		"name": name,
		"type": type,
		"hint": hint,
		"hint_string": hint_string
	}

	ProjectSettings.set_setting(name, initial_value)
	ProjectSettings.add_property_info(property_info)
	ProjectSettings.set_initial_value(name, initial_value)


func remove_project_setting(name: String):
	ProjectSettings.clear(name)


func add_editor_setting(name: String, initial_value, type: int, hint: int, hint_string: String) -> void:
	var editor_settings := get_editor_interface().get_editor_settings()
	if editor_settings.has_setting(name):
		return

	var property_info: Dictionary = {
		"name": name,
		"type": type,
		"hint": hint,
		"hint_string": hint_string
	}

	editor_settings.set_setting(name, initial_value)
	editor_settings.add_property_info(property_info)
	editor_settings.set_initial_value(name, initial_value, false)


func remove_editor_setting(name: String):
	var editor_settings := get_editor_interface().get_editor_settings()
	editor_settings.erase(name)


func get_editor_file_name() -> String:
	match OS.get_name():
		"Windows": return "Effekseer.exe"
		"OSX": return "Effekseer.app"
		"X11": return "Effekseer"
	return ""
