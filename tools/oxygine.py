#this file is part of oxygine-framework. Copy it from oxygine-framework/tools/ to your folder and import for usage
import os
path_root = os.environ["OXYGINE"] + "/"
path_process_xml = path_root + "tools/process_xml2.py"
path_bmfont = path_root + "/3rdPartyTools/BMFont/bmfont.com"
path_pvrtextool = path_root + "/3rdPartyTools/PVRTexToolCL.exe "

def process_xml(embedded, args):
    import sys
    folder = path_root + "tools"
    if folder not in sys.path:
        sys.path.append(folder)
        
    import process_xml2
    if embedded:
        process_xml2.process(args)
    else:
        os.system(path_process_xml + " " + args)