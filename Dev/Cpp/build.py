import sys
import subprocess

def replace_word(file_name, target_str, replace_str):
    text = ""
    with open(file_name, "r") as file:
        text = file.read()

    text = text.replace(target_str, replace_str)

    with open(file_name, "w") as file:
        file.write(text)


replace_word("godot-cpp/Sconstruct", "/MD", "/MT")

if "platform=windows" in sys.argv:
    subprocess.run("scons platform=windows bits=32 generate_bindings=yes target=release -j4", cwd = "godot-cpp", shell = True)
    subprocess.run("scons platform=windows bits=64 generate_bindings=yes target=release -j4", cwd = "godot-cpp", shell = True)

    subprocess.run("scons platform=windows bits=32 target=release -j4", shell = True)
    subprocess.run("scons platform=windows bits=64 target=release -j4", shell = True)

elif "platform=android" in sys.argv:
    subprocess.run("scons platform=android android_arch=armv7 generate_bindings=yes target=release -j4", cwd = "godot-cpp", shell = True)
    subprocess.run("scons platform=android android_arch=arm64v8 generate_bindings=yes target=release -j4", cwd = "godot-cpp", shell = True)
    subprocess.run("scons platform=android android_arch=x86 generate_bindings=yes target=release -j4", cwd = "godot-cpp", shell = True)
    subprocess.run("scons platform=android android_arch=x86_64 generate_bindings=yes target=release -j4", cwd = "godot-cpp", shell = True)

    subprocess.run("scons platform=android android_arch=armv7 target=release -j4", shell = True)
    subprocess.run("scons platform=android android_arch=arm64v8 target=release -j4", shell = True)
    subprocess.run("scons platform=android android_arch=x86 target=release -j4", shell = True)
    subprocess.run("scons platform=android android_arch=x86_64 target=release -j4", shell = True)
