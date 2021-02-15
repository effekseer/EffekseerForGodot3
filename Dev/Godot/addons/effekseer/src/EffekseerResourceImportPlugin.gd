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

func get_import_options(preset):
	return []

func get_preset_count():
	return 0

func import(source_file, save_path, options, r_platform_variants, r_gen_files):
	#print(source_file)
	if source_file.ends_with(".efkefc"):
		printerr("Failed to import: " + source_file)
		return null
	
	var resource = preload("res://addons/effekseer/src/EffekseerResource.gdns").new()
	
	resource.load(source_file)
	
	return ResourceSaver.save(
		"%s.%s" % [save_path, get_save_extension()], resource)
