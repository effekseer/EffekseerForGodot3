tool
extends EditorPlugin

var effect_import_plugin
var emitter_gizmo_plugin

func _enter_tree():
	add_project_setting("effekseer/instance_max_count", 2000, TYPE_INT, PROPERTY_HINT_RANGE, "40,8000")
	add_project_setting("effekseer/square_max_count", 8000, TYPE_INT, PROPERTY_HINT_RANGE, "80,32000")
	add_project_setting("effekseer/draw_max_count", 128, TYPE_INT, PROPERTY_HINT_RANGE, "16,1024")
	
	add_autoload_singleton("EffekseerSystem", "res://addons/effekseer/src/EffekseerSystem.gdns")
	#add_custom_type("EffekseerEffect", "Resource", preload("res://addons/effekseer/src/EffekseerEffect.gdns"), null)
	add_custom_type("EffekseerEmitter", "Spatial", preload("res://addons/effekseer/src/EffekseerEmitter.gdns"), null)
	
	effect_import_plugin = preload("res://addons/effekseer/src/EffekseerEffectImportPlugin.gd").new()
	emitter_gizmo_plugin = preload("res://addons/effekseer/src/EffekseerEmitterGizmoPlugin.gd").new()
	add_spatial_gizmo_plugin(emitter_gizmo_plugin)
	add_import_plugin(effect_import_plugin)
	

func _exit_tree():
	remove_import_plugin(effect_import_plugin)
	remove_spatial_gizmo_plugin(emitter_gizmo_plugin)
	
	remove_custom_type("EffekseerEmitter")
	#remove_custom_type("EffekseerEffect")
	remove_autoload_singleton("EffekseerSystem")

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

