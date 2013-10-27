import os
import sys
path = sys.argv[1]
prefix = sys.argv[2]
ls = os.listdir(path)
ext = [".c", ".cpp", ".h"]
fn = open("ls.txt", "w")
for name in ls:
    e = os.path.splitext(name)[1]
    if e in ext:
        nm = prefix + "/" + name
        #nm = os.path.normpath(prefix + "/" + name)
        fn.write(nm + "\n")