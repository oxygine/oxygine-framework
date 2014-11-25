import os
import sys

class helper:
    def __init__(self, root, version = 0):
        self.path_root = root + "/"
        self.path_process_xml = self.path_root + "tools/oxyresbuild.py"
        self.path_bmfont = self.path_root + "/3rdPartyTools/BMFont/bmfont.com"

        if sys.platform == "darwin":
            self.path_pvrtextool = self.path_root + "/3rdPartyTools/macosx/PVRTexToolCLI "
        elif sys.platform == "linux2":
            self.path_pvrtextool = self.path_root + "/3rdPartyTools/linux/PVRTexToolCLI "
        else:
            self.path_pvrtextool = self.path_root + "/3rdPartyTools/PVRTexToolCL.exe "
        
    def process_xml(self, embedded, args):        
        if embedded:
            folder = self.path_root + "tools"
            if folder not in sys.path:
                sys.path.append(folder)            
            import oxyresbuild
            oxyresbuild.process(args)
        else:
            os.system(self.path_process_xml + " " + args)
