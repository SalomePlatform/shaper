# Copyright (C) 2017-2026  CEA, EDF
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
    Test middle point on an arc - moving the start/end/center of the arc
    NOTE: when rotating the start point in 5 deg steps clockwise and reaching 160 deg,
    the MiddlePoint constraint sometimes fails and turns this test unstable.
"""

import unittest
import math

from salome.shaper import model
from GeomAPI import GeomAPI_Dir2d

__updated__ = "2019-09-03"

class TestMiddlePointOnArc(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myTestPassed = True
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myArc = self.mySketch.addArc(50, 50, 70, 50, 50, 70, False)
    self.myLine = self.mySketch.addLine(55, 60, 50, 0)
    self.myDOF = 9
    model.do()
    self.checkDOF()

  def tearDown(self):
    if self.myTestPassed:
      model.assertArcValidity(self.myArc)
      self.checkMiddlePoint(self.myLine.startPoint().pnt(), self.myArc)
      self.checkDOF()
    model.end()
    assert(model.checkPythonDump())

  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

  def checkMiddlePoint(self, thePoint, theArc):
    self.myTestPassed = False
    # check point on arc
    dist = thePoint.distance(theArc.center().pnt())
    NB_DIGITS = 7 - math.floor(math.log10(theArc.radius().value()))
    self.assertAlmostEqual(dist, theArc.radius().value(), NB_DIGITS)
    # check middle point
    dirPC = GeomAPI_Dir2d(thePoint.x() - theArc.center().x(),
                          thePoint.y() - theArc.center().y())
    dirSC = GeomAPI_Dir2d(theArc.startPoint().x() - theArc.center().x(),
                          theArc.startPoint().y() - theArc.center().y())
    dirEC = GeomAPI_Dir2d(theArc.endPoint().x() - theArc.center().x(),
                          theArc.endPoint().y() - theArc.center().y())
    angleSP = dirSC.angle(dirPC)
    anglePE = dirPC.angle(dirEC)
    self.assertAlmostEqual(angleSP, anglePE)
    self.assertEqual(angleSP < 0, theArc.reversed().value())
    self.myTestPassed = True

  def checkMiddlePoint_moveStart(self, thePoint, theArc, angleMoved):
    self.myTestPassed = False
    # check point on arc
    dist = thePoint.distance(theArc.center().pnt())
    NB_DIGITS = 7 - math.floor(math.log10(theArc.radius().value()))
    self.assertAlmostEqual(dist, theArc.radius().value(), NB_DIGITS)
    # check middle point
    dirPC = GeomAPI_Dir2d(thePoint.x() - theArc.center().x(),
                          thePoint.y() - theArc.center().y())
    dirSC = GeomAPI_Dir2d(theArc.startPoint().x() - theArc.center().x(),
                          theArc.startPoint().y() - theArc.center().y())
    dirEC = GeomAPI_Dir2d(theArc.endPoint().x() - theArc.center().x(),
                          theArc.endPoint().y() - theArc.center().y())
    angleSP = dirSC.angle(dirPC)
    anglePE = dirPC.angle(dirEC)
    print(f"angleMoved={angleMoved}")
    # C = theArc.center()
    # S = theArc.startPoint()
    # E = theArc.endPoint()
    # LS = self.myLine.startPoint()
    # LE = self.myLine.endPoint()
    # print(f"angleSP={angleSP}, anglePE={anglePE},  diff={math.fabs(angleSP-anglePE)},  C=({C.x():.7f},{C.y():.7f}), S=({S.x():.7f},{S.y():.7f}), E=({E.x():.7f},{E.y():.7f}), M=({thePoint.x():.7f},{thePoint.y():.7f}), LS=({LS.x():.7f},{LS.y():.7f}), LE=({LE.x():.7f},{LE.y():.7f})")
    # if math.fabs(angleMoved-2.7925268031909254) < 1.e-7 and math.fabs(angleSP-anglePE) > 3.0:
    #   # It was observed that the middle point constraint sometime fails when the START point is
    #   # rotated by 160 deg (~2.7925 rad).
    #   # => So let's catch this case and print a warning but let the test pass.
    #   print(f"WARNING: CheckMiddlePoint FAILED: angleSP: {angleSP}, anglePE: {anglePE},  diff: {math.fabs(angleSP-anglePE)} ... ignoring FAILURE")
    # else:
    #   self.assertAlmostEqual(angleSP, anglePE)
    #   self.assertEqual(angleSP < 0, theArc.reversed().value())
    self.assertAlmostEqual(angleSP, anglePE)
    self.assertEqual(angleSP < 0, theArc.reversed().value())
    self.myTestPassed = True

  def rotatePoint(self, thePoint, theCenter, theAngle):
    dirX = thePoint.x() - theCenter.x()
    dirY = thePoint.y() - theCenter.y()
    newX = theCenter.x() + dirX * math.cos(theAngle) - dirY * math.sin(theAngle)
    newY = theCenter.y() + dirX * math.sin(theAngle) + dirY * math.cos(theAngle)
    self.mySketch.move(thePoint, newX, newY)

  def moveArc(self):
    ANGLE_STEP = math.pi * 5.0 / 180.0
    ANGLE_THRESHOLD = math.pi
    # move start point of the arc clockwise
    print(f"moveArc: move START point of arc clockwise:")
    fullAngle = 0.0
    while fullAngle < ANGLE_THRESHOLD:
      self.rotatePoint(self.myArc.startPoint(), self.myArc.center(), -ANGLE_STEP)
      model.do()
      self.checkMiddlePoint_moveStart(self.myLine.startPoint().pnt(), self.myArc, fullAngle)
      fullAngle += ANGLE_STEP
    # move start point of the arc conterclockwise
    print(f"moveArc: move START point of arc counterclockwise:")
    fullAngle = 0.0
    while fullAngle < ANGLE_THRESHOLD:
      self.rotatePoint(self.myArc.startPoint(), self.myArc.center(), ANGLE_STEP)
      model.do()
      self.checkMiddlePoint(self.myLine.startPoint().pnt(), self.myArc)
      fullAngle += ANGLE_STEP

    # move end point of the arc clockwise
    print(f"moveArc: move END point of arc clockwise:")
    fullAngle = 0.0
    while fullAngle < ANGLE_THRESHOLD:
      self.rotatePoint(self.myArc.endPoint(), self.myArc.center(), -ANGLE_STEP)
      model.do()
      self.checkMiddlePoint(self.myLine.startPoint().pnt(), self.myArc)
      fullAngle += ANGLE_STEP
    # move end point of the arc conterclockwise
    print(f"moveArc: move END point of arc counterclockwise:")
    fullAngle = 0.0
    while fullAngle < ANGLE_THRESHOLD:
      self.rotatePoint(self.myArc.endPoint(), self.myArc.center(), ANGLE_STEP)
      model.do()
      self.checkMiddlePoint(self.myLine.startPoint().pnt(), self.myArc)
      fullAngle += ANGLE_STEP

    # move center of the arc
    print(f"moveArc: move CENTER point of arc along positive diagonal:")
    DELTA = [1.0, 1.0]
    for i in range(0, 40):
      if i == 10 or i == 30:
        DELTA = [-DELTA[0], -DELTA[1]]
      self.mySketch.move(self.myArc.center(), self.myArc.center().x() + DELTA[0], self.myArc.center().y() + DELTA[1])
      model.do()
      self.checkMiddlePoint(self.myLine.startPoint().pnt(), self.myArc)
    print(f"moveArc: move CENTER point of arc along negative diagonal")
    DELTA = [-1.0, 1.0]
    for i in range(0, 40):
      if i == 10 or i == 30:
        DELTA = [-DELTA[0], -DELTA[1]]
      self.mySketch.move(self.myArc.center(), self.myArc.center().x() + DELTA[0], self.myArc.center().y() + DELTA[1])
      model.do()
      self.checkMiddlePoint(self.myLine.startPoint().pnt(), self.myArc)


  def test_middle_point_coincidence_move_arc(self):
    """ Test 7. Set coincidence and middle point constraint and move arc
    """
    self.mySketch.setCoincident(self.myLine.startPoint(), self.myArc.results()[1])
    model.do()
    self.mySketch.setMiddlePoint(self.myLine.startPoint(), self.myArc.results()[1])
    self.myDOF -= 2
    model.do()
    self.checkMiddlePoint(self.myLine.startPoint().pnt(), self.myArc)
    self.moveArc()

if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
