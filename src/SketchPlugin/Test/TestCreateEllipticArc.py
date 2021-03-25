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
    Test creation of elliptic arc by center, semi-axis, start and end points
"""

import unittest
from salome.shaper import model

from GeomAPI import *
from SketchAPI import *

__updated__ = "2019-10-01"

class TestEllipticArc(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myCenter = GeomAPI_Pnt2d(50., 50.)
    self.myFocus = GeomAPI_Pnt2d(70., 60.)
    self.myStartPoint = GeomAPI_Pnt2d(60., 65.)
    self.myEndPoint = GeomAPI_Pnt2d(60., 42.535751)
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


  def test_elliptic_arc_by_coordinates(self):
    """ Test 1. Create elliptic arc by coordinates of center, point on the major axis, start and end points
    """
    self.myEllipse1 = self.mySketch.addEllipticArc(self.myCenter.x(), self.myCenter.y(),
                                                   self.myFocus.x(), self.myFocus.y(),
                                                   self.myStartPoint.x(), self.myStartPoint.y(),
                                                   self.myEndPoint.x(), self.myEndPoint.y(), False)
    self.myDOF += 7

    self.myEllipse2 = self.mySketch.addEllipticArc(self.myCenter.x(), self.myCenter.y(),
                                                   self.myFocus.x(), self.myFocus.y(),
                                                   self.myStartPoint.x(), self.myStartPoint.y(),
                                                   self.myEndPoint.x(), self.myEndPoint.y(), True)
    self.myDOF += 7
    model.do()

    # check both ellipses are equal
    anArcEdge1 = self.myEllipse1.defaultResult().shape().edge()
    anArcEdge2 = self.myEllipse2.defaultResult().shape().edge()
    anEllipse1 = anArcEdge1.ellipse()
    anEllipse2 = anArcEdge2.ellipse()
    self.checkPointsEqual(anEllipse1.center(), anEllipse2.center())
    self.checkPointsEqual(anEllipse1.firstFocus(), anEllipse2.firstFocus())
    self.checkPointsEqual(anEllipse1.secondFocus(), anEllipse2.secondFocus())
    self.assertAlmostEqual(anEllipse1.minorRadius(), anEllipse2.minorRadius())
    self.assertAlmostEqual(anEllipse1.majorRadius(), anEllipse2.majorRadius())
    self.checkPointsEqual(self.myEllipse1.startPoint(), self.myEllipse2.startPoint())
    self.checkPointsEqual(self.myEllipse1.endPoint(), self.myEllipse2.endPoint())
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 0)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 0)
    model.testNbSubFeatures(self.mySketch, "SketchEllipticArc", 2)
    # check middle points are different
    assert(anArcEdge1.middlePoint().x() < self.myStartPoint.x())
    assert(anArcEdge2.middlePoint().x() > self.myStartPoint.x())

  def test_elliptic_arc_by_points(self):
    """ Test 2. Create elliptic arc by points
    """
    self.mySketch.addEllipticArc(self.myCenter, self.myFocus, self.myStartPoint, self.myEndPoint, False)
    self.myDOF += 7
    model.do()
    anEllipseFeature1 = model.lastSubFeature(self.mySketch, "SketchEllipticArc")
    self.myEllipse1 = SketchAPI_EllipticArc(anEllipseFeature1)

    self.mySketch.addEllipticArc(self.myCenter, self.myFocus, self.myStartPoint, self.myEndPoint, True)
    self.myDOF += 7
    model.do()
    anEllipseFeature2 = model.lastSubFeature(self.mySketch, "SketchEllipticArc")
    self.myEllipse2 = SketchAPI_EllipticArc(anEllipseFeature2)

    # check both ellipses are equal
    anArcEdge1 = anEllipseFeature1.lastResult().shape().edge()
    anArcEdge2 = anEllipseFeature2.lastResult().shape().edge()
    anEllipse1 = anArcEdge1.ellipse()
    anEllipse2 = anArcEdge2.ellipse()
    self.checkPointsEqual(anEllipse1.center(), anEllipse2.center())
    self.checkPointsEqual(anEllipse1.firstFocus(), anEllipse2.firstFocus())
    self.checkPointsEqual(anEllipse1.secondFocus(), anEllipse2.secondFocus())
    self.assertAlmostEqual(anEllipse1.minorRadius(), anEllipse2.minorRadius())
    self.assertAlmostEqual(anEllipse1.majorRadius(), anEllipse2.majorRadius())
    self.checkPointsEqual(self.myEllipse1.startPoint(), self.myEllipse2.startPoint())
    self.checkPointsEqual(self.myEllipse1.endPoint(), self.myEllipse2.endPoint())
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 14)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 4)
    model.testNbSubFeatures(self.mySketch, "SketchEllipticArc", 2)
    # check middle points are different
    assert(anArcEdge1.middlePoint().x() < self.myStartPoint.x())
    assert(anArcEdge2.middlePoint().x() > self.myStartPoint.x())

  def test_elliptic_arc_with_fixed_center(self):
    """ Test 3. Create elliptic arc which center is coincident with another point
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.mySketch.addEllipticArc(aLine.endPoint(), self.myFocus, self.myStartPoint, self.myEndPoint, True)
    self.myDOF += 5
    model.do()
    # check ellipse
    anEllipseFeature = model.lastSubFeature(self.mySketch, "SketchEllipticArc")
    self.myEllipse1 = SketchAPI_EllipticArc(anEllipseFeature)
    anEllipse = anEllipseFeature.lastResult().shape().edge().ellipse()
    self.checkPointsEqual(anEllipse.center(), aLine.endPoint())
    self.checkPointOnEllipse(self.myStartPoint, anEllipse)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 3)
    model.testNbSubFeatures(self.mySketch, "SketchEllipticArc", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_elliptic_arc_with_center_on_line(self):
    """ Test 4. Create elliptic arc which center is coincident with a line
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.mySketch.addEllipticArc([self.myCenter, aLine.result()], self.myFocus, self.myStartPoint, self.myEndPoint, False)
    self.myDOF += 6
    model.do()

    anEllipseFeature = model.lastSubFeature(self.mySketch, "SketchEllipticArc")
    self.myEllipse1 = SketchAPI_EllipticArc(anEllipseFeature)
    anEllipse = anEllipseFeature.lastResult().shape().edge().ellipse()
    # check center on line
    self.checkPointOnLine(anEllipse.center(), aLine)
    self.checkPointOnEllipse(self.myEllipse1.startPoint(), anEllipse)
    self.checkPointOnEllipse(self.myEllipse1.endPoint(), anEllipse)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 3)
    model.testNbSubFeatures(self.mySketch, "SketchEllipticArc", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_elliptic_arc_with_center_on_circle(self):
    """ Test 5. Create elliptic arc which center is coincident with a circle
    """
    aCircle = self.mySketch.addCircle(10, 10, 20)
    self.myDOF += 3
    model.do()

    self.mySketch.addEllipticArc([self.myCenter, aCircle.defaultResult()], self.myFocus, self.myStartPoint, self.myEndPoint, False)
    self.myDOF += 6
    model.do()

    anEllipseFeature = model.lastSubFeature(self.mySketch, "SketchEllipticArc")
    self.myEllipse1 = SketchAPI_EllipticArc(anEllipseFeature)
    anEllipse = anEllipseFeature.lastResult().shape().edge().ellipse()
    # check center on circle
    self.checkPointOnCircle(anEllipse.center(), aCircle)
    self.checkPointOnEllipse(self.myEllipse1.startPoint(), anEllipse)
    self.checkPointOnEllipse(self.myEllipse1.endPoint(), anEllipse)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 2)
    model.testNbSubFeatures(self.mySketch, "SketchCircle", 1)
    model.testNbSubFeatures(self.mySketch, "SketchEllipticArc", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_elliptic_arc_with_center_on_ellipse(self):
    """ Test 6. Create elliptic arc which center is coincident with another ellipse
    """
    anOtherEllipse = self.mySketch.addEllipse(10, 10, 30, 20, 10)
    self.myDOF += 5
    model.do()

    self.mySketch.addEllipticArc([self.myCenter, anOtherEllipse.defaultResult()], self.myFocus, self.myStartPoint, self.myEndPoint, False)
    self.myDOF += 6
    model.do()

    anEllipseFeature = model.lastSubFeature(self.mySketch, "SketchEllipticArc")
    self.myEllipse1 = SketchAPI_EllipticArc(anEllipseFeature)
    anEllipse = anEllipseFeature.lastResult().shape().edge().ellipse()
    # check center on ellipse
    self.checkPointOnEllipse(anEllipse.center(), anOtherEllipse)
    self.checkPointOnEllipse(self.myEllipse1.startPoint(), anEllipse)
    self.checkPointOnEllipse(self.myEllipse1.endPoint(), anEllipse)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 2)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 1)
    model.testNbSubFeatures(self.mySketch, "SketchEllipticArc", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_elliptic_arc_with_fixed_axis(self):
    """ Test 7. Create elliptic arc which point on major semi-axis is coincident with another point
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.mySketch.addEllipticArc(self.myCenter, aLine.endPoint(), self.myStartPoint, self.myEndPoint, False)
    self.myDOF += 6
    model.do()
    # check ellipse
    anEllipseFeature = model.lastSubFeature(self.mySketch, "SketchEllipticArc")
    self.myEllipse1 = SketchAPI_EllipticArc(anEllipseFeature)
    anEllipse = anEllipseFeature.lastResult().shape().edge().ellipse()
    self.checkPointOnEllipse(self.myStartPoint, anEllipse)
    self.checkPointOnEllipse(aLine.endPoint(), anEllipse)
    # check distance is equal to major semi-axis
    dist = model.distancePointPoint(GeomAPI_Pnt2d(anEllipse.center().x(), anEllipse.center().y()), aLine.endPoint())
    self.assertAlmostEqual(dist, anEllipse.majorRadius(), 7)
    self.checkPointsEqual(self.myEllipse1.majorAxisPositive(), aLine.endPoint())
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 3)
    model.testNbSubFeatures(self.mySketch, "SketchEllipticArc", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_elliptic_arc_with_axis_on_line(self):
    """ Test 8. Create elliptic arc which point on major semi-axis is coincident with a line.
                Check no coincidence constraint is created.
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.mySketch.addEllipticArc(self.myCenter, [self.myFocus, aLine.result()], self.myStartPoint, self.myEndPoint, True)
    self.myDOF += 7
    model.do()

    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 3)
    model.testNbSubFeatures(self.mySketch, "SketchEllipticArc", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 0)

  def test_elliptic_arc_with_fixed_start_point(self):
    """ Test 9. Create elliptic arc which start point is coincident with another point
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.mySketch.addEllipticArc(self.myCenter, self.myFocus, aLine.endPoint(), self.myEndPoint, True)
    self.myDOF += 5
    model.do()
    # check ellipse
    anEllipseFeature = model.lastSubFeature(self.mySketch, "SketchEllipticArc")
    self.myEllipse1 = SketchAPI_EllipticArc(anEllipseFeature)
    anEllipse = anEllipseFeature.lastResult().shape().edge().ellipse()
    self.checkPointOnEllipse(aLine.endPoint(), anEllipse)
    self.checkPointsEqual(aLine.endPoint(), self.myEllipse1.startPoint())
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 3)
    model.testNbSubFeatures(self.mySketch, "SketchEllipticArc", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_elliptic_arc_with_start_point_on_line(self):
    """ Test 10. Create elliptic arc which start point is placed on a line.
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.mySketch.addEllipticArc(self.myCenter, self.myFocus, [self.myStartPoint, aLine.result()], self.myEndPoint, False)
    self.myDOF += 6
    model.do()

    anEllipseFeature = model.lastSubFeature(self.mySketch, "SketchEllipticArc")
    self.myEllipse1 = SketchAPI_EllipticArc(anEllipseFeature)
    anEllipse = anEllipseFeature.lastResult().shape().edge().ellipse()
    self.checkPointOnLine(self.myEllipse1.startPoint(), aLine)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 3)
    model.testNbSubFeatures(self.mySketch, "SketchEllipticArc", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_elliptic_arc_with_fixed_end_point(self):
    """ Test 11. Create elliptic arc which end point is coincident with another point
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.mySketch.addEllipticArc(self.myCenter, self.myFocus, self.myStartPoint, aLine.endPoint(), True)
    self.myDOF += 5
    model.do()
    # check ellipse
    anEllipseFeature = model.lastSubFeature(self.mySketch, "SketchEllipticArc")
    self.myEllipse1 = SketchAPI_EllipticArc(anEllipseFeature)
    anEllipse = anEllipseFeature.lastResult().shape().edge().ellipse()
    self.checkPointOnEllipse(aLine.endPoint(), anEllipse)
    self.checkPointsEqual(aLine.endPoint(), self.myEllipse1.endPoint())
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 3)
    model.testNbSubFeatures(self.mySketch, "SketchEllipticArc", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)

  def test_elliptic_arc_with_end_point_on_line(self):
    """ Test 12. Create elliptic arc which end point is placed on a line.
    """
    aLine = self.mySketch.addLine(10, 10, 30, 40)
    self.myDOF += 4
    model.do()

    self.mySketch.addEllipticArc(self.myCenter, self.myFocus, self.myStartPoint, [self.myEndPoint, aLine.result()], False)
    self.myDOF += 6
    model.do()

    anEllipseFeature = model.lastSubFeature(self.mySketch, "SketchEllipticArc")
    self.myEllipse1 = SketchAPI_EllipticArc(anEllipseFeature)
    anEllipse = anEllipseFeature.lastResult().shape().edge().ellipse()
    self.checkPointOnLine(self.myEllipse1.endPoint(), aLine)
    # check number of features
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 3)
    model.testNbSubFeatures(self.mySketch, "SketchEllipticArc", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", 1)


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    assert model.checkPythonDump()
