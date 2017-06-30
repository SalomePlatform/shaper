## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

import unittest
from salome.shaper import model
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
        mirror = self.sketch.addMirror(self.line, [self.circle_1])
        model.do()
        mirrored_objects = mirror.mirroredObjects()
        mirrored_circle = mirrored_objects.object(0)
        circle_feature = ModelAPI_Feature.feature(mirrored_circle)
        center = geomDataAPI_Point2D(circle_feature.attribute("circle_center"))
        self.assertEqual(center.x(), -30)
        # dir(mirrored_circle)
        # center = geomDataAPI_Point2D(mirrored_circle.data().attribute("circle_center"))
        # self.assertEqual(center.x(), -30, msg="%s"%(dir(mirrored_circle)))
        # self.assertTrue(False,  msg="%s" % (dir(mirrored_circle)))

if __name__ == "__main__":
    unittest.main(verbosity=2)
