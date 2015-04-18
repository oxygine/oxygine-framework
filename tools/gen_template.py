#!/usr/bin/python

from string import Template
import os
import io
import glob
import shutil
from mimetypes import guess_type
from mimetypes import add_type



platforms = ("win32", "android", "macosx", "ios", "cmake", "emscripten", "all")

def relpath(a, b):
    try:
        return os.path.relpath(a, b)
    except ValueError:
        pass   
    return a

def unixpath(path):
    return path.replace("\\", "/")

def run(args):
    if args.hello:
        dest = args.dest
        
        local = os.path.dirname(__file__)
        if not local:
            local = "."
        
        src = local + "/../examples/HelloWorld/src"
        shutil.copytree(src, dest + "/src")
        
        data = local + "/../examples/HelloWorld/data"
        shutil.copytree(src, dest + "/data")
        
        for p in platforms:
            if p == "all":
                continue
            args.type = p
            args.src = dest + "/src"
            args.dest = dest + "/proj." + p
            _run(args)
    else:
        _run(args)
            

def _run(args):
    name = args.name
    project = "proj." + args.type + "/"
    values = {"PROJECT":name}


    text = (".properties", ".cmd", ".mk", ".java", ".sln", ".vcxproj", ".filters", ".user", ".plist", ".pch", ".json", ".pbxproj")
    for t in text:
        add_type("text/plain", t)

    try:
        os.makedirs(args.dest)
    except OSError:
        #print "folder already exists"
        #return
        pass

    templates_path = os.path.dirname(__file__)
    if not templates_path:
        templates_path = "."
    templates_path += "/templates/"
    ox_path = templates_path + "/../../"
    root_path = ox_path + "../"    
    root_path = os.path.abspath(root_path)
    ox_path = os.path.abspath(ox_path)


    dest_path = os.path.abspath(args.dest) + "/"

    root_path = relpath(root_path, dest_path) + "/"
    ox_path = relpath(ox_path, dest_path) + "/"


    values["OXYGINE"] = unixpath(ox_path)
    values["ROOT"] = unixpath(root_path)


    def process(template, relto, path, gl, fn = None):
        res = ""
        for src in gl:
            ext = os.path.splitext(src)[1]
            if ext in (".icf", ".orig"):
                continue

            rel_src_path = relpath(src, relto)    
            data = {"FILE":unixpath(rel_src_path), "FULLFILE":src}
            if fn:
                data = fn(data)
            t = Template(template)
            res += t.safe_substitute(data)

        return res

    SRC = ""
    INCLUDE = ""

    data = "../data"
    abs_data = os.path.normpath(args.dest + "/" + data)
    
    data_files_ = sorted(glob.glob(abs_data + "/*"))
    data_files = []
    for d in data_files_:
        if os.path.splitext(d)[1]  in (".dll", ):
            continue
        data_files.append(d)

    cpp_files = sorted(glob.glob(args.src + "/*.cpp"))
    h_files = sorted(glob.glob(args.src + "/*.h") )   

    values["PBXBuildFile"] = ""
    values["PBXFileReference"] = ""
    values["PBXGroup_src"] = ""
    values["PBXGroupSupporting"] = ""
    values["PBXResourcesBuildPhase"] = ""
    values["PBXSourcesBuildPhase"] = ""
    

    if args.src:
        
        relto = dest_path

        if args.type == "android":
            tmsrc = "${FILE} "
            relto = dest_path + "/jni/src/"
            
            SRC = process(tmsrc, relto, args.src, cpp_files)
            #INCLUDE = process(tminc, relto, args.src, h_files)              

        if args.type == "cmake" or args.type == "emscripten":
            tmsrc = "${FILE} "
            relto = dest_path
            
            SRC = process(tmsrc, relto, args.src, cpp_files)
            INCLUDE = process(tmsrc, relto, args.src, h_files)              
            
        if args.type == "win32":
            tmsrc = """<ClCompile Include="${FILE}" />"""
            tminc = """<ClInclude Include="${FILE}" />"""
            
            SRC = process(tmsrc, relto, args.src, cpp_files)
            INCLUDE = process(tminc, relto, args.src, h_files)              
            

        if args.type in ("macosx", "ios"):
            import random            
            r = random.Random()
            r.seed(1)

            class Refs:
                def __init__(self):
                    self._refs = []
                def getObject(self, file):
                    for ref in self._refs:
                        if ref[0] == file:
                            return ref[1]
                    obj = ""
                    for i in range(24):
                        obj += r.choice("0123456789ABCDEF")
                    self._refs.append((file, obj))
                    return obj
                def getFile(obj):
                    for ref in self._refs:
                        if ref[1] == obj:
                            return ref[0]
                    raise KeyError


            globalRefs = Refs()
            fileRefs = Refs()

            def fn(values):
                FILE = values["FILE"]
                values["GREF"] = globalRefs.getObject(FILE)
                values["FREF"] = fileRefs.getObject(FILE)
                values["NAME"] = os.path.split(FILE)[1]
                FULL_FILE = values["FULLFILE"]
                tp = "wtf"
                if os.path.isdir(FULL_FILE):
                    tp = "folder"
                else:
                    ext = os.path.splitext(FULL_FILE)[1]
                    if ext == ".cpp":
                        tp = "sourcecode.cpp.cpp"
                    if ext == ".h":
                        tp = "sourcecode.c.h"


                values["TYPE"] = tp
                return values


            #0405A0061872139000BA6557 /* demo in Resources */ = {isa = PBXBuildFile; fileRef = 0405A0021872139000BA6557 /* demo */; };
            #0405A0071872139000BA6557 /* ext in Resources */ = {isa = PBXBuildFile; fileRef = 0405A0031872139000BA6557 /* ext */; };
            #0405A0081872139000BA6557 /* images in Resources */ = {isa = PBXBuildFile; fileRef = 0405A0041872139000BA6557 /* images */; };
            #0405A0091872139000BA6557 /* xmls in Resources */ = {isa = PBXBuildFile; fileRef = 0405A0051872139000BA6557 /* xmls */; };


            #04A57D601871FF9F0068B1E5 /* entry_point.cpp in Sources */ = {isa = PBXBuildFile; fileRef = 04A57D3A1871FF9F0068B1E5 /* entry_point.cpp */; };
            #04A57D621871FF9F0068B1E5 /* example.cpp in Sources */ = {isa = PBXBuildFile; fileRef = 04A57D3C1871FF9F0068B1E5 /* example.cpp */; };
            #04A57D651871FF9F0068B1E5 /* test.cpp in Sources */ = {isa = PBXBuildFile; fileRef = 04A57D401871FF9F0068B1E5 /* test.cpp */; };     	    

            tm = "\t\t${GREF} /* ${FILE} in Sources */ = {isa = PBXBuildFile; fileRef = ${FREF} /* ${FILE} */; };\n"  
            values["PBXBuildFile"] = process(tm, relto, args.src, cpp_files, fn)
            values["PBXBuildFile"]+= process(tm, relto, abs_data, data_files, fn)


            #04A57D3A1871FF9F0068B1E5 /* entry_point.cpp */ = {isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = sourcecode.cpp.cpp; name = entry_point.cpp; path = ../../src/entry_point.cpp; sourceTree = "<group>"; };
            #04A57D3C1871FF9F0068B1E5 /* example.cpp */ = {isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = sourcecode.cpp.cpp; name = example.cpp; path = ../../src/example.cpp; sourceTree = "<group>"; };
            #04A57D3E1871FF9F0068B1E5 /* example.h */ = {isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = sourcecode.c.h; name = example.h; path = ../../src/example.h; sourceTree = "<group>"; };
            #04A57D401871FF9F0068B1E5 /* test.cpp */ = {isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = sourcecode.cpp.cpp; name = test.cpp; path = ../../src/test.cpp; sourceTree = "<group>"; };

            #0405A0021872139000BA6557 /* demo */ = {isa = PBXFileReference; lastKnownFileType = folder; name = demo; path = ../../data/demo; sourceTree = "<group>"; };
            #0405A0031872139000BA6557 /* ext */ = {isa = PBXFileReference; lastKnownFileType = folder; name = ext; path = ../../data/ext; sourceTree = "<group>"; };
            #0405A0041872139000BA6557 /* images */ = {isa = PBXFileReference; lastKnownFileType = folder; name = images; path = ../../data/images; sourceTree = "<group>"; };
            #0405A0051872139000BA6557 /* xmls */ = {isa = PBXFileReference; lastKnownFileType = folder; name = xmls; path = ../../data/xmls; sourceTree = "<group>"; };	    

            
            files = []
            files.extend(cpp_files)
            files.extend(h_files)
            files.extend(data_files)
            
            tm = """\t\t${FREF} /* ${FILE} */ = {isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = ${TYPE}; name = ${NAME}; path = ${FILE}; sourceTree = "<group>"; };""" + "\n"              
            values["PBXFileReference"] = process(tm, relto, args.src, files, fn)


            #04A57D3A1871FF9F0068B1E5 /* entry_point.cpp */,
            #04A57D3C1871FF9F0068B1E5 /* example.cpp */,
            #04A57D3E1871FF9F0068B1E5 /* example.h */,
            #04A57D401871FF9F0068B1E5 /* test.cpp */,   

            tm = "\t\t\t\t${FREF} /* ${NAME} */, \n"   

            files = []
            files.extend(cpp_files)
            files.extend(h_files)
            values["PBXGroup_src"] = process(tm, relto, args.src, files, fn)	    



            #0405A0021872139000BA6557 /* demo */,
            #0405A0031872139000BA6557 /* ext */,
            #0405A0041872139000BA6557 /* images */,
            #0405A0051872139000BA6557 /* xmls */,     

            values["PBXGroupSupporting"] = process(tm, relto, abs_data, data_files, fn)


            #0405A0081872139000BA6557 /* images in Resources */,
            #0405A0071872139000BA6557 /* ext in Resources */,
            #0405A0061872139000BA6557 /* demo in Resources */,				
            #0405A0091872139000BA6557 /* xmls in Resources */,	
            tm = "\t\t\t\t${GREF} /* ${NAME} */, \n" 
            values["PBXResourcesBuildPhase"] = process(tm, relto, abs_data, data_files, fn)

            #04A57D621871FF9F0068B1E5 /* example.cpp in Sources */,
            #04A57D601871FF9F0068B1E5 /* entry_point.cpp in Sources */,
            #04A57D651871FF9F0068B1E5 /* test.cpp in Sources */,     
            values["PBXSourcesBuildPhase"] = process(tm, relto, args.src, cpp_files, fn)



            

    values["SRC"] = SRC
    values["INCLUDE"] = INCLUDE


    values["DATA"] = "../data"


    def process_folder(path, dest):
        try:
            os.mkdir(dest)
        except OSError:
            pass

        for src in os.listdir(path):
            src_path = path + src         
            t = Template(src)
            dest_local = t.substitute(**values)

            if os.path.isdir(src_path):

                process_folder(src_path + "/", dest + dest_local + "/")         
                continue

            dest_path = dest + dest_local


            from mimetypes import guess_type
            print("src " + src_path)
            tp = guess_type(src_path)
            if not tp[0]:
                continue

            print(tp[0])

            q = tp[0].split("/")

            if q[0] == "text" or q[1] in ("xml", "x-msdos-program"):
                print("creating file: " + dest_path)
                src_data = open(src_path, "r").read()
                t = Template(src_data)
                dest_data = t.safe_substitute(**values)                 

                if args.type == "ios" or args.type == "macosx":
                    dest_file = io.open(dest_path, "w", newline="\n")
                    try:
                        dest_file.write(str(dest_data, encoding='utf-8')) 
                    except TypeError:
                        dest_file.write(unicode(dest_data, encoding='utf-8')) 

                else:
                    dest_file = open(dest_path, "w")
                    dest_file.write(dest_data)      

                shutil.copystat(src_path, dest_path)
                
            else:
                print("copying file: " + dest_path)                                
                shutil.copyfile(src_path, dest_path)


    top_path = templates_path + project + "/"

    process_folder(top_path, args.dest + "/")


if __name__ == "__main__":
    import argparse	
    parser = argparse.ArgumentParser(description="oxygine projects template generator")
    parser.add_argument("-t", "--type", help = "choose your IDE/build tools", 
                        choices = platforms, default = "win32")

    parser.add_argument("-s", "--src", help = "folder with already created source files", default = "")   

    parser.add_argument("-d", "--dest", help = "destination location", default = ".")

    parser.add_argument("--hello", help = "generates full copy of HelloWorld example. It includes all platforms, data and src folder", action="store_true", default = False)

    parser.add_argument("name")

    args = parser.parse_args()
    run(args)