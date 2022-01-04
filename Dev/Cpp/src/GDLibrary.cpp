#include "GDLibrary.h"
#include <ClassDB.hpp>
#include "EffekseerSystem.h"
#include "EffekseerEffect.h"
#include "EffekseerResource.h"
#include "EffekseerEmitter.h"
#include "EffekseerEmitter2D.h"

using namespace godot;

extern "C" void GDN_EXPORT EffekseerGodot_gdnative_init(godot_gdnative_init_options *option)
{
	Godot::gdnative_init(option);
}

extern "C" void GDN_EXPORT EffekseerGodot_gdnative_terminate(godot_gdnative_terminate_options *option)
{
	Godot::gdnative_terminate(option);
}

extern "C" void GDN_EXPORT EffekseerGodot_nativescript_init(void *handle)
{
	Godot::nativescript_init(handle);

	register_tool_class<EffekseerSystem>();
	register_tool_class<EffekseerEffect>();
	register_tool_class<EffekseerResource>();
	register_tool_class<EffekseerEmitter>();
	register_tool_class<EffekseerEmitter2D>();
}
