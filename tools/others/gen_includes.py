#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os

dest = "../../include/ox/"
 
def rec(path, local):
    for item in os.listdir(path):
        if item in ("winnie_alloc", "minizip", "s3e", "curl", "dev_tools", "gl", "android", "emscripten", "ios", 
                    "closure", "pugixml", "json", "text_utils", ):
            continue

        name = path + item
        local_name = local + item
        if os.path.isdir(name):
            rec(name + "/", local_name + "/")
        else:
            a, b = os.path.splitext(item)
            if a in ("oxygine-include", "oxygine-forwards", "system_data", "cdecode"):
                continue
            if b == ".h":
                c = "#include \"oxygine/%s\"" % (local_name, )
                open(dest + a + ".hpp", "w").write(c)


rec("../../oxygine/src/oxygine/", "")
