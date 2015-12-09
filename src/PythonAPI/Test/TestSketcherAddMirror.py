import unittest
import model
from TestSketcher import SketcherTestCase
from ModelAPI import *
from GeomDataAPI import geomDataAPI_Point2D

import logging

class SketcherAddLine(SketcherTestCase):
    def setUp(self):
        SketcherTestCase.setUp(self)
        self.line = self.sketch.addLine(0, 0, 0, 1)
        self.circle_1 = self.sketch.addCircle(30, 0, 10)
        model.do()

    def test_add_mirror(self):
        mirror = self.sketch.addMirror(
            self.line.result(), self.circle_1.result()
            )
        model.do()
        mirrored_objects = mirror.mirroredObjects()
        mirrored_circle = mirrored_objects.object(0)
        circle_feature = ModelAPI_Feature.feature(mirrored_circle)
        center = geomDataAPI_Point2D(circle_feature.attribute("CircleCenter"))
        self.assertEqual(center.x(), -30, msg="%s" % center.x())
        # dir(mirrored_circle)
        # center = geomDataAPI_Point2D(mirrored_circle.data().attribute("CircleCenter"))
        # self.assertEqual(center.x(), -30, msg="%s"%(dir(mirrored_circle)))
        # self.assertTrue(False,  msg="%s" % (dir(mirrored_circle)))

if __name__ == "__main__":
    unittest.main(verbosity=2)
