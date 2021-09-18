# API Reference

## EffekseerEmitter

**Extends**: Spatial < Node < Object

Source of Effekseer effect in 3D scene.

### Descriptions

A 3D object for playing and rendering Effekseer effects.
You can play it by setting `effect` and `play()`.
In addition, the set transforms (position, rotation, scaling) are reflected in the rendering of the effect.


### Properties

#### EffekseerEffect effect

|           |                   |
|-----------|-------------------|
| *Setter*	| set_effect(value) |
| *Getter*	| get_effect()      |

The effect resource set on the emitter.

----

#### bool autoplay

|           |                     |
|-----------|---------------------|
| *Setter*	| set_autoplay(value) |
| *Getter*	| is_autoplay()       |

Autoplay settings.

- true: play automatically
- false: Do not play automatically

----

#### bool paused

|           |                   |
|-----------|-------------------|
| *Setter*	| set_paused(value) |
| *Getter*	| is_paused()       |

Pause settings.

- true: Pause
- false: Do not pause

----

#### bool speed

|           |                   |
|-----------|-------------------|
| *Setter*	| set_speed(value)  |
| *Getter*	| get_speed()       |

Playback speed setting. Range: 0.0 to 10.0

- 0.5: Half speed playback
- 2.0: Double speed playback

----

#### Color color

|           |                   |
|-----------|-------------------|
| *Setter*	| set_color(value)  |
| *Getter*	| get_color()       |

Rendering color setting. It is multiplied by the color of the effect.

----

#### Vector3 target_position

|           |                   |
|-----------|-------------------|
| *Setter*	| set_target_position(value)  |
| *Getter*	| get_target_position()       |

Target position setting. The position of the effect with the target.

----

### Methods

#### void play()
Playback will start.

----

#### void stop()
Playback will stop.

----

#### void stop_root()
Stop the root node.

----

#### bool is_playing()
Get the playback status.

- true: Playing
- false: Not played or finished playing

----

### Signals

#### finished()
Emitted when the effect finishes playing.

----

## EffekseerEmitter2D

**Extends**: Node2D < CanvasItem < Node < Object

Source of Effekseer effect in 2D scene.

### Descriptions

A 2D object for playing and rendering Effekseer effects.
You can play it by setting `effect` and `play()`.
In addition, the set transforms (position, rotation, scaling) are reflected in the rendering of the effect.


### Properties

#### EffekseerEffect effect

|           |                   |
|-----------|-------------------|
| *Setter*	| set_effect(value) |
| *Getter*	| get_effect()      |

The effect set on the emitter.

----

#### bool autoplay

|           |                     |
|-----------|---------------------|
| *Setter*	| set_autoplay(value) |
| *Getter*	| is_autoplay()       |

Autoplay settings.

- true: play automatically
- false: Do not play automatically

----

#### bool paused

|           |                   |
|-----------|-------------------|
| *Setter*	| set_paused(value) |
| *Getter*	| is_paused()       |

Pause settings.

- true: Pause
- false: Do not pause

----

#### bool speed

|           |                   |
|-----------|-------------------|
| *Setter*	| set_speed(value)  |
| *Getter*	| get_speed()       |

Playback speed setting. Range: 0.0 to 10.0

- 0.5: Half speed playback
- 2.0: Double speed playback

----

#### Color color

|           |                   |
|-----------|-------------------|
| *Setter*	| set_color(value)  |
| *Getter*	| get_color()       |

Rendering color setting. It is multiplied by the color of the effect.

----

#### Vector3 orientation

|           |                   |
|-----------|-------------------|
| *Setter*	| set_orientation(value)  |
| *Getter*	| get_orientation()       |

3D orientation settings for effects. It is applied before the rotation of Transform2D.

----

#### Vector2 target_position

|           |                   |
|-----------|-------------------|
| *Setter*	| set_target_position(value)  |
| *Getter*	| get_target_position()       |

Target position setting. The position of the effect with the target.

----

### Methods

#### void play()
Playback will start.

----

#### void stop()
Playback will stop.

----

#### void stop_root()
Stop the root node.

----

#### bool is_playing()
Get the playback status.

- true: Playing
- false: Not played or finished playing

----

### Signals

#### finished()
Emitted when the effect finishes playing.

----

## EffekseerEffect

**Extends**: Resource < Reference < Object

Effekseer effect resource.

### Properties

#### String data_path

|           |                        |
|-----------|------------------------|
| *Setter*	| set_data_path(value)  |
| *Getter*	| get_data_path()       |

The path of the loaded effect file.

Normally do not change.

----

#### PoolByteArray data_bytes

|           |                        |
|-----------|------------------------|
| *Setter*	| set_data_bytes(value)  |
| *Getter*	| get_data_bytes()       |

Byte data of the loaded effect file.

Normally do not change.

----

#### Array subresources

|           |                          |
|-----------|--------------------------|
| *Setter*	| set_subresources(value)  |
| *Getter*	| get_subresources()       |

Dependent subresources for the loaded effect.

Normally do not change.

----

### Methods

#### void load(String path)
Load the effect by specifying the file path.

Basically, don't use this method, just use Godot's resource load.

----

#### void release()
Release the effect.

Basically, don't use this method, just release it with Godot's resource feature.

----

## EffekseerSystem

**Extends**: Node < Object

Effekseer singleton for system management.

### Methods

#### void stop_all_effects()
Stops all currently playing effects.

----

#### void set_paused_to_all_effects()
Pause settings for all currently playing effects.

----

#### int get_total_instance_count()
Gets the number of instances currently in use.

----
