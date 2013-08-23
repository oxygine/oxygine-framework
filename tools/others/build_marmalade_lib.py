import os
import shutil
import glob
import subprocess

print "building oxygine"
marmalade = os.environ["S3E_DIR"]

os.chdir("../")

RESULT = "oxygine.zip"

print "cleaning temp..."
shutil.rmtree("temp", True)

if os.path.exists(RESULT):
    shutil.move(RESULT, "_" + RESULT)


print "hg archive..."
os.system("hg archive temp/oxygine")

os.chdir("temp\\oxygine")

mkb = "oxygine\\marmalade\\oxygine_lib.mkb"
cmd = "%(marmalade)s\\python\\python.exe %(marmalade)s\\makefile_builder\\mkb.py --make " % {"marmalade" : marmalade, };

cm = "%(cmd)s --arm --compiler=gcc --no-ide %(mkb)s" % {"cmd":cmd, "mkb":mkb}
os.system(cm)
cm = "%(cmd)s --x86 --no-ide %(mkb)s" % {"cmd":cmd, "mkb":mkb}
os.system(cm)

os.system("doxygen")


def rec_del(path):
    for each in os.listdir(path):
        new_path = os.path.join(path, each)
        
        if not os.path.isfile(new_path):
            rec_del(new_path)
        else:
            os.remove(new_path)
    os.rmdir(path)


def processFolder(folder_path):
    folder_path += "//"
    try:
        oxbuild_path = folder_path + ".oxbuild"
        oxbuild = open(oxbuild_path)
        for line in oxbuild.readlines():
            line = line.rstrip()
            item2delete = folder_path + line
            
            filelist = glob.glob(item2delete)
            for file in filelist:
                #print file
                if os.path.isfile(file):
                    os.remove(file)
                else:
                    rec_del(file)
                
            #print delit
            
        oxbuild.close()
        os.remove(oxbuild_path)
    except IOError:
        pass
        
    for each in os.listdir(folder_path):
        full_path = os.path.join(folder_path, each)  
        if not os.path.isfile(full_path):
            #print full_path
            processFolder(full_path)
            
        #print full_path


processFolder(".")



def recursive_zip(zipf, directory, folder = ""):
    for item in os.listdir(directory):
        if os.path.isfile(os.path.join(directory, item)):
            zipf.write(os.path.join(directory, item), folder + os.sep + item)
        elif os.path.isdir(os.path.join(directory, item)):
            recursive_zip(zipf, os.path.join(directory, item), folder + os.sep + item)
            
os.chdir("..\\..\\")
            
print "zipping..."
import zipfile

zp = zipfile.ZipFile("temp/oxygine/doc.zip", "w", compression = zipfile.ZIP_DEFLATED)
recursive_zip(zp, "temp/oxygine/doc")
zp.close()
shutil.rmtree("temp/oxygine/doc")

zipf = zipfile.ZipFile(RESULT, "w", compression = zipfile.ZIP_DEFLATED)
path = "temp"
recursive_zip(zipf, path)
zipf.close()

print "done."