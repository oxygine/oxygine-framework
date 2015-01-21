#!/usr/bin/python

import os
from xml.dom import minidom
from gen_view_code.class_type import class_type
import sys
PY3 = sys.version > '3'

class class_member:
    def __init__(self, name, ct):
        self.name = name
        self.class_type = ct

def get_plain_actors(root, res):    
    for node in root.childNodes:        
        if node.nodeType == node.TEXT_NODE:
            continue        
        res.append(node)
        get_plain_actors(node, res)
                
def save_if_changed(name, content):    
    try:
        with open(name, "r") as rd:
            data = rd.read()            
            if data == content:
                return
    except IOError:
        pass

    with open(name, "w") as rd:
        rd.write(content)



mp_actor = class_type("spActor", "Actor", "Actor.h")
mp_button = class_type("spButton", "Button", "Button.h")
mp_text = class_type("spTextField", "TextField", "TextField.h")
mp_bar = class_type("spProgressBar", "ProgressBar", "ProgressBar.h")
mp_clip = class_type("spClipRectActor", "ClipRectActor", "ClipRectActor.h")
mp_sprite = class_type("spSprite", "Sprite", "Sprite.h")
mp_sliding = class_type("spSlidingActor", "SlidingActor", "SlidingActor.h")
mp_color = class_type("spColorRectSprite", "ColorRectSprite", "ColorRectSprite.h")
mp_box9sprite = class_type("spBox9Sprite", "Box9Sprite", "Box9Sprite.h")    
mp_view = class_type("spView", "View", "View.h")    

def_mappings = (mp_bar,
                mp_clip,
                mp_button,
                mp_text,
                mp_actor,
                mp_sprite, 
                mp_sliding,
                mp_color,
                mp_box9sprite)

def get_mapping(lst, name):
    for m in lst:
        if m.className == name:
            return m

    return None

user_mp = None
def find_mapping(name, classes):
    if user_mp:
        mp = get_mapping(user_mp, name)
        if mp:
            return mp

    mp = get_mapping(def_mappings, name)
    if mp:
        return mp
    
    mp = get_mapping(classes, name)
    if mp:
        return mp

    return None            

def gen_classes(nodes, ct, classes):
    classes.add(ct)
    
    for child in nodes:
        class_mapping = mp_actor

        member = child.attrib["member"]
        
        res_id = ""
        if "id" in child.attrib:
            res_id = child.attrib["id"]

        if not res_id:
            res_id = os.path.splitext(child.attrib["file"])[0]
            class_mapping = mp_sprite        

        if "ref" in child.attrib:
            class_mapping = mp_sprite        

        try:
            #print res_id
            
            class_name = child.attrib["class"]
            class_mapping = find_mapping(class_name, classes)

            
            if not class_mapping:
                if class_name.endswith("<"):
                    class_name = class_name.rstrip("<")
                    class_mapping = class_type(class_name, "sp" + class_name, class_name, class_name + ".h", ns = "", member = res_id, parent = mp_sprite, generated=True)
                    gen_classes(nodes, class_mapping, classes)
                elif class_name.startswith(">"):
                    return
                else:                        
                    class_mapping = class_type(class_name, "sp" + class_name, class_name, class_name + ".h", ns = "")
                
        except KeyError:
            pass
        
        classes.add(class_mapping)
        
        index = child.attrib["order"]
        ct.members.append(class_member(member, class_mapping, res_id))


