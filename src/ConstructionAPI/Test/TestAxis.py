# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import unittest
from salome.shaper import model
from GeomAPI import *
from ModelAPI import *

class AxisTestCase(unittest.TestCase):

    def setUp(self):
        self.doc = model.moduleDocument()

    def tearDown(self):
        model.end()
        assert(model.checkPythonDump())
        ModelAPI_Session.get().closeAll()

    def checkAxis(self, axis, x1, y1, z1, x2, y2, z2):
        edge = GeomAPI_Edge(axis.results()[0].resultSubShapePair()[0].shape())
        self.assertTrue(edge.isLine())

        p1 = edge.firstPoint()
        self.assertAlmostEqual(p1.x(), x1, 6)
        self.assertAlmostEqual(p1.y(), y1, 6)
        self.assertAlmostEqual(p1.z(), z1, 6)

        p2 = edge.lastPoint()
        self.assertAlmostEqual(p2.x(), x2, 6)
        self.assertAlmostEqual(p2.y(), y2, 6)
        self.assertAlmostEqual(p2.z(), z2, 6)


    def test_axis_by_dimensions(self):
        """ Test 1. Create axis starting from Origin with the given dimensions
        """
        axis = model.addAxis(self.doc, 10, 50, 100)
        self.checkAxis(axis, 0, 0, 0, 10, 50, 100)

    def test_axis_by_points(self):
        """ Test 2. Create axis between pair of points
        """
        model.addPoint(self.doc, 10, 0, 0)
        axis = model.addAxis(self.doc, model.selection("VERTEX", "Origin"), model.selection("VERTEX", "Point_2"))
        self.checkAxis(axis, 0, 0, 0, 10, 0, 0)

    def test_axis_by_point_and_plane(self):
        """ Test 3. Create axis orthogonal to a plane and passing through a point
        """
        model.addPoint(self.doc, 10, 0, 0)
        axis = model.addAxis(self.doc, model.selection("FACE", "YOZ"), model.selection("VERTEX", "Point_2"))
        self.checkAxis(axis, 10, 0, 0, 0, 0, 0)

    def test_axis_by_line(self):
        """ Test 4. Create axis by edge on a box
        """
        partDoc = model.addPart(self.doc).document()
        model.addBox(partDoc, 10, 10, 10)
        axis = model.addAxis(partDoc, model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"))
        self.checkAxis(axis, 10, 0, 0, 10, 0, 10)

    def test_axis_by_cylinder(self):
        """ Test 5. Create axis by cylindrical face
        """
        partDoc = model.addPart(self.doc).document()
        model.addCylinder(partDoc, 5, 10)
        axis = model.addAxis(partDoc, model.selection("FACE", "Cylinder_1_1/Face_1"))
        self.checkAxis(axis, 0, 0, -1, 0, 0, 11)

    def test_axis_by_point_and_direction(self):
        """ Test 6. Create axis starting from a point with a given direction
        """
        model.addPoint(self.doc, 10, 0, 0)
        axis = model.addAxis(self.doc, model.selection("VERTEX", "Point_2"), 10, 20, 30)
        self.checkAxis(axis, 10, 0, 0, 20, 20, 30)

    def test_axis_by_planes(self):
        """ Test 7. Create axis as intersection of two planes
        """
        axis = model.addAxis(self.doc, model.selection("FACE", "XOZ"), model.selection("FACE", "YOZ"))
        self.checkAxis(axis, 0, 0, 0, 0, 0, 100)

        axis = model.addAxis(self.doc, model.selection("FACE", "XOZ"), 10, False, model.selection("FACE", "YOZ"))
        self.checkAxis(axis, 0, -10, 0, 0, -10, 100)

        axis = model.addAxis(self.doc, model.selection("FACE", "XOZ"), model.selection("FACE", "YOZ"), 10, False)
        self.checkAxis(axis, 10, 0, 0, 10, 0, 100)


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
