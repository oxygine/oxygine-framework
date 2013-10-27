from PIL import Image
from xml.dom import minidom
import os
import atlas
import process

def as_int(attr, df = 0):
    if not attr:
        return df
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
        self.resanim = rs
        self.border_top = self.border_left = 0
        self.border_right = self.border_bottom = 1

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
        
def nextPOT(v):
    v = v - 1;
    v = v | (v >> 1);
    v = v | (v >> 2);
    v = v | (v >> 4);
    v = v | (v >> 8);
    v = v | (v >>16);
    return v + 1

class settings:
    def __init__(self):
        self.get_size = None
        self.set_node = None
        self.padding = 1
        self.max_w = 2048
        self.max_h = 2048
        self.atlasses = []

def pack(st, frames, sw, sh):                       
                
    atl = atlas.Atlas(st.padding, sw, sh)            
            
    not_packed = []    
    for fr in frames:
        ns = st.get_size(fr)
        node = atl.add(ns[0], ns[1], fr)
        if not node:
            not_packed.append(fr)
        else:
            st.set_node(fr, node)   
        
    #atl.add(250, 250)
    #atl.save()
        
    return not_packed, atl


def get_pow2list(mn, mx):
    ls = []
    mn = nextPOT(mn)
    mx = nextPOT(mx)
    while 1:
        ls.append(mn)
        mn *= 2
        if mn > mx:
            break
    return ls

def pck(st, frames):        
    if 0:
        st = settings()
    while frames:
        sq = 0
        min_w = 64
        min_h = 64
        for f in frames:
            size = st.get_size(f)
            sq += size[0] * size[1]
            min_w = max(min_w, size[0])
            min_h = max(min_h, size[1])
            
        sizes_w = get_pow2list(min_w, st.max_w)
        sizes_h = get_pow2list(min_h, st.max_h)
                               
        
        for sw in sizes_w:
            for sh in sizes_h:
                end = sh == sizes_h[-1] and sw == sizes_w[-1]
                
                if sw * sh < sq and not end:
                    continue                
                
                not_packed, bn = pack(st, frames, sw, sh)
                st.atlasses.append(bn)
                if not not_packed:
                    return
                
                if end:                    
                    frames = not_packed   
                else:
                    st.atlasses.pop()

class atlas_Processor(process.Process):
    node_id = "atlas"

    def __init__(self):
        self.atlas_group_id = 0

    def process(self, context, el):
        self.atlas_group_id += 1

        meta = context.add_meta()

        anims = []

        frames = []

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
            path = context.get_current_src_path(image_name)
            try:
                image = Image.open(path)
            except IOError:      
                pass


            if image:
                #   virtual_width = int(image.size[0] * scale + 0.001)
                #   virtual_height= int(image.size[1] * scale + 0.001)    
                pass
            else:
                context.error("can't find image:\n%s\n" % (path, ))
                image = Image.new("RGBA", (0, 0))



            resAnim = ResAnim()            
            resAnim.image = image
            resAnim.name = image_name                        

            anims.append(resAnim)

            columns = as_int(image_el.getAttribute("cols"))
            frame_width = as_int(image_el.getAttribute("frame_width"))
            rows = as_int(image_el.getAttribute("rows"))
            frame_height = as_int(image_el.getAttribute("frame_height"))
            border = as_int(image_el.getAttribute("border"))
            
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

                    if not frame_bbox:
                        frame_bbox = (0,0,0,0)

                    w = frame_bbox[2] - frame_bbox[0]
                    h = frame_bbox[3] - frame_bbox[1]                    


                    frame_image = frame_image.crop(frame_bbox)

                    fr = frame(frame_image, frame_bbox, image_el, resAnim)
                    if border:
                        fr.border_left = fr.border_right = fr.border_top = fr.border_bottom = border
                    

                    frames.append(fr)
                    resAnim.frames.append(fr)
        
        #sort frames by size
        #frames = sorted(frames, key = lambda fr: -fr.image.size[1])
        #frames = sorted(frames, key = lambda fr: -fr.image.size[0])
        frames = sorted(frames, key = lambda fr: -1 * max(fr.image.size[0], fr.image.size[1]) * max(fr.image.size[0], fr.image.size[1]))

        sq = 0
        for f in frames:
            sq += f.image.size[0] * f.image.size[1]


        compression = context.compression

        def get_frame_size(frame):
            
            def align_pixel(p):
                if not context.compression:
                    return p + 1
                align = 4
                v = p % align
                if v == 0:
                    p += align
                else:
                    p += 8 - v
                return p
            sz = frame.image.size            
            return align_pixel(sz[0] + frame.border_left + frame.border_right), align_pixel(sz[1] + frame.border_top + frame.border_bottom)
        
        def set_node(frame, node):
            frame.node = node
        
        st = settings()
        st.get_size = get_frame_size
        st.set_node = set_node
        st.max_w = context.args.max_width
        st.max_h = context.args.max_height
        
        pck(st, frames) 

        #print "done"
        
        for atlas_id, atl in enumerate(st.atlasses):
            image = Image.new("RGBA", (atl.w, atl.h))
            
            for node in atl.nodes:
                fr = node.data
                x = node.rect.x + fr.border_left
                y = node.rect.y + fr.border_top
                sz = fr.image.size
                rect = (x, y, x + sz[0], y + sz[1])
                image.paste(fr.image, rect)
                fr.atlas_id = atlas_id

            image_atlas_el = context.get_meta_doc().createElement("image")                
            meta.appendChild(image_atlas_el)            


            base_name = "%d_%d" % (self.atlas_group_id, atlas_id)
            ox_fmt = "r8g8b8a8"

            def compress(src, dest, fmt):
                cmd = context.helper.path_pvrtextool + " -i %s -f %s,UBN,lRGB -o %s" % (src, fmt, dest)
                cmd += " -l" #alpha bleed
                if upscale:
                    cmd += " -dither"                            
                cmd += " -shh" #silent
                os.system(cmd)

            if compression == "etc1":
                #premultipliedAlpha(v)
                r, g, b, a = image.split()
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
                image.save(path)            

                if context.compression == "pvrtc":
                    ox_fmt = "PVRTC_4RGBA"

                    compress(path, context.get_inner_dest(base_name + ".pvr"), "PVRTC1_4")
                    image_atlas_el.setAttribute("file", base_name + ".pvr")
                    os.remove(path)


            image_atlas_el.setAttribute("format", ox_fmt)

            image_atlas_el.setAttribute("w", str(image.size[0]))
            image_atlas_el.setAttribute("h", str(image.size[1]))  


        for anim in anims:                
            if 0:
                anim = ResAnim()

            image_frames_el = context.get_meta_doc().createElement("frames")

            image_frames_el.setAttribute("fs", "%d,%d,%d,%d,%f" % (anim.columns, anim.rows, 
                                                                   anim.frame_size[0], anim.frame_size[1], 
                                                                   anim.frame_scale))
            meta.appendChild(image_frames_el)

            if context.debug:
                image_frames_el.setAttribute("debug_image", anim.name)


            data = ""
            for fr in anim.frames:
                data += "%d,%d,%d,%d,%d,%d,%d;" %(fr.atlas_id, 
                                                  fr.node.rect.x + fr.border_left, fr.node.rect.y + fr.border_top, 
                                                  fr.bbox[0], fr.bbox[1],
                                                  fr.bbox[2] - fr.bbox[0], fr.bbox[3] - fr.bbox[1])

            text = context.get_meta_doc().createTextNode(data)
            image_frames_el.appendChild(text)