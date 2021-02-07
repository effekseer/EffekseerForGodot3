extends MeshInstance

func _ready():
	var material = material_override as ShaderMaterial
	material.set_shader_param("Texture", $Viewport2D.get_texture())


