# Release notes

## 1.62
- Changes
  - Updated to Effekseer 1.62
  - Updated to Godot-cpp 3.4
  - Add a inspector plugin for effects preview
  - Add support for `get_tree().paused`.
  - Add support `visibility` of Emitter/Emitter2D to be reflected in the effect display
  - Optimize Emitter/Emitter2D update/render loop
  - Optimize the size of the EffectResource (reduced unnecessary data and applied compression)
  - EffekseerSpatialGizmoPlugin has been named
- Bugfixes
  - `target_position` doesn't work when play() in Emitter

## 1.61c
- Changes
  - Updated to Effekseer 1.61c
  - Shrink the size of binary files such as dll/so/dylib/wasm
  - Add signal `finished` to Emitter/Emitter2D
  - Add property `target_position` to Emitter/Emitter2D
- Bugfixes
  - Runtime error when using depth textures with custom materials
  - Crash when specifying null for `effect` of Emitter/Emitter2D
  - Not correct color space (Corrected to linear color)

## 1.60 Beta3
- Update to latest of Effekseer 1.60 branch.

## 1.60 Beta2
- Bugfixes
  - Not good 2D transform calculation.
  - Crash in 2D sound playback.
  - Effects would not be updated when high FPS. (#1)

## 1.60 Beta1
- First release the EffekseerForGodot.