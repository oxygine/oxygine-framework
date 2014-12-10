import os
import sys
import shutil
import zipfile

temp = "../../temp"
SDL_dest = temp + "/SDL"
OXYGINE_dest = temp + "/oxygine-framework/"
SOUND_dest = temp + "/oxygine-sound/"

print("cleaning temp...")
shutil.rmtree(temp, True)

print("hg archive...")
cmd = "hg archive " + OXYGINE_dest
os.system(cmd)

cmd = "hg archive -R ../../../SDL %s" % (SDL_dest, )
os.system(cmd)

cmd = "hg archive -R ../../../oxygine-sound %s" % (SOUND_dest, )
os.system(cmd)

shutil.rmtree(SDL_dest + "/test")

def fix_file(name, cb):
    data = open(name, "rb").read()    
    data = cb(data)
    open(name, "wb").write(data)
    


fix_file(SDL_dest + "/include/SDL_config_windows.h", 
         lambda data: data.replace("#define SDL_AUDIO_DRIVER_XAUDIO2", "//#define SDL_AUDIO_DRIVER_XAUDIO2")
         )

fix_file(SDL_dest + "/src/video/uikit/SDL_uikitview.h", 
         lambda data: data.replace("#define IPHONE_TOUCH_EFFICIENT_DANGEROUS", "//#define IPHONE_TOUCH_EFFICIENT_DANGEROUS")
         )

def enum(folder, cb):
    for item in os.listdir(folder):        
        path = folder + item 
        if os.path.isdir(path):
            if item == "data":
                print(path)
                cb(path)
            enum(path + "/", cb)
            
def copy(path):
    win32 = OXYGINE_dest + "/oxygine/third_party/win32/dlls/"
    items = (win32 + "zlib1.dll", 
             win32 + "pthreadVCE2.dll",
             "../../libs/SDL2.dll")
    for item in items:
        name = os.path.split(item)[1]
        shutil.copy(item, path + "/" + name)
        
    
        
enum(OXYGINE_dest + "/examples/", copy)
enum(SOUND_dest + "/examples/", copy)

shutil.copy(SDL_dest + "/android-project/src/org/libsdl/app/SDLActivity.java", 
            OXYGINE_dest + "/oxygine/SDL/android/lib/src/org/libsdl/app/SDLActivity.java")

libs = ("SDL2.lib", "SDL2main.lib")
for lib in libs:
    shutil.copy("../../libs/" + lib, OXYGINE_dest + "/libs/" + lib)

os.chdir(OXYGINE_dest)



def recursive_zip(zipf, directory, folder = ""):
    for item in os.listdir(directory):
        if os.path.isfile(os.path.join(directory, item)):
            zipf.write(os.path.join(directory, item), folder + os.sep + item)
        elif os.path.isdir(os.path.join(directory, item)):
            recursive_zip(zipf, os.path.join(directory, item), folder + os.sep + item)

destzip = "../../oxygine-framework-with-sdl.zip"
with zipfile.ZipFile(destzip, "w", compression = zipfile.ZIP_DEFLATED) as zp:
    recursive_zip(zp, "../")
    
shutil.copyfile(destzip, "../../../gdrive/oxygine/oxygine-framework-with-sdl.zip")

print("done.")