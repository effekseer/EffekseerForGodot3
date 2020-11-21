#include <File.hpp>
#include <ResourceSaver.hpp>
#include <SpatialMaterial.hpp>
#include "EffekseerEffect.h"
#include "EffekseerEffectImporter.h"
#include "EffekseerEmitter.h"

namespace godot {

void EffekseerEffectImporter::_register_methods()
{
	register_method("_init", &EffekseerEffectImporter::_init);
	register_method("get_importer_name", &EffekseerEffectImporter::get_importer_name);
	register_method("get_visible_name", &EffekseerEffectImporter::get_visible_name);
	register_method("get_recognized_extensions", &EffekseerEffectImporter::get_recognized_extensions);
	register_method("get_save_extension", &EffekseerEffectImporter::get_save_extension);
	register_method("get_resource_type", &EffekseerEffectImporter::get_resource_type);
	register_method("import", &EffekseerEffectImporter::import);
}

EffekseerEffectImporter::EffekseerEffectImporter()
{
}

EffekseerEffectImporter::~EffekseerEffectImporter()
{
}

void EffekseerEffectImporter::_init()
{
}

String EffekseerEffectImporter::get_importer_name()
{
	return "effekseer.effect";
}

String EffekseerEffectImporter::get_visible_name()
{
	return "Effekseer Effect";
}

Array EffekseerEffectImporter::get_recognized_extensions()
{
	Array list;
	list.push_back("efkefc");
	return list;
}

String EffekseerEffectImporter::get_save_extension()
{
	return "efk";
}

String EffekseerEffectImporter::get_resource_type()
{
	return "EffekseerEffect";
}

int64_t EffekseerEffectImporter::import(const String source_file, const String save_path, const Dictionary options, const Array platform_variants, const Array gen_files)
{
	//Ref<File> file;
	//file.instance();
	//file->open(source_file, File::READ);
	
	//int64_t file_size = file->get_len();
	//auto buffer = file->get_buffer(file_size);
	//const uint8_t* file_data = buffer.read().ptr();

	//Ref<EffekseerEffect> effect;
	//effect.instance();
	//effect->load(source_file);
	//
	//return (int64_t)ResourceSaver::get_singleton()->save(
	//	save_path + "." + get_save_extension(), effect);
	return 0;
}

}
