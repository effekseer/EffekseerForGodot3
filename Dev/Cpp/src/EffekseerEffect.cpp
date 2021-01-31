#include <File.hpp>
#include <ResourceLoader.hpp>
#include "EffekseerSystem.h"
#include "EffekseerEffect.h"
#include "RendererGodot/../Utils/EffekseerGodot.Utils.h"

namespace godot {

void EffekseerEffect::_register_methods()
{
	register_method("_init", &EffekseerEffect::_init);
	register_method("load", &EffekseerEffect::load);
	register_method("release", &EffekseerEffect::release);
	register_property<EffekseerEffect, String>("data_path", 
		&EffekseerEffect::set_data_path, &EffekseerEffect::get_data_path, "");
	register_property<EffekseerEffect, PoolByteArray>("data_bytes", 
		&EffekseerEffect::set_data_bytes, &EffekseerEffect::get_data_bytes, {});
	register_property<EffekseerEffect, Array>("subresources", 
		&EffekseerEffect::set_subresources, &EffekseerEffect::get_subresources, {});
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
	m_data_path = path;

	godot::Ref<godot::File> file = godot::File::_new();
	if (file->open(path, godot::File::READ) != godot::Error::OK) {
		Godot::print_error(String("Failed open file: ") + path, __FUNCTION__, "", __LINE__);
		return;
	}

	int64_t size = file->get_len();
	m_data_bytes = file->get_buffer(size);
	file->close();

	load_in_editor();
	load_in_runtime();
}

void EffekseerEffect::load_in_editor()
{
	auto system = EffekseerSystem::get_instance();
	if (system != nullptr) return;

	auto setting = Effekseer::Setting::Create();
	auto native = Effekseer::Effect::Create(setting, m_data_bytes.read().ptr(), (int32_t)m_data_bytes.size());
	if (native == nullptr)
	{
		Godot::print_error(String("Failed load effect: ") + m_data_path, __FUNCTION__, "", __LINE__);
		return;
	}

	auto nativeptr = native.Get();
	
	char16_t materialPath[1024];
	get_material_path(materialPath, sizeof(materialPath) / sizeof(materialPath[0]));
	godot::String materialPathStr = EffekseerGodot::ToGdString(materialPath) + "/";

	auto loader = godot::ResourceLoader::get_singleton();
	
	auto enumerateResouces = [&](const char16_t* (Effekseer::Effect::*getter)(int) const, int count)
	{
		for (int i = 0; i < count; i++)
		{
			godot::String path = materialPathStr + EffekseerGodot::ToGdString((nativeptr->*getter)(i));
			m_subresources.append(loader->load(path));
		}
	};

	enumerateResouces(&Effekseer::Effect::GetColorImagePath, native->GetColorImageCount());
	enumerateResouces(&Effekseer::Effect::GetNormalImagePath, native->GetNormalImageCount());
	enumerateResouces(&Effekseer::Effect::GetDistortionImagePath, native->GetDistortionImageCount());
	enumerateResouces(&Effekseer::Effect::GetModelPath, native->GetModelCount());
	enumerateResouces(&Effekseer::Effect::GetCurvePath, native->GetCurveCount());
	enumerateResouces(&Effekseer::Effect::GetMaterialPath, native->GetMaterialCount());
	enumerateResouces(&Effekseer::Effect::GetWavePath, native->GetWaveCount());
}

void EffekseerEffect::load_in_runtime()
{
	auto system = EffekseerSystem::get_instance();
	if (system == nullptr) return;

	char16_t materialPath[1024];
	get_material_path(materialPath, sizeof(materialPath) / sizeof(materialPath[0]));

	auto manager = system->get_manager();
	if (manager == nullptr) return;

	m_native = Effekseer::Effect::Create(manager, 
		m_data_bytes.read().ptr(), (int32_t)m_data_bytes.size(), 1.0f, materialPath);
	if (m_native == nullptr)
	{
		Godot::print_error(String("Failed load effect: ") + m_data_path, __FUNCTION__, "", __LINE__);
		return;
	}
}

void EffekseerEffect::get_material_path(char16_t* path, size_t path_size)
{
	int len = (int)EffekseerGodot::ToEfkString(path, m_data_path, path_size);
	
	for (int i = len - 1; i >= 0; i--) {
		if (path[i] == u'/') {
			path[i] = u'\0';
			break;
		}
	}
}

void EffekseerEffect::release()
{
	m_native.Reset();
}

void EffekseerEffect::set_data_bytes(PoolByteArray bytes)
{
	m_data_bytes = bytes;
	load_in_runtime();
}

}
