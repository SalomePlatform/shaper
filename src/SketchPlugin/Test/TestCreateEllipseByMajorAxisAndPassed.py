# Copyright (C) 2019-2021  CEA/DEN, EDF R&D
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

"""
    Test creation of ellipse by majoraxis and passed point
"""

import unittest
from salome.shaper import model

from GeomAPI import *
from SketchAPI import *

__updated__ = "2019-09-12"

class TestEllipse(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myAxisStart = GeomAPI_Pnt2d(30., 60.)
    self.myAxisEnd = GeomAPI_Pnt2d(80., 50.)
    self.myPassedPoint = GeomAPI_Pnt2d(60., 60.)
    self.myDOF = 0

  def tearDown(self):
    self.checkDOF()
    model.end()


  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

  def checkPointsEqual(self, thePoint1, thePoint2):
    self.assertAlmostEqual(thePoint1.x(), thePoint2.x(), 5)
    self.assertAlmostEqual(thePoint1.y(), thePoint2.y(), 5)

  def checkPointOnLine(self, theCoordinates, theLine):
    point = GeomAPI_Pnt2d(theCoordinates.x(), theCoordinates.y())
    dist = model.distancePointLine(point, theLine)
    self.assertAlmostEqual(dist, 0, 7)

  def checkPointOnCircle(self, theCoordinates, theCircle):
    point = GeomAPI_Pnt2d(theCoordinates.x(), theCoordinates.y())
    dist = model.distancePointPoint(point, theCircle.center())
    self.assertAlmostEqual(dist , theCircle.radius().value(), 7)

  def checkPointOnEllipse(self, theCoordinates, theEllipse):
    point = GeomAPI_Pnt2d(theCoordinates.x(), theCoordinates.y())
    firstFocus2d = GeomAPI_Pnt2d(theEllipse.firstFocus().x(), theEllipse.firstFocus().y())
    distPF1 = model.distancePointPoint(firstFocus2d,  point)
    secondFocus2d = GeomAPI_Pnt2d(theEllipse.secondFocus().x(), theEllipse.secondFocus().y())
    distPF2 = model.distancePointPoint(secondFocus2d,  point)
    self.assertAlmostEqual(distPF1 + distPF2, 2.0 * theEllipse.majorRadius(), 7)


  def test_ellipse_by_axis_and_passed(self):
    """ Test 1. Create ellipse by points defining major semi-axis and a passed point on ellipse
    """
    self.myEllipse1 = self.mySketch.addEllipse(self.myAxisStart.x(), self.myAxisStart.y(), self.myAxisEnd.x(), self.myAxisEnd.y(), self.myPassedPoint.x(), self.myPassedPoint.y(), False)
    self.myDOF += 5
    model.do()
    anEllipseFeature1 = model.lastSubFeature(self.mySketch, "SketchEllipse")

    self.myEllipse2 = self.mySketch.addEllipse(self.myAxisStart, self.myAxisEnd, self.myPassedPoint, False)
    self.myDOF += 5
    model.do()
    anEllipseFeature2 = model.lastSubFeature(self.mySketch, "SketchEllipse")

    # check both ellipses are equal
    anEllipse1 = anEllipseFeature1.lastResult().shape().edge().ellipse()
    anEllipse2 = anEllipseFeature2.lastResult().shape().edge().ellipse()
    self.checkPointsEqual(anEllipse1.center(), anEllipse2.center())
    self.checkPointsEqual(anEllipse1.firstFocus(), anEllipse2.firstFocus())
    self.checkPointsEqual(anEllipse1.secondFocus(), anEllipse2.secondFocus())
    self.assertAlmostEqual(anEllipse1.minorRadius(), anEllipse2.minorRadius())
    self.assertAlmostEqual(anEllipse1.majorRadius(), anEllipse2.majorRadius())
    # check passed point on ellipse
    self.checkPointOnEllipse(self.myPassedPoint, anEllipse1)
    self.checkPointOnEllipse(self.myPassedPoint, anEllipse2)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 14)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 4)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 2)

  def test_ellipse_with_fixed_axis_start(self):
    """ Test 2. Create ellipse which negative point on the major axis coincident with another point
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse(aLine.startPoint(), self.myAxisEnd, self.myPassedPoint, False)
    self.myDOF += 3
    model.do()
    # check ellipse
    anEllipseFeature = model.lastSubFeature(self.mySketch, "SketchEllipse")
    anEllipse = anEllipseFeature.lastResult().shape().edge().ellipse()
    self.checkPointOnEllipse(self.myPassedPoint, anEllipse)
    self.checkPointOnEllipse(aLine.startPoint(), anEllipse)
    # check distance is equal to major semi-axis
    dist = model.distancePointPoint(GeomAPI_Pnt2d(anEllipse.center().x(), anEllipse.center().y()), aLine.startPoint())
    self.assertAlmostEqual(dist, anEllipse.majorRadius(), 7)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 3)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_ellipse_with_axis_start_on_line(self):
    """ Test 3. Create ellipse which negative point on the major axis coincident with a line
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse([self.myAxisStart, aLine.result()], self.myAxisEnd, self.myPassedPoint, False)
    self.myDOF += 4
    model.do()

    anEllipse = SketchAPI_Ellipse(model.lastSubFeature(self.mySketch, "SketchEllipse"))
    # check negative point of major axis on line
    self.checkPointOnLine(anEllipse.majorAxisNegative(), aLine)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 3)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_ellipse_with_axis_start_on_circle(self):
    """ Test 4. Create ellipse which negative point on the major axis coincident with a circle
    """
    aCircle = self.mySketch.addCircle(10, 10, 20)
    self.myDOF += 3
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse([self.myAxisStart, aCircle.defaultResult()], self.myAxisEnd, self.myPassedPoint, False)
    self.myDOF += 4
    model.do()

    anEllipse = SketchAPI_Ellipse(model.lastSubFeature(self.mySketch, "SketchEllipse"))
    # check center on circle
    self.checkPointOnCircle(anEllipse.majorAxisNegative(), aCircle)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 2)
    model.testNbSubFeatures(self.mySketch, "SketchCircle", 1)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_ellipse_with_axis_start_on_ellipse(self):
    """ Test 5. Create ellipse which negative point on the major axis coincident with another ellipse
    """
    anOtherEllipse = self.mySketch.addEllipse(10, 10, 30, 20, 10)
    self.myDOF += 5
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse([self.myAxisStart, anOtherEllipse.defaultResult()], self.myAxisEnd, self.myPassedPoint, False)
    self.myDOF += 4
    model.do()

    anEllipse = SketchAPI_Ellipse(model.lastSubFeature(self.mySketch, "SketchEllipse"))
    # check center on ellipse
    self.checkPointOnEllipse(anEllipse.majorAxisNegative(), anOtherEllipse.defaultResult().shape().edge().ellipse())
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 2)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 2)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_ellipse_with_fixed_axis_end(self):
    """ Test 6. Create ellipse which positive point on the major axis coincident with another point
    """
    aLine = self.mySketch.addLine(10, 10, 90, 40)
    self.myDOF += 4
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse(self.myAxisStart, aLine.endPoint(), self.myPassedPoint, False)
    self.myDOF += 3
    model.do()
    # check ellipse
    anEllipseFeature = model.lastSubFeature(self.mySketch, "SketchEllipse")
    anEllipse = anEllipseFeature.lastResult().shape().edge().ellipse()
    self.checkPointOnEllipse(self.myPassedPoint, anEllipse)
    self.checkPointOnEllipse(aLine.endPoint(), anEllipse)
    # check distance is equal to major semi-axis
    dist = model.distancePointPoint(GeomAPI_Pnt2d(anEllipse.center().x(), anEllipse.center().y()), aLine.endPoint())
    self.assertAlmostEqual(dist, anEllipse.majorRadius(), 7)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 3)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_ellipse_with_axis_end_on_line(self):
    """ Test 7. Create ellipse which negative point on the major axis coincident with a line
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse(self.myAxisStart, [self.myAxisEnd, aLine.result()], self.myPassedPoint, False)
    self.myDOF += 4
    model.do()

    anEllipse = SketchAPI_Ellipse(model.lastSubFeature(self.mySketch, "SketchEllipse"))
    # check axis point on line
    self.checkPointOnLine(anEllipse.majorAxisPositive(), aLine)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 3)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_ellipse_with_axis_end_on_circle(self):
    """ Test 8. Create ellipse which negative point on the major axis coincident with a circle
    """
    aCircle = self.mySketch.addCircle(10, 10, 20)
    self.myDOF += 3
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse(self.myAxisStart, [self.myAxisEnd, aCircle.defaultResult()], self.myPassedPoint, False)
    self.myDOF += 4
    model.do()

    anEllipse = SketchAPI_Ellipse(model.lastSubFeature(self.mySketch, "SketchEllipse"))
    # check center on circle
    self.checkPointOnCircle(anEllipse.majorAxisPositive(), aCircle)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 2)
    model.testNbSubFeatures(self.mySketch, "SketchCircle", 1)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_ellipse_with_axis_end_on_ellipse(self):
    """ Test 9. Create ellipse which negative point on the major axis coincident with another ellipse
    """
    anOtherEllipse = self.mySketch.addEllipse(10, 10, 90, 40, 30)
    self.myDOF += 5
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse(self.myAxisStart, [self.myAxisEnd, anOtherEllipse.defaultResult()], self.myPassedPoint, False)
    self.myDOF += 4
    model.do()

    anEllipse = SketchAPI_Ellipse(model.lastSubFeature(self.mySketch, "SketchEllipse"))
    # check center on ellipse
    self.checkPointOnEllipse(anEllipse.majorAxisPositive(), anOtherEllipse.defaultResult().shape().edge().ellipse())
    # check coincidence feature exists
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 2)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 2)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_ellipse_with_fixed_passed_point(self):
    """ Test 10. Create ellipse which passed point is coincident with another point
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse(self.myAxisStart, self.myAxisEnd, aLine.endPoint(), False)
    self.myDOF += 4
    model.do()
    # check ellipse
    anEllipseFeature = model.lastSubFeature(self.mySketch, "SketchEllipse")
    anEllipse = anEllipseFeature.lastResult().shape().edge().ellipse()
    self.checkPointOnEllipse(aLine.endPoint(), anEllipse)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 3)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_ellipse_with_passed_point_on_line(self):
    """ Test 11. Create ellipse which passed point is placed on a line.
                 Check no constraints is applied.
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse(self.myAxisStart, self.myAxisEnd, [self.myPassedPoint, aLine.result()], False)
    self.myDOF += 5
    model.do()

    anEllipseFeature = model.lastSubFeature(self.mySketch, "SketchEllipse")
    anEllipse = anEllipseFeature.lastResult().shape().edge().ellipse()
    self.checkPointOnEllipse(self.myPassedPoint, anEllipse)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 3)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 1)
    # check neither coincidence nor tangent feature exists
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 0)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintTangent", 0)


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    assert model.checkPythonDump()
