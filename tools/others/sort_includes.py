import os
import sys

def sort_file(path):
    lines = open(path, "r").readlines()
    if len(lines) == 0:
        return

    skipped = 0
    while True:
        line = lines[skipped]
        if line.startswith("#pragma once") or line == "\n" or line.startswith("//"):
            skipped += 1
        else:
            break

    n = skipped
    for line in lines[skipped:]:
        if not line.startswith("#include") and not line.startswith("//#include") and not line == "\n":
            break
        n += 1

    includes = set(lines[skipped:n])

    def cm(s1, s2):
        return s1 >= s2

    (fname, ext) = os.path.splitext(path)
    fname = os.path.split(fname)[1]
    pair_header = "\"{}.h\"".format(fname)

    def k(s):
        w = 0
        def getw(s):
            if s.find("<") != -1 and s.find(">") != -1:
                return 5

            if s.find("oxygine-include") != -1:
                return 1

            parts = len(s.split("/"))
            if ext == ".cpp" and parts == 1 and s.find(pair_header) != -1:
                return 0

            if parts == 1:
                return 2

            return 3#parts

        w = getw(s)
        return (w, s)

    includes = sorted(includes, key = k)
    includes = filter(lambda s: s != "\n", includes)
    if len(includes) == 0:
        print("skipped {}".format(path))
        return

    includes += "\n"

    with open(path, "w") as fh:
        fh.writelines(lines[0:skipped])
        fh.writelines(includes)
        fh.writelines(lines[n:])

    #print path

def run(folder):
    items = os.listdir(folder)
    for item in items:
        path = os.path.join(folder, item)
        if os.path.isdir(path):
            run(path)
            continue
        sort_file(path)

if __name__ == "__main__":
    if len(sys.argv) > 1:
        path = sys.argv[1]
    else:
        path = "../../oxygine/src/oxygine"
    run(path)

#sort_file("../project/src/packs/app_packs.cpp")
#sort_file("../../oxygine/src/oxygine/stdrenderer.cpp")