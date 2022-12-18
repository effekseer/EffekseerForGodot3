#pragma once

#include <Godot.hpp>
#include <Resource.hpp>
#include <Effekseer.h>

namespace godot {

class EffekseerEffect : public Resource
{
	GODOT_CLASS(EffekseerEffect, Resource)

public:
	enum class TargetLayer : int32_t {
		Both,
		_2D,
		_3D,
	};

public:
	static void _register_methods();

	EffekseerEffect();
	~EffekseerEffect();

	void _init();

	void import(String path, bool shrink_binary);

	void load();

	void release();

	Dictionary get_subresources() const { return m_subresources; }

	void set_subresources(Dictionary subresources) { m_subresources = subresources; }

	PoolByteArray get_data_bytes() const { return m_data_bytes; }

	void set_data_bytes(PoolByteArray bytes) { m_data_bytes = bytes; }

	float get_scale() const { return m_scale; }

	void set_scale(float scale) { m_scale = scale; }

	Effekseer::EffectRef& get_native() { return m_native; }

private:
	void setup_node_render(Effekseer::EffectNode* node, TargetLayer targetLayer);

	PoolByteArray m_data_bytes;
	Dictionary m_subresources;
	float m_scale = 1.0f;
	TargetLayer m_targetLayer = TargetLayer::Both;
	Effekseer::EffectRef m_native;
};

}
