extends EditorSpatialGizmoPlugin


var EffekseerEmitter = preload("EffekseerEmitter.gdns")


func _init():
	create_material("lines", Color(0.60, 0.93, 0.95), false, false, true);
	create_icon_material("icon", preload("../icon64.png"));


func has_gizmo(spatial):
	return spatial.get_script() == EffekseerEmitter


func redraw(gizmo: EditorSpatialGizmo):
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
