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
    Test movement of the sketch line
"""

import unittest
from GeomDataAPI import geomDataAPI_Point2D
from salome.shaper import model

__updated__ = "2017-05-11"

class TestMoveLine(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myStart = [70., 50.]
    self.myEnd = [100., 20.]
    self.myLine = self.mySketch.addLine(self.myStart[0], self.myStart[1], self.myEnd[0], self.myEnd[1])
    self.myDOF = 4
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

  def checkPointOnLine(self, theCoordinates):
    self.assertAlmostEqual(model.distancePointLine(theCoordinates, self.myLine), 0.)

  def test_move_start_of_free_line(self):
    """ Test 1. Movement of start point of a free line
    """
    newPosition = [self.myStart[0] + 20., self.myStart[1] + 10.]
    self.mySketch.move(self.myLine.startPoint(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myLine.startPoint(), newPosition)
    self.checkPointCoordinates(self.myLine.endPoint(), self.myEnd)

  def test_move_end_of_free_line(self):
    """ Test 2. Movement of end point of a free line
    """
    newPosition = [self.myEnd[0] + 20., self.myEnd[1] + 10.]
    self.mySketch.move(self.myLine.endPoint(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myLine.startPoint(), self.myStart)
    self.checkPointCoordinates(self.myLine.endPoint(), newPosition)

  def test_move_free_line(self):
    """ Test 3. Movement of free line
    """
    diff = [self.myEnd[0] - self.myStart[0], self.myEnd[1] - self.myStart[1]]

    newPosition = [100., 100.]
    self.mySketch.move(self.myLine.defaultResult(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointOnLine(newPosition)

    # additionally check the line keeps geometry (relative positions of extremities)
    startPoint = self.myLine.startPoint()
    endPoint = [startPoint.x() + diff[0], startPoint.y() + diff[1]]
    self.checkPointCoordinates(self.myLine.endPoint(), endPoint)

  def test_move_line_start_fixed(self):
    """ Test 4. Movement of a line, which start point is fixed
    """
    fixed = self.mySketch.setFixed(self.myLine.startPoint())
    self.myDOF -= 2
    model.do()
    self.checkDOF()

    newPosition = [100., 100.]
    self.mySketch.move(self.myLine.defaultResult(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myLine.startPoint(), self.myStart)
    self.assertNotEqual(self.myLine.endPoint().x(), self.myEnd[0])
    self.assertNotEqual(self.myLine.endPoint().y(), self.myEnd[1])

  def test_move_line_end_fixed(self):
    """ Test 5. Movement of a line, which end point is fixed
    """
    fixed = self.mySketch.setFixed(self.myLine.endPoint())
    self.myDOF -= 2
    model.do()
    self.checkDOF()

    newPosition = [100., 100.]
    self.mySketch.move(self.myLine.defaultResult(), newPosition[0], newPosition[1])
    model.do()
    self.assertNotEqual(self.myLine.startPoint().x(), self.myStart[0])
    self.assertNotEqual(self.myLine.startPoint().y(), self.myStart[1])
    self.checkPointCoordinates(self.myLine.endPoint(), self.myEnd)

  def test_move_line_fully_fixed(self):
    """ Test 6. Movement of fully fixed line (should not change its coordinates)
    """
    fixed = self.mySketch.setFixed(self.myLine.defaultResult())
    self.myDOF -= 4
    model.do()

    newPosition = [100., 100.]
    self.mySketch.move(self.myLine.defaultResult(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myLine.startPoint(), self.myStart)
    self.checkPointCoordinates(self.myLine.endPoint(), self.myEnd)


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
