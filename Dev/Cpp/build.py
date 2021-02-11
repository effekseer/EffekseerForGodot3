import sys
import os
import shutil
import subprocess

def replace_word(file_name, target_str, replace_str):
    text = ""
    with open(file_name, "r") as file:
        text = file.read()

    text = text.replace(target_str, replace_str)

    with open(file_name, "w") as file:
        file.write(text)


if "platform=windows" in sys.argv:
    replace_word("godot-cpp/Sconstruct", "/MD", "/MT")

    subprocess.run("scons platform=windows bits=32 generate_bindings=yes target=release -j4", cwd = "godot-cpp", shell = True)
    subprocess.run("scons platform=windows bits=64 generate_bindings=yes target=release -j4", cwd = "godot-cpp", shell = True)

    subprocess.run("scons platform=windows bits=32 target=release -j4", shell = True)
    subprocess.run("scons platform=windows bits=64 target=release -j4", shell = True)

    os.makedirs("../Godot/addons/effekseer/bin/win32", exist_ok = True)
    os.makedirs("../Godot/addons/effekseer/bin/win64", exist_ok = True)

    shutil.copy2("bin/libeffekseer.win32.dll", "../Godot/addons/effekseer/bin/windows/")
    shutil.copy2("bin/libeffekseer.win64.dll", "../Godot/addons/effekseer/bin/windows/")

elif "platform=osx" in sys.argv:
    subprocess.run("scons platform=osx bits=64 generate_bindings=yes target=release -j4", cwd = "godot-cpp", shell = True)

    subprocess.run("scons platform=osx bits=64 target=release -j4", shell = True)

    os.makedirs("../Godot/addons/effekseer/bin/osx", exist_ok = True)

    shutil.copy2("bin/libeffekseer.osx.dylib", "../Godot/addons/effekseer/bin/osx/")

elif "platform=android" in sys.argv:
    subprocess.run("scons platform=android android_arch=armv7 generate_bindings=yes target=release -j4", cwd = "godot-cpp", shell = True)
    subprocess.run("scons platform=android android_arch=arm64v8 generate_bindings=yes target=release -j4", cwd = "godot-cpp", shell = True)
    subprocess.run("scons platform=android android_arch=x86 generate_bindings=yes target=release -j4", cwd = "godot-cpp", shell = True)
    subprocess.run("scons platform=android android_arch=x86_64 generate_bindings=yes target=release -j4", cwd = "godot-cpp", shell = True)

    subprocess.run("scons platform=android android_arch=armv7 target=release -j4", shell = True)
    subprocess.run("scons platform=android android_arch=arm64v8 target=release -j4", shell = True)
    subprocess.run("scons platform=android android_arch=x86 target=release -j4", shell = True)
    subprocess.run("scons platform=android android_arch=x86_64 target=release -j4", shell = True)

    os.makedirs("../Godot/addons/effekseer/bin/android", exist_ok = True)

    shutil.copy2("bin/libeffekseer.android-armv7.so", "../Godot/addons/effekseer/bin/android/")
    shutil.copy2("bin/libeffekseer.android-arm64v8.so", "../Godot/addons/effekseer/bin/android/")
    shutil.copy2("bin/libeffekseer.android-x86.so", "../Godot/addons/effekseer/bin/android/")
    shutil.copy2("bin/libeffekseer.android-x86_64.so", "../Godot/addons/effekseer/bin/android/")

elif "platform=ios" in sys.argv:
    replace_word("godot-cpp/Sconstruct", "-version-min=10.0", "-version-min=9.0")

    subprocess.run("scons platform=ios ios_arch=armv7 generate_bindings=yes target=release -j4", cwd = "godot-cpp", shell = True)
    subprocess.run("scons platform=ios ios_arch=arm64 generate_bindings=yes target=release -j4", cwd = "godot-cpp", shell = True)
    subprocess.run("scons platform=ios ios_arch=x86_64 generate_bindings=yes target=release -j4", cwd = "godot-cpp", shell = True)

    subprocess.run("scons platform=ios ios_arch=armv7 target=release -j4", shell = True)
    subprocess.run("scons platform=ios ios_arch=arm64 target=release -j4", shell = True)
    subprocess.run("scons platform=ios ios_arch=x86_64 target=release -j4", shell = True)

    subprocess.run("lipo -create bin/libeffekseer.ios-armv7.dylib bin/libeffekseer.ios-arm64.dylib bin/libeffekseer.ios-x86_64.dylib -output bin/libeffekseer.ios.dylib", shell = True)

    os.makedirs("../Godot/addons/effekseer/bin/ios", exist_ok = True)
    
    shutil.copy2("bin/libeffekseer.ios.dylib", "../Godot/addons/effekseer/bin/ios/")

if "platform=linux" in sys.argv:
    subprocess.run("scons platform=linux bits=32 generate_bindings=yes target=release use_llvm=1 -j4", cwd = "godot-cpp", shell = True)
    subprocess.run("scons platform=linux bits=64 generate_bindings=yes target=release use_llvm=1 -j4", cwd = "godot-cpp", shell = True)

    subprocess.run("scons platform=linux bits=32 target=release use_llvm=1 -j4", shell = True)
    subprocess.run("scons platform=linux bits=64 target=release use_llvm=1 -j4", shell = True)

    os.makedirs("../Godot/addons/effekseer/bin/linux", exist_ok = True)

    shutil.copy2("bin/libeffekseer.linux-32.so", "../Godot/addons/effekseer/bin/linux/")
    shutil.copy2("bin/libeffekseer.linux-64.so", "../Godot/addons/effekseer/bin/linux/")
