#!/usr/bin/python

from PIL import Image
import glob
import argparse

if __name__ == "__main__":
    import argparse	
    parser = argparse.ArgumentParser(description="Generates single png strip from multiple images. Just copy it to folder and run.")
    parser.add_argument("-p", "--pattern", help = "searching files pattern ", default = "*.png")
    parser.add_argument("-d", "--dest", help="destination file", default = "anim.png")
    args = parser.parse_args()

    images = []
    w = 0
    h = 0
    size = None
    for g in sorted(glob.glob(args.pattern)):
        im = Image.open(g)        
        images.append(im)
        h = max(h, im.size[1])        
        w += im.size[0]
        if not size:
            size = im.size        
        print ("appending image: '{}' with size ({}, {}) ".format(g, im.size[0], im.size[1]))
        
        if size[0] != im.size[0]:
            print ("warning! width should be {}".format(size[0]))

    anim = Image.new("RGBA", (w, h))
    w = 0
    for im in images:
        anim.paste(im, (w, 0))
        w += im.size[0]

    print ("writing result...\nfile=\"{}\" cols=\"{}\"".format(args.dest, len(images)))
    anim.save(args.dest)
