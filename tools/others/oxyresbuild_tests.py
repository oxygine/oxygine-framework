#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import unittest

import oxyresbuild
from resbuild import process_atlas


# sys.path.append(os.path.split(__file__)[0] + "/..")
sys.path.append("d:/oxygine-framework/tools")


class ResAnim(object):

    def __init__(self, data):
        items = data.split(",")
        self.columns = int(items[0])
        self.rows = int(items[1])
        self.frame_size = (int(items[2]), int(items[3]))
        self.frame_scale = float(items[4])
        pass


class Frame(object):

    def __init__(self, data):
        data = data[0:-1]
        items = data.split(",")
        self.atlas_id = int(items[0])
        self.src_pos = (int(items[1]), int(items[2]))
        self.src_destpos = (int(items[3]), int(items[4]))
        self.dest_size = (int(items[5]), int(items[6]))
        self.src = (self.src_pos[0], self.src_pos[1],
                    self.dest_size[0], self.dest_size[1], )


class MyTest(unittest.TestCase):

    def setUp(self):
        pass

    def getResult(self):
        from xml.dom import minidom
        with open("tests/%s.ox/meta.xml" % (self.xml, ), "r") as file:
            doc = minidom.parse(file)
            atlas = doc.documentElement.getElementsByTagName("atlas")[0]
            image = atlas.getElementsByTagName("image")[0]
            fs = image.getAttribute("fs")
            data = image.childNodes[0].data

        return (ResAnim(fs), Frame(data))

    def process(self, xml, args):
        self.xml = xml
        oxyresbuild.process(
            "-x %s --src_data tests --dest_data tests --debug " %
            (xml, ) + args
        )
        return self.getResult()

    def testNoScaleFactor(self):
        xml = "res1.xml"

        anim, frame = self.process(xml, "")

        if 0:
            anim = ResAnim()
            frame = Frame()

        self.assertEqual(anim.frame_size, (128, 128))
        self.assertEqual(frame.src, (0, 0, 64, 64))

        anim, frame = self.process(xml, "-r ")

        self.assertEqual(anim.frame_size, (128, 128))
        self.assertEqual(frame.src, (0, 0, 64, 64))

        anim, frame = self.process(xml, "-r -s 1")

        self.assertEqual(anim.frame_size, (128, 128))
        self.assertEqual(frame.src, (0, 0, 64, 64))

    def testScaleFactor(self):
        xml = "res2.xml"

        if 0:
            anim = ResAnim()
            frame = Frame()

        anim, frame = self.process(xml, "-r -s 3 -us")
        self.assertEqual(anim.frame_size, (64, 64))
        self.assertEqual(frame.src, (0, 0, 99, 99))
        self.assertAlmostEqual(anim.frame_scale, 1 / 3.0, delta=0.0001)

        anim, frame = self.process(xml, "-r -s 3")
        self.assertEqual(anim.frame_size, (64, 64))
        self.assertEqual(frame.src, (0, 0, 64, 64))
        self.assertEqual(anim.frame_scale, 0.5)

        anim, frame = self.process(xml, "-r -s 2")
        self.assertEqual(anim.frame_size, (64, 64))
        self.assertEqual(frame.src, (0, 0, 64, 64))
        self.assertEqual(anim.frame_scale, 0.5)

        anim, frame = self.process(xml, "-r")

        # <image debug_image="test.png" fs="1,1,64,64,0.500000">0,0,0,13,15,37,37;</image>
        # 0, (0,0), (13,15), 37,37
        #

        self.assertEqual(anim.frame_size, (64, 64))
        self.assertEqual(frame.src, (0, 0, 34, 34))
        self.assertEqual(anim.frame_scale, 1.0)

        anim, frame = self.process(xml, "-r -s 0.5")
        self.assertEqual(anim.frame_size, (64, 64))
        self.assertEqual(frame.src, (0, 0, 18, 18))
        self.assertEqual(anim.frame_scale, 2)


if __name__ == "__main__":
    unittest.main()
