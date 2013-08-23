import os
import sys
import shutil
import zipfile

dest = "../../temp/oxygine-framework/"

print "cleaning temp..."
shutil.rmtree("temp", True)

print "hg archive..."
os.system("hg archive " + dest)
os.chdir(dest)

os.system("doxygen " + dest + "Doxyfile")

def recursive_zip(zipf, directory, folder = ""):
    for item in os.listdir(directory):
        if os.path.isfile(os.path.join(directory, item)):
            zipf.write(os.path.join(directory, item), folder + os.sep + item)
        elif os.path.isdir(os.path.join(directory, item)):
            recursive_zip(zipf, os.path.join(directory, item), folder + os.sep + item)


with zipfile.ZipFile("doc.zip", "w", compression = zipfile.ZIP_DEFLATED) as zp:
	recursive_zip(zp, "doc")
	shutil.rmtree("doc")

with zipfile.ZipFile("../../oxygine-framework.zip", "w", compression = zipfile.ZIP_DEFLATED) as zp:
	recursive_zip(zp, "../")

print "done."