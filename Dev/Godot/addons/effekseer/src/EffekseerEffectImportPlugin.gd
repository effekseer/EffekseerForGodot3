extends EditorImportPlugin

func get_importer_name():
	return "effekseer.effect"

func get_visible_name():
	return "Effekseer Effect";

func get_recognized_extensions():
	return ["efkefc"]

func get_save_extension():
	return "res";

func get_resource_type():
	return "Resource";

enum Presets { DEFAULT }

func get_import_options(preset):
	match preset:
		Presets.DEFAULT:
			return [{
					   "name": "scale",
					   "default_value": 1.0
					}]
		_:
			return []

func get_preset_name(preset):
	match preset:
		Presets.DEFAULT:
			return "Default"
		_:
			return "Unknown"

func get_preset_count():
	return 1

func get_option_visibility(option, options):
	return true

func import(source_file, save_path, options, r_platform_variants, r_gen_files):
	#print(source_file)
	
	var effect = preload("res://addons/effekseer/src/EffekseerEffect.gdns").new()
	
	effect.load(source_file)
	effect.resolve_dependencies()
	effect.scale = options.scale
	
	return ResourceSaver.save(
		"%s.%s" % [save_path, get_save_extension()], effect)
