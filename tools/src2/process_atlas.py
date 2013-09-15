from PIL import Image
from xml.dom import minidom
import os
import atlas
import process
import oxygine


class MyExc(Exception):
    pass

def as_int(attr):
    if not attr:
        return 0
    return int(attr)

def as_bool(attr):
    if not attr:
        return False
    lw = attr.lower()
    return lw == "true" or lw == "1"


def fixImage(image):
    if image.mode != "RGBA":
        return image
    
    data = image.load()
    for y in xrange(image.size[1]):
        for x in xrange(image.size[0]):
            a = data[x,y][3]
            #if a == 0 or a == 1:
            if a == 0:
                data[x, y] = (0,0,0,0)
    return image

def premultipliedAlpha(image):
    if image.mode != "RGBA":
        return image    
    data = image.load()
    for y in xrange(image.size[1]):
        for x in xrange(image.size[0]):
            dt = data[x,y]
            a = dt[3]
            data[x, y] = ((dt[0] * a) / 255, (dt[1] * a) / 255, (dt[2] * a) / 255, a)
            
    return image    

class frame:
    def __init__(self, image, bbox, image_element, rs):
        self.image = image
        self.image_element = image_element

        self.node = None
        self.atlas = None
        self.resanim = rs

        if not bbox:
            bbox = (0,0,1,1)

        self.bbox = bbox

class ResAnim:
    def __init__(self):
        self.frames = []
        self.name = ""
        self.frame_size = (1, 1)
        self.frame_scale = 1.0
        self.columns = 0
        self.rows = 0


def frame_cmp_sort(f1, f2):
    return f2.image.size[0] - f1.image.size[0]

def applyScale(intVal, scale):
    return int(intVal * scale + 0.5)

    
def applyScale2(x, scale):
    initialX = x
    best = None
    while 1:
        X = x * scale        
        d = abs(X - int(X))
        
        if not best:
            best = (d, X)        

        if best[0] > d:
            best = (d, X)        
        
        eps = 0.00000001
        if d < eps:
            return int(X)        
        
            
        if x > initialX * 2:
            return int(best[1])
        
        x += 1
        
