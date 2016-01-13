#!/usr/bin/python

import glob
import os

path = "../../oxygine/src/"

def write_folder(path, local):
    #write("\n")
    #write("//%s\n" % local)    

    if local in ("winnie_alloc/", 
                 "dev_tools/", 
                 "text_utils/", 
                 "closure/", 
                 "minizip/", 
                 "utils/",
                 "core/android/",
                 "core/curl/",
                 "core/emscripten/",
                 "core/gl/",
                 "core/ios/",
                 "core/s3e/",
                 ):
        return


    print local

    folders = []
    for f in os.listdir(path + local):        
        full = path + local + f
        name, ext = os.path.splitext(f)
        if ext == ".h" or ext == ".hpp":
            #name = f



            
            nm = "ox/" + name + ".hpp"
            with open(nm, "w") as header:
                header.write("#include \"%s\"" % (local+f, ));

        
        if os.path.isdir(full):
            folders.append(f)
            
    for f in folders:
        write_folder(path, local + f + "/")            

write_folder(path, "")
    