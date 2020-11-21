#pragma once

#include <EditorSpatialGizmo.hpp>
#include <EditorSpatialGizmoPlugin.hpp>

namespace godot {

class EffekseerEmitterGizmo : public EditorSpatialGizmoPlugin
{
	GODOT_CLASS(EffekseerEmitterGizmo, EditorSpatialGizmoPlugin)

public:
	static void _register_methods();

	EffekseerEmitterGizmo();

	~EffekseerEmitterGizmo();

	void _init();

	//String get_name();

	bool has_gizmo(Spatial* spatial);

	void redraw(EditorSpatialGizmo* gizmo);

private:
};

}
