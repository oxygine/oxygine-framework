import os
import sys
path = sys.argv[1]
prefix = sys.argv[2]
ls = os.listdir(path)
ext = [".c", ".cpp", ".h"]
fn = open("ls.txt", "w")

def write(*ext):
	for name in ls:		
	    e = os.path.splitext(name)[1]
	    if e in ext:
	        nm = "\t" + prefix + "/" + name
	        fn.write(nm + " \ \n")

write(".c", ".cpp")
write(".h", ".hpp")