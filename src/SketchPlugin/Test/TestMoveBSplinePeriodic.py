# Copyright (C) 2019-2023  CEA, EDF
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
    Test movement of the periodic B-spline curve
"""

import unittest
import math
from GeomAPI import *
from GeomDataAPI import geomDataAPI_Point2DArray
from SketchAPI import *
from salome.shaper import model

__updated__ = "2020-01-20"

class TestMoveBSpline(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myPoles = [GeomAPI_Pnt2d(20., 50.),
                    GeomAPI_Pnt2d(70., 70.),
                    GeomAPI_Pnt2d(80., 30.),
                    GeomAPI_Pnt2d(50., -10.),
                    GeomAPI_Pnt2d(90., -30.)]
    self.mySpline = self.mySketch.addSpline(poles = self.myPoles, periodic = True)
    self.myDOF = len(self.myPoles) * 2
    model.do()
    self.checkDOF()

  def tearDown(self):
    self.checkDOF()
    model.end()

  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

  def checkPointCoordinates(self, thePoint, theCoordinates):
    aCoord = []
    if issubclass(type(theCoordinates), GeomAPI_Pnt2d):
      aCoord = [theCoordinates.x(), theCoordinates.y()]
    else:
      aCoord = theCoordinates
    DIGITS = 7 - math.floor(math.log10(math.hypot(aCoord[0], aCoord[1])))
    self.assertAlmostEqual(thePoint.x(), aCoord[0], DIGITS)
    self.assertAlmostEqual(thePoint.y(), aCoord[1], DIGITS)

  def checkPoles(self, theBSpline, theCoordinates):
    poles = theBSpline.poles()
    for index, point in zip(range(0, len(theCoordinates)), theCoordinates):
      self.checkPointCoordinates(poles.pnt(index), point)

  def fixPoint(self, thePoint):
    self.mySketch.setFixed(thePoint)
    self.myDOF -= 2
    model.do()
    self.checkDOF()


  def test_move_free_bspline(self):
    """ Test 1. Movement of a free B-spline dragging the edge
    """
    oldPosition = GeomAPI_Edge(self.mySpline.defaultResult().shape()).middlePoint()
    newPosition = GeomAPI_Pnt2d(120., 90.)
    self.mySketch.move(self.mySpline.defaultResult(), newPosition)
    model.do()

    # plane is XOY, no need to project oldPosition point
    dx = newPosition.x() - oldPosition.x()
    dy = newPosition.y() - oldPosition.y()

    newPoles = []
    for pole in self.myPoles:
      newPoles.append(GeomAPI_Pnt2d(pole.x() + dx, pole.y() + dy))
    self.checkPoles(self.mySpline, newPoles)


  def test_move_fixed_bspline(self):
    """ Test 2. Movement of a fully fixed B-spline
    """
    self.mySketch.setFixed(self.mySpline.defaultResult())
    self.myDOF = 0
    model.do()

    newPosition = GeomAPI_Pnt2d(120., 90.)
    self.mySketch.move(self.mySpline.defaultResult(), newPosition)
    model.do()

    self.checkPoles(self.mySpline, self.myPoles)


  def test_move_bspline_with_fixed_pole(self):
    """ Test 3. Movement of a B-spline curve with fixed pole
    """
    [Point_1, Point_2, Point_3, Point_4, Point_5] = self.mySpline.controlPoles(auxiliary = [0, 1, 2, 3, 4])
    model.do()

    self.fixPoint(Point_2.defaultResult())
    model.do()

    oldPosition = GeomAPI_Edge(self.mySpline.defaultResult().shape()).middlePoint()
    newPosition = GeomAPI_Pnt2d(120., 90.)
    self.mySketch.move(self.mySpline.defaultResult(), newPosition)
    model.do()

    # plane is XOY, no need to project oldPosition point
    dx = newPosition.x() - oldPosition.x()
    dy = newPosition.y() - oldPosition.y()

    newPoles = []
    for pole in self.myPoles:
      newPoles.append(GeomAPI_Pnt2d(pole.x() + dx, pole.y() + dy))
    newPoles[1].setX(newPoles[1].x() - dx)
    newPoles[1].setY(newPoles[1].y() - dy)
    self.checkPoles(self.mySpline, newPoles)


  def test_move_bspline_with_fixed_segment(self):
    """ Test 4. Movement of a B-spline curve with fixed control segment
    """
    [Line_1, Line_2, Line_3, Line_4, Line_5] = self.mySpline.controlPolygon(auxiliary = [0, 1, 2, 3, 4])
    model.do()

    self.mySketch.setFixed(Line_1.defaultResult())
    self.myDOF -= 4
    model.do()

    oldPosition = GeomAPI_Edge(self.mySpline.defaultResult().shape()).middlePoint()
    newPosition = GeomAPI_Pnt2d(120., 90.)
    self.mySketch.move(self.mySpline.defaultResult(), newPosition)
    model.do()

    # plane is XOY, no need to project oldPosition point
    dx = newPosition.x() - oldPosition.x()
    dy = newPosition.y() - oldPosition.y()

    newPoles = [self.myPoles[0], self.myPoles[1]]
    for pole in self.myPoles[2:]:
      newPoles.append(GeomAPI_Pnt2d(pole.x() + dx, pole.y() + dy))
    self.checkPoles(self.mySpline, newPoles)


  def test_move_pole_of_free_bspline(self):
    """ Test 5. Movement of a pole of a B-spline curve
    """
    [Point_1, Point_2, Point_3, Point_4, Point_5] = self.mySpline.controlPoles(auxiliary = [0, 1, 2, 3, 4])
    [Line_1, Line_2, Line_3, Line_4, Line_5] = self.mySpline.controlPolygon(auxiliary = [0, 1, 2, 3, 4])
    model.do()

    newPoles = self.myPoles

    newPoles[2].setX(newPoles[2].x() + 20.)
    newPoles[2].setY(newPoles[2].y() + 20.)
    self.mySketch.move(SketchAPI_Point(Point_3).coordinates(), newPoles[2])
    model.do()

    self.checkPoles(self.mySpline, newPoles)

  def test_move_segment_of_free_bspline(self):
    """ Test 6. Movement of a control segment of a B-spline curve
    """
    [Point_1, Point_2, Point_3, Point_4, Point_5] = self.mySpline.controlPoles(auxiliary = [0, 1, 2, 3, 4])
    [Line_1, Line_2, Line_3, Line_4, Line_5] = self.mySpline.controlPolygon(auxiliary = [0, 1, 2, 3, 4])
    model.do()

    oldPosition = GeomAPI_Pnt2d(0.5 * (self.myPoles[2].x() + self.myPoles[3].x()),
                                0.5 * (self.myPoles[2].y() + self.myPoles[3].y()))
    newPosition = GeomAPI_Pnt2d(120., 90.)
    self.mySketch.move(SketchAPI_Line(Line_3).defaultResult(), newPosition)
    model.do()

    dx = newPosition.x() - oldPosition.x()
    dy = newPosition.y() - oldPosition.y()

    newPoles = self.myPoles
    newPoles[2].setX(newPoles[2].x() + dx)
    newPoles[2].setY(newPoles[2].y() + dy)
    newPoles[3].setX(newPoles[3].x() + dx)
    newPoles[3].setY(newPoles[3].y() + dy)

    self.checkPoles(self.mySpline, newPoles)



if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    assert model.checkPythonDump()
