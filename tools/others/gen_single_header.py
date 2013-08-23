import glob
import os

path = "../oxygine/src/"

def write_folder(path, local, handle):
    write("\n")
    write("//%s\n" % local)
    
    folders = []
    for f in os.listdir(path + local):        
        full = path + local + f
        name, ext = os.path.splitext(f)
        if ext == ".h" or ext == ".hpp":
            name = f
            write("#include \"%s\"\n" % (local + name, ))
        
        if os.path.isdir(full):
            folders.append(f)
            
    for f in folders:
        write_folder(path, local + f + "/", handle)            
    

with open("oxygine-framework.h", "w") as header:
    write = header.write
    write("//oxygine single include\n")
    
    write_folder(path, "", header)
    