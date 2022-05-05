# Copyright (C) 2019-2022  CEA/DEN, EDF R&D
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
    Test constraint coincidence applied for ellipse and its sub-results
"""

import unittest
import math

from salome.shaper import model

from GeomAPI import *
from SketchAPI import *

__updated__ = "2019-09-12"

class TestCoincidenceEllipse(unittest.TestCase):
  def setUp(self):
    axisStart = GeomAPI_Pnt2d(30., 60.)
    axisEnd = GeomAPI_Pnt2d(80., 50.)
    passedPoint = GeomAPI_Pnt2d(60., 60.)

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
    self.myNbCoindicences = 1

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
      model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", self.myNbCoindicences)


  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

  def checkPointFixing(self, thePoint):
    self.mySketch.setCoincident(thePoint, self.myOrigin.coordinates())
    self.myDOF -= 2
    model.do()
    if not self.myExpectFailure:
      self.assertPoints(thePoint, self.myOrigin.coordinates())
      self.assertGreater(self.myEllipse.majorRadius().value(), 0.0)
      self.assertGreater(self.myEllipse.minorRadius().value(), 0.0)

  def assertPoints(self, thePoint1, thePoint2):
    self.assertAlmostEqual(thePoint1.x(), thePoint2.x(), delta = 1.e-6)
    self.assertAlmostEqual(thePoint1.y(), thePoint2.y(), delta = 1.e-6)

  def checkPointOnAxis(self, thePoint):
    self.mySketch.setCoincident(thePoint, self.myOX.result())
    self.myDOF -= 1
    model.do()
    if not self.myExpectFailure:
      self.assertAlmostEqual(thePoint.y(), 0.0, delta = 1.e-6)
      self.assertGreater(self.myEllipse.majorRadius().value(), 0.0)
      self.assertGreater(self.myEllipse.minorRadius().value(), 0.0)

  def checkPointOnLine(self, thePoint, theLineStart, theLineEnd):
    vecP = [thePoint.x() - theLineStart.x(), thePoint.y() - theLineStart.y()]
    vecL = [theLineEnd.x() - theLineStart.x(), theLineEnd.y() - theLineStart.y()]
    dist = math.fabs(vecP[0] * vecL[1] - vecP[1] * vecL[0]) / math.hypot(vecL[0], vecL[1])

    self.assertAlmostEqual(dist, 0.0, delta = 1.e-6)
    self.assertGreater(self.myEllipse.majorRadius().value(), 0.0)
    self.assertGreater(self.myEllipse.minorRadius().value(), 0.0)

  def checkPointOnEllipse(self, thePoint, theEllipse):
    firstFocus2d = GeomAPI_Pnt2d(theEllipse.firstFocus().x(), theEllipse.firstFocus().y())
    distPF1 = model.distancePointPoint(firstFocus2d,  thePoint)
    secondFocus2d = GeomAPI_Pnt2d(theEllipse.secondFocus().x(), theEllipse.secondFocus().y())
    distPF2 = model.distancePointPoint(secondFocus2d,  thePoint)
    self.assertAlmostEqual(distPF1 + distPF2, 2.0 * theEllipse.majorRadius().value(), 7)


  def test_concident_center(self):
    """ Test 1. Make center of ellipse coincident with the Origin
    """
    self.checkPointFixing(self.myCenter.coordinates())

  def test_coincident_first_focus(self):
    """ Test 2. Make first focus of ellipse coincident with the Origin
    """
    self.checkPointFixing(self.myFocus1.coordinates())

  def test_coincident_second_focus(self):
    """ Test 3. Make second focus of ellipse coincident with the Origin
    """
    self.checkPointFixing(self.myFocus2.coordinates())

  def test_coincident_major_axis_start(self):
    """ Test 4. Make start point on the major axis of ellipse coincident with the Origin
    """
    self.checkPointFixing(self.myMajorStart.coordinates())

  def test_coincident_major_axis_end(self):
    """ Test 5. Make end point on the major axis of ellipse coincident with the Origin
    """
    self.checkPointFixing(self.myMajorEnd.coordinates())

  def test_coincident_minor_axis_start(self):
    """ Test 6. Make start point on the minor axis of ellipse coincident with the Origin
    """
    self.checkPointFixing(self.myMinorStart.coordinates())
    # workaround: kill the constraint to avoid instability on dump check
    model.end()
    model.undo()
    model.begin()
    self.myDOF += 2
    self.myNbCoindicences -= 1

  def test_coincident_minor_axis_end(self):
    """ Test 7. Make end point on the minor axis of ellipse coincident with the Origin.
                Check solver is failed to compute the coincidence.
    """
    self.myExpectFailure = True
    self.checkPointFixing(self.myMinorEnd.coordinates())


  def test_center_on_line(self):
    """ Test 8. Make center of ellipse coincident with the OX
    """
    self.checkPointOnAxis(self.myCenter.coordinates())

  def test_first_focus_on_line(self):
    """ Test 9. Make first focus of ellipse coincident with the OX
    """
    self.checkPointOnAxis(self.myFocus1.coordinates())

  def test_second_focus_on_line(self):
    """ Test 10. Make second focus of ellipse coincident with the OX
    """
    self.checkPointOnAxis(self.myFocus2.coordinates())

  def test_major_axis_start_on_line(self):
    """ Test 11. Make start point on the major axis of ellipse coincident with the OX
    """
    self.checkPointOnAxis(self.myMajorStart.coordinates())

  def test_major_axis_end_on_line(self):
    """ Test 12. Make end point on the major axis of ellipse coincident with the OX
    """
    self.checkPointOnAxis(self.myMajorEnd.coordinates())

  def test_minor_axis_start_on_line(self):
    """ Test 13. Make start point on the minor axis of ellipse coincident with the OX
    """
    self.checkPointOnAxis(self.myMinorStart.coordinates())

  def test_minor_axis_end_on_line(self):
    """ Test 14. Make end point on the minor axis of ellipse coincident with the OX
    """
    self.myExpectFailure = True
    self.checkPointOnAxis(self.myMinorEnd.coordinates())


  def test_origin_on_major_axis(self):
    """ Test 15. Make origin coincident with the major axis of the ellipse
    """
    self.mySketch.setCoincident(self.myMajorAxis.result(), self.myOrigin.coordinates())
    self.myDOF -= 1
    model.do()
    self.checkPointOnLine(self.myOrigin.coordinates(), self.myMajorStart.coordinates(), self.myMajorEnd.coordinates())

  def test_origin_on_minor_axis(self):
    """ Test 16. Make origin coincident with the minor axis of the ellipse
    """
    self.mySketch.setCoincident(self.myMinorAxis.result(), self.myOrigin.coordinates())
    self.myDOF -= 1
    model.end()
    # solver shows wrong result
    assert(self.mySketch.solverError() != ""), "PlaneGCS limitation: if you see this message, then PlaneGCS has solved the set of constraints correctly"
    model.undo()

    # move ellipse and set coincidence once again
    model.begin()
    self.mySketch.move(self.myMinorStart, 20, 10)
    model.do()
    self.mySketch.setCoincident(self.myMinorAxis.results()[-1], self.myOrigin.coordinates())
    model.do()
    self.checkPointOnLine(self.myOrigin.coordinates(), self.myMinorStart.coordinates(), self.myMinorEnd.coordinates())


  def test_origin_on_ellipse(self):
    """ Test 17. Make origin coincident with the ellipse
    """
    self.mySketch.setCoincident(self.myEllipse.results()[-1], self.myOrigin.coordinates())
    self.myDOF -= 1
    model.do()
    self.checkPointOnEllipse(self.myOrigin.coordinates(), self.myEllipse)


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    assert model.checkPythonDump()
