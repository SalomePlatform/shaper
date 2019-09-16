# Copyright (C) 2019  CEA/DEN, EDF R&D
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
    Test creation of ellipse by center, semi-axis and passed point
"""

import unittest
from salome.shaper import model

from GeomAPI import *
from SketchAPI import *

__updated__ = "2019-09-09"

class TestEllipse(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myCenter = GeomAPI_Pnt2d(50., 50.)
    self.myFocus = GeomAPI_Pnt2d(70., 50.)
    self.myPassedPoint = GeomAPI_Pnt2d(60., 60.)
    self.myMinorRadius = 20.
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
    if issubclass(type(theEllipse), SketchAPI_Ellipse):
      majorRad = theEllipse.majorRadius().value()
    else:
      majorRad = theEllipse.majorRadius()
    self.assertAlmostEqual(distPF1 + distPF2, 2.0 * majorRad, 7)


  def test_ellipse_by_center_and_focus(self):
    """ Test 1. Create ellipse by coordinates of center, focus and minor radius
    """
    self.myEllipse1 = self.mySketch.addEllipse(self.myCenter.x(), self.myCenter.y(), self.myFocus.x(), self.myFocus.y(), self.myMinorRadius)
    self.myDOF += 5

    self.myEllipse2 = self.mySketch.addEllipse(self.myCenter, self.myFocus, self.myMinorRadius)
    self.myDOF += 5
    model.do()

    # check both ellipses are equal
    anEllipse1 = self.myEllipse1.defaultResult().shape().edge().ellipse()
    anEllipse2 = self.myEllipse2.defaultResult().shape().edge().ellipse()
    self.checkPointsEqual(anEllipse1.center(), anEllipse2.center())
    self.checkPointsEqual(anEllipse1.firstFocus(), anEllipse2.firstFocus())
    self.checkPointsEqual(anEllipse1.secondFocus(), anEllipse2.secondFocus())
    self.assertAlmostEqual(anEllipse1.minorRadius(), anEllipse2.minorRadius())
    self.assertAlmostEqual(anEllipse1.majorRadius(), anEllipse2.majorRadius())
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 0)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 0)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 2)

  def test_ellipse_by_semiaxis_and_passed(self):
    """ Test 2. Create ellipse by coordinates of center, major semi-axis point and a passed point on ellipse
    """
    self.myEllipse1 = self.mySketch.addEllipse(self.myCenter.x(), self.myCenter.y(), self.myFocus.x(), self.myFocus.y(), self.myPassedPoint.x(), self.myPassedPoint.y(), True)
    self.myDOF += 5
    model.do()
    anEllipseFeature1 = model.lastSubFeature(self.mySketch, "SketchEllipse")

    self.myEllipse2 = self.mySketch.addEllipse(self.myCenter, self.myFocus, self.myPassedPoint, True)
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

  def test_ellipse_with_fixed_center(self):
    """ Test 3. Create ellipse which center is coincident with another point
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse(aLine.endPoint(), self.myFocus, self.myPassedPoint, True)
    self.myDOF += 3
    model.do()
    # check ellipse
    anEllipseFeature = model.lastSubFeature(self.mySketch, "SketchEllipse")
    anEllipse = anEllipseFeature.lastResult().shape().edge().ellipse()
    self.checkPointsEqual(anEllipse.center(), aLine.endPoint())
    self.checkPointOnEllipse(self.myPassedPoint, anEllipse)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 3)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_ellipse_with_center_on_line(self):
    """ Test 4. Create ellipse which center is coincident with a line
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse([self.myCenter, aLine.result()], self.myFocus, self.myPassedPoint, True)
    self.myDOF += 4
    model.do()

    anEllipseFeature = model.lastSubFeature(self.mySketch, "SketchEllipse")
    anEllipse = anEllipseFeature.lastResult().shape().edge().ellipse()
    # check center on line
    self.checkPointOnLine(anEllipse.center(), aLine)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 3)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_ellipse_with_center_on_circle(self):
    """ Test 5. Create ellipse which center is coincident with a circle
    """
    aCircle = self.mySketch.addCircle(10, 10, 20)
    self.myDOF += 3
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse([self.myCenter, aCircle.defaultResult()], self.myFocus, self.myPassedPoint, True)
    self.myDOF += 4
    model.do()

    anEllipseFeature = model.lastSubFeature(self.mySketch, "SketchEllipse")
    anEllipse = anEllipseFeature.lastResult().shape().edge().ellipse()
    # check center on circle
    self.checkPointOnCircle(anEllipse.center(), aCircle)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 2)
    model.testNbSubFeatures(self.mySketch, "SketchCircle", 1)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_ellipse_with_center_on_ellipse(self):
    """ Test 6. Create ellipse which center is coincident with another ellipse
    """
    anOtherEllipse = self.mySketch.addEllipse(10, 10, 30, 20, 10)
    self.myDOF += 5
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse([self.myCenter, anOtherEllipse.defaultResult()], self.myFocus, self.myPassedPoint, True)
    self.myDOF += 4
    model.do()

    anEllipseFeature = model.lastSubFeature(self.mySketch, "SketchEllipse")
    anEllipse = anEllipseFeature.lastResult().shape().edge().ellipse()
    # check center on ellipse
    self.checkPointOnEllipse(anEllipse.center(), anOtherEllipse)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 2)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 2)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_ellipse_with_fixed_axis(self):
    """ Test 7. Create ellipse which point on major semi-axis is coincident with another point
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse(self.myCenter, aLine.endPoint(), self.myPassedPoint, True)
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

  def test_ellipse_with_axis_on_line(self):
    """ Test 8. Create ellipse which point on major semi-axis is coincident with a line
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse(self.myCenter, [self.myFocus, aLine.result()], self.myPassedPoint, True)
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

  def test_ellipse_with_axis_on_circle(self):
    """ Test 9. Create ellipse which point on major semi-axis is coincident with a circle
    """
    aCircle = self.mySketch.addCircle(10, 10, 20)
    self.myDOF += 3
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse(self.myCenter, [self.myFocus, aCircle.defaultResult()], self.myPassedPoint, True)
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

  def test_ellipse_with_axis_on_ellipse(self):
    """ Test 10. Create ellipse which point on major semi-axis is coincident with another ellipse
    """
    anOtherEllipse = self.mySketch.addEllipse(10, 10, 90, 40, 30)
    self.myDOF += 5
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse(self.myCenter, [self.myFocus, anOtherEllipse.defaultResult()], self.myPassedPoint, True)
    self.myDOF += 4
    model.do()

    anEllipse = SketchAPI_Ellipse(model.lastSubFeature(self.mySketch, "SketchEllipse"))
    # check center on ellipse
    self.checkPointOnEllipse(anEllipse.majorAxisPositive(), anOtherEllipse)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 2)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 2)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_ellipse_with_fixed_passed_point(self):
    """ Test 11. Create ellipse which passed point is coincident with another point
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse(self.myCenter, self.myFocus, aLine.endPoint(), True)
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
    """ Test 12. Create ellipse which passed point is placed on a line.
                 Check no constraints is applied.
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.myEllipse1 = self.mySketch.addEllipse(self.myCenter, self.myFocus, [self.myPassedPoint, aLine.result()], True)
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
