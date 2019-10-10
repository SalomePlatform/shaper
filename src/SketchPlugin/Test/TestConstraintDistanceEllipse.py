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
    Test constraint "Distance" applied sub-elements of an ellipse
"""

import unittest
import math

from salome.shaper import model

from GeomAPI import *
from SketchAPI import *

__updated__ = "2019-09-12"
TOLERANCE = 1.e-7

class TestDistanceEllipse(unittest.TestCase):
  def setUp(self):
    axisStart = GeomAPI_Pnt2d(20., 60.)
    axisEnd = GeomAPI_Pnt2d(80., 50.)
    passedPoint = GeomAPI_Pnt2d(60., 70.)

    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    macroEllipse = self.mySketch.addEllipse(axisStart, axisEnd, passedPoint, False)
    self.myDOF = 5
    self.myOrigin = self.mySketch.addPoint("Origin")
    self.myOX = self.mySketch.addLine("OX")
    model.do()
    self.myEllipse = SketchAPI_Ellipse(model.lastSubFeature(self.mySketch, "SketchEllipse"))
    self.myCenter = macroEllipse.center()
    self.myFocus1 = macroEllipse.focus1()
    self.myFocus2 = macroEllipse.focus2()
    self.myMajorAxis = macroEllipse.majorAxis()
    self.myMajorStart = macroEllipse.majorAxisStart()
    self.myMajorEnd = macroEllipse.majorAxisEnd()
    self.myMinorAxis = macroEllipse.minorAxis()
    self.myMinorStart = macroEllipse.minorAxisStart()
    self.myMinorEnd = macroEllipse.minorAxisEnd()
    self.myExpectFailure = False
    self.myDistance = 50

  def tearDown(self):
    model.end()
    if self.myExpectFailure:
      assert(self.mySketch.solverError() != ""), "PlaneGCS limitation: if you see this message, then PlaneGCS has solved the set of constraints correctly"
      model.undo()
    else:
      self.checkDOF()
      self.assertPoints(self.myCenter.coordinates(), self.myEllipse.center())
      self.assertPoints(self.myFocus1.coordinates(), self.myEllipse.firstFocus())
      self.assertPoints(self.myFocus2.coordinates(), self.myEllipse.secondFocus())
      self.assertPoints(self.myMajorStart.coordinates(), self.myEllipse.majorAxisNegative())
      self.assertPoints(self.myMajorEnd.coordinates(), self.myEllipse.majorAxisPositive())
      self.assertPoints(self.myMajorAxis.startPoint(), self.myEllipse.majorAxisNegative())
      self.assertPoints(self.myMajorAxis.endPoint(), self.myEllipse.majorAxisPositive())
      self.assertPoints(self.myMinorStart.coordinates(), self.myEllipse.minorAxisNegative())
      self.assertPoints(self.myMinorEnd.coordinates(), self.myEllipse.minorAxisPositive())
      self.assertPoints(self.myMinorAxis.startPoint(), self.myEllipse.minorAxisNegative())
      self.assertPoints(self.myMinorAxis.endPoint(), self.myEllipse.minorAxisPositive())
      model.testNbSubFeatures(self.mySketch, "SketchPoint", 8)
      model.testNbSubFeatures(self.mySketch, "SketchLine", 3)
      model.testNbSubFeatures(self.mySketch, "SketchEllipse", 1)
      model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidenceInternal", 11)
      model.testNbSubFeatures(self.mySketch, "SketchConstraintDistance", 1)


  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

  def checkPointPointDistance(self, thePoint1, thePoint2):
    self.mySketch.setDistance(thePoint1, thePoint2, self.myDistance)
    self.myDOF -= 1
    model.do()
    if not self.myExpectFailure:
      NB_DIGITS = math.floor(-math.log10(TOLERANCE) - math.log10(self.myDistance))
      self.assertAlmostEqual(model.distancePointPoint(thePoint1, thePoint2), self.myDistance, NB_DIGITS)
      self.assertGreater(self.myEllipse.majorRadius().value(), 0.0)
      self.assertGreater(self.myEllipse.minorRadius().value(), 0.0)

  def checkPointLineDistance(self, thePoint, theLine):
    self.mySketch.setDistance(thePoint, theLine.result(), self.myDistance)
    self.myDOF -= 1
    model.do()
    NB_DIGITS = math.floor(-math.log10(TOLERANCE) - math.log10(self.myDistance))
    self.assertAlmostEqual(model.distancePointLine(thePoint, theLine), self.myDistance, NB_DIGITS)
    self.assertGreater(self.myEllipse.majorRadius().value(), 0.0)
    self.assertGreater(self.myEllipse.minorRadius().value(), 0.0)

  def assertPoints(self, thePoint1, thePoint2):
    NB_DIGITS = math.floor(-math.log10(TOLERANCE) - math.log10(self.myDistance))
    self.assertAlmostEqual(thePoint1.x(), thePoint2.x(), NB_DIGITS)
    self.assertAlmostEqual(thePoint1.y(), thePoint2.y(), NB_DIGITS)


  def test_distance_center(self):
    """ Test 1. Set distance to the Origin from the center of ellipse
    """
    self.checkPointPointDistance(self.myCenter.coordinates(), self.myOrigin.coordinates())

  def test_distance_first_focus(self):
    """ Test 2. Set distance to the Origin from the first focus of ellipse
    """
    self.checkPointPointDistance(self.myFocus1.coordinates(), self.myOrigin.coordinates())

  def test_distance_second_focus(self):
    """ Test 3. Set distance to the Origin from the second focus of ellipse
    """
    self.checkPointPointDistance(self.myFocus2.coordinates(), self.myOrigin.coordinates())

  def test_distance_major_axis_start(self):
    """ Test 4. Set distance to the Origin from the start point on the major axis of ellipse
    """
    self.checkPointPointDistance(self.myMajorStart.coordinates(), self.myOrigin.coordinates())

  def test_distance_major_axis_end(self):
    """ Test 5. Set distance to the Origin from the end point on the major axis of ellipse
    """
    self.checkPointPointDistance(self.myMajorEnd.coordinates(), self.myOrigin.coordinates())

  def test_distance_minor_axis_start(self):
    """ Test 6. Set distance to the Origin from the start point on the minor axis of ellipse
    """
    self.checkPointPointDistance(self.myMinorStart.coordinates(), self.myOrigin.coordinates())

  def test_distance_minor_axis_end(self):
    """ Test 7. Set distance to the Origin from the end point on the minor axis of ellipse
    """
    self.myExpectFailure = True
    self.checkPointPointDistance(self.myMinorEnd.coordinates(), self.myOrigin.coordinates())


  def test_distance_center_to_line(self):
    """ Test 8. Set distance from theOX to the center of ellipse
    """
    self.checkPointLineDistance(self.myCenter.coordinates(), self.myOX)

  def test_distance_first_focus_to_line(self):
    """ Test 9. Set distance from theOX to the first focus of ellipse
    """
    self.checkPointLineDistance(self.myFocus1.coordinates(), self.myOX)

  def test_distance_second_focus_to_line(self):
    """ Test 10. Set distance from theOX to the second focus of ellipse
    """
    self.checkPointLineDistance(self.myFocus2.coordinates(), self.myOX)

  def test_distance_major_axis_start_to_line(self):
    """ Test 11. Set distance from theOX to the start point on the major axis of ellipse
    """
    self.checkPointLineDistance(self.myMajorStart.coordinates(), self.myOX)

  def test_distance_major_axis_end_to_line(self):
    """ Test 12. Set distance from theOX to the end point on the major axis of ellipse
    """
    self.checkPointLineDistance(self.myMajorEnd.coordinates(), self.myOX)

  def test_distance_minor_axis_start_to_line(self):
    """ Test 13. Set distance from theOX to the start point on the minor axis of ellipse
    """
    self.checkPointLineDistance(self.myMinorStart.coordinates(), self.myOX)

  def test_distance_minor_axis_end_to_line(self):
    """ Test 14. Set distance from theOX to the end point on the minor axis of ellipse
    """
    self.myDistance = 150
    self.checkPointLineDistance(self.myMinorEnd.coordinates(), self.myOX)


  def test_distance_origin_to_major_axis(self):
    """ Test 15. Set distance from the Origin to the major axis of the ellipse
    """
    self.checkPointLineDistance(self.myOrigin.coordinates(), self.myMajorAxis)

  def test_distance_origin_to_minor_axis(self):
    """ Test 16. Set distance from the Origin to the minor axis of the ellipse
    """
    self.checkPointLineDistance(self.myOrigin.coordinates(), self.myMinorAxis)


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    assert model.checkPythonDump()
