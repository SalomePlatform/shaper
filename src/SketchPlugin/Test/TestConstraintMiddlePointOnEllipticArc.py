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
    Test middle point on an elliptic arc
"""

import unittest
import math

from salome.shaper import model
from GeomAPI import *
from SketchAPI import *

__updated__ = "2019-10-02"

class TestMiddlePointOnEllipticArc(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myTestPassed = True
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myArc = self.mySketch.addEllipticArc(30, 20, 50, 30, 45, 40, 5, 6.11485435, False)
    self.myLine = self.mySketch.addLine(10, 40, 40, 80)
    self.myDOF = 11
    model.do()
    self.checkDOF()

  def tearDown(self):
    if self.myTestPassed:
      self.assertArc(self.myArc)
      self.checkMiddlePoint(self.myLine.startPoint(), self.myArc)
      self.checkDOF()
    model.end()

  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

  def toPeriod(self, theValue):
    while theValue < -math.pi:
      theValue += 2.0 * math.pi
    while theValue >= math.pi:
      theValue -= 2.0 * math.pi
    return theValue

  def checkMiddlePoint(self, thePoint, theArc):
    self.myTestPassed = False
    anEllipse = theArc.defaultResult().shape().edge().ellipse()
    # check point on ellipse
    self.checkPointOnEllipse(thePoint, anEllipse)
    # check angles
    TOLERANCE = 1.e-5
    startPoint = GeomAPI_Pnt(theArc.startPoint().x(), theArc.startPoint().y(), 0)
    isCalculated, startAngle = anEllipse.parameter(startPoint, TOLERANCE)
    endPoint = GeomAPI_Pnt(theArc.endPoint().x(), theArc.endPoint().y(), 0)
    isCalculated, endAngle = anEllipse.parameter(endPoint, TOLERANCE)
    midPoint = GeomAPI_Pnt(thePoint.x(), thePoint.y(), 0)
    isCalculated, midAngle = anEllipse.parameter(midPoint, TOLERANCE)
    diffMS = self.toPeriod(midAngle - startAngle)
    diffEM = self.toPeriod(endAngle - midAngle)
    self.assertAlmostEqual(diffMS, diffEM)
    self.assertEqual(diffMS < 0, theArc.reversed().value())
    self.myTestPassed = True

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
    NB_DIGITS = 7 - math.floor(math.log10(majorRad))
    self.assertAlmostEqual(distPF1 + distPF2, 2.0 * majorRad, NB_DIGITS)

  def assertArc(self, theArc):
    anEllipse = theArc.defaultResult().shape().edge().ellipse()
    self.checkPointOnEllipse(theArc.startPoint(), anEllipse)
    self.checkPointOnEllipse(theArc.endPoint(), anEllipse)

  def rotatePoint(self, thePoint, theCenter, theAngle):
    dirX = thePoint.x() - theCenter.x()
    dirY = thePoint.y() - theCenter.y()
    newX = theCenter.x() + dirX * math.cos(theAngle) - dirY * math.sin(theAngle)
    newY = theCenter.y() + dirX * math.sin(theAngle) + dirY * math.cos(theAngle)
    self.mySketch.move(thePoint, newX, newY)

  def moveArc(self):
    ANGLE_STEP = math.pi * 5.0 / 180.0
    ANGLE_THRESHOLD = math.pi / 2.0
    # move start point of the arc clockwise
    fullAngle = 0.0
    while fullAngle < ANGLE_THRESHOLD:
      self.rotatePoint(self.myArc.startPoint(), self.myArc.center(), -ANGLE_STEP)
      model.do()
      self.checkMiddlePoint(self.myLine.startPoint(), self.myArc)
      fullAngle += ANGLE_STEP
    # move start point of the arc conterclockwise
    fullAngle = 0.0
    while fullAngle < ANGLE_THRESHOLD:
      self.rotatePoint(self.myArc.startPoint(), self.myArc.center(), ANGLE_STEP)
      model.do()
      self.checkMiddlePoint(self.myLine.startPoint(), self.myArc)
      fullAngle += ANGLE_STEP

    # move end point of the arc clockwise
    fullAngle = 0.0
    while fullAngle < ANGLE_THRESHOLD:
      self.rotatePoint(self.myArc.endPoint(), self.myArc.center(), -ANGLE_STEP)
      model.do()
      self.checkMiddlePoint(self.myLine.startPoint(), self.myArc)
      fullAngle += ANGLE_STEP
    # move end point of the arc conterclockwise
    fullAngle = 0.0
    while fullAngle < ANGLE_THRESHOLD:
      self.rotatePoint(self.myArc.endPoint(), self.myArc.center(), ANGLE_STEP)
      model.do()
      self.checkMiddlePoint(self.myLine.startPoint(), self.myArc)
      fullAngle += ANGLE_STEP

    # move center of the arc
    DELTA = [0.1, 0.1]
    for i in range(0, 40):
      if i == 10 or i == 30:
        DELTA = [-DELTA[0], -DELTA[1]]
      self.mySketch.move(self.myArc.center(), self.myArc.center().x() + DELTA[0], self.myArc.center().y() + DELTA[1])
      model.do()
      self.checkMiddlePoint(self.myLine.startPoint(), self.myArc)
    DELTA = [-0.1, 0.1]
    for i in range(0, 40):
      if i == 10 or i == 30:
        DELTA = [-DELTA[0], -DELTA[1]]
      self.mySketch.move(self.myArc.center(), self.myArc.center().x() + DELTA[0], self.myArc.center().y() + DELTA[1])
      model.do()
      self.checkMiddlePoint(self.myLine.startPoint(), self.myArc)

  def moveLine(self):
    DELTA = [0.1, 0.0]
    for i in range(0, 40):
      if i == 10 or i == 30:
        DELTA = [-DELTA[0], -DELTA[1]]
      self.mySketch.move(self.myLine.startPoint(), self.myLine.startPoint().x() + DELTA[0], self.myLine.startPoint().y() + DELTA[1])
      model.do()
      self.checkMiddlePoint(self.myLine.startPoint(), self.myArc)
    DELTA = [0.0, 0.1]
    for i in range(0, 40):
      if i == 10 or i == 30:
        DELTA = [-DELTA[0], -DELTA[1]]
      self.mySketch.move(self.myLine.startPoint(), self.myLine.startPoint().x() + DELTA[0], self.myLine.startPoint().y() + DELTA[1])
      model.do()
      self.checkMiddlePoint(self.myLine.startPoint(), self.myArc)


  def test_middle_point_PA(self):
    """ Test 1. Set middle point constraint (point is the first argument)
    """
    self.mySketch.setMiddlePoint(self.myLine.startPoint(), self.myArc.results()[-1])
    self.myDOF -= 2
    model.do()

  def test_middle_point_AP(self):
    """ Test 2. Set middle point constraint (point is the second argument)
    """
    self.mySketch.setMiddlePoint(self.myArc.results()[-1], self.myLine.startPoint())
    self.myDOF -= 2
    model.do()

  def test_coincident_middle_point(self):
    """ Test 3. Set middle point constraint for the point already coincident with the arc
    """
    self.mySketch.setCoincident(self.myLine.startPoint(), self.myArc.results()[-1])
    model.do()
    self.mySketch.setMiddlePoint(self.myLine.startPoint(), self.myArc.results()[-1])
    self.myDOF -= 2
    model.do()

  def test_middle_point_coincident(self):
    """ Test 4. Set concidence of the point and the arc which are already constrained with middle point
    """
    self.mySketch.setMiddlePoint(self.myLine.startPoint(), self.myArc.results()[-1])
    model.do()
    self.mySketch.setCoincident(self.myLine.startPoint(), self.myArc.results()[-1])
    self.myDOF -= 2
    model.do()

  @unittest.expectedFailure
  def test_middle_point_limitation(self):
    """ Test 5. Check middle point fails if the point's coordinates are equal to the arc start point
    """
    self.myLine.startPoint().setValue(self.myArc.startPoint().pnt())
    model.do()
    coincidence = self.mySketch.setCoincident(self.myLine.startPoint(), self.myArc.results()[-1])
    self.mySketch.setMiddlePoint(self.myLine.startPoint(), self.myArc.results()[-1])
    self.myDOF -= 2
    model.do()
    # this check will fail due to the limitation of PlanGCS
    self.checkMiddlePoint(self.myLine.startPoint(), self.myArc)

  def test_middle_point_equal_start(self):
    """ Test 6. Check middle point does not fail if the point's coordinates are equal to the arc end point
    """
    self.myLine.startPoint().setValue(self.myArc.endPoint().pnt())
    model.do()
    coincidence = self.mySketch.setCoincident(self.myLine.startPoint(), self.myArc.results()[-1])
    self.mySketch.setMiddlePoint(self.myLine.startPoint(), self.myArc.results()[-1])
    self.myDOF -= 2
    model.do()

  def test_middle_point_move_arc(self):
    """ Test 7. Set middle point constraint and move arc
    """
    self.mySketch.setMiddlePoint(self.myLine.startPoint(), self.myArc.results()[-1])
    self.myDOF -= 2
    model.do()
    self.checkMiddlePoint(self.myLine.startPoint(), self.myArc)
    self.moveArc()

  def test_middle_point_coincidence_move_arc(self):
    """ Test 8. Set coincidence and middle point constraint and move arc
    """
    self.mySketch.setCoincident(self.myLine.startPoint(), self.myArc.results()[-1])
    model.do()
    self.mySketch.setMiddlePoint(self.myLine.startPoint(), self.myArc.results()[-1])
    self.myDOF -= 2
    model.do()
    self.checkMiddlePoint(self.myLine.startPoint(), self.myArc)
    self.moveArc()

  def test_middle_point_move_line(self):
    """ Test 9. Set middle point constraint and move line
    """
    self.mySketch.setMiddlePoint(self.myLine.startPoint(), self.myArc.results()[-1])
    self.myDOF -= 2
    model.do()
    self.checkMiddlePoint(self.myLine.startPoint(), self.myArc)
    self.moveLine()

  def test_middle_point_coincidence_move_line(self):
    """ Test 10. Set coincidence and middle point constraint and move line
    """
    self.mySketch.setCoincident(self.myLine.startPoint(), self.myArc.results()[-1])
    model.do()
    self.mySketch.setMiddlePoint(self.myLine.startPoint(), self.myArc.results()[-1])
    self.myDOF -= 2
    model.do()
    self.checkMiddlePoint(self.myLine.startPoint(), self.myArc)
    self.moveLine()

  def test_remove_middle_point(self):
    """ Test 11. Set and then remove middle point constraint
    """
    mp = self.mySketch.setMiddlePoint(self.myLine.startPoint(), self.myArc.results()[-1])
    self.myDOF -= 2
    model.do()
    self.assertArc(self.myArc)
    self.checkMiddlePoint(self.myLine.startPoint(), self.myArc)
    self.checkDOF()
    # remove middle point
    self.myDocument.removeFeature(mp.feature())
    self.myDOF += 2
    model.do()
    self.checkDOF()
    # set flag False to avoid checking middle point constraint in tearDown() method
    self.myTestPassed = False


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    assert(model.checkPythonDump())
