extends MenuButton

signal effect_choosed(effect_path)

var effect_list = []

func _ready():
	setup_menu("res://effect", get_popup())


func setup_menu(path: String, menu: PopupMenu):
	menu.name = path
	menu.connect("id_pressed", self, "_on_item_pressed")
	var file_count := 0
	var dir := Directory.new()
	if dir.open(path) == OK:
		dir.list_dir_begin(true)
		var name = dir.get_next()
		while name != "":
			if dir.current_is_dir():
				var submenu := PopupMenu.new()
				if setup_menu(path + "/" + name, submenu) > 0:
					menu.add_submenu_item(name, submenu.name)
					menu.add_child(submenu)
			elif name.ends_with("efkefc.import"):
				name = name.substr(0, name.find_last(".import"))
				file_count += 1
				menu.add_item(name, len(effect_list))
				effect_list.append(path + "/" + name)
			name = dir.get_next()
		dir.list_dir_end()
	return file_count


func _on_item_pressed(id):
	print(effect_list[id])
	emit_signal("effect_choosed", effect_list[id])
