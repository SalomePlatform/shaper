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
    Test the zero value of the constraint "DistanceVertical"
"""

import unittest
import math

from salome.shaper import model
from SketchAPI import *

__updated__ = "2019-10-22"
TOLERANCE = 1.e-6

class TestZeroDistanceVertical(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myLine1 = self.mySketch.addLine(10, 10, 45, 27.5)
    self.myLine2 = self.mySketch.addLine(20, 15, 30, 40)
    self.myLine3 = self.mySketch.addLine(0, 10, 10, 10)
    model.do()
    self.myDOF = 12

  def tearDown(self):
    model.end()
    model.checkSketch(self.mySketch, self.myDOF)

  def assertDistanceVertical(self, theObject1, theObject2, theDistance):
    dist = theObject2.y() - theObject1.y()
    self.assertTrue(math.fabs(dist - theDistance) < TOLERANCE, "Current distance = {}, reference = {}".format(dist, theDistance))
    model.checkSketch(self.mySketch, self.myDOF)


  def test_distance_positive_nzznz(self):
    """ Test 1. Change distance from non-zero to zero and back to non-zero
    """
    dist = self.mySketch.setVerticalDistance(self.myLine1.startPoint(), self.myLine2.startPoint(), 5)
    self.myDOF -= 1
    model.do()
    self.assertDistanceVertical(self.myLine1.startPoint(), self.myLine2.startPoint(), 5)

    SketchAPI_Constraint(dist).setValue(0)
    model.do()
    self.assertDistanceVertical(self.myLine1.startPoint(), self.myLine2.startPoint(), 0)

    SketchAPI_Constraint(dist).setValue(10)
    model.do()
    self.assertDistanceVertical(self.myLine1.startPoint(), self.myLine2.startPoint(), 10)

  def test_distance_negative_nzznz(self):
    """ Test 2. Change distance from non-zero to zero and back to non-zero
    """
    dist = self.mySketch.setVerticalDistance(self.myLine1.endPoint(), self.myLine2.startPoint(), 12.5)
    self.myDOF -= 1
    model.do()
    self.assertDistanceVertical(self.myLine1.endPoint(), self.myLine2.startPoint(), -12.5)

    SketchAPI_Constraint(dist).setValue(0)
    model.do()
    self.assertDistanceVertical(self.myLine1.endPoint(), self.myLine2.startPoint(), 0)

    SketchAPI_Constraint(dist).setValue(12.5)
    model.do()
    self.assertDistanceVertical(self.myLine1.endPoint(), self.myLine2.startPoint(), -12.5)

  def test_distance_equal_znzz(self):
    """ Test 3. Change distance from zero to non-zero and back to zero
    """
    dist = self.mySketch.setVerticalDistance(self.myLine1.startPoint(), self.myLine3.endPoint(), 0)
    self.myDOF -= 1
    model.do()
    self.assertDistanceVertical(self.myLine1.startPoint(), self.myLine3.endPoint(), 0)

    SketchAPI_Constraint(dist).setValue(10)
    model.do()
    self.assertDistanceVertical(self.myLine1.startPoint(), self.myLine3.endPoint(), 10)

    SketchAPI_Constraint(dist).setValue(0)
    model.do()
    self.assertDistanceVertical(self.myLine1.startPoint(), self.myLine3.endPoint(), 0)

  def test_distance_notequal_znzz(self):
    """ Test 4. Change distance from zero to non-zero and back to zero
    """
    dist = self.mySketch.setVerticalDistance(self.myLine1.startPoint(), self.myLine3.startPoint(), 0)
    self.myDOF -= 1
    model.do()
    self.assertDistanceVertical(self.myLine1.startPoint(), self.myLine3.startPoint(), 0)

    SketchAPI_Constraint(dist).setValue(10)
    model.do()
    self.assertDistanceVertical(self.myLine1.startPoint(), self.myLine3.startPoint(), 10)

    SketchAPI_Constraint(dist).setValue(0)
    model.do()
    self.assertDistanceVertical(self.myLine1.startPoint(), self.myLine3.startPoint(), 0)


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    assert model.checkPythonDump()
