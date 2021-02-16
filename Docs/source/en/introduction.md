# Introduction

## Welcome to EffekseerForGodot
This is the EffekseerForGodot documentation.
Learn about the Effekseer plugin for Godot Engine.
By deploying this plugin into your Godot project, you can play effects created with Effekseer on your Godot Engine.

## Environment and targets

### Godot Engine versions
- Godot Engine 3.2.3 or later
- Godot Engine 4.x is not supported yet

### Target support status

The Effekseer plugin is implemented in GDNative (C ++).
It supports common environments, but other platforms require the user to build it.

The support status for each target of EffekseerForGodot is as follows.

| Target | Status | Architectures |
|-----------|:-------:|---------------|
| Windows   | ✅ | x86, x86_64 |
| macOS     | ✅ | x86_64, arm64 |
| Linux     | ✅ | x86, x86_64 |
| Android   | ✅ | armv7, arm64, x86, x86_64 |
| iOS       | ✅ | armv7, arm64, x86_64(Simulator) |
| HTML5     | ✅ | wasm32 |
| その他    | ❓ | User needs to build | 


## Known issues
- In Advanced Render of Effekseer 1.60 or later, Features other than soft particles do not work.

## Todo
- Advanced Rendering
