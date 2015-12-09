#!/usr/bin/python

import os

SKIP_EXT = (".dll", ".py", ".icf", ".py", ".js", ".ogg")

def do(src, dest):
    def listdir(path, data):
        for f in os.listdir(path):
            if path != ".":        
                full = path + "/" + f
            else:
                full = f
                
            if os.path.isdir(full):
                listdir(full, data)
            else:
                ext = os.path.splitext(f)[1]
                if ext not in SKIP_EXT:
                    data.append(full)

    files = []

    listdir(src, files)
    print(files)
    s = ""
    for f in files:
        s = s + " --embed " + f
        
    cmd = "python \"" + os.environ["EMSCRIPTEN"] + "/tools/file_packager.py\" test --lz4  --js-output=\"%s\" %s" % (dest, s)
    print(cmd)
    os.system(cmd)

    
if __name__ == "__main__":

    import argparse 
    parser = argparse.ArgumentParser(description = "embedding folder into one js file")
    parser.add_argument("-s", "--src", help = "source folder", default = ".")
    parser.add_argument("-o", "--output", help = "destination js file", default = "data.js")
    
    args = parser.parse_args()
    output = os.path.relpath(args.output, args.src)
    os.chdir(args.src)
    do(".", output)