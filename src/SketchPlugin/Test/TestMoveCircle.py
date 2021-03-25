# Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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
    Test movement of the sketch circle
"""

import unittest
from GeomDataAPI import geomDataAPI_Point2D
from salome.shaper import model

__updated__ = "2017-05-11"

class TestMoveCircle(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myCenter = [70., 50.]
    self.myRadius = 20.
    self.myCircle = self.mySketch.addCircle(self.myCenter[0], self.myCenter[1], self.myRadius)
    self.myDOF = 3
    model.do()
    self.checkDOF()

  def tearDown(self):
    self.checkDOF()
    model.end()
    assert(model.checkPythonDump())

  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

  def checkPointCoordinates(self, thePoint, theCoordinates):
    self.assertAlmostEqual(thePoint.x(), theCoordinates[0])
    self.assertAlmostEqual(thePoint.y(), theCoordinates[1])

  def checkPointOnCircle(self, theCoordinates):
    self.assertAlmostEqual(model.distancePointPoint(self.myCircle.center(),  theCoordinates), self.myCircle.radius().value())

  def fixCircleRadius(self):
    self.mySketch.setRadius(self.myCircle.results()[1], self.myRadius)
    self.myDOF -= 1
    model.do()
    self.checkDOF()

  def fixCircleCenter(self):
    self.mySketch.setFixed(self.myCircle.center())
    self.myDOF -= 2
    model.do()
    self.checkDOF()


  def test_move_center_free_circle(self):
    """ Test 1. Movement of central point of a free circle
    """
    newPosition = [self.myCenter[0] + 20., self.myCenter[1] + 10.]
    self.mySketch.move(self.myCircle.center(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myCircle.center(), newPosition)
    self.assertAlmostEqual(self.myCircle.radius().value(), self.myRadius)

  def test_move_free_circle(self):
    """ Test 2. Movement of a free circle dragging the edge
    """
    newPosition = [120., 90.]
    self.mySketch.move(self.myCircle.defaultResult(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myCircle.center(), self.myCenter)
    self.checkPointOnCircle(newPosition)
    self.assertNotEqual(self.myCircle.radius().value(), self.myRadius)

  def test_move_center_circle_fixed_radius(self):
    """ Test 3. Movement of central point of a circle with fixed radius
    """
    self.fixCircleRadius()

    newPosition = [self.myCenter[0] + 20., self.myCenter[1] + 10.]
    self.mySketch.move(self.myCircle.center(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myCircle.center(), newPosition)
    self.assertAlmostEqual(self.myCircle.radius().value(), self.myRadius)

  def test_move_circle_fixed_radius(self):
    """ Test 4. Movement of a circle with fixed radius
    """
    self.fixCircleRadius()

    newPosition = [120., 90.]
    self.mySketch.move(self.myCircle.defaultResult(), newPosition[0], newPosition[1])
    model.do()
    self.assertAlmostEqual(self.myCircle.radius().value(), self.myRadius)
    self.assertNotEqual(self.myCircle.center().x(), self.myCenter[0])
    self.assertNotEqual(self.myCircle.center().y(), self.myCenter[1])

  def test_move_center_circle_fixed_center(self):
    """ Test 5. Movement of central point of a circle with fixed center (nothing should be changed)
    """
    self.fixCircleCenter()

    newPosition = [self.myCenter[0] + 20., self.myCenter[1] + 10.]
    self.mySketch.move(self.myCircle.center(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myCircle.center(), self.myCenter)
    self.assertAlmostEqual(self.myCircle.radius().value(), self.myRadius)

  def test_move_circle_fixed_center(self):
    """ Test 6. Movement of a circle with fixed center
    """
    self.fixCircleCenter()

    newPosition = [120., 90.]
    self.mySketch.move(self.myCircle.defaultResult(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointOnCircle(newPosition)
    self.assertNotEqual(self.myCircle.radius().value(), self.myRadius)

  def test_move_fixed_circle(self):
    """ Test 7. Trying to move fully fixed circle
    """
    self.fixCircleCenter()
    self.fixCircleRadius()

    newPosition = [120., 90.]
    self.mySketch.move(self.myCircle.defaultResult(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myCircle.center(), self.myCenter)
    self.assertAlmostEqual(self.myCircle.radius().value(), self.myRadius)


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
