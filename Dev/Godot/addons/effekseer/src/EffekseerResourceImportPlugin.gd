extends EditorImportPlugin

func get_importer_name():
	return "effekseer.resource"

func get_visible_name():
	return "Effekseer Resource";

func get_recognized_extensions():
	return ["efkmat", "efkmodel", "efkcurve"]

func get_save_extension():
	return "res";

func get_resource_type():
	return "Resource";

enum Presets { DEFAULT }

func get_import_options(preset):
	match preset:
		Presets.DEFAULT:
			return [
				{ "name": "compress", "default_value": true },
			]
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
	if source_file.ends_with(".efkefc"):
		printerr("Failed to import: " + source_file)
		return null
	
	var resource = preload("res://addons/effekseer/src/EffekseerResource.gdns").new()
	
	resource.load(source_file)
	
	var save_name = "%s.%s" % [save_path, get_save_extension()]
	var flags = ResourceSaver.FLAG_COMPRESS if options.compress else 0
	return ResourceSaver.save(save_name, resource, flags)
