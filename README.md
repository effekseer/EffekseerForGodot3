# EffekseerForGodot3

## Overview
This is the Effekseer's runtime plugin for [Godot Engine](https://godotengine.org).  
You will be able to show the effects that was created with Effekseer.  

[Godot Engine](https://godotengine.org)向けEffekseerプラグインです。  
Effekseerで作成したエフェクトをGodot Engineで表示することができます。

- [Official website](http://effekseer.github.io)

## How to use the plugin

- [How to use (English)](https://effekseer.github.io/Help_Godot/en/how-to-use.html)
- [How to use (Japanese)](https://effekseer.github.io/Help_Godot/en/how-to-use.html)

## Documents

- [Help](https://effekseer.github.io/Help_Godot/index.html)

## How to develop the plugin

### Clone the repository

```
git clone https://github.com/effekseer/EffekseerForGodot3 --recursive
```

### Build binaries

#### Windows

- python3 and scons `pip install scons`
- Visual Studio 2017 or later.

Execute `python3 Dev/Cpp/build.py platform=windows`

#### macOS, iOS

- python3 and scons `brew install scons`
- Latest Xcode

Execute `python3 Dev/Cpp/build.py platform=osx`
Execute `python3 Dev/Cpp/build.py platform=ios`

#### Android

- macOS or Linux
- python3 and scons
- Android NDK. ANDROID_NDK_ROOT required.

Execute `python3 Dev/Cpp/build.py platform=android`

#### Linux

- Ubuntu (64bit)
- python3 and scons `apt install scons`
- Multilib `apt install gcc-multilib g++-multilib`

Execute `python3 Dev/Cpp/build.py platform=linux`

### Edit native codes

#### Windows

Create an effekseer build environment.

```
mkdir Dev/Effekseer/build
cd Dev/Effekseer/build
cmake -DBUILD_VIEWER=ON -DBUILD_EDITOR=ON .. 
```

Uses Visual Studio 2017 or later, to open and build the following solution file.

- Dev/Cpp/EffekseerGodot.sln

### Run by Godot Engine

Uses Godot Engine 3.x, to open the following project file.

- Dev/Godot/project.godot
