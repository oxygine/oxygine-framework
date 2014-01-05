import os
import shutil
import zipfile

shutil.rmtree("data", True)
shutil.copytree("original", "data")
os.system("..\\..\\tools\\process_xml2.py -x system\\res.xml --src_data data --dest_data data\\system -np")
shutil.rmtree("data/system/anims/")

def recursive_zip(zipf, directory, folder = ""):
    for item in os.listdir(directory):
        if os.path.isfile(os.path.join(directory, item)):
            zipf.write(os.path.join(directory, item), folder + os.sep + item)
        elif os.path.isdir(os.path.join(directory, item)):
            recursive_zip(zipf, os.path.join(directory, item), folder + os.sep + item)
      
      
import cStringIO
data = cStringIO.StringIO()
with zipfile.ZipFile(data, "w", zipfile.ZIP_DEFLATED) as zp:
    recursive_zip(zp, "data")

fmtH = """
#pragma once
#ifndef %(MODULE)s_DATA
#define %(MODULE)s_DATA
extern unsigned int %(module)s_size;
extern const unsigned char %(module)s_data[];
#endif
"""

fmtCPP = """
#include "%(HEADER)s"
unsigned int %(module)s_size = %(SIZE)d;
const unsigned char %(module)s_data[] = {%(DATA)s};

"""

def gen(module, data):
    st = ",".join("0x{:02x}".format(ord(c)) for c in data)
    MODULE = module.upper()
    SIZE = len(data)
    DATA = st
    args = {"MODULE":MODULE, "SIZE":SIZE, "DATA":DATA, "module":module, "HEADER":module + "_data.h"}
    return (fmtH % args, fmtCPP % args, )
    
rs = gen("system", data.getvalue())
with open("../../oxygine/src/core/system_data.h", "w") as fh:
    fh.write(rs[0])
with open("../../oxygine/src/core/system_data.cpp", "w") as fh:
    fh.write(rs[1])

with open("system.zip", "wb") as zp:
    zp.write(data.getvalue())