def gen2(xml_res_file, dest_folder, mappings):    
    global user_mp
    user_mp = mappings
    import os
    from jinja2 import Environment, FileSystemLoader    

    if not os.path.exists(dest_folder):
        os.makedirs(dest_folder)

    xml_res_file = os.path.normpath(xml_res_file)
    xml_res_file = xml_res_file.replace("\\", "/")
    
    from os import path
    doc = minidom.parse(xml_res_file)
    root = doc.documentElement


    folder = os.path.split(__file__)[0] + "/gen_view_code/templates"
    env = Environment(trim_blocks = True, lstrip_blocks = True, loader = FileSystemLoader(folder))

    class_h_template = env.get_template("class.h")
    class_cpp_template = env.get_template("class.cpp")

    
    
    import io

    classes_node = root.getElementsByTagName("class")[0]
    
    classes = set()
    
    for class_node in classes_node.childNodes:        
        if class_node.nodeType == class_node.TEXT_NODE:
            continue
        res = []
        get_plain_actors(class_node, res)
        
        class_name = class_node.getAttribute("class")
        
    
        local_classes = set()    
        
        parent = find_mapping(class_node.nodeName, classes)
        
        custom_class = class_type("sp" + class_name, class_name, class_name + ".h", ns = "", member = "", parent = parent, generated=True)


        classes.add(custom_class)
        local_classes.add(custom_class)
    
        header = io.StringIO()
        cpp = io.StringIO()
        
        for node in res:
            name = node.getAttribute("name")
            ct = find_mapping(node.nodeName, classes)
            custom_class.members.append(class_member(name, ct))
            classes.add(ct)
            local_classes.add(ct)

        cls = list(local_classes)
        q = 0
        def ff(a, b):
            def cmp(a, b):
                if a < b:
                    return -1
                if a > b:
                    return 1
                return 0
            return cmp(b.ns, a.ns) or cmp(b.primary, a.primary) or cmp(a.className, b.className)

        import functools
        cls.sort(key = functools.cmp_to_key(ff))
        
        includes = [inc for inc in cls if inc.header]    
        
        args = {"types":cls, 
                "ct":custom_class,
                "includes":includes}
        
        header.write(env.get_template("header.h").render(**args))        
        cpp.write(env.get_template("header.cpp").render(**args))
        
        args = {"ct":custom_class,
                "xml":xml_res_file,
                "members":custom_class.members}                
        
        header.write(class_h_template.render(**args))        
        cpp.write(class_cpp_template.render(**args))
        
        header_name = class_name + ".h"
        cpp_name = class_name + ".cpp"        
        
        gen_code = class_node.getAttribute("gencode")
        if gen_code == "false":
            continue
        save_if_changed(dest_folder + header_name, header.getvalue())
        save_if_changed(dest_folder + cpp_name, cpp.getvalue())


if __name__ == "__main__":
    import argparse	
    parser = argparse.ArgumentParser(description="generates h/cpp files from oxygine xml")
    parser.add_argument("xml", help = "xml file to process")
    parser.add_argument("-d", "--dest", help = "destination folder for generated classes", default = ".")
    """
    parser.add_argument("-mw", "--max_width", help = "max atlas width", type=int, default = 2048)
    parser.add_argument("-mh", "--max_height", help = "max atlas height", type=int, default = 2048)
    parser.add_argument("-s", "--scale", help = "scale factor", type=float, default = 1.0)
    parser.add_argument("-r", "--resize", help = "downscale/upscale by scale factor", action="store_true", default = False)
    parser.add_argument("-us", "--upscale", help = "allow upscale. good option for very HD displays with texture compression", action="store_true", default = False)
    parser.add_argument("-c", "--compression", help = "type of images compression. default is pure rgba8888 packed to png", 
                        choices = ["pvrtc", "etc1", "no"], default = "")
    parser.add_argument("-np", "--nopng", help = "store images without packing to png", 
                            action="store_true", default = False)    
    parser.add_argument("-q", "--quality", help = "select quality to compressed textures", 
                        choices = ["default", "fast", "best"], default = "default")
    parser.add_argument("-d", "--dither", help = "added dithering to compressed textures (pvr option)", action="store_true", default = False)
    #parser.add_argument("--android_sdk", help = "path to android sdk", default = "")
    parser.add_argument("-debug", "--debug", help = "debug mode", action="store_true", default = False)
    parser.add_argument("-w", "--warnings", help = "show warnings", action="store_true", default = False)
    parser.add_argument("-v", "--verbosity", help = "verbosity level. 1 - only errors, 2 - normal. Default value is 2", type=int, default = 2)
    parser.add_argument("--md5", help = "generates md5 lists for some special files", type=bool, default = False)        

    gen2(".", "test.xml", "./", "Qwe", None)
    """
    
    args = parser.parse_args()
    gen2(args.xml, args.dest + "/", None)