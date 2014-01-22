#!/usr/bin/python

def get_parser():
    import argparse	
    parser = argparse.ArgumentParser(description="process xml resources file")
    parser.add_argument("--src_data", help = "root folder contains all resources", default = ".")
    parser.add_argument("--dest_data", help = "dest root folder for generated files", default = ".")
    parser.add_argument("-x", "--xml", help = "xml file to process", default = ".", required = True)
    parser.add_argument("-mw", "--max_width", help = "max atlas width", type=int, default = 2048)
    parser.add_argument("-mh", "--max_height", help = "max atlas height", type=int, default = 2048)
    parser.add_argument("-s", "--scale", help = "scale factor", type=float, default = 1.0)
    parser.add_argument("-r", "--resize", help = "downscale/upscale by scale factor", action="store_true", default = False)
    parser.add_argument("-us", "--upscale", help = "allow upscale. good option for very HD displays with texture compression", action="store_true", default = False)
    parser.add_argument("-c", "--compression", help = "type of images compression. default is pure rgba8888 packed to png", 
                        choices = ["pvrtc", "etc1", "no"], default = "")
    parser.add_argument("-np", "--nopng", help = "store images without packing to png", 
                            action="store_true", default = False)    
    parser.add_argument("-q", "--quality", help = "select quality to compressed textures", 
                        choices = ["default", "fast", "best"], default = "default")
    parser.add_argument("-d", "--dither", help = "added dithering to compressed textures (pvr option)", action="store_true", default = False)
    #parser.add_argument("--android_sdk", help = "path to android sdk", default = "")
    parser.add_argument("-debug", "--debug", help = "debug mode", action="store_true", default = False)
    parser.add_argument("-w", "--warnings", help = "show warnings", action="store_true", default = False)
    parser.add_argument("-v", "--verbosity", help = "verbosity level. 1 - only errors, 2 - normal. Default value is 2", type=int, default = 2)
    parser.add_argument("--md5", help = "generates md5 lists for some special files", type=bool, default = False)    
    return parser

def do(args):
    import os   
    import sys

    folder = os.path.split(os.path.abspath(__file__))[0] + "/src2"  
    if folder not in sys.path:
        sys.path.append(folder)

    import xml_processor
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
