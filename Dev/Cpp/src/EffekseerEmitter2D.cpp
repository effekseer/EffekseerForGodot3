#include <Engine.hpp>
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
	register_method("_notification", &EffekseerEmitter2D::_notification);
	register_method("_enter_tree", &EffekseerEmitter2D::_enter_tree);
	register_method("_exit_tree", &EffekseerEmitter2D::_exit_tree);
	register_method("play", &EffekseerEmitter2D::play);
	register_method("stop", &EffekseerEmitter2D::stop);
	register_method("stop_root", &EffekseerEmitter2D::stop_root);
	register_method("is_playing", &EffekseerEmitter2D::is_playing);
	register_method("set_dynamic_input", &EffekseerEmitter2D::set_dynamic_input);
	register_method("send_trigger", &EffekseerEmitter2D::send_trigger);
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
	register_property<EffekseerEmitter2D, Vector2>("target_position", 
		&EffekseerEmitter2D::set_target_position, &EffekseerEmitter2D::get_target_position, {},
		GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_NOEDITOR);
	register_signal<EffekseerEmitter2D>("finished", {});
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
	set_notify_transform(true);

	if (m_autoplay && !Engine::get_singleton()->is_editor_hint()) {
		play();
	}
}

void EffekseerEmitter2D::_enter_tree()
{
	if (auto system = EffekseerSystem::get_instance()) {
		m_layer = system->attach_layer(get_viewport(), EffekseerSystem::LayerType::_2D);
	}
}

void EffekseerEmitter2D::_exit_tree()
{
	stop();

	if (auto system = EffekseerSystem::get_instance()) {
		system->detach_layer(get_viewport(), EffekseerSystem::LayerType::_2D);
		m_layer = -1;
	}
}

void EffekseerEmitter2D::_notification(int what)
{
	switch (what) {
	case NOTIFICATION_TRANSFORM_CHANGED:
		_update_transform();
		break;
	case NOTIFICATION_VISIBILITY_CHANGED:
		_update_visibility();
		break;
	case NOTIFICATION_PAUSED:
		_update_paused();
		break;
	case NOTIFICATION_UNPAUSED:
		_update_paused();
	default:
		break;
	}
}

void EffekseerEmitter2D::_update_transform()
{
	auto system = EffekseerSystem::get_instance();
	if (system == nullptr) return;
	auto manager = system->get_manager();
	if (manager == nullptr) return;

	auto matrix = EffekseerGodot::ToEfkMatrix43(get_global_transform(), 
		m_orientation * (3.141592f / 180.0f), m_flip_h, m_flip_v);
	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetMatrix(m_handles[i], matrix);
	}
}

void EffekseerEmitter2D::_update_visibility()
{
	auto system = EffekseerSystem::get_instance();
	if (system == nullptr) return;
	auto manager = system->get_manager();
	if (manager == nullptr) return;

	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetShown(m_handles[i], is_visible());
	}
}

void EffekseerEmitter2D::_update_paused()
{
	auto system = EffekseerSystem::get_instance();
	if (system == nullptr) return;
	auto manager = system->get_manager();
	if (manager == nullptr) return;

	const bool node_paused = m_paused || !can_process();
	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetPaused(m_handles[i], node_paused);
	}
}

void EffekseerEmitter2D::remove_handle(Effekseer::Handle handle)
{
	m_handles.erase(handle);
}

