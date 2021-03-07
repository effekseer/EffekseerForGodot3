#include <Viewport.hpp>
#include <VisualServer.hpp>
#include "GDLibrary.h"
#include "EffekseerSystem.h"
#include "EffekseerEmitter2D.h"
#include "Utils/EffekseerGodot.Utils.h"

namespace godot {

void EffekseerEmitter2D::_register_methods()
{
	register_method("_init", &EffekseerEmitter2D::_init);
	register_method("_ready", &EffekseerEmitter2D::_ready);
	register_method("_process", &EffekseerEmitter2D::_process);
	register_method("_enter_tree", &EffekseerEmitter2D::_enter_tree);
	register_method("_exit_tree", &EffekseerEmitter2D::_exit_tree);
	register_method("_update_draw", &EffekseerEmitter2D::_update_draw);
	register_method("play", &EffekseerEmitter2D::play);
	register_method("stop", &EffekseerEmitter2D::stop);
	register_method("stop_root", &EffekseerEmitter2D::stop_root);
	register_method("is_playing", &EffekseerEmitter2D::is_playing);
	register_property<EffekseerEmitter2D, Ref<EffekseerEffect>>("effect", 
		&EffekseerEmitter2D::set_effect, &EffekseerEmitter2D::get_effect, nullptr);
	register_property<EffekseerEmitter2D, bool>("autoplay", 
		&EffekseerEmitter2D::set_autoplay, &EffekseerEmitter2D::is_autoplay, true);
	register_property<EffekseerEmitter2D, bool>("paused", 
		&EffekseerEmitter2D::set_paused, &EffekseerEmitter2D::is_paused, false);
	register_property<EffekseerEmitter2D, float>("speed", 
		&EffekseerEmitter2D::set_speed, &EffekseerEmitter2D::get_speed, 1.0f,
		GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT,
		GODOT_PROPERTY_HINT_RANGE, "0.0,10.0,0.01");
	register_property<EffekseerEmitter2D, Color>("color", 
		&EffekseerEmitter2D::set_color, &EffekseerEmitter2D::get_color, Color(1.0f, 1.0f, 1.0f, 1.0f));
	register_property<EffekseerEmitter2D, Vector3>("orientation", 
		&EffekseerEmitter2D::set_orientation, &EffekseerEmitter2D::get_orientation, {});
	register_property<EffekseerEmitter2D, bool>("flip_h", 
		&EffekseerEmitter2D::set_flip_h, &EffekseerEmitter2D::get_flip_h, false);
	register_property<EffekseerEmitter2D, bool>("flip_v", 
		&EffekseerEmitter2D::set_flip_v, &EffekseerEmitter2D::get_flip_v, true);
}

EffekseerEmitter2D::EffekseerEmitter2D()
{
}

EffekseerEmitter2D::~EffekseerEmitter2D()
{
}

void EffekseerEmitter2D::_init()
{
}

void EffekseerEmitter2D::_ready()
{
	if (m_autoplay) {
		play();
	}
}

void EffekseerEmitter2D::_enter_tree()
{
	VisualServer::get_singleton()->connect("frame_pre_draw", this, "_update_draw");
}

void EffekseerEmitter2D::_exit_tree()
{
	VisualServer::get_singleton()->disconnect("frame_pre_draw", this, "_update_draw");
}

void EffekseerEmitter2D::_process(float delta)
{
	auto system = EffekseerSystem::get_instance();
	auto manager = system->get_manager();

	for (int i = 0; i < m_handles.size(); ) {
		auto handle = m_handles[i];
		if (!manager->Exists(handle)) {
			m_handles.remove(i);
			continue;
		}
		manager->SetMatrix(handle, EffekseerGodot::ToEfkMatrix43(get_global_transform(), 
			m_orientation * (3.141592f / 180.0f), m_flip_h, m_flip_v));
		i++;
	}
}

void EffekseerEmitter2D::_update_draw()
{
	if (!is_visible()) {
		return;
	}

	auto viewport = get_viewport();
	if (viewport == nullptr) return;

	auto system = EffekseerSystem::get_instance();

	VisualServer::get_singleton()->canvas_item_clear(get_canvas_item());

	for (int i = 0; i < m_handles.size(); i++) {
		system->draw2D(m_handles[i], viewport->get_canvas_transform());
	}
}

void EffekseerEmitter2D::play()
{
	auto system = EffekseerSystem::get_instance();
	auto manager = system->get_manager();

	if (m_effect.is_valid()) {
		Effekseer::Handle handle = manager->Play(m_effect->get_native(), Effekseer::Vector3D(0, 0, 0));
		if (handle >= 0) {
			manager->SetMatrix(handle, EffekseerGodot::ToEfkMatrix43(get_global_transform(), 
				m_orientation * (3.141592f / 180.0f), m_flip_h, m_flip_v));
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

void EffekseerEmitter2D::stop()
{
	auto system = EffekseerSystem::get_instance();
	auto manager = system->get_manager();

	for (int i = 0; i < m_handles.size(); i++) {
		manager->StopEffect(m_handles[i]);
	}
	
	m_handles.clear();
}

void EffekseerEmitter2D::stop_root()
{
	auto system = EffekseerSystem::get_instance();
	auto manager = system->get_manager();

	for (int i = 0; i < m_handles.size(); i++) {
		manager->StopRoot(m_handles[i]);
	}
}

bool EffekseerEmitter2D::is_playing()
{
	return !m_handles.empty();
}

void EffekseerEmitter2D::set_paused(bool paused)
{
	m_paused = paused;

	auto system = EffekseerSystem::get_instance();
	auto manager = system->get_manager();

	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetPaused(m_handles[i], paused);
	}
}

bool EffekseerEmitter2D::is_paused() const
{
	return m_paused;
}

void EffekseerEmitter2D::set_speed(float speed)
{
	m_speed = speed;

	auto system = EffekseerSystem::get_instance();
	auto manager = system->get_manager();

	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetSpeed(m_handles[i], speed);
	}
}

float EffekseerEmitter2D::get_speed() const
{
	return m_speed;
}

void EffekseerEmitter2D::set_color(Color color)
{
	m_color = EffekseerGodot::ToEfkColor(color);

	auto system = EffekseerSystem::get_instance();
	auto manager = system->get_manager();

	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetAllColor(m_handles[i], m_color);
	}
}

Color EffekseerEmitter2D::get_color() const
{
	return EffekseerGodot::ToGdColor(m_color);
}

void EffekseerEmitter2D::set_effect(Ref<EffekseerEffect> effect)
{
	m_effect = effect;
	m_effect->setup();
}

}
