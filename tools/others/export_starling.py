from lxml import etree
import os
import sys

import Image

xml = sys.argv[1]
doc = etree.parse(xml)
root = doc.getroot()    

imagePath = root.attrib["imagePath"]
folder = os.path.split(xml)[0]
if not folder:
    folder = "."
src = Image.open(folder + "/" + imagePath)


for node in root:
    if node.tag == "SubTexture":
        
        #<SubTexture name="wild/0000" x="1" y="1" width="306" height="306" frameX="-48" frameY="-48" frameWidth="504" frameHeight="504"/>
        name       = node.attrib["name"]
        try:
            os.makedirs(os.path.split(name)[0])
        except OSError:
            pass
        x          = int(node.attrib["x"])
        y          = int(node.attrib["y"])
        width      = int(node.attrib["width"])
        height     = int(node.attrib["height"])
        frameX     = int(node.attrib["frameX"])
        frameY     = int(node.attrib["frameY"])

        frameWidth = int(node.attrib["frameWidth"])
        frameHeight= int(node.attrib["frameHeight"])
        
        size = (frameWidth, frameHeight)
        dest = Image.new("RGBA", size)
        im = src.crop((x, y, x + width, y + height))
        dest.paste(im, (-frameX, -frameY, -frameX + width, -frameY + height))
        dest.save(name + ".png")
