#include <File.hpp>
#include "EffekseerSystem.h"
#include "EffekseerEffect.h"

namespace godot {

void EffekseerEffect::_register_methods()
{
	register_method("_init", &EffekseerEffect::_init);
	register_method("load", &EffekseerEffect::load);
	register_method("release", &EffekseerEffect::release);
	register_property<EffekseerEffect, String>("load_path", 
		&EffekseerEffect::load, &EffekseerEffect::get_load_path, "");
}

EffekseerEffect::EffekseerEffect()
{
}

EffekseerEffect::~EffekseerEffect()
{
	release();
}

void EffekseerEffect::_init()
{
}

void EffekseerEffect::load(String path)
{
	m_load_path = path;
	
	auto system = EffekseerSystem::get_instance();
	if (system == nullptr) return;

	auto manager = EffekseerSystem::get_instance()->get_manager();
	if (manager == nullptr) return;

	godot::Ref<godot::File> file = godot::File::_new();

	if (file->open(path, godot::File::READ) != godot::Error::OK) {
		return;
	}

	int64_t size = file->get_len();
	auto buffer = file->get_buffer(size);
	file->close();

	char16_t materialPath[1024];
	int32_t materialPathLen = Effekseer::ConvertUtf8ToUtf16(
		(int16_t*)materialPath, sizeof(materialPath) / sizeof(char16_t), 
		(const int8_t*)path.utf8().get_data());
	
	for (int32_t i = materialPathLen - 1; i >= 0; i--) {
		if (materialPath[i] == u'/') {
			materialPath[i] = u'\0';
			break;
		}
	}

	m_native = Effekseer::Effect::Create(manager, (void*)buffer.read().ptr(), (int32_t)size, 1.0f, materialPath);
}

void EffekseerEffect::release()
{
	m_native.Reset();
}

}
