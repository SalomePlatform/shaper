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
    Test the zero value of the constraint "Distance"
"""

import unittest
import math

from salome.shaper import model
from SketchAPI import *

__updated__ = "2019-10-22"
TOLERANCE = 1.e-6

class TestZeroDistance(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myLine1 = self.mySketch.addLine(10, 10, 45, 27.5)
    self.myLine2 = self.mySketch.addLine(20, 15, 30, 40)
    self.myLine3 = self.mySketch.addLine(10, 0, 10, 10)
    model.do()
    self.myDOF = 12

  def tearDown(self):
    model.end()
    model.checkSketch(self.mySketch, self.myDOF)

  def assertDistance(self, theObject1, theObject2, theDistance, isSigned = False):
    dist = -1.
    if issubclass(type(theObject1), SketchAPI_SketchEntity):
      if isSigned:
        dist = model.signedDistancePointLine(theObject2, theObject1)
      else:
        dist = model.distancePointLine(theObject2, theObject1)
    elif issubclass(type(theObject2), SketchAPI_SketchEntity):
      if isSigned:
        dist = model.signedDistancePointLine(theObject1, theObject2)
      else:
        dist = model.distancePointLine(theObject1, theObject2)
    else:
      dist = model.distancePointPoint(theObject1, theObject2)
    self.assertTrue(math.fabs(dist - theDistance) < TOLERANCE, "Current distance = {}, reference = {}".format(dist, theDistance))
    model.checkSketch(self.mySketch, self.myDOF)


  def test_distance_points_nzznz(self):
    """ Test 1. Change point-point distance from non-zero to zero and back to non-zero
    """
    dist = self.mySketch.setDistance(self.myLine1.endPoint(), self.myLine2.endPoint(), 20)
    self.myDOF -= 1
    model.do()
    self.assertDistance(self.myLine1.endPoint(), self.myLine2.endPoint(), 20)

    SketchAPI_Constraint(dist).setValue(0)
    self.myDOF -= 1
    model.do()
    self.assertDistance(self.myLine1.endPoint(), self.myLine2.endPoint(), 0)

    SketchAPI_Constraint(dist).setValue(20)
    self.myDOF += 1
    model.do()
    self.assertDistance(self.myLine1.endPoint(), self.myLine2.endPoint(), 20)

  def test_distance_points_znzz(self):
    """ Test 2. Change point-point distance from zero to non-zero and back to zero
    """
    dist = self.mySketch.setDistance(self.myLine1.startPoint(), self.myLine3.endPoint(), 0)
    self.myDOF -= 2
    model.do()
    self.assertDistance(self.myLine1.startPoint(), self.myLine3.endPoint(), 0)

    SketchAPI_Constraint(dist).setValue(10)
    self.myDOF += 1
    model.do()
    self.assertDistance(self.myLine1.startPoint(), self.myLine3.endPoint(), 10)

    SketchAPI_Constraint(dist).setValue(0)
    self.myDOF -= 1
    model.do()
    self.assertDistance(self.myLine1.startPoint(), self.myLine3.endPoint(), 0)


  def test_unsigned_distance_nzznz(self):
    """ Test 3. Change unsigned point-line distance from non-zero to zero and back to non-zero
    """
    dist = self.mySketch.setDistance(self.myLine1.result(), self.myLine2.endPoint(), 20)
    self.myDOF -= 1
    model.do()
    self.assertDistance(self.myLine1, self.myLine2.endPoint(), 20)

    SketchAPI_Constraint(dist).setValue(0)
    model.do()
    self.assertDistance(self.myLine1, self.myLine2.endPoint(), 0)

    SketchAPI_Constraint(dist).setValue(20)
    model.do()
    self.assertDistance(self.myLine1, self.myLine2.endPoint(), 20)

  def test_unsigned_distance_znzz(self):
    """ Test 4. Change unsigned point-line distance from zero to non-zero and back to zero
    """
    dist = self.mySketch.setDistance(self.myLine2.startPoint(), self.myLine1.result(), 0)
    self.myDOF -= 1
    model.do()
    self.assertDistance(self.myLine2.startPoint(), self.myLine1, 0)

    SketchAPI_Constraint(dist).setValue(10)
    model.do()
    self.assertDistance(self.myLine2.startPoint(), self.myLine1, 10)

    SketchAPI_Constraint(dist).setValue(0)
    model.do()
    self.assertDistance(self.myLine2.startPoint(), self.myLine1, 0)


  def test_signed_distance_nzznz(self):
    """ Test 5. Change signed point-line distance from non-zero to zero and back to non-zero
    """
    dist = self.mySketch.setDistance(self.myLine1.result(), self.myLine2.endPoint(), 20, True)
    self.myDOF -= 1
    model.do()
    self.assertDistance(self.myLine1, self.myLine2.endPoint(), -20, True)

    SketchAPI_Constraint(dist).setValue(0)
    model.do()
    self.assertDistance(self.myLine1, self.myLine2.endPoint(), 0, True)

    SketchAPI_Constraint(dist).setValue(20)
    model.do()
    self.assertDistance(self.myLine1, self.myLine2.endPoint(), -20, True)

  def test_signed_distance_nzznz_2(self):
    """ Test 6. Change signed point-line distance from non-zero to zero and back to non-zero
    """
    dist = self.mySketch.setDistance(self.myLine3.startPoint(), self.myLine1.result(), 10, True)
    self.myDOF -= 1
    model.do()
    self.assertDistance(self.myLine1, self.myLine3.startPoint(), 10, True)

    SketchAPI_Constraint(dist).setValue(0)
    model.do()
    self.assertDistance(self.myLine1, self.myLine3.startPoint(), 0, True)

    SketchAPI_Constraint(dist).setValue(20)
    model.do()
    self.assertDistance(self.myLine1, self.myLine3.startPoint(), 20, True)

  def test_signed_distance_znzz(self):
    """ Test 7. Change signed point-line distance from zero to non-zero and back to zero
    """
    dist = self.mySketch.setDistance(self.myLine2.startPoint(), self.myLine1.result(), 0, True)
    self.myDOF -= 1
    model.do()
    self.assertDistance(self.myLine2.startPoint(), self.myLine1, 0)

    SketchAPI_Constraint(dist).setValue(10)
    model.do()
    self.assertDistance(self.myLine2.startPoint(), self.myLine1, 10)

    SketchAPI_Constraint(dist).setValue(0)
    model.do()
    self.assertDistance(self.myLine2.startPoint(), self.myLine1, 0)


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    assert model.checkPythonDump()
