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
    Test movement of the sketch ellipse
"""

import unittest
import math
from GeomAPI import GeomAPI_Pnt2d
from GeomDataAPI import geomDataAPI_Point2D
from salome.shaper import model

__updated__ = "2019-09-12"

class TestMoveEllipse(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myCenter = [70., 50.]
    self.myFocus = [100., 70.]
    self.myMinorRadius = 20.
    self.myEllipse = self.mySketch.addEllipse(self.myCenter[0], self.myCenter[1], self.myFocus[0], self.myFocus[1], self.myMinorRadius)
    self.myDOF = 5
    model.do()
    self.checkDOF()
    self.myMajorRadius = self.myEllipse.majorRadius().value()
    self.NB_DIGITS_RADIUS = 7 - math.floor(math.log10(self.myMinorRadius))

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

  def checkPointOnEllipse(self, theCoordinates, theEllipse):
    point = GeomAPI_Pnt2d(theCoordinates.x(), theCoordinates.y())
    firstFocus2d = GeomAPI_Pnt2d(theEllipse.firstFocus().x(), theEllipse.firstFocus().y())
    distPF1 = model.distancePointPoint(firstFocus2d,  point)
    secondFocus2d = GeomAPI_Pnt2d(theEllipse.secondFocus().x(), theEllipse.secondFocus().y())
    distPF2 = model.distancePointPoint(secondFocus2d,  point)
    self.assertAlmostEqual(distPF1 + distPF2, 2.0 * theEllipse.majorRadius().value(), 7 - math.floor(math.log10(2.0 * theEllipse.majorRadius().value())))

  def fixMajorRadius(self):
    self.mySketch.setDistance(self.myEllipse.center(), self.myEllipse.majorAxisPositive(), self.myMajorRadius)
    self.myDOF -= 1
    model.do()
    self.checkDOF()

  def fixMinorRadius(self):
    self.mySketch.setDistance(self.myEllipse.center(), self.myEllipse.minorAxisPositive(), self.myMinorRadius)
    self.myDOF -= 1
    model.do()
    self.checkDOF()

  def fixPoint(self, thePoint):
    self.mySketch.setFixed(thePoint)
    self.myDOF -= 2
    model.do()
    self.checkDOF()


  def test_move_center_free_ellipse(self):
    """ Test 1. Movement of central point of a free ellipse
    """
    newPosition = [self.myCenter[0] + 20., self.myCenter[1] + 10.]
    self.mySketch.move(self.myEllipse.center(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myEllipse.center(), newPosition)
    self.assertAlmostEqual(self.myEllipse.minorRadius().value(), self.myMinorRadius, self.NB_DIGITS_RADIUS)
    self.assertAlmostEqual(self.myEllipse.majorRadius().value(), self.myMajorRadius, self.NB_DIGITS_RADIUS)

  def test_move_free_ellipse(self):
    """ Test 2. Movement of a free ellipse dragging the edge
    """
    newPosition = GeomAPI_Pnt2d(120., 90.)
    self.mySketch.move(self.myEllipse.defaultResult(), newPosition.x(), newPosition.y())
    model.do()
    self.checkPointCoordinates(self.myEllipse.center(), self.myCenter)
    self.checkPointOnEllipse(newPosition, self.myEllipse)
    self.assertNotEqual(self.myEllipse.minorRadius().value(), self.myMinorRadius)
    self.assertNotEqual(self.myEllipse.majorRadius().value(), self.myMajorRadius)

  def test_move_center_ellipse_fixed_major_radius(self):
    """ Test 3. Movement of central point of ellipse with fixed major radius
    """
    self.fixMajorRadius()

    newPosition = [self.myCenter[0] + 20., self.myCenter[1] + 10.]
    self.mySketch.move(self.myEllipse.center(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myEllipse.center(), newPosition)
    self.assertAlmostEqual(self.myEllipse.minorRadius().value(), self.myMinorRadius, self.NB_DIGITS_RADIUS)
    self.assertAlmostEqual(self.myEllipse.majorRadius().value(), self.myMajorRadius, self.NB_DIGITS_RADIUS)

  def test_move_ellipse_fixed_major_radius(self):
    """ Test 4. Movement of ellipse with fixed major radius
    """
    self.fixMajorRadius()

    newPosition = GeomAPI_Pnt2d(80., 80.)
    self.mySketch.move(self.myEllipse.defaultResult(), newPosition.x(), newPosition.y())
    model.do()
    self.checkPointOnEllipse(newPosition, self.myEllipse)
    self.assertNotEqual(self.myEllipse.minorRadius().value(), self.myMinorRadius)
    self.assertAlmostEqual(self.myEllipse.majorRadius().value(), self.myMajorRadius, self.NB_DIGITS_RADIUS)

  def test_move_center_ellipse_fixed_minor_radius(self):
    """ Test 5. Movement of central point of ellipse with fixed minor radius
    """
    self.fixMinorRadius()

    newPosition = [self.myCenter[0] + 20., self.myCenter[1] + 10.]
    self.mySketch.move(self.myEllipse.center(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myEllipse.center(), newPosition)
    self.assertAlmostEqual(self.myEllipse.minorRadius().value(), self.myMinorRadius, self.NB_DIGITS_RADIUS)
    self.assertAlmostEqual(self.myEllipse.majorRadius().value(), self.myMajorRadius, self.NB_DIGITS_RADIUS)

  def test_move_ellipse_fixed_minor_radius(self):
    """ Test 6. Movement of ellipse with fixed minor radius
    """
    self.fixMinorRadius()

    newPosition = GeomAPI_Pnt2d(120., 90.)
    self.mySketch.move(self.myEllipse.defaultResult(), newPosition.x(), newPosition.y())
    model.do()
    self.checkPointOnEllipse(newPosition, self.myEllipse)
    self.assertAlmostEqual(self.myEllipse.minorRadius().value(), self.myMinorRadius, self.NB_DIGITS_RADIUS)
    self.assertNotEqual(self.myEllipse.majorRadius().value(), self.myMajorRadius)

  def test_move_center_ellipse_fixed_center(self):
    """ Test 7. Movement of central point of ellipse with fixed center (nothing should be changed)
    """
    self.fixPoint(self.myEllipse.center())

    newPosition = [self.myCenter[0] + 20., self.myCenter[1] + 10.]
    self.mySketch.move(self.myEllipse.center(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myEllipse.center(), self.myCenter)
    self.assertAlmostEqual(self.myEllipse.minorRadius().value(), self.myMinorRadius, self.NB_DIGITS_RADIUS)
    self.assertAlmostEqual(self.myEllipse.majorRadius().value(), self.myMajorRadius, self.NB_DIGITS_RADIUS)

  def test_move_ellipse_fixed_center(self):
    """ Test 8. Movement of ellipse with fixed center
    """
    self.fixPoint(self.myEllipse.center())

    newPosition = GeomAPI_Pnt2d(120., 90.)
    self.mySketch.move(self.myEllipse.defaultResult(), newPosition.x(), newPosition.y())
    model.do()
    self.checkPointOnEllipse(newPosition, self.myEllipse)
    self.assertNotEqual(self.myEllipse.minorRadius().value(), self.myMinorRadius)
    self.assertNotEqual(self.myEllipse.majorRadius().value(), self.myMajorRadius)

  def test_move_center_ellipse_fixed_focus(self):
    """ Test 9. Movement of central point of ellipse with fixed focus
    """
    self.fixPoint(self.myEllipse.firstFocus())

    newPosition = GeomAPI_Pnt2d(self.myCenter[0] + 20., self.myCenter[1] + 10.)
    self.mySketch.move(self.myEllipse.center(), newPosition.x(), newPosition.y())
    model.do()
    self.checkPointCoordinates(self.myEllipse.center(), newPosition)
    self.checkPointCoordinates(self.myEllipse.firstFocus(), self.myFocus)
    self.assertNotEqual(self.myEllipse.minorRadius().value(), self.myMinorRadius)
    self.assertNotEqual(self.myEllipse.majorRadius().value(), self.myMajorRadius)

  def test_move_focus_ellipse_fixed_focus(self):
    """ Test 10. Movement of a focus point of ellipse with fixed focus (nothing should be changed)
    """
    self.fixPoint(self.myEllipse.firstFocus())

    newPosition = GeomAPI_Pnt2d(self.myFocus[0] + 10., self.myFocus[1] + 10.)
    self.mySketch.move(self.myEllipse.firstFocus(), newPosition.x(), newPosition.y())
    model.do()
    self.checkPointCoordinates(self.myEllipse.firstFocus(), self.myFocus)
    self.assertAlmostEqual(self.myEllipse.minorRadius().value(), self.myMinorRadius, self.NB_DIGITS_RADIUS)
    self.assertAlmostEqual(self.myEllipse.majorRadius().value(), self.myMajorRadius, self.NB_DIGITS_RADIUS)

  def test_move_ellipse_fixed_focus(self):
    """ Test 11. Movement of ellipse with fixed focus
    """
    self.fixPoint(self.myEllipse.firstFocus())

    newPosition = GeomAPI_Pnt2d(80., 90.)
    self.mySketch.move(self.myEllipse.defaultResult(), newPosition.x(), newPosition.y())
    model.do()
    self.checkPointOnEllipse(newPosition, self.myEllipse)
    self.checkPointCoordinates(self.myEllipse.firstFocus(), self.myFocus)
    self.assertNotEqual(self.myEllipse.minorRadius().value(), self.myMinorRadius)
    self.assertNotEqual(self.myEllipse.majorRadius().value(), self.myMajorRadius)

  def test_move_fixed_ellipse(self):
    """ Test 12. Trying to move fully fixed ellipse
    """
    self.mySketch.setFixed(self.myEllipse.results()[-1])
    self.myDOF -= 5
    model.do()
    self.checkDOF()

    newPosition = [120., 90.]
    self.mySketch.move(self.myEllipse.defaultResult(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myEllipse.center(), self.myCenter)
    self.checkPointCoordinates(self.myEllipse.firstFocus(), self.myFocus)
    self.assertAlmostEqual(self.myEllipse.minorRadius().value(), self.myMinorRadius, self.NB_DIGITS_RADIUS)


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
