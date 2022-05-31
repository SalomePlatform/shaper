# Copyright (C) 2017-2022  CEA/DEN, EDF R&D
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
    Test movement of the sketch arc of circle
"""

import unittest
from GeomDataAPI import geomDataAPI_Point2D
from salome.shaper import model

__updated__ = "2017-05-11"

class TestMoveArc(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myCenter = [50., 50.]
    self.myStart = [70., 50.]
    self.myEnd = [50., 70.]
    self.myArc = self.mySketch.addArc(self.myCenter[0], self.myCenter[1], self.myStart[0], self.myStart[1], self.myEnd[0], self.myEnd[1], False)
    self.myDOF = 5
    model.do()
    self.checkDOF()

  def tearDown(self):
    model.assertArcValidity(self.myArc)
    self.checkDOF()
    model.end()
    assert(model.checkPythonDump())

  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

  def checkPointCoordinates(self, thePoint, theCoordinates):
    self.assertAlmostEqual(thePoint.x(), theCoordinates[0], 5)
    self.assertAlmostEqual(thePoint.y(), theCoordinates[1], 5)

  def checkPointOnArc(self, theCoordinates):
    distPC = model.distancePointPoint(self.myArc.center(),  theCoordinates)
    radius = model.distancePointPoint(self.myArc.center(), self.myArc.startPoint())
    self.assertAlmostEqual(distPC, radius, 5)

  def checkArcRadius(self):
    radius = model.distancePointPoint(self.myArc.center(), self.myArc.startPoint())
    self.assertAlmostEqual(radius, self.myRadius, 5)

  def fixArcRadius(self):
    self.myRadius = 20.
    self.mySketch.setRadius(self.myArc.results()[1], self.myRadius)
    self.myDOF -= 1
    model.do()
    self.checkDOF()

  def fixPoint(self, thePoint):
    self.mySketch.setFixed(thePoint)
    self.myDOF -= 2
    model.do()
    self.checkDOF()

  def fixArc(self):
    self.mySketch.setFixed(self.myArc.results()[1])
    self.myDOF -= 5
    model.do()
    self.checkDOF()


  def test_move_center_of_free_arc(self):
    """ Test 1. Movement of center of a free arc
    """
    newPosition = [self.myCenter[0] + 10., self.myCenter[1] - 15.]
    self.mySketch.move(self.myArc.center(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.center(), newPosition)

  def test_move_start_of_free_arc(self):
    """ Test 2. Movement of start point of a free arc
    """
    newPosition = [self.myStart[0] - 10., self.myStart[1] + 5.]
    self.mySketch.move(self.myArc.startPoint(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.center(), self.myCenter)
    self.checkPointCoordinates(self.myArc.startPoint(), newPosition)

  def test_move_end_of_free_arc(self):
    """ Test 3. Movement of end point of a free arc
    """
    newPosition = [self.myEnd[0] + 10., self.myEnd[1] + 5.]
    self.mySketch.move(self.myArc.endPoint(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.center(), self.myCenter)
    self.checkPointCoordinates(self.myArc.endPoint(), newPosition)

  def test_move_free_arc(self):
    """ Test 4. Movement of a free arc dragging the edge
    """
    newPosition = [100., 80.]
    self.mySketch.move(self.myArc.defaultResult(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointOnArc(newPosition)
    self.checkPointCoordinates(self.myArc.center(), self.myCenter)

  def test_move_center_of_arc_fixed_center(self):
    """ Test 5. Movement of center of the arc with fixed center (nothing should be changed)
    """
    self.fixPoint(self.myArc.center())

    newPosition = [self.myCenter[0] + 10., self.myCenter[1] - 15.]
    self.mySketch.move(self.myArc.center(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.center(), self.myCenter)

  def test_move_start_of_arc_fixed_center(self):
    """ Test 6. Movement of start point of the arc with fixed center
    """
    self.fixPoint(self.myArc.center())

    newPosition = [self.myStart[0] - 10., self.myStart[1] + 5.]
    self.mySketch.move(self.myArc.startPoint(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.center(), self.myCenter)
    self.checkPointCoordinates(self.myArc.startPoint(), newPosition)

  def test_move_end_of_arc_fixed_center(self):
    """ Test 7. Movement of end point of the arc with fixed center
    """
    self.fixPoint(self.myArc.center())

    newPosition = [self.myEnd[0] + 10., self.myEnd[1] + 5.]
    self.mySketch.move(self.myArc.endPoint(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.center(), self.myCenter)
    self.checkPointCoordinates(self.myArc.endPoint(), newPosition)

  def test_move_arc_fixed_center(self):
    """ Test 8. Movement of the arc with fixed center dragging the edge
    """
    self.fixPoint(self.myArc.center())

    newPosition = [100., 80.]
    self.mySketch.move(self.myArc.defaultResult(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointOnArc(newPosition)
    self.checkPointCoordinates(self.myArc.center(), self.myCenter)

  def test_move_center_of_arc_fixed_start(self):
    """ Test 9. Movement of center of the arc with fixed start point
    """
    self.fixPoint(self.myArc.startPoint())

    newPosition = [self.myCenter[0] + 10., self.myCenter[1] - 15.]
    self.mySketch.move(self.myArc.center(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.center(), newPosition)
    self.checkPointCoordinates(self.myArc.startPoint(), self.myStart)

  def test_move_start_of_arc_fixed_start(self):
    """ Test 10. Movement of start point of the arc with fixed start point (nothing should be changed)
    """
    self.fixPoint(self.myArc.startPoint())

    newPosition = [self.myStart[0] - 10., self.myStart[1] + 5.]
    self.mySketch.move(self.myArc.startPoint(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.center(), self.myCenter)
    self.checkPointCoordinates(self.myArc.startPoint(), self.myStart)
    self.checkPointCoordinates(self.myArc.endPoint(), self.myEnd)

  def test_move_end_of_arc_fixed_start(self):
    """ Test 11. Movement of end point of the arc with fixed start point
    """
    self.fixPoint(self.myArc.startPoint())

    newPosition = [self.myEnd[0] + 10., self.myEnd[1] + 5.]
    self.mySketch.move(self.myArc.endPoint(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.startPoint(), self.myStart)
    self.assertNotEqual(self.myArc.center().x(), self.myCenter[0])
    self.assertNotEqual(self.myArc.center().y(), self.myCenter[1])

  def test_move_arc_fixed_start(self):
    """ Test 12. Movement of the arc with fixed start point dragging the edge
    """
    self.fixPoint(self.myArc.startPoint())

    newPosition = [100., 80.]
    self.mySketch.move(self.myArc.defaultResult(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.startPoint(), self.myStart)
    self.assertNotEqual(self.myArc.center().x(), self.myCenter[0])
    self.assertNotEqual(self.myArc.center().y(), self.myCenter[1])

  def test_move_center_of_arc_fixed_end(self):
    """ Test 13. Movement of center of the arc with fixed end point
    """
    self.fixPoint(self.myArc.endPoint())

    newPosition = [self.myCenter[0] + 10., self.myCenter[1] - 15.]
    self.mySketch.move(self.myArc.center(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.center(), newPosition)
    self.checkPointCoordinates(self.myArc.endPoint(), self.myEnd)

  def test_move_start_of_arc_fixed_end(self):
    """ Test 14. Movement of start point of the arc with fixed end point
    """
    self.fixPoint(self.myArc.endPoint())

    newPosition = [self.myStart[0] - 10., self.myStart[1] + 5.]
    self.mySketch.move(self.myArc.startPoint(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.endPoint(), self.myEnd)
    self.assertNotEqual(self.myArc.center().x(), self.myCenter[0])
    self.assertNotEqual(self.myArc.center().y(), self.myCenter[1])

  def test_move_end_of_arc_fixed_end(self):
    """ Test 15. Movement of end point of the arc with fixed end point (nothing should be changed)
    """
    self.fixPoint(self.myArc.endPoint())

    newPosition = [self.myEnd[0] + 10., self.myEnd[1] + 5.]
    self.mySketch.move(self.myArc.endPoint(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.center(), self.myCenter)
    self.checkPointCoordinates(self.myArc.startPoint(), self.myStart)
    self.checkPointCoordinates(self.myArc.endPoint(), self.myEnd)

  def test_move_arc_fixed_end(self):
    """ Test 16. Movement of the arc with fixed end point dragging the edge
    """
    self.fixPoint(self.myArc.endPoint())

    newPosition = [100., 80.]
    self.mySketch.move(self.myArc.defaultResult(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.endPoint(), self.myEnd)
    self.assertTrue(fabs(self.myArc.center().x() - self.myCenter[0]) > 1.e-5 or fabs(self.myArc.center().y() - self.myCenter[1]) > 1.e-5)

  def test_move_center_of_arc_fixed_radius(self):
    """ Test 17. Movement of center of the arc with fixed radius
    """
    self.fixArcRadius()

    newPosition = [self.myCenter[0] + 10., self.myCenter[1] - 15.]
    self.mySketch.move(self.myArc.center(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.center(), newPosition)
    self.checkArcRadius()

  def test_move_start_of_arc_fixed_radius(self):
    """ Test 18. Movement of start point of the arc with fixed radius
    """
    self.fixArcRadius()

    newPosition = [self.myStart[0] - 10., self.myStart[1] + 5.]
    self.mySketch.move(self.myArc.startPoint(), newPosition[0], newPosition[1])
    model.do()
    self.checkArcRadius()

  def test_move_end_of_arc_fixed_radius(self):
    """ Test 19. Movement of end point of the arc with fixed radius
    """
    self.fixArcRadius()

    newPosition = [self.myEnd[0] + 10., self.myEnd[1] + 5.]
    self.mySketch.move(self.myArc.endPoint(), newPosition[0], newPosition[1])
    model.do()
    self.checkArcRadius()

  def test_move_arc_fixed_radius(self):
    """ Test 20. Movement of the arc with fixed radius dragging the edge
    """
    self.fixArcRadius()

    newPosition = [100., 80.]
    self.mySketch.move(self.myArc.defaultResult(), newPosition[0], newPosition[1])
    model.do()
    self.checkArcRadius()

  def test_move_center_of_fixed_arc(self):
    """ Test 21. Movement of center of fully fixed arc (nothing should be changed)
    """
    self.fixArc()

    newPosition = [self.myCenter[0] + 10., self.myCenter[1] - 15.]
    self.mySketch.move(self.myArc.center(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.center(), self.myCenter)
    self.checkPointCoordinates(self.myArc.startPoint(), self.myStart)
    self.checkPointCoordinates(self.myArc.endPoint(), self.myEnd)

  def test_move_start_of_fixed_arc(self):
    """ Test 22. Movement of start point of fully fixed arc (nothing should be changed)
    """
    self.fixArc()

    newPosition = [self.myStart[0] - 10., self.myStart[1] + 5.]
    self.mySketch.move(self.myArc.startPoint(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.center(), self.myCenter)
    self.checkPointCoordinates(self.myArc.startPoint(), self.myStart)
    self.checkPointCoordinates(self.myArc.endPoint(), self.myEnd)

  def test_move_end_of_fixed_arc(self):
    """ Test 23. Movement of end point of fully fixed arc (nothing should be changed)
    """
    self.fixArc()

    newPosition = [self.myEnd[0] + 10., self.myEnd[1] + 5.]
    self.mySketch.move(self.myArc.endPoint(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.center(), self.myCenter)
    self.checkPointCoordinates(self.myArc.startPoint(), self.myStart)
    self.checkPointCoordinates(self.myArc.endPoint(), self.myEnd)

  def test_move_fixed_arc(self):
    """ Test 24. Movement of fully fixed arc (nothing should be changed)
    """
    self.fixArc()

    newPosition = [100., 80.]
    self.mySketch.move(self.myArc.defaultResult(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myArc.center(), self.myCenter)
    self.checkPointCoordinates(self.myArc.startPoint(), self.myStart)
    self.checkPointCoordinates(self.myArc.endPoint(), self.myEnd)


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
