extends EditorInspectorPlugin

var editor: EditorInterface

func _init(editor_: EditorInterface):
	editor = editor_


func can_handle(object: Object) -> bool:
	return object.get_script() == preload("res://addons/effekseer/src/EffekseerEffect.gdns")


func parse_begin(object: Object) -> void:
	var inspector = load("res://addons/effekseer/res/EffekseerEffectInspector.tscn").instance()
	inspector.editor = editor
	inspector.effect = object
	add_custom_control(inspector)
