from xml.dom import minidom
import os
import shutil
import process_atlas
import process_font
import process_starling_atlas

class XmlProcessor:
    def __init__(self, args):
        self.src_data = args.src_data + "/"
        self.dest_data = args.dest_data + "/"
        self.compression = args.compression.lower()
        #self.etc1tool = args.android_sdk + "\\tools\\etc1tool.exe "
        #if self.compression == "etc1":
        #    if not os.path.exists(self.etc1tool):
        #        raise Exception("can't find etc1tool. please pass correct path to android_sdk")
        
        self.path_xml = args.xml
        self.xml_name = os.path.split(self.path_xml)[1]
        self.atlas_group_id = 0
        self.args = args
        self.verbosity = args.verbosity 
        
        self.warnings = 0
        self.errors = 0
        self.scale_factor = 1.0
        self.scale_quality = 1.0
        self.scale = args.scale
        self.debug = args.debug
        
        self.processors = {}
        self.path_current = ""

        self._meta_doc = None
        self._meta_element = None

        all_sizes = [64, 128, 256, 512, 1024, 2048, 4096]
        self.sizes = [s for s in all_sizes if s <= self.args.max_size]
        
        self.register_processor(process_font.bmfc_font_Processor())
        self.register_processor(process_font.font_Processor())
        self.register_processor(process_atlas.atlas_Processor())
        self.register_processor(process_starling_atlas.starling_atlas_Processor())
        
        self._current_processor = None
        
    def register_processor(self, processor):
        self.processors[processor.node_id] = processor
        
    def get_apply_scale(self, applyScaleFactor, scale_quality = 1.0):
        """
        returns scale should be applied to image
        """
        v = self.scale * self.scale_quality * scale_quality
        if applyScaleFactor:
            v *= self.scale_factor
        return v
    
    def add_meta(self):
        meta = self._meta_doc.createElement(self._current_processor.node_id)
        self._meta_element.appendChild(meta) 
        
        return meta

    def get_meta_doc(self):
        return self._meta_doc    

    def _process_set(self, el):        
        path = el.getAttribute("path")
        if path:
            if path.startswith(".\\") or path.startswith("./"):
                path = self.path_current + path
    
            path = os.path.normpath(path) + "\\"
            self.path_current = path
            
        scale_factor = el.getAttribute("scale_factor")
        if scale_factor:
            self.scale_factor = float(scale_factor)
        
        scale_quality = el.getAttribute("scale_quality")
        if scale_quality:
            self.scale_quality = float(scale_quality)
            

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
    
    def get_current_src_path(self, local = ""):
        return self._get_src_path(self.path_current + local)    

    def get_inner_dest(self, inner_local_path = ""):
        return self._get_meta_xml_path(self._current_processor.node_id + "/" + inner_local_path)
    
    def log(self, st):
        print st
        
    def warning(self, st):
        if self.args.warnings:        
            print st
                
    def process(self):
        #print self.path_data
        #print self.path_xml
        #print self.path_atlasses
        
        try:
            nm = self._get_src_path(self.path_xml)
            file = open(nm, "r")
        except IOError:
            print "can't open file: " + nm
            return 
        doc = minidom.parse(file)
        del file

        self._meta_doc = minidom.Document()
        self._meta_element = self._meta_doc.createElement("resources")
        self._meta_doc.appendChild(self._meta_element)
        
        totalAtlasses = 0

        folder = self._get_meta_xml_path("")
        shutil.rmtree(folder, True)
            
        try:
            os.makedirs(folder)
        except WindowsError:
            pass
        

        for el in doc.documentElement.childNodes:
            name = el.nodeName
            
            if name in self.processors:
                proc = self.processors[name]
                self._current_processor = proc                
                try:
                    if proc.create_folder:
                        os.makedirs(self.get_inner_dest(""))
                except WindowsError:
                    pass
                proc.process(self, el)
                
            if name == "set":                   
                self._process_set(el)                
            if name == "sdfont":      
                self._process_sdf_font(el)      

        path_ox_dest = self._get_meta_xml_path("meta.xml")
        
        file = open(path_ox_dest, "w")
        if self.verbosity > 1:
            print "saving ox file: \n" + os.path.normpath(path_ox_dest)
            
        if self.args.debug:
            self._meta_element.writexml(file, "\t", "\t", "\n")
        else:
            self._meta_element.writexml(file)
        
                
        #if self.verbosity > 1:
        #    print "created %d atlasses" % (totalAtlasses, )  
                
        if self.warnings or self.errors:
            print "warnings %d, errors %d" % (self.warnings, self.errors)