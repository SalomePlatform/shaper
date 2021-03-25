# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

import math
import unittest

import ModelAPI
import SketchAPI

from salome.shaper import geom
from salome.shaper import model

class SketchTestCase(unittest.TestCase):

    def setUp(self):
        model.begin()
        self.session = ModelAPI.ModelAPI_Session.get()
        aPartSet = self.session.moduleDocument()
        self.doc = model.addPart(aPartSet).document()
        model.addCylinder(self.doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
        model.addCylinder(self.doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10, 10, 90)
        self.sketch = model.addSketch(self.doc, model.defaultPlane("XOY"))

    def tearDown(self):
        model.end()
        assert(model.checkPythonDump())
        self.session.closeAll()

    def checkPoint(self, thePoint, theReference):
        self.assertAlmostEqual(thePoint.x(), theReference.x(), 6)
        self.assertAlmostEqual(thePoint.y(), theReference.y(), 6)

    def test_circle_by_point(self):
        """ Test 1. Create point by GeomAPI_Pnt and circle coincident by the center
        """
        point = geom.Pnt2d(10., 10.)
        aPoint = self.sketch.addPoint(point)
        aCircle = self.sketch.addCircle(point, 10.)
        aCoincidence = self.sketch.setCoincident(aPoint.coordinates(), aCircle.center())
        model.do()
        self.checkPoint(aPoint.coordinates(), point)
        self.checkPoint(aCircle.center(), point)

    def test_circle_by_external(self):
        """ Test 2. Create point and circle by external features
        """
        aPoint = self.sketch.addPoint("PartSet/Origin")
        aCircle = self.sketch.addCircle("[Cylinder_1_1/Face_1][Cylinder_1_1/Face_3]")
        model.do()
        point = geom.Pnt2d(0., 0.)
        self.checkPoint(aPoint.coordinates(), point)
        self.checkPoint(aCircle.center(), point)

    def test_circle_center_passed(self):
        """ Test 3. Create circle by center and passed point
        """
        aCenter = geom.Pnt2d(10., 10.)
        aPassed = geom.Pnt2d(20., 30.)

        self.sketch.addCircle(aCenter.x(), aCenter.y(), aPassed.x(), aPassed.y())
        model.do()
        aCircle1 = SketchAPI.SketchAPI_Circle(model.lastSubFeature(self.sketch, "SketchCircle"))

        self.sketch.addCircle(aCenter, aPassed)
        model.do()
        aCircle2 = SketchAPI.SketchAPI_Circle(model.lastSubFeature(self.sketch, "SketchCircle"))

        aRadius = math.sqrt((aCenter.x()-aPassed.x())**2 + (aCenter.y()-aPassed.y())**2)
        self.checkPoint(aCircle1.center(), aCenter)
        self.assertAlmostEqual(aCircle1.radius().value(), aRadius, 6)
        self.checkPoint(aCircle2.center(), aCenter)
        self.assertAlmostEqual(aCircle2.radius().value(), aRadius, 6)

    def test_circle_by_three_points(self):
        """ Test 4. Create circle by three passed points
        """
        aCenter = geom.Pnt2d(10., 10.)
        aRadius = 5.

        self.sketch.addCircle(aCenter.x() - aRadius, aCenter.y(), aCenter.x() + aRadius, aCenter.y(), aCenter.x(), aCenter.y() + aRadius)
        model.do()
        aCircle = SketchAPI.SketchAPI_Circle(model.lastSubFeature(self.sketch, "SketchCircle"))

        self.checkPoint(aCircle.center(), aCenter)
        self.assertAlmostEqual(aCircle.radius().value(), aRadius, 6)

    def test_arc_by_three_points(self):
        """ Test 5. Create arc by three passed points
        """
        aCenter = geom.Pnt2d(10., 10.)
        aRadius = 5.

        aPoint1 = geom.Pnt2d(aCenter.x() - aRadius, aCenter.y())
        aPoint2 = geom.Pnt2d(aCenter.x() + aRadius, aCenter.y())
        aPoint3 = geom.Pnt2d(aCenter.x(), aCenter.y() + aRadius)

        self.sketch.addArc(aPoint1.x(), aPoint1.y(), aPoint2.x(), aPoint2.y(), aPoint3.x(), aPoint3.y())
        model.do()
        anArc1 = SketchAPI.SketchAPI_Arc(model.lastSubFeature(self.sketch, "SketchArc"))

        self.checkPoint(anArc1.center(), aCenter)
        self.assertAlmostEqual(anArc1.radius().value(), aRadius, 6)

        self.sketch.addArc(aPoint1, aPoint2, aPoint3)
        model.do()
        anArc2 = SketchAPI.SketchAPI_Arc(model.lastSubFeature(self.sketch, "SketchArc"))

        self.checkPoint(anArc2.center(), aCenter)
        self.assertAlmostEqual(anArc2.radius().value(), aRadius, 6)

    def test_arc_by_tangent_point(self):
        """ Test 6. Create arc tangent to a line
        """
        aTgPnt = geom.Pnt2d(10., 0.)
        aRadius = 5.

        aLine = self.sketch.addLine(0., 0., aTgPnt.x(), aTgPnt.y())
        model.do()

        aPassed = geom.Pnt2d(aTgPnt.x(), aTgPnt.y() + 2. * aRadius)
        self.sketch.addArc(aLine.endPoint(), aPassed, False)
        model.do()
        anArc = SketchAPI.SketchAPI_Arc(model.lastSubFeature(self.sketch, "SketchArc"))

        aCenter = geom.Pnt2d(aTgPnt.x(), aTgPnt.y() + aRadius)
        self.checkPoint(anArc.center(), aCenter)
        self.assertAlmostEqual(anArc.radius().value(), aRadius, 6)

    def test_arc_by_external(self):
        """ Test 7. Create arc by external feature
        """
        anArc = self.sketch.addArc(model.selection("EDGE", "[Cylinder_2_1/Face_1][Cylinder_2_1/Face_3]"))
        model.do()
        point = geom.Pnt2d(0., 0.)
        self.checkPoint(anArc.center(), point)
        self.assertAlmostEqual(anArc.radius().value(), 10, 6)

    def test_arc_by_name(self):
        """ Test 8. Create arc by external feature
        """
        anArc = self.sketch.addArc("[Cylinder_2_1/Face_1][Cylinder_2_1/Face_3]")
        model.do()
        point = geom.Pnt2d(0., 0.)
        self.checkPoint(anArc.center(), point)
        self.assertAlmostEqual(anArc.radius().value(), 10, 6)

    def test_point_by_intersection(self):
        """ Test 9. Create point as intersection with external edge given by a name
        """
        self.sketch.addIntersectionPoint("[Cylinder_2_1/Face_1][Cylinder_2_1/Face_4]")
        model.do()
        aPoint = SketchAPI.SketchAPI_Point(model.lastSubFeature(self.sketch, "SketchPoint"))

        point = geom.Pnt2d(10., 0.)
        self.checkPoint(aPoint.coordinates(), point)

    def test_arc_by_projection(self):
        """ Test 10. Create arc by projection of external feature
        """
        self.sketch.addProjection(model.selection("EDGE", "[Cylinder_2_1/Face_1][Cylinder_2_1/Face_3]"))
        model.do()
        anArc = SketchAPI.SketchAPI_Arc(model.lastSubFeature(self.sketch, "SketchArc"))

        point = geom.Pnt2d(0., 0.)
        self.checkPoint(anArc.center(), point)
        self.assertAlmostEqual(anArc.radius().value(), 10, 6)


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
