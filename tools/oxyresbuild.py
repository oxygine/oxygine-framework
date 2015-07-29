#!/usr/bin/python

def str2bool(v):
    return v.lower() in ("yes", "true", "t", "1")

def get_parser():
    import argparse	
    parser = argparse.ArgumentParser(description="oxyresbuild is being used to processing and optimizing xml resources file. It generated <xml-name>.ox folder with meta.xml file inside. Meta file has optimized information about resources, atlasses.")
    parser.add_argument("--src_data", help = "root data folder contains all resources", default = ".")
    parser.add_argument("--dest_data", help = "destination data folder for generated files", default = ".")
    parser.add_argument("-x", "--xml", help = "xml file to process", default = ".", required = True)
    parser.add_argument("-mw", "--max_width", help = "max atlas width", type=int, default = 2048)
    parser.add_argument("-mh", "--max_height", help = "max atlas height", type=int, default = 2048)
    parser.add_argument("-s", "--scale", help = "Scale value applied when resizing images. Value > 1 - upscale, Value < 1 - downscale. Should be used with --resize", type=float, default = 1.0)
    parser.add_argument("-r", "--resize", help = "Resize images by scale value", action="store_true", default = False)
    parser.add_argument("-us", "--upscale", help = "allow upscale. good option for very HD displays with texture compression", action="store_true", default = False)
    parser.add_argument("-c", "--compression", help = "type of images compression. default is pure rgba8888 packed to png", 
                        choices = ["pvrtc", "pvrtc2", "etc1", "no"], default = "")
    parser.add_argument("--npot", help = "not power of two atlasses", action="store_true", default = False)    
    parser.add_argument("-q", "--quality", help = "select quality to compressed textures (default is not best)", 
                        choices = ["default", "fast", "best"], default = "default")
    parser.add_argument("-d", "--dither", help = "added dithering to compressed textures (pvr option)", action="store_true", default = False)    
    parser.add_argument("-w", "--warnings", help = "show warnings", action="store_true", default = False)
    parser.add_argument("-v", "--verbosity", help = "verbosity level. 1 - only errors, 2 - normal. Default value is 2", type=int, default = 2)
    parser.add_argument("--hash", help = "enables creating md5 hash lists for some special files", action="store_true", default = False)
    parser.add_argument("--debug", help = "debug mode", action="store_true", default = False)
    parser.add_argument("--no_hit_test", help = "disables generation hit_test data by default", action="store_true", default = False)
    parser.add_argument("--nopng", help = "stores images without packing to png. TGA will be used if compressiong disabled.", 
                        action="store_true", default = False)            
    return parser

def do(args):
    import os   
    import sys

    import resbuild.xml_processor as xml_processor
    p = xml_processor.XmlProcessor(args)
    p.process()

def process(values):
    ar = values.split(" ")
    args = []
    for a in ar:
        v = a.strip()
        if not v:
            continue
        args.append(v)

    args = get_parser().parse_args(args)
    do(args)


if __name__ == "__main__":
    parser = get_parser()

    import time
    dt = time.clock()
    do(parser.parse_args())
    dt = time.clock() - dt

    #print "total time: " + str(dt)