class atlas_Processor(process.Process):
    node_id = "atlas"
    
    def __init__(self):
        self.atlas_group_id = 0
    
    def process(self, context, el):

        dx = 2
        dy = 2        
    
        self.atlas_group_id += 1
        
        meta = context.add_meta()
        
        anims = []
    
        frames = []
        
        all_sizes = [64, 128, 256, 512, 1024, 2048, 4096]
        sizes = [s for s in all_sizes if s <= context.args.max_size]        
                
        for image_el in el.childNodes:
            if image_el.nodeName == "set":
                context._process_set(image_el)
    
            if image_el.nodeName != "image":
                continue
    
            image_name = image_el.getAttribute("file")
            if not image_name:
                continue
    
            
            #print image_path
    
            image = None
    
            #fn = self._getExistsFile(image_path)
    
            #virtual_width = 1        
            #virtual_height = 1
            try:
                image = Image.open(context.get_current_src_path(image_name))
            except IOError:      
                pass
            
                   
            if image:
             #   virtual_width = int(image.size[0] * scale + 0.001)
             #   virtual_height= int(image.size[1] * scale + 0.001)    
                pass
            else:
                print "<error> can't find image:\n%s\n" % (image_path, )
                self.errors += 1
                image = Image.new("RGBA", (0, 0))
                
                
    
            resAnim = ResAnim()            
            resAnim.image = image
            resAnim.name = image_name                        
    
            anims.append(resAnim)
    
            columns = as_int(image_el.getAttribute("cols"))
            frame_width = as_int(image_el.getAttribute("frame_width"))
            rows = as_int(image_el.getAttribute("rows"))
            frame_height = as_int(image_el.getAttribute("frame_height"))
    
            if not columns:
                columns = 1
            if not rows:
                rows = 1
    
    
            if frame_width:
                columns = image.size[0] / frame_width
            else:
                frame_width = image.size[0] / columns
    
            if frame_height:
                rows = image.size[1] / frame_height
            else:
                frame_height = image.size[1] / rows
                
            size_warning = False
            if frame_width * columns != image.size[0]:
                size_warning = True
                context.warning("image has width %d and %d columns:" % (image.size[0], columns))
            if frame_height * rows != image.size[1]:
                size_warning = True
                context.warning("<image has height %d and %d rows:" % (image.size[1], rows))
                
            if size_warning:
                context.warnings += 1
                
            scale_quality = image_el.getAttribute("scale_quality")
            if scale_quality:
                scale_quality = float(scale_quality)
            else:
                scale_quality = 1.0
    
            
    
            finalScale = context.get_apply_scale(True, scale_quality)
            upscale = False            
            if finalScale > 1:
                if not context.args.upscale:
                    finalScale = 1
                else:
                    upscale = True
                
            resAnim.frame_scale = context.get_apply_scale(False, scale_quality)
                
            resAnim.frame_size = (applyScale(frame_width, finalScale),
                                  applyScale(frame_height, finalScale))
    
            resAnim.columns = columns
            resAnim.rows = rows
    
    
    
            rect = (0, 0, 0, 0)
            for row in xrange(rows):
                for col in xrange(columns):
                    rect = (col * frame_width, row * frame_height, (col + 1) * frame_width, (row + 1)* frame_height, )
                    
                    frame_image = image.crop(rect)      
                    
                    
                    def resize():
                        ax = applyScale2(frame_width, finalScale);
                        ay = applyScale2(frame_height, finalScale);
                        bx = int(ax / finalScale)
                        by = int(ay / finalScale)
                        im = Image.new("RGBA", (bx, by))
                        im.paste(frame_image, (0, 0, frame_image.size[0], frame_image.size[1]))
                        frame_image = im.resize((ax, ay), Image.ANTIALIAS)
                        frame_image = frame_image.crop((0, 0, resAnim.frame_size[0], resAnim.frame_size[1]))                        
                    
                    if context.args.resize:
                        if as_bool(image_el.getAttribute("trueds")):
                            frame_image = frame_image.resize((resAnim.frame_size[0], resAnim.frame_size[1]), Image.ANTIALIAS)
                        else:
                            ax = applyScale2(frame_width, finalScale);
                            ay = applyScale2(frame_height, finalScale);
                            bx = int(ax / finalScale)
                            by = int(ay / finalScale)
                            im = Image.new("RGBA", (bx, by))
                            im.paste(frame_image, (0, 0, frame_image.size[0], frame_image.size[1]))
                            frame_image = im.resize((ax, ay), Image.ANTIALIAS)
                            frame_image = frame_image.crop((0,0,resAnim.frame_size[0], resAnim.frame_size[1]))
                                       
                    trim = True
                    if image_el.getAttribute("trim") == "0":
                        trim = False
                    if image.mode == "RGBA" and trim:
                        r,g,b,a = frame_image.split()
                        frame_bbox = a.getbbox()
                    else:
                        frame_bbox = frame_image.getbbox()
                        
                    w = frame_bbox[2] - frame_bbox[0]
                    h = frame_bbox[3] - frame_bbox[1]                    
                    
    
                    frame_image = frame_image.crop(frame_bbox)
    
                    fr = frame(frame_image, frame_bbox, image_el, resAnim)
                    fr.sizes = sizes

		    def nextPOT(v):
			v = v - 1;
			v = v | (v >> 1);
			v = v | (v >> 2);
			v = v | (v >> 4);
			v = v | (v >> 8);
			v = v | (v >>16);
			return v + 1
		    
                    if w > sizes[-1] or h > sizes[-1]:
                        context.warning("image '%s' is too big. Can't fit it into atlas. Atlas size increased" % (image_name, ))
                        fr.sizes = sizes[0:len(sizes)]
                        mx = max(w, h)
			mx = nextPOT(mx + 5)
                        fr.sizes = [s for s in all_sizes if s <= mx]                        
                                            
                    frames.append(fr)
                    resAnim.frames.append(fr)
    
        #sort frames by size
        frames.sort(frame_cmp_sort)            
    
    
        all_frames = frames[0:len(frames)]
        
        compression = context.compression
    
        while frames:
            try:
                for sh in fr.sizes:
                    for sw in fr.sizes:
                        
                        if compression.startswith("pvrtc"):
                            if sh != sw:
                                continue
                            
                        atl = atlas.Atlas(sw, sh)
                        atl.add(sw, 1)
                        atl.add(1, sh)
                        #print "image: %d %d" % (sw, sh)
    
                        frames_copy = frames[0:len(frames)]                        
                        #print len(frames)
                        try:
                            for fr in frames:
                                s = fr.image.size                                
                                ns = (s[0] + dx, s[1] + dy)
                                if context.compression:
                                    #4 is block size in etc1/pvrtc4
                                    def align_pixel(p):
                                        align = 4
                                        v = p % align
                                        if v == 0:
                                            p += align
                                        else:
                                            p += 8 - v
                                        return p
                                    """
                                    for q in xrange(10):
                                        p = align_pixel(q)
                                        print "test %d -> %d" % (q, p)
                                    """
                                    ns = (align_pixel(ns[0]), align_pixel(ns[1]))
                                
                                fr.node = atl.add(ns[0], ns[1])
                                
                                if not fr.node:
                                    #if atl.get_root().is_free():
                                        #image is too big
                                    raise MyExc()
                                                                                                
                                fr.atlas = atl
                                frames_copy.remove(fr)
    
                            frames = frames_copy
    
                        except MyExc:
                            if sw == fr.sizes[-1] and sh == fr.sizes[-1]:
                                frames = frames_copy
                                raise MyExc()
    
                        if not frames:
                            break
            except MyExc:
                #print "restart images"
                pass
    
        #print "done"
    
        atlases = {}            
        for fr in all_frames:
            if str(fr.atlas) not in atlases:                
                atlas_image = Image.new("RGBA", (fr.atlas.w, fr.atlas.h))
                atlases[str(fr.atlas)] = atlas_image
    
            atlas_image = atlases[str(fr.atlas)]
            rect = (fr.node.rect.x, fr.node.rect.y, 
                    fr.node.rect.x + fr.image.size[0], 
                    fr.node.rect.y + fr.image.size[1])
            atlas_image.paste(fr.image, rect)
    
            fr.atlas_image = atlas_image
    
    
        atlas_id = 0
        atlases_ordered = []
        for k, v in atlases.iteritems():
            if 0:
                v = Image.new()
                
            image_atlas_el = context.get_meta_doc().createElement("image")                
            meta.appendChild(image_atlas_el)            
            
            atlases_ordered.append(v)            
    
            base_name = "%d_%d" % (self.atlas_group_id, atlas_id)
            
            ox_fmt = "r8g8b8a8"
            
                
            def compress(src, dest, fmt):
                cmd = oxygine.path_pvrtextool + " -i %s -f %s,UBN,lRGB -o %s" % (src, fmt, dest)
                cmd += " -l" #alpha bleed
                if upscale:
                    cmd += " -dither"                            
                cmd += " -shh" #silent
                os.system(cmd)
          
            if compression == "etc1":
                #premultipliedAlpha(v)
                r, g, b, a = v.split()
                rgb = Image.merge("RGB", (r, g, b))
                alpha = Image.merge("RGB", (a,a,a))
                base_alpha_name = base_name + "_alpha"
                alpha_path = context.get_inner_dest(base_alpha_name + ".png")
                alpha.save(alpha_path)
                image_atlas_el.setAttribute("alpha", base_alpha_name + ".png")
                
                path_base = base_name + ".png"
                
                rs = ".pvr"
                
                rgb_path = context.get_inner_dest(path_base)
                path = context.get_inner_dest(path_base)                
                rgb.save(path)
                pkm_rgb = base_name + rs
                pkm_alpha = base_alpha_name + rs
                
                def compress_etc1(src, dest):
                    compress(src, dest, "etc1")
                    #os.system(context.etc1tool + "%s -o %s -f etc1" %(src, dest))
                               
                
                ox_fmt = "ETC1"
                
                compress_etc1(rgb_path, context.get_inner_dest() + pkm_rgb)                    
                os.remove(rgb_path)
                image_atlas_el.setAttribute("file", pkm_rgb)                
                
                compress_etc1(alpha_path, context.get_inner_dest() + pkm_alpha)
                os.remove(alpha_path)
                image_atlas_el.setAttribute("alpha", pkm_alpha)            
            else:
                path_base = base_name + ".png"
                path = context.get_inner_dest(path_base)
                image_atlas_el.setAttribute("file", path_base)
                v.save(path)            
                
                if context.compression == "pvrtc":
                    ox_fmt = "PVRTC_4RGBA"
                    
                    compress(path, context.get_inner_dest(base_name + ".pvr"), "PVRTC1_4")
                    image_atlas_el.setAttribute("file", base_name + ".pvr")
                    os.remove(path)
                    
            
            image_atlas_el.setAttribute("format", ox_fmt)
            
            image_atlas_el.setAttribute("w", str(v.size[0]))
            image_atlas_el.setAttribute("h", str(v.size[1]))    
            
            atlas_id += 1            
    
    
        for anim in anims:                
            if 0:
                anim = ResAnim()
            atlas_image = atlases[str(fr.atlas)]
    
            image_frames_el = context.get_meta_doc().createElement("frames")
    
            image_frames_el.setAttribute("fs", "%d,%d,%d,%d,%f" % (anim.columns, anim.rows, 
                                                                   anim.frame_size[0], anim.frame_size[1], 
                                                                   anim.frame_scale))
            meta.appendChild(image_frames_el)
    
            if context.debug:
                image_frames_el.setAttribute("debug_image", anim.name)
    
    
            data = ""
            for fr in anim.frames:
                index = atlases_ordered.index(fr.atlas_image)
                data += "%d,%d,%d,%d,%d,%d,%d;" %(index, 
                                                  fr.node.rect.x, fr.node.rect.y, 
                                                  fr.bbox[0], fr.bbox[1],
                                                  fr.bbox[2] - fr.bbox[0], fr.bbox[3] - fr.bbox[1])
    
            text = context.get_meta_doc().createTextNode(data)
            image_frames_el.appendChild(text)      
            
        return atlas_id