# -*- coding: utf-8 -*-

from __future__ import unicode_literals, print_function

try:
    import Image
except ImportError:
    from PIL import Image

import os
import struct
import base64

from . import atlas
from . import process


def as_int(attr, df=0):
    if not attr:
        return df
    return int(attr)


def as_float(attr, df=0):
    if not attr:
        return df
    return float(attr)


def as_bool(attr, df=False):
    if not attr:
        return df
    lw = attr.lower()
    return lw == "true" or lw == "1"


def fixImage(image):
    if image.mode != "RGBA":
        return image

    data = image.load()
    for y in range(image.size[1]):
        for x in range(image.size[0]):
            a = data[x, y][3]
            # if a == 0 or a == 1:
            if a == 0:
                data[x, y] = (0, 0, 0, 0)
    return image


def premultipliedAlpha(image):
    if image.mode != "RGBA":
        return image
    data = image.load()
    for y in range(image.size[1]):
        for x in range(image.size[0]):
            dt = data[x, y]
            a = dt[3]
            data[x, y] = ((dt[0] * a) / 255, (dt[1] * a) /
                          255, (dt[2] * a) / 255, a)

    return image


def bbox(x, y, w, h):
    return (x, y, x + w, y + h)


class alphaData(object):

    def __init__(self, w, h, data):
        self.w = w
        self.h = h
        self.data = data


class frame(object):

    def __init__(self, image, bbox, image_element, rs, adata, extend):

        self.image = image
        self.image_element = image_element
        self.adata = adata
        self.extend = extend

        self.node = None
        self.resanim = rs
        self.border_top = self.border_left = 2
        self.border_right = self.border_bottom = 1

        if not bbox:
            bbox = (0, 0, 1, 1)

        self.bbox = bbox


class ResAnim(object):

    def __init__(self):
        self.frames = []
        self.name = ""
        self.frame_size2 = (1, 1)  # original size * scale_factor
        self.frame_scale2 = 1.0
        self.columns = 0
        self.rows = 0
        self.walker = None


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
    v = v - 1
    v = v | (v >> 1)
    v = v | (v >> 2)
    v = v | (v >> 4)
    v = v | (v >> 8)
    v = v | (v >> 16)
    return v + 1


class settings(object):

    def __init__(self):
        self.get_size = None
        self.set_node = None
        self.padding = 1
        self.max_w = 2048
        self.max_h = 2048
        self.atlasses = []
        self.square = False
        self.npot = False


def makeAlpha(a):

    def roundUp(v, multiple):
        if multiple == 0:
            return v
        rem = v % multiple
        if rem == 0:
            return v
        res = v + multiple - rem
        return res

    asmall = a.resize(
        (int(a.size[0] / 4), int(a.size[1] / 4)), Image.ANTIALIAS)

    b = asmall.getextrema()

    if not b:
        return None

    if b[0] > 10:
        return None

    asmall_size = asmall.size

    BITS = 32

    val = roundUp(asmall_size[0], BITS)
    lineLen = val // BITS

    buff = b''

    for y in range(asmall_size[1]):

        line = [0 for x in range(lineLen)]
        for x in range(asmall_size[0]):
            p = asmall.getpixel((x, y))
            if p > 5:
                n = x // BITS
                b = x % BITS
                line[n] |= 1 << b

        for v in line:
            buff += struct.pack("<I", v)

    adata = alphaData(asmall_size[0], asmall_size[1], buff)
    return adata


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

    # atl.add(250, 250)
    # atl.save()

    return not_packed, atl


def get_pow2list(npot, mn, mx):
    # ls = []
    if npot:
        while 1:
            yield mn
            mn += 64
            if mn > mx:
                yield mx
                break
    else:
        mn = nextPOT(mn)
        mx = nextPOT(mx)
        while 1:
            yield mn
            mn *= 2
            if mn > mx:
                break


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

        sizes_w = list(get_pow2list(st.npot, min_w, st.max_w))
        sizes_h = list(get_pow2list(st.npot, min_h, st.max_h))

        for sw in sizes_w:
            for sh in sizes_h:
                end = sh == sizes_h[-1] and sw == sizes_w[-1]

                if st.square and sw != sh:
                    continue

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


