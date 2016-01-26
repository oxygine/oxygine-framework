#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import unicode_literals, print_function

import os
import math
import sys
from xml.dom import minidom

import Image

if sys.version_info[0] < 3:
    xrange = xrange
else:
    xrange = range


def _open_xml(path):
    file = open(path, "r")
    font_doc = minidom.parse(file)
    del file
    return font_doc


class point:

    def __init__(self, x, y):
        self.dx = x
        self.dy = y

    def distSq(self):
        return self.dx * self.dx + self.dy * self.dy

    def copy(self):
        return point(self.dx, self.dy)


def compare(g, p, x, y, offsetX, offsetY):
    other = g.get(x + offsetX, y + offsetY)
    other.dx += offsetX
    other.dy += offsetY

    if other.distSq() < p.distSq():
        return other

    return p


class grid:

    def __init__(self, w, h):
        self.data = [None for x in range(w) for y in range(h)]
        self.w = w
        self.h = h

    def put(self, x, y, val):
        self.data[x + y * self.w] = val  # .copy()

    def get(self, x, y):
        if x < 0 or y < 0:
            return point(9999, 9999)
        if x >= self.w or y >= self.h:
            return point(9999, 9999)
        return self.data[x + y * self.w].copy()


def generateSD_pass(g):
    print("generateSD_pass...")
    get = g.get
    put = g.put

    w = g.w
    h = g.h
    cmp = compare

    xr = xrange

    for y in xr(h):
        for x in xr(w):
            p = get(x, y)
            p = cmp(g, p, x, y, -1, 0)
            p = cmp(g, p, x, y, 0, -1)
            p = cmp(g, p, x, y, -1, -1)
            p = cmp(g, p, x, y, 1, -1)

            put(x, y, p)

        for x in xr(w - 1, -1, -1):
            p = get(x, y)
            p = cmp(g, p, x, y, 1, 0)
            put(x, y, p)

    for y in xr(h - 1, -1, -1):
        for x in xr(w - 1, -1, -1):
            p = get(x, y)
            p = cmp(g, p, x, y, 1, 0)
            p = cmp(g, p, x, y, 0, 1)
            p = cmp(g, p, x, y, -1, 1)
            p = cmp(g, p, x, y, 1, 1)

            put(x, y, p)

        for x in xr(w):
            p = get(x, y)
            p = cmp(g, p, x, y, 1, 0)
            put(x, y, p)


def generateSD(image):
    w = image.size[0]
    h = image.size[1]
    g = grid(w, h)

    grid1 = grid(w, h)
    grid2 = grid(w, h)

    data = image.load()
    getpixel = image.im.getpixel
    g1put = grid1.put
    g2put = grid2.put

    pZero = point(0, 0)
    pInf = point(9999, 9999)

    for y in range(h):
        for x in range(w):
            alpha = getpixel((x, y))[3]
            if alpha < 128:
                g1put(x, y, pZero)
                g2put(x, y, pInf)
            else:
                g1put(x, y, pInf)
                g2put(x, y, pZero)

    generateSD_pass(grid1)
    generateSD_pass(grid2)

    sd_image = Image.new("L", (w, h))
    data = sd_image.getdata()

    putpixel = sd_image.im.putpixel

    for y in range(h):
        for x in range(w):
            dist1 = int(math.sqrt(grid1.get(x, y).distSq()))
            dist2 = int(math.sqrt(grid2.get(x, y).distSq()))
            dist = dist1 - dist2
            dist = dist * 10
            dist += 128
            #dist = dist * 3 + 128
            if dist < 0:
                dist = 0
            if dist > 255:
                dist = 255
            putpixel((x, y), dist)

    return sd_image


def downscaleAttr(el, attr, ds):
    v = el.getAttribute(attr)
    el.setAttribute(attr, str(int(v) / ds))


def convert(args):
    xml_doc = _open_xml(args.bmfont)
    xml = xml_doc.documentElement

    folder, file = os.path.split(args.bmfont)
    if folder:
        folder += "/"
    pages = xml.getElementsByTagName("pages")[0].getElementsByTagName("page")

    downscale = args.scale
    for page in pages:

        image_file_name = page.getAttribute("file")

        image = Image.open(folder + image_file_name)
        image.load()

        #image = image.crop((0,0, 256, 256))

        w = image.size[0]
        h = image.size[1]

        sd_image = generateSD(image)
        sd_image = sd_image.resize(
            (w / downscale, h / downscale), Image.ANTIALIAS)

        sd_image_name = "sd_" + image_file_name
        sd_image.save(folder + sd_image_name, "PNG")

        page.setAttribute("file", sd_image_name)

    info_el = xml.getElementsByTagName("info")[0]
    downscaleAttr(info_el, "size", downscale)
    common_el = xml.getElementsByTagName("common")[0]
    downscaleAttr(common_el, "lineHeight", downscale)
    downscaleAttr(common_el, "base", downscale)
    downscaleAttr(common_el, "scaleW", downscale)
    downscaleAttr(common_el, "scaleH", downscale)

    new_font_xml = folder + "sd_" + file
    fileh = open(new_font_xml, "w")
    xml_doc.writexml(fileh)


if __name__ == "__main__":
    import os
    import sys

    folder = os.path.split(os.path.abspath(__file__))[0] + "\\src"
    sys.path.append(folder)

    # print sys.path

    import argparse
    parser = argparse.ArgumentParser(
        description="convert xml bmfont to signed distance font")

    parser.add_argument("bmfont", help="src font")
    parser.add_argument(
        "-s", "--scale", help="scale factor", type=int, default=1)

    import xml_processor

    args = parser.parse_args()
    convert(args)
    #p = xml_processor.XmlProcessor(args)
    # p.process()
