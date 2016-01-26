# -*- coding: utf-8 -*-

class class_type(object):

    def __init__(self, spClassName, className, header, ns="oxygine", member="", parent=None, generated=False):
        self.spClassName = spClassName
        self.className = className
        self.header = header
        self.members = []
        self.ns = ns
        self.member = member
        if member:
            self.primary = False
        else:
            self.primary = True

        self.parent = parent
        self.generated = generated


def user_class(*args):
    return class_type(*args, ns="")
