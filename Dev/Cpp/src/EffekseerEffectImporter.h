#pragma once

#include <Godot.hpp>
#include <EditorImportPlugin.hpp>

namespace godot {

class EffekseerEffectImporter : public EditorImportPlugin
{
	GODOT_CLASS(EffekseerEffectImporter, EditorImportPlugin)

public:
	static void _register_methods();

	EffekseerEffectImporter();
	~EffekseerEffectImporter();

	void _init();

	String get_importer_name();

	String get_visible_name();

	Array get_recognized_extensions();

	String get_save_extension();

	String get_resource_type();

	int64_t import(const String source_file, const String save_path, const Dictionary options, const Array platform_variants, const Array gen_files);

private:
};

}
