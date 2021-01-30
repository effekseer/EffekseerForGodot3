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

func get_import_options(preset):
	return []

func get_preset_count():
	return 0


func import(source_file, save_path, options, r_platform_variants, r_gen_files):
	#print(source_file)
	
	var effect = preload("res://addons/effekseer/src/EffekseerEffect.gdns").new()
	
	effect.load(source_file)
	
	return ResourceSaver.save(
		"%s.%s" % [save_path, get_save_extension()], effect)
