extends EditorSpatialGizmoPlugin


var is_first_draw = true

func get_name() -> String:
	return "EffekseerEmitter"


func has_gizmo(spatial) -> bool:
	return spatial.get_script() == preload("res://addons/effekseer/src/EffekseerEmitter.gdns")


func redraw(gizmo: EditorSpatialGizmo):
	if is_first_draw:
		create_material("lines", Color(0.60, 0.93, 0.95), false, false, true);
		create_icon_material("icon", load("res://addons/effekseer/icon64.png"));
		is_first_draw = false
	
	var emitter = gizmo.get_spatial_node()
	gizmo.clear()

	gizmo.add_unscaled_billboard(get_material("icon", gizmo), 0.04);
	
	var aabb: AABB
	aabb.position = Vector3(-0.5, -0.5, -0.5)
	aabb.size = Vector3(1.0, 1.0, 1.0)

	var lines: PoolVector3Array	
	for i in [0, 1, 2, 3]:
		var pos = aabb.get_endpoint(i)
		lines.push_back(pos)
		lines.push_back(pos + Vector3(1, 0, 0))
	for i in [0, 2, 4, 6]:
		var pos = aabb.get_endpoint(i)
		lines.push_back(pos)
		lines.push_back(pos + Vector3(0, 0, 1))
	for i in [0, 1, 4, 5]:
		var pos = aabb.get_endpoint(i)
		lines.push_back(pos)
		lines.push_back(pos + Vector3(0, 1, 0))
	
	gizmo.add_lines(lines, get_material("lines", gizmo));
