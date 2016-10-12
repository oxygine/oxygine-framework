# -*- coding: utf-8 -*-

from __future__ import unicode_literals, print_function


import os
import shutil

from xml.dom import minidom

from . import (
    process_atlas, process_font, process_starling_atlas, oxygine_helper
)


def as_bool(attr):
    if not attr:
        return False
    lw = attr.lower()
    return lw == "true" or lw == "1"


class XmlWalker:

    def __init__(self, src, xml_folder, path, scale_factor, node, meta_node, scale_quality, hit_test):
        self.xml_folder = xml_folder
        self.path = path
        self.scale_factor = scale_factor
        self.root = node
        self.last = None
        self.root_meta = meta_node
        self.last_meta = None
        self.src = src
        self.scale_quality = scale_quality
        self.hit_test = hit_test
        self.checkSetAttributes()

    def getType(self):
        return self.root

    def getXMLFolder(self):
        return self.xml_folder

    def getPath(self, attr):
        path = self.root.getAttribute(attr)
        if path.startswith("./") or path.startswith(".\\"):
            return self.xml_folder + path[2:len(path)]

        return self.path + path

    def setSrcFullPath(self, path):
        return self.src + path

    def checkSetAttributes(self):
        self._checkSetAttributes(self.root)

    def _checkSetAttributes(self, node):
        path = node.getAttribute("path")
        if path:
            if 0:
                path = ""
            if path.startswith("./") or path.startswith(".\\"):
                path = self.xml_folder + path[2:len(path)]
            self.path = path + "/"

        scale_factor = node.getAttribute("scale_factor")
        if scale_factor:
            self.scale_factor = float(scale_factor)

        scale_quality = node.getAttribute("scale_quality")
        if scale_quality:
            self.scale_quality = float(scale_quality)

        attr = node.getAttribute("hit_test")
        if attr:
            self.hit_test = as_bool(attr)

    def next(self):
        while True:
            if not self.last:
                if len(self.root.childNodes) == 0:
                    return None
                self.last = self.root.childNodes[0]
            else:
                self.last = self.last.nextSibling

            if not self.last:
                return None

            if self.last.nodeType == self.last.TEXT_NODE:
                continue

            if self.last.nodeType == self.last.COMMENT_NODE:
                continue

            meta = self.root_meta.ownerDocument.createElement(
                self.last.nodeName)
            self.root_meta.appendChild(meta)
            self.last_meta = meta

            if self.last.nodeName == "set":
                self._checkSetAttributes(self.last)
                continue

            break

        return XmlWalker(self.src, self.xml_folder, self.path, self.scale_factor, self.last, self.last_meta, self.scale_quality, self.hit_test)


