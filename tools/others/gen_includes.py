#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os


def rec(path, local):
    for item in os.listdir(path):
        if item in ("winnie_alloc", "minizip", "dev_tools", "gl", "android",
                    "closure", "pugixml", "text_utils", ):
            continue
        name = path + item
        local_name = local + item
        if os.path.isdir(name):
            rec(name + "/", local_name + "/")
        else:
            a, b = os.path.splitext(item)
            if b == ".h":
                c = "#include \"%s\"" % (local_name, )
                open("../../oxygine/include/oxygine/" + a, "w").write(c)

rec("../../oxygine/src/", "")
