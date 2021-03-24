#include <Viewport.hpp>
#include <VisualServer.hpp>
#include "GDLibrary.h"
#include "EffekseerSystem.h"
#include "EffekseerEmitter.h"
#include "Utils/EffekseerGodot.Utils.h"

namespace godot {

void EffekseerEmitter::_register_methods()
{
	register_method("_init", &EffekseerEmitter::_init);
	register_method("_ready", &EffekseerEmitter::_ready);
	register_method("_process", &EffekseerEmitter::_process);
	register_method("_enter_tree", &EffekseerEmitter::_enter_tree);
	register_method("_exit_tree", &EffekseerEmitter::_exit_tree);
	register_method("_update_draw", &EffekseerEmitter::_update_draw);
	register_method("play", &EffekseerEmitter::play);
	register_method("stop", &EffekseerEmitter::stop);
	register_method("stop_root", &EffekseerEmitter::stop_root);
	register_method("is_playing", &EffekseerEmitter::is_playing);
	register_property<EffekseerEmitter, Ref<EffekseerEffect>>("effect", 
		&EffekseerEmitter::set_effect, &EffekseerEmitter::get_effect, nullptr);
	register_property<EffekseerEmitter, bool>("autoplay", 
		&EffekseerEmitter::set_autoplay, &EffekseerEmitter::is_autoplay, true);
	register_property<EffekseerEmitter, bool>("paused", 
		&EffekseerEmitter::set_paused, &EffekseerEmitter::is_paused, false);
	register_property<EffekseerEmitter, float>("speed", 
		&EffekseerEmitter::set_speed, &EffekseerEmitter::get_speed, 1.0f,
		GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_RANGE, "0.0,10.0,0.01");
	register_property<EffekseerEmitter, Color>("color", 
		&EffekseerEmitter::set_color, &EffekseerEmitter::get_color, Color(1.0f, 1.0f, 1.0f, 1.0f));
}

EffekseerEmitter::EffekseerEmitter()
{
}

EffekseerEmitter::~EffekseerEmitter()
{
}

void EffekseerEmitter::_init()
{
}

void EffekseerEmitter::_ready()
{
	if (m_autoplay) {
		play();
	}
}

void EffekseerEmitter::_enter_tree()
{
	VisualServer::get_singleton()->connect("frame_pre_draw", this, "_update_draw");
}

void EffekseerEmitter::_exit_tree()
{
	VisualServer::get_singleton()->disconnect("frame_pre_draw", this, "_update_draw");
}

void EffekseerEmitter::_process(float delta)
{
	auto system = EffekseerSystem::get_instance();
	auto manager = system->get_manager();

	for (int i = 0; i < m_handles.size(); ) {
		auto handle = m_handles[i];
		if (!manager->Exists(handle)) {
			m_handles.remove(i);
			continue;
		}
		manager->SetMatrix(handle, EffekseerGodot::ToEfkMatrix43(get_global_transform()));
		i++;
	}
}

void EffekseerEmitter::_update_draw()
{
	if (!is_visible()) {
		return;
	}

	auto viewport = get_viewport();
	if (viewport == nullptr) return;

	auto camera = viewport->get_camera();
	if (camera == nullptr) return;

	auto system = EffekseerSystem::get_instance();
	
	for (int i = 0; i < m_handles.size(); i++) {
		system->draw3D(m_handles[i], camera->get_camera_transform());
	}
}

void EffekseerEmitter::play()
{
	auto system = EffekseerSystem::get_instance();
	auto manager = system->get_manager();

	if (m_effect.is_valid()) {
		Effekseer::Handle handle = manager->Play(m_effect->get_native(), Effekseer::Vector3D(0, 0, 0));
		if (handle >= 0) {
			manager->SetMatrix(handle, EffekseerGodot::ToEfkMatrix43(get_global_transform()));
			manager->SetUserData(handle, this);

			if (m_paused) {
				manager->SetPaused(handle, true);
			}
			if (m_speed != 1.0f) {
				manager->SetSpeed(handle, m_speed);
			}
			if (m_color != Effekseer::Color(255, 255, 255, 255)) {
				manager->SetAllColor(handle, m_color);
			}
			m_handles.push_back(handle);
		}
	}
}

void EffekseerEmitter::stop()
{
	auto system = EffekseerSystem::get_instance();
	auto manager = system->get_manager();

	for (int i = 0; i < m_handles.size(); i++) {
		manager->StopEffect(m_handles[i]);
	}
	
	m_handles.clear();
}

void EffekseerEmitter::stop_root()
{
	auto system = EffekseerSystem::get_instance();
	auto manager = system->get_manager();

	for (int i = 0; i < m_handles.size(); i++) {
		manager->StopRoot(m_handles[i]);
	}
}

bool EffekseerEmitter::is_playing()
{
	return !m_handles.empty();
}

void EffekseerEmitter::set_paused(bool paused)
{
	m_paused = paused;

	auto system = EffekseerSystem::get_instance();
	auto manager = system->get_manager();

	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetPaused(m_handles[i], paused);
	}
}

bool EffekseerEmitter::is_paused() const
{
	return m_paused;
}

void EffekseerEmitter::set_speed(float speed)
{
	m_speed = speed;

	auto system = EffekseerSystem::get_instance();
	auto manager = system->get_manager();

	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetSpeed(m_handles[i], speed);
	}
}

float EffekseerEmitter::get_speed() const
{
	return m_speed;
}

void EffekseerEmitter::set_color(Color color)
{
	m_color = EffekseerGodot::ToEfkColor(color);

	auto system = EffekseerSystem::get_instance();
	auto manager = system->get_manager();

	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetAllColor(m_handles[i], m_color);
	}
}

Color EffekseerEmitter::get_color() const
{
	return EffekseerGodot::ToGdColor(m_color);
}

void EffekseerEmitter::set_effect(Ref<EffekseerEffect> effect)
{
	m_effect = effect;
	m_effect->setup();
}

}