class XmlProcessor(object):

    def __init__(self, args):
        self.src_data = args.src_data + "/"
        self.dest_data = args.dest_data + "/"
        self.compression = args.compression.lower()
        # self.etc1tool = args.android_sdk + "\\tools\\etc1tool.exe "
        # if self.compression == "etc1":
        #    if not os.path.exists(self.etc1tool):
        #        raise Exception("can't find etc1tool. please pass correct path to android_sdk")

        self.path_xml = args.xml
        self.xml_name = os.path.split(self.path_xml)[1]
        self.atlas_group_id = 0
        self.args = args
        self.verbosity = args.verbosity

        self.warnings = 0
        self.errors = 0
        #self.scale_factor = 1.0
        #self.scale_quality = 1.0
        self.scale = args.scale
        self.debug = args.debug

        self.processors = {}
        #self.path_current = ""

        self._meta_doc = None
        self._npot = args.npot
        #self._meta_element = None

        rp = os.path.abspath(__file__) 
        rp = os.path.dirname(rp)
        rp = os.path.join(rp, "../../")
        rp = os.path.normpath(rp) + "/"
        self.helper = oxygine_helper.helper(rp)

        self.register_processor(process_font.bmfc_font_Processor())
        self.register_processor(process_font.font_Processor())
        self.register_processor(process_atlas.atlas_Processor())
        self.register_processor(
            process_starling_atlas.starling_atlas_Processor())

        self._current_processor = None

    def register_processor(self, processor):
        self.processors[processor.node_id] = processor

    def get_apply_scale(self, applyScaleFactor, walker):
        """
        returns scale should be applied to image
        """
        v = self.scale * walker.scale_quality
        if applyScaleFactor:
            v *= walker.scale_factor
        return v

    """
    def add_meta(self, node_id = ""):
        if not node_id:
            node_id = self._current_processor.node_id
        meta = self._meta_doc.createElement(node_id)
        self._meta_element.appendChild(meta)

        return meta

    def get_meta_doc(self):
        return self._meta_doc
        """

    """
    def _process_set(self, el):
        path = el.getAttribute("path")
        if path:
            if path.startswith(".\\") or path.startswith("./"):
                path = self.path_current + path

            path = os.path.normpath(path) + "/"
            self.path_current = path

        scale_factor = el.getAttribute("scale_factor")
        if scale_factor:
            self.scale_factor = float(scale_factor)

        scale_quality = el.getAttribute("scale_quality")
        if scale_quality:
            self.scale_quality = float(scale_quality)

        self.add_meta("set");
        """

    def _open_xml(self, path):
        with open(path, "r") as file:
            font_doc = minidom.parse(file)
            return font_doc.documentElement

    def _get_src_path(self, local_path):
        return self.src_data + local_path

    def _get_dest_path(self, local_path):
        return self.dest_data + local_path

    def _get_meta_xml_path(self, local_path):
        return self._get_dest_path(self.xml_name) + ".ox" + "/" + local_path
    """
    def get_current_src_path(self, local = ""):
        return self._get_src_path(self.path_current + local)
    """

    def get_inner_dest(self, inner_local_path=""):
        return self._get_meta_xml_path(self._current_processor.node_id + "/" + inner_local_path)

    def log(self, st):
        print(st)

    def warning(self, st):
        if self.args.warnings:
            print("warning: " + st)
        self.warnings += 1

    def error(self, st):
        print("error: " + st)
        self.errors += 1

    def process(self):
        # print self.path_data
        # print self.path_xml
        # print self.path_atlasses

        try:
            nm = self._get_src_path(self.path_xml)
            file = open(nm, "r")
        except IOError:
            print("can't open file: " + nm)
            return
        doc = minidom.parse(file)
        del file

        self._meta_doc = minidom.Document()
        meta_element = self._meta_doc.createElement("resources")
        meta_element.setAttribute("version", "2")
        self._meta_doc.appendChild(meta_element)

        # note: variable unused
        totalAtlasses = 0

        folder = self._get_meta_xml_path("")
        shutil.rmtree(folder, True)

        try:
            os.makedirs(folder)
        except OSError:
            pass

        xml_folder = os.path.split(self.path_xml)[0] + "/"

        hit_test = True
        if self.args.no_hit_test:
            hit_test = False

        walker = XmlWalker(self.src_data, xml_folder, "", 1.0,
                           doc.documentElement, meta_element, 1.0, hit_test)

        while True:
            next = walker.next()
            if not next:
                break

            name = next.root.nodeName

            if name in self.processors:
                proc = self.processors[name]
                self._current_processor = proc
                try:
                    if proc.create_folder:
                        os.makedirs(self.get_inner_dest(""))
                except OSError:
                    pass
                proc.process(self, next)

        """
        for el in doc.documentElement.childNodes:
            name = el.nodeName

            if name in self.processors:
                proc = self.processors[name]
                self._current_processor = proc
                try:
                    if proc.create_folder:
                        os.makedirs(self.get_inner_dest(""))
                except OSError:
                    pass
                proc.process(self, el)

            if name == "set":
                self._process_set(el)
            if name == "sdfont":
                self._process_sdf_font(el)
        """

        path_ox_dest = self._get_meta_xml_path("meta.xml")

        file = open(path_ox_dest, "w")
        if self.verbosity > 1:
            print("saving ox file: \n" + os.path.normpath(path_ox_dest))

        if self.args.debug:
            meta_element.writexml(file, "\t", "\t", "\n")
        else:
            meta_element.writexml(file)

        # if self.verbosity > 1:
        #    print "created %d atlasses" % (totalAtlasses, )

        if self.warnings or self.errors:
            print("warnings %d, errors %d" % (self.warnings, self.errors))
