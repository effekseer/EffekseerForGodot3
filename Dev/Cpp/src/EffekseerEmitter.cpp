#include <Viewport.hpp>
#include "GDLibrary.h"
#include "EffekseerSystem.h"
#include "EffekseerEmitter.h"

namespace godot {

void EffekseerEmitter::_register_methods()
{
	register_method("_init", &EffekseerEmitter::_init);
	register_method("_ready", &EffekseerEmitter::_ready);
	register_method("_process", &EffekseerEmitter::_process);
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

void EffekseerEmitter::_process(float delta)
{
	if (!is_visible()) {
		return;
	}

	auto system = EffekseerSystem::get_instance();

	for (int i = 0; i < m_handles.size(); ) {
		auto handle = m_handles[i];
		if (!system->exists(handle)) {
			m_handles.remove(i);
			continue;
		}

		system->draw(get_viewport()->get_camera(), handle);
		i++;
	}
}

void EffekseerEmitter::play()
{
	if (m_effect.is_valid()) {
		auto system = EffekseerSystem::get_instance();
		auto handle = system->play(m_effect, this);
		if (handle >= 0) {
			if (m_paused) {
				system->set_paused(handle, true);
			}
			m_handles.push_back(handle);
		}
	}
}

void EffekseerEmitter::stop()
{
	auto system = EffekseerSystem::get_instance();

	for (int i = 0; i < m_handles.size(); i++) {
		system->stop(m_handles[i]);
	}
	
	m_handles.clear();
}

void EffekseerEmitter::stop_root()
{
	auto system = EffekseerSystem::get_instance();

	for (int i = 0; i < m_handles.size(); i++) {
		system->stop_root(m_handles[i]);
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

	for (int i = 0; i < m_handles.size(); i++) {
		system->set_paused(m_handles[i], paused);
	}
}

bool EffekseerEmitter::is_paused() const
{
	return m_paused;
}

}