def processRS(context, walker):

    image_el = walker.root

    image_name = image_el.getAttribute("file")
    if not image_name:
        return None

    file_path = walker.getPath("file")

    # print image_path

    image = None

    # fn = self._getExistsFile(image_path)

    # virtual_width = 1
    # virtual_height = 1
    path = context.src_data + file_path
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
    resAnim.walker = walker
    resAnim.image = image
    resAnim.name = image_name

    columns = as_int(image_el.getAttribute("cols"))
    frame_width = as_int(image_el.getAttribute("frame_width"))
    rows = as_int(image_el.getAttribute("rows"))
    frame_height = as_int(image_el.getAttribute("frame_height"))
    border = as_int(image_el.getAttribute("border"))

    trim = as_bool(image_el.getAttribute("trim"), True)
    extend = as_bool(image_el.getAttribute("extend"), True)
    
    if not extend:
        pass
    

    # sq = as_float(image_el.getAttribute("scale_quality"), 1)
    # next.scale_quality *= sq

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
        context.warning("image has width %d and %d columns:" %
                        (image.size[0], columns))
    if frame_height * rows != image.size[1]:
        size_warning = True
        context.warning("<image has height %d and %d rows:" %
                        (image.size[1], rows))

    if size_warning:
        context.warnings += 1

    scale_factor = walker.scale_factor

    resAnim.frame_scale2 = scale_factor
    finalScale = 1

    upscale = False

    if context.args.resize:
        max_scale = 1.0 / scale_factor

        finalScale = context.scale * walker.scale_quality

        if finalScale > max_scale:
            if not context.args.upscale:
                finalScale = max_scale
            else:
                upscale = True

        resAnim.frame_scale2 = 1.0 / finalScale

        finalScale = finalScale * scale_factor

    frame_size = (applyScale(frame_width, finalScale),
                  applyScale(frame_height, finalScale))

    resAnim.frame_size2 = (applyScale(frame_width, scale_factor),
                           applyScale(frame_height, scale_factor))

    resAnim.columns = columns
    resAnim.rows = rows

    for row in range(rows):
        for col in range(columns):

            rect = (int(col * frame_width),
                    int(row * frame_height),
                    int((col + 1) * frame_width),
                    int((row + 1) * frame_height))

            frame_image = image.crop(rect)

            def resize():
                ax = applyScale2(frame_width, finalScale)
                ay = applyScale2(frame_height, finalScale)
                bx = int(ax / finalScale)
                by = int(ay / finalScale)
                im = Image.new("RGBA", (bx, by))
                im.paste(frame_image, (0, 0, frame_image.size[
                         0], frame_image.size[1]))
                frame_image = im.resize((ax, ay), Image.ANTIALIAS)
                frame_image = frame_image.crop(
                    (0, 0, frame_size[0], frame_size[1]))

            resize_filter = Image.ANTIALIAS
            if upscale:
                resize_filter = Image.BICUBIC

            if context.args.resize:
                if as_bool(image_el.getAttribute("trueds")):
                    frame_image = frame_image.resize(
                        (frame_size[0], frame_size[1]), resize_filter)
                else:
                    ax = applyScale2(frame_width, finalScale)
                    ay = applyScale2(frame_height, finalScale)
                    bx = int(ax / finalScale)
                    by = int(ay / finalScale)
                    im = Image.new("RGBA", (bx, by))
                    im.paste(frame_image, (0, 0, frame_image.size[
                             0], frame_image.size[1]))
                    frame_image = im.resize((ax, ay), resize_filter)
                    frame_image = frame_image.crop(
                        (0, 0, frame_size[0], frame_size[1]))


            adata = None

            if image.mode == "RGBA" and trim:
                r, g, b, a = frame_image.split()
                a = a.point(lambda p: p - 2)

                if walker.hit_test:
                    adata = makeAlpha(a)

                frame_bbox = a.getbbox()
            else:
                frame_bbox = (0, 0, frame_image.size[0], frame_image.size[1])

            if not frame_bbox:
                frame_bbox = (0, 0, 0, 0)

            frame_image = frame_image.crop(frame_bbox)

            fr = frame(frame_image, frame_bbox, image_el, resAnim, adata, extend)
            
            if border:
                fr.border_left = fr.border_right = fr.border_top = fr.border_bottom = border
                
                
            if not extend:
                fr.border_left = fr.border_right = fr.border_top = fr.border_bottom = 0

            resAnim.frames.append(fr)

    return resAnim


