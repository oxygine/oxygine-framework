import os
import shutil
shutil.rmtree("data", True)
shutil.copytree("original", "data")
os.system("..\\..\\tools\\process_xml2.py -x system\\res.xml --src_data data --dest_data data\\system")
shutil.rmtree("data/system/anims/")

d = ["..\\..\\examples\\Demo\\data",
     "..\\..\\examples\\DemoBox2D\\data",
     "..\\..\\examples\\GameTemplate\\data",
     "..\\..\\examples\\HelloWorld\\data",
     "..\\..\\examples\\Match3\\data",
     "..\\..\\..\\oxygine-movieclip\\examples\\example1\\data",
     "..\\..\\..\\oxygine-frame\\examples\\example1\\data",
     "..\\..\\..\\oxygine-frame\\examples\\example2\\data",
     "..\\..\\..\\oxygine-sound\\test\\data",
     "..\\..\\..\\oxygine-pipeline\\example\\game\\runnable\\data",
     ]
for dest in d:
    s = dest + "\\system"
    shutil.rmtree(s, True)
    print s
    shutil.copytree("data\\system", s)
    