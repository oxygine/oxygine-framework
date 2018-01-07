# -*- coding: utf-8 -*-

from __future__ import unicode_literals
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

try:
    import Image
except ImportError:
    from PIL import Image

import os
from xml.dom import minidom
import tempfile
import hashlib
from . import process


class bmfc_font_Processor(process.Process):
    node_id = "bmfc_font"

    def __init__(self):
        pass

    def process(self, context, walker):
        el = walker.root
        file_name = el.getAttribute("file")
        id = el.getAttribute("id")
        if not id:
            id = os.path.split(os.path.splitext(file_name)[0])[1]

        path_font = context.src_data + walker.getPath("file")
        chars = walker.getPath("chars")
        scale = context.scale * walker.scale_quality * walker.scale_factor
        build_bmfont(context.args.hash,
                     context.get_inner_dest(id + ".fnt"),
                     path_font,
                     context.get_inner_dest(""),
                     scale,
                     context.src_data + chars)

        meta = walker.root_meta
        ns = get_bmfc_fontSize(path_font)
        font_size = int(ns * walker.scale_factor)
        meta.setAttribute("size", str(font_size))
        meta.setAttribute("sf", str(scale))


class font_Processor(process.Process):
    create_folder = False
    node_id = "font"

    def __init__(self):
        pass

    def process(self, context, walker):
        el = walker.root

        # note: variable unused
        file_name = el.getAttribute("file")

        meta = walker.root_meta
        
        path = context.src_data + walker.getPath("file")
        try:
            font_doc = context._open_xml(path)
            font_info = font_doc.getElementsByTagName("info")[0]
            size = int(font_info.getAttribute("size"))
        except:
            data = open(path, "r").read()            
            pos = data.find("size=") + 5
            end = data.find(" ", pos)        
            st = data[pos:end]
            size = int(st)
        
        size = int(abs(size) * walker.scale_factor)            
        
        meta.setAttribute("size", str(size))
        meta.setAttribute("sf", str(1))


def as_int(attr):
    if not attr:
        return 0
    return int(attr)


def get_bmfc_fontSize(bm_config):
    font_file = open(bm_config, "r")

    lines = font_file.readlines()
    for line in lines:
        spl = line.split("=")
        if spl[0] == "fontSize":
            return abs(int(spl[1]))


def build_bmfont(need_md5, fnt, bm_config, ext_folder, scale, font_chars):
    # open config file and apply scale to size
    if not os.path.exists(font_chars):
        print("error! you didn't set to bmfont 'chars'")

    bm_config_temp = tempfile.NamedTemporaryFile(
        prefix="oxygine", delete=False)
    bm_config_temp.close()
    bm_config_temp = bm_config_temp.name

    # font_size = get_bmfc_fontSize(bm_config)

    def rewrite_config(bm_original, bm_new, pngHeight):
        font_file = open(bm_original, "r")

        scaled_font_file = open(bm_new, "w")
        lines = font_file.readlines()
        for line in lines:
            # line = ""
            spl = line.split("=")
            if spl[0] == "fontSize":
                new_size = int(abs(int(spl[1])) * scale + 0.5)
                new_size = -new_size

                line = "fontSize=%(size)d\n" % {"size": new_size}

            if spl[0] == "outHeight":
                line = "outHeight=%(pngHeight)d\n" % {"pngHeight": pngHeight}

            scaled_font_file.write(line)
        font_file.close()
        scaled_font_file.close()

    rewrite_config(bm_config, bm_config_temp, 1024)

    lang = font_chars
    bmfont = os.path.split(__file__)[0] + \
        "/../../3rdPartyTools/BMFont/bmfont.com"
    cmd = "%(bmfont)s -t %(lang)s -c %(bmfc)s -o %(fnt)s" % {
        "bmfont": bmfont, "bmfc": bm_config_temp, "fnt": fnt, "lang": lang}
    cmd = cmd.replace("/", "\\")
    os.system(cmd)

    png_file = os.path.splitext(fnt)[0] + "_0.png"
    font_image = Image.open(png_file)
    font_image.load()
    _, _, _, a = font_image.split()
    bbox = a.getbbox()
    h = bbox[3] + 2
    if h > 512:
        h = 1024
    elif h > 256:
        h = 512
    elif h > 128:
        h = 256
    elif h > 64:
        h = 128
    elif h < 64:
        h = 64
    del font_image

    rewrite_config(bm_config, bm_config_temp, h)
    os.system(cmd)

    if need_md5:
        # if 0:
        md = hashlib.md5()
        md.update(open(bm_config_temp).read())
        md.update(open(lang).read())

        with open(os.path.split(fnt)[0] + "/md5.oxygine", "a") as m:
            m.write("%s\n%s\n" % (os.path.split(png_file)[1], md.hexdigest()))
            m.write("%s\n%s\n" % (os.path.split(fnt)[1], md.hexdigest()))

    file = open(fnt, "r")
    doc = minidom.parse(file)
    kern = doc.documentElement.getElementsByTagName("kernings")
    if kern:
        el = kern[0]
        el.parentNode.removeChild(el)
    pages = doc.documentElement.getElementsByTagName("pages")[0]
    for page in pages.getElementsByTagName("page"):
        fn = os.path.split(page.getAttribute("file"))[1]
        page.setAttribute("file", fn)
    file.close()

    file = open(fnt, "w")
    doc.writexml(file)
    file.close()
    os.remove(bm_config_temp)
