# Target specific informations

## Mobile
The following Effekseer features will not work with the default settings on mobile targets.

- Distortion shader
- Soft particles

For the above effects to work, you need to set `3D` or `2D` to `Framebuffer Allocation.mobile` in` Quality` of Godot's `project settings`.

![](../img/Godot_mobile_framebuffer.png)

## HTML5
The `Export Type` must be set to` GDNative` to enable the Effekseer plugin on HTML5 targets.

![](../img/Godot_export_html5.png)
