#include <SpatialMaterial.hpp>
#include <ResourceLoader.hpp>
#include <Texture.hpp>
#include "EffekseerEmitter.h"
#include "EffekseerEmitterGizmo.h"

namespace godot {

void EffekseerEmitterGizmo::_register_methods()
{
	register_method("_init", &EffekseerEmitterGizmo::_init);
	//register_method("get_name", &EffekseerEmitterGizmo::get_name);
	register_method("has_gizmo", &EffekseerEmitterGizmo::has_gizmo);
	register_method("redraw", &EffekseerEmitterGizmo::redraw);
}

EffekseerEmitterGizmo::EffekseerEmitterGizmo()
{
}

EffekseerEmitterGizmo::~EffekseerEmitterGizmo()
{
}

void EffekseerEmitterGizmo::_init()
{
	auto loader = godot::ResourceLoader::get_singleton();
	auto iconTexture = loader->load("res://addons/effekseer/icon64.png");
	
	create_material("lines", Color(0.60f, 0.93f, 0.95f), false, false, true);
	create_icon_material("icon", iconTexture);
}

//String EffekseerEmitterGizmo::get_name()
//{
//	return "EffekseerEmitter";
//}

bool EffekseerEmitterGizmo::has_gizmo(Spatial* spatial)
{
	return Object::cast_to<EffekseerEmitter>(spatial) != nullptr;
}

void EffekseerEmitterGizmo::redraw(EditorSpatialGizmo* gizmo)
{
	EffekseerEmitter* emitter = Object::cast_to<EffekseerEmitter>(gizmo->get_spatial_node());

	gizmo->clear();

	{
		auto iconMaterial = get_material("icon", gizmo);
		gizmo->add_unscaled_billboard(iconMaterial, 0.04f);
	}

	{
		AABB aabb;
		aabb.position = Vector3(-0.5f, -0.5f, -0.5f);
		aabb.size = Vector3(1.0f, 1.0f, 1.0f);

		PoolVector3Array lines;
		for (int i = 0; i < 12; i++) {
			Vector3 a, b;
			aabb.get_edge(i, a, b);
			lines.push_back(a);
			lines.push_back(b);
		}

		auto linesMaterial = get_material("lines", gizmo);
		gizmo->add_lines(lines, linesMaterial);
	}
}

}
