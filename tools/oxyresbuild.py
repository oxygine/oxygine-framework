#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import unicode_literals, print_function

import time
import resbuild.xml_processor as xml_processor


def str2bool(v):
    return v.lower() in ("yes", "true", "t", "1")


def get_parser():
    import argparse
    parser = argparse.ArgumentParser(
        description="oxyresbuild is a tool for optimizing xml resource files."
        "It generates a <xml-name>.ox folder with a"
        "meta.xml file inside. The meta file has optimized information "
        "about resources including atlasses."
    )
    parser.add_argument(
        "--src_data", help="The data folder containing all the resources",
        default="."
    )
    parser.add_argument(
        "--dest_data", help="Destination data folder",
        default="."
    )
    parser.add_argument(
        "-x", "--xml", help="The xml file to process", default=".", required=True)
    parser.add_argument("-mw", "--max_width",
                        help="max atlas width", type=int, default=2048)
    parser.add_argument("-mh", "--max_height",
                        help="max atlas height", type=int, default=2048)
    parser.add_argument(
        "-s", "--scale", help="Scale value applied when resizing images. "
        "Value > 1 - upscale, Value < 1 - downscale. Should be used "
        "with --resize", type=float, default=1.0
    )
    parser.add_argument(
        "--trim_threshold", help="Alpha trim threshold. Used to optimize image when trimming transparent borders of image", type=int, default=2)
    parser.add_argument("-r", "--resize", help="Resize images by a scale value",
                        action="store_true", default=False)
    parser.add_argument("-us", "--upscale",
                        help="allow upscale (Recommended for HD resources)"
                        "displays with texture compression",
                        action="store_true", default=False)
    parser.add_argument("-c", "--compression", help="type of image "
                        "compression. Defaults to pure rgba8888 packed to png",
                        choices=["pvrtc", "pvrtc2", "etc1", "no"], default="")
    parser.add_argument("--npot", help="Atlasses dimensions are not power of twos",
                        action="store_true", default=False)
    parser.add_argument("-q", "--quality", help="select quality to "
                        "compressed textures (default is fast)",
                        choices=["default", "fast", "best"], default="default")
    parser.add_argument("-d", "--dither", help="added dithering to "
                        "compressed textures (pvr option)",
                        action="store_true", default=False)
    parser.add_argument("-w", "--warnings", help="show warnings",
                        action="store_true", default=False)
    parser.add_argument("--simple_downsample", help="don't use smart algorithm when resizing args",
                        action="store_true", default=False)
    parser.add_argument(
        "-v", "--verbosity", help="verbosity level. 1 - only errors, "
        "2 - normal. Default value is 2", type=int, default=2)
    parser.add_argument("--hash", help="enables creating md5 hash lists for "
                        "certain special files",
                        action="store_true", default=False)
    parser.add_argument("--debug", help="debug mode",
                        action="store_true", default=False)
    parser.add_argument("--no_hit_test", help="disables generation of"
                        "hit_test data by default",
                        action="store_true", default=False)
    parser.add_argument("--nopng", help="stores images without "
                        "packing to png. TGA will be used if "
                        "compressiong is disabled.",
                        action="store_true", default=False)
    return parser


def do(args):
    p = xml_processor.XmlProcessor(args)
    p.process()


def process(values):
    import shlex
    ar = shlex.split(values)
    #ar = values.split(" ")
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

    dt = time.clock()
    do(parser.parse_args())
    dt = time.clock() - dt

    # print("total time: " + str(dt))
