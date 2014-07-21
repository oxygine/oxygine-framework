import os

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
            if ext not in (".dll", ".py", ".icf", ".py", ".js"):
                data.append(full)

files = []

listdir(".", files)
print files
s = ""
for f in files:
    s = s + " --embed " + f
    
cmd = "file_packager.py test --js-output=data.js " + s
print cmd
os.system(cmd)