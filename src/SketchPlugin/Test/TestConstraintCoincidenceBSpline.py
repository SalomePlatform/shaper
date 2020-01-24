# Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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
    Test constraint coincidence applied for B-spline curve and its sub-results
"""

import unittest
import math

from salome.shaper import model

from GeomAPI import *
from SketchAPI import *

__updated__ = "2020-01-21"

class TestCoincidenceBSpline(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))

    self.myPoles = [GeomAPI_Pnt2d(-10, -30), GeomAPI_Pnt2d(20, -15), GeomAPI_Pnt2d(-10, 0), GeomAPI_Pnt2d(20, 15), GeomAPI_Pnt2d(-10, 30)]
    self.myWeights = [1, 3, 5, 3, 1]
    self.mySpline = self.mySketch.addSpline(poles = self.myPoles, weights = self.myWeights)
    self.myControlPoles = self.mySpline.controlPoles(auxiliary = [0, 1, 2, 3, 4])
    self.myControlLines = self.mySpline.controlPolygon(auxiliary = [0, 1, 2, 3])

    self.myDOF = len(self.myPoles) * 2
    self.myOrigin = self.mySketch.addPoint("Origin")
    self.myOX = self.mySketch.addLine("OX")
    model.do()
    self.myExpectFailure = False
    self.myNbPoints = len(self.myPoles) + 1
    self.myNbLines = len(self.myPoles)
    self.myNbBSplines = 1
    self.myNbInternalConstraints = len(self.myPoles) * 3 - 2
    self.myNbCoincidences = 1

  def tearDown(self):
    model.end()
    if self.myExpectFailure:
      assert(self.mySketch.solverError() != ""), "PlaneGCS limitation: if you see this message, then PlaneGCS has solved the set of constraints correctly"
      model.undo()
    else:
      self.checkDOF()
      model.testNbSubFeatures(self.mySketch, "SketchPoint", self.myNbPoints)
      model.testNbSubFeatures(self.mySketch, "SketchLine", self.myNbLines)
      model.testNbSubFeatures(self.mySketch, "SketchBSpline", self.myNbBSplines)
      model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidenceInternal", self.myNbInternalConstraints)
      model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", self.myNbCoincidences)


  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

  def setCoincidentWithOrigin(self, thePoint):
    self.mySketch.setCoincident(thePoint, self.myOrigin.coordinates())
    self.myDOF -= 2
    model.do()

  def setCoincidentWithOX(self, thePoint):
    self.mySketch.setCoincident(thePoint, self.myOX.result())
    self.myDOF -= 1
    model.do()

  def assertPoles(self):
    poles = self.mySpline.poles()
    assert(poles.size() == len(self.myPoles))
    for index in range(0, len(self.myPoles)):
      self.assertPoints(self.myPoles[index], poles.pnt(index))

  def assertPoints(self, thePoint1, thePoint2):
    self.assertAlmostEqual(thePoint1.x(), thePoint2.x())
    self.assertAlmostEqual(thePoint1.y(), thePoint2.y())

  def assertPointOnLine(self, thePoint, theLineStart, theLineEnd):
    vecP = [thePoint.x() - theLineStart.x(), thePoint.y() - theLineStart.y()]
    vecL = [theLineEnd.x() - theLineStart.x(), theLineEnd.y() - theLineStart.y()]
    dist = math.fabs(vecP[0] * vecL[1] - vecP[1] * vecL[0]) / math.hypot(vecL[0], vecL[1])
    self.assertAlmostEqual(dist, 0.0)

  def assertPointOnSpline(self, thePoint, theSpline):
    point = GeomAPI_Pnt(thePoint.x(), thePoint.y(), 0.0)
    bspline = GeomAPI_BSpline(GeomAPI_Curve(theSpline.results()[-1].resultSubShapePair()[0].shape()))
    proj = bspline.project(point)
    self.assertAlmostEqual(point.distance(proj), 0.0)


  def test_origin_equal_start_point(self):
    """ Test 1. Make start point of B-spline coincident with the Origin
    """
    self.setCoincidentWithOrigin(self.mySpline.startPoint())
    self.myPoles[0].setX(0)
    self.myPoles[0].setY(0)
    self.assertPoles()

  def test_origin_equal_end_point(self):
    """ Test 2. Make end point of B-spline coincident with the Origin
    """
    self.setCoincidentWithOrigin(self.mySpline.endPoint())
    self.myPoles[-1].setX(0)
    self.myPoles[-1].setY(0)
    self.assertPoles()

  def test_origin_equal_pole(self):
    """ Test 3. Make one of B-spline poles coincident with the Origin
    """
    self.setCoincidentWithOrigin(SketchAPI_Point(self.myControlPoles[1]).coordinates())
    self.myPoles[1].setX(0)
    self.myPoles[1].setY(0)
    self.assertPoles()

  def test_origin_on_bspline(self):
    """ Test 4. (expected failure) Make Origin lying on the B-spline curve
    """
    self.mySketch.setCoincident(self.mySpline.defaultResult(), self.myOrigin.coordinates())
    self.myDOF -= 1
    model.do()
    self.myExpectFailure = True

  def test_point_on_bspline(self):
    """ Test 5. Place free point on the B-spline curve
    """
    point = self.mySketch.addPoint(1, 0)
    self.mySketch.setCoincident(self.myOX.defaultResult(), point.coordinates())
    self.mySketch.setCoincident(self.mySpline.defaultResult(), point.coordinates())
    model.do()
    self.myNbPoints += 1
    self.myNbCoincidences += 1
    self.assertPointOnSpline(point.coordinates(), self.mySpline)


  def test_start_point_on_axis(self):
    """ Test 6. Make start point of B-spline coincident with the OX
    """
    self.setCoincidentWithOX(self.mySpline.startPoint())
    self.myPoles[0].setY(0)
    self.assertPoles()

  def test_end_point_on_axis(self):
    """ Test 7. Make end point of B-spline coincident with the OX
    """
    self.setCoincidentWithOX(self.mySpline.endPoint())
    self.myPoles[-1].setY(0)
    self.assertPoles()

  def test_pole_on_axis(self):
    """ Test 8. Make one of B-spline poles coincident with the OX
    """
    self.setCoincidentWithOX(SketchAPI_Point(self.myControlPoles[1]).coordinates())
    self.myPoles[1].setY(0)
    self.assertPoles()


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    assert model.checkPythonDump()