void EffekseerEmitter2D::play()
{
	auto system = EffekseerSystem::get_instance();
	if (system == nullptr) return;
	auto manager = system->get_manager();
	if (manager == nullptr) return;

	if (m_effect.is_valid() && m_layer >= 0) {
		Effekseer::Handle handle = manager->Play(m_effect->get_native(), Effekseer::Vector3D(0, 0, 0));
		if (handle >= 0) {
			manager->SetLayer(handle, m_layer);
			manager->SetMatrix(handle, EffekseerGodot::ToEfkMatrix43(get_global_transform(), 
				m_orientation * (3.141592f / 180.0f), m_flip_h, m_flip_v));
			manager->SetUserData(handle, this);
			manager->SetRemovingCallback(handle, [](Effekseer::Manager* manager, Effekseer::Handle handle, bool isRemovingManager){
				if (!isRemovingManager) {
					EffekseerEmitter2D* emitter = static_cast<EffekseerEmitter2D*>(manager->GetUserData(handle));
					if (emitter) {
						emitter->emit_signal("finished");
						emitter->remove_handle(handle);
					}
				}
			});

			if (!is_visible()) {
				manager->SetShown(handle, false);
			}
			if (m_paused || !can_process()) {
				manager->SetPaused(handle, true);
			}
			if (m_speed != 1.0f) {
				manager->SetSpeed(handle, m_speed);
			}
			if (m_color != Effekseer::Color(255, 255, 255, 255)) {
				manager->SetAllColor(handle, m_color);
			}
			if (m_target_position != Vector2::ZERO) {
				Vector2 scaled_position = m_target_position / get_scale();
				manager->SetTargetLocation(handle, EffekseerGodot::ToEfkVector3(scaled_position));
			}
			m_handles.push_back(handle);
		}
	}
}

void EffekseerEmitter2D::stop()
{
	auto system = EffekseerSystem::get_instance();
	if (system == nullptr) return;
	auto manager = system->get_manager();
	if (manager == nullptr) return;

	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetUserData(m_handles[i], nullptr);
		manager->StopEffect(m_handles[i]);
	}
	
	m_handles.clear();
}

void EffekseerEmitter2D::stop_root()
{
	auto system = EffekseerSystem::get_instance();
	if (system == nullptr) return;
	auto manager = system->get_manager();
	if (manager == nullptr) return;

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
	_update_paused();
}

bool EffekseerEmitter2D::is_paused() const
{
	return m_paused;
}

void EffekseerEmitter2D::set_speed(float speed)
{
	m_speed = speed;

	auto system = EffekseerSystem::get_instance();
	if (system == nullptr) return;
	auto manager = system->get_manager();
	if (manager == nullptr) return;

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
	if (system == nullptr) return;
	auto manager = system->get_manager();
	if (manager == nullptr) return;

	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetAllColor(m_handles[i], m_color);
	}
}

Color EffekseerEmitter2D::get_color() const
{
	return EffekseerGodot::ToGdColor(m_color);
}

void EffekseerEmitter2D::set_target_position(Vector2 position)
{
	m_target_position = position;

	auto system = EffekseerSystem::get_instance();
	if (system == nullptr) return;
	auto manager = system->get_manager();
	if (manager == nullptr) return;

	Vector2 scaled_position = position / get_scale();
	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetTargetLocation(m_handles[i], EffekseerGodot::ToEfkVector3(scaled_position));
	}
}

Vector2 EffekseerEmitter2D::get_target_position() const
{
	return m_target_position;
}

void EffekseerEmitter2D::set_effect(Ref<EffekseerEffect> effect)
{
	m_effect = effect;

	if (m_effect.is_valid()) {
		m_effect->load();
	}
}

void EffekseerEmitter2D::set_dynamic_input(int index, float value)
{
	if ((size_t)index >= 4) {
		Godot::print_error(String("Invalid range of dynamic input index: "), __FUNCTION__, "", __LINE__);
		return;
	}

	auto system = EffekseerSystem::get_instance();
	if (system == nullptr) return;
	auto manager = system->get_manager();
	if (manager == nullptr) return;

	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetDynamicInput(m_handles[i], index, value);
	}
}

void EffekseerEmitter2D::send_trigger(int index)
{
	if ((size_t)index >= 4) {
		Godot::print_error(String("Invalid range of trigger index: "), __FUNCTION__, "", __LINE__);
		return;
	}

	auto system = EffekseerSystem::get_instance();
	if (system == nullptr) return;
	auto manager = system->get_manager();
	if (manager == nullptr) return;

	for (int i = 0; i < m_handles.size(); i++) {
		manager->SendTrigger(m_handles[i], index);
	}
}

}
