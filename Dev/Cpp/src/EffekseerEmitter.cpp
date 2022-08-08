#include <Engine.hpp>
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
	register_method("_notification", &EffekseerEmitter::_notification);
	register_method("_enter_tree", &EffekseerEmitter::_enter_tree);
	register_method("_exit_tree", &EffekseerEmitter::_exit_tree);
	register_method("play", &EffekseerEmitter::play);
	register_method("stop", &EffekseerEmitter::stop);
	register_method("stop_root", &EffekseerEmitter::stop_root);
	register_method("is_playing", &EffekseerEmitter::is_playing);
	register_method("set_dynamic_input", &EffekseerEmitter::set_dynamic_input);
	register_method("send_trigger", &EffekseerEmitter::send_trigger);
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
	register_property<EffekseerEmitter, Vector3>("target_position", 
		&EffekseerEmitter::set_target_position, &EffekseerEmitter::get_target_position, {},
		GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_NOEDITOR);
	register_signal<EffekseerEmitter>("finished", {});
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
	set_notify_transform(true);

	if (m_autoplay && !Engine::get_singleton()->is_editor_hint()) {
		play();
	}
}

void EffekseerEmitter::_enter_tree()
{
	if (auto system = EffekseerSystem::get_instance()) {
		m_layer = system->attach_layer(get_viewport(), EffekseerSystem::LayerType::_3D);
	}
}

void EffekseerEmitter::_exit_tree()
{
	stop();

	if (auto system = EffekseerSystem::get_instance()) {
		system->detach_layer(get_viewport(), EffekseerSystem::LayerType::_3D);
	}
}

void EffekseerEmitter::_notification(int what)
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
		break;
	default:
		break;
	}
}

void EffekseerEmitter::_update_transform()
{
	auto system = EffekseerSystem::get_instance();
	if (system == nullptr) return;
	auto manager = system->get_manager();
	if (manager == nullptr) return;

	auto matrix = EffekseerGodot::ToEfkMatrix43(get_global_transform());
	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetMatrix(m_handles[i], matrix);
	}
}

void EffekseerEmitter::_update_visibility()
{
	auto system = EffekseerSystem::get_instance();
	if (system == nullptr) return;
	auto manager = system->get_manager();
	if (manager == nullptr) return;

	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetShown(m_handles[i], is_visible());
	}
}

void EffekseerEmitter::_update_paused()
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

void EffekseerEmitter::remove_handle(Effekseer::Handle handle)
{
	m_handles.erase(handle);
}

void EffekseerEmitter::play()
{
	auto system = EffekseerSystem::get_instance();
	if (system == nullptr) return;
	auto manager = system->get_manager();
	if (manager == nullptr) return;

	if (m_effect.is_valid() && m_layer >= 0) {
		Effekseer::Handle handle = manager->Play(m_effect->get_native(), Effekseer::Vector3D(0, 0, 0));
		if (handle >= 0) {
			manager->SetLayer(handle, m_layer);
			manager->SetMatrix(handle, EffekseerGodot::ToEfkMatrix43(get_global_transform()));
			manager->SetUserData(handle, this);
			manager->SetRemovingCallback(handle, [](Effekseer::Manager* manager, Effekseer::Handle handle, bool isRemovingManager){
				if (!isRemovingManager) {
					EffekseerEmitter* emitter = static_cast<EffekseerEmitter*>(manager->GetUserData(handle));
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
			if (m_target_position != Vector3::ZERO) {
				Vector3 scaled_position = m_target_position / get_scale();
				manager->SetTargetLocation(handle, EffekseerGodot::ToEfkVector3(scaled_position));
			}
			m_handles.push_back(handle);
		}
	}
}

void EffekseerEmitter::stop()
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

void EffekseerEmitter::stop_root()
{
	auto system = EffekseerSystem::get_instance();
	if (system == nullptr) return;
	auto manager = system->get_manager();
	if (manager == nullptr) return;

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
	_update_paused();
}

bool EffekseerEmitter::is_paused() const
{
	return m_paused;
}

void EffekseerEmitter::set_speed(float speed)
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

float EffekseerEmitter::get_speed() const
{
	return m_speed;
}

void EffekseerEmitter::set_color(Color color)
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

Color EffekseerEmitter::get_color() const
{
	return EffekseerGodot::ToGdColor(m_color);
}

void EffekseerEmitter::set_target_position(Vector3 position)
{
	m_target_position = position;

	auto system = EffekseerSystem::get_instance();
	if (system == nullptr) return;
	auto manager = system->get_manager();
	if (manager == nullptr) return;

	Vector3 scaled_position = position / get_scale();
	for (int i = 0; i < m_handles.size(); i++) {
		manager->SetTargetLocation(m_handles[i], EffekseerGodot::ToEfkVector3(scaled_position));
	}
}

Vector3 EffekseerEmitter::get_target_position() const
{
	return m_target_position;
}

void EffekseerEmitter::set_effect(Ref<EffekseerEffect> effect)
{
	m_effect = effect;

	if (m_effect.is_valid()) {
		m_effect->load();
	}
}

void EffekseerEmitter::set_dynamic_input(int index, float value)
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

void EffekseerEmitter::send_trigger(int index)
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
