import os
import sys
import platform

class helper(object):

    def __init__(self, root, version=0):
        self.path_root = root + "/"
        self.path_process_xml = self.path_root + "tools/oxyresbuild.py"
        self.path_bmfont = self.path_root + "/3rdPartyTools/BMFont/bmfont.com"

        bit64 = ""
        if platform.machine().endswith('64'): 
            bit64 = "_64"

        if sys.platform == "darwin":
            self.path_pvrtextool = self.path_root + "/3rdPartyTools/macosx/PVRTexToolCLI "
        elif sys.platform == "linux2":
            self.path_pvrtextool = self.path_root + "/3rdPartyTools/linux/PVRTexToolCLI{} ".format(bit64, )
        else:            
            self.path_pvrtextool = self.path_root + "/3rdPartyTools/PVRTexToolCLI{}.exe ".format(bit64, )

    def process_xml(self, embedded, args):
        if embedded:
            folder = self.path_root + "tools"
            if folder not in sys.path:
                sys.path.append(folder)
            import oxyresbuild
            return oxyresbuild.process(args)
        else:
            os.system("python \"{}\" {}".format(self.path_process_xml, args))