class atlas_Processor(process.Process):
    node_id = "atlas"

    def __init__(self):
        self.atlas_group_id = 0

    def process(self, context, walker):
        self.atlas_group_id += 1

        # meta = context.add_meta()

        anims = []
        frames = []

        alphaData = ""

        while True:
            next = walker.next()
            if 0:
                import xml_processor
                next = xml_processor.XmlWalker()

            if not next:
                break

            anim = processRS(context, next)

            if anim:
                anims.append(anim)
                frames.extend(anim.frames)

        # sort frames by size
        #frames = sorted(frames, key = lambda fr: -fr.image.size[1])
        #frames = sorted(frames, key = lambda fr: -fr.image.size[0])
        frames = sorted(frames, key=lambda fr: -1 * max(
            fr.image.size[0], fr.image.size[1]) * max(fr.image.size[0], fr.image.size[1]))

        sq = 0
        for f in frames:
            sq += f.image.size[0] * f.image.size[1]

        compression = context.compression

        def get_aligned_frame_size(frame):
            def align_pixel(p):
                if not compression or compression == "no":
                    return p + 1
                align = 4
                v = p % align
                if v == 0:
                    p += align
                else:
                    p += 8 - v
                return p
            sz = frame.image.size
            if frame.extend:
                return align_pixel(sz[0] + frame.border_left + frame.border_right), align_pixel(sz[1] + frame.border_top + frame.border_bottom)
            return sz

        def get_original_frame_size(frame):
            sz = frame.image.size
            return sz

        def set_node(frame, node):
            frame.node = node

        st = settings()
        st.npot = context._npot
        st.get_size = get_aligned_frame_size
        st.set_node = set_node
        st.max_w = context.args.max_width
        st.max_h = context.args.max_height
        st.square = context.compression == "pvrtc"
        st.padding = 0

        if len(frames) == 1:
            st.get_size = get_original_frame_size

        pck(st, frames)

        # print "done"

        for atlas_id, atl in enumerate(st.atlasses):
            image = Image.new("RGBA", (atl.w, atl.h))

            for node in atl.nodes:
                fr = node.data
                x = node.rect.x + fr.border_left
                y = node.rect.y + fr.border_top
                sz = fr.image.size
                rect = bbox(x, y, sz[0], sz[1])

                if fr.extend:
                    part = fr.image.crop(bbox(0, 0,     sz[0], 1))
                    image.paste(part,    bbox(x, y - 1, sz[0], 1))

                    part = fr.image.crop(bbox(0, sz[1] - 1, sz[0], 1))
                    image.paste(part,    bbox(x, y + sz[1], sz[0], 1))

                    part = fr.image.crop(bbox(0, 0,     1, sz[1]))
                    image.paste(part,    bbox(x - 1, y,     1, sz[1]))

                    part = fr.image.crop(bbox(sz[0] - 1, 0,  1, sz[1]))
                    image.paste(part,    bbox(x + sz[0],  y,  1, sz[1]))

                image.paste(fr.image, rect)

                fr.atlas_id = atlas_id

            image_atlas_el = walker.root_meta.ownerDocument.createElement(
                "atlas")
            walker.root_meta.insertBefore(
                image_atlas_el,  anims[0].walker.root_meta)
            # meta.appendChild(image_atlas_el)

            base_name = "%d_%d" % (self.atlas_group_id, atlas_id)
            ox_fmt = "r8g8b8a8"

            def compress(src, dest, fmt):
                cmd = context.helper.path_pvrtextool + \
                    " -i %s -f %s,UBN,lRGB -o %s" % (src, fmt, dest)
                cmd += " -l"  # alpha bleed

                if context.args.quality == "best":
                    cmd += " -q pvrtcbest"
                else:
                    cmd += " -q pvrtcfast"

                if context.args.dither:
                    cmd += " -dither"
                cmd += " -shh"  # silent
                os.system(cmd)

            if compression == "etc1":
                # premultipliedAlpha(v)
                r, g, b, a = image.split()
                rgb = Image.merge("RGB", (r, g, b))
                alpha = Image.merge("RGB", (a, a, a))
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
                if context.args.nopng:
                    path_base = base_name + ".tga"
                else:
                    path_base = base_name + ".png"

                path = context.get_inner_dest(path_base)
                image_atlas_el.setAttribute("file", path_base)
                image.save(path)

                if context.compression == "pvrtc":
                    ox_fmt = "PVRTC_4RGBA"

                    compress(path, context.get_inner_dest(
                        base_name + ".pvr"), "PVRTC1_4")
                    image_atlas_el.setAttribute("file", base_name + ".pvr")
                    os.remove(path)

                if context.compression == "pvrtc2":
                    ox_fmt = "PVRTC2_4RGBA"

                    compress(path, context.get_inner_dest(
                        base_name + ".pvr"), "PVRTC2_4")
                    image_atlas_el.setAttribute("file", base_name + ".pvr")
                    os.remove(path)

            image_atlas_el.setAttribute("format", ox_fmt)

            image_atlas_el.setAttribute("w", str(image.size[0]))
            image_atlas_el.setAttribute("h", str(image.size[1]))

        alpha = b''

        for anim in anims:

            if 0:
                anim = ResAnim()

            image_frames_el = anim.walker.root_meta

            image_frames_el.setAttribute("fs", "%d,%d,%d,%d,%f" % (anim.columns, anim.rows,
                                                                   anim.frame_size2[
                                                                       0], anim.frame_size2[1],
                                                                   anim.frame_scale2))
            adata = anim.frames[0].adata
            if adata:
                image_frames_el.setAttribute("ht", "%d,%d,%d,%d" % (
                    len(alpha), len(adata.data), adata.w, adata.h))

            if context.debug:
                image_frames_el.setAttribute("debug_image", anim.name)

            data = ""
            for fr in anim.frames:
                data += "%d,%d,%d,%d,%d,%d,%d;" % (fr.atlas_id,
                                                   fr.node.rect.x + fr.border_left, fr.node.rect.y + fr.border_top,
                                                   fr.bbox[0], fr.bbox[1],
                                                   fr.bbox[2] - fr.bbox[0], fr.bbox[3] - fr.bbox[1])
                if fr.adata:
                    alpha += fr.adata.data

            text = image_frames_el.ownerDocument.createTextNode(data)
            image_frames_el.appendChild(text)

        if alpha:
            doc = walker.root_meta.ownerDocument

            alpha_el = doc.createElement("ht")
            adata_str = base64.b64encode(alpha)
            text = doc.createTextNode(adata_str.decode("utf-8"))
            alpha_el.setAttribute("len", str(len(adata_str)))
            alpha_el.appendChild(text)

            walker.root_meta.appendChild(alpha_el)
