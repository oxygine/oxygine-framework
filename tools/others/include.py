#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import glob


def process(path):
    files = os.listdir(path)
    for f in files:
        name = path + f
        if os.path.isfile(name):
            if os.path.splitext(name)[1] == ".h":
                lines = []
                with open(name, "r") as fh:
                    lines = fh.readlines()
                with open(name, "w") as fh:
                    lines.insert(1, "#include \"oxygine_include.h\"\n")
                    fh.writelines(lines)

        else:
            process(name + "/")

process("./")
