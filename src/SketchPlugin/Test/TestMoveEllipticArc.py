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
from SketchAPI import *
from salome.shaper import model

__updated__ = "2019-10-15"

class TestMoveEllipticArc(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myCenter = GeomAPI_Pnt2d(70., 50.)
    self.myAxisPoint = GeomAPI_Pnt2d(100., 70.)
    self.myStartPoint = GeomAPI_Pnt2d(75., 75.)
    self.myEndPoint = GeomAPI_Pnt2d(45., 55.)
    macroEllipticArc = self.mySketch.addEllipticArc(self.myCenter, self.myAxisPoint, self.myStartPoint, self.myEndPoint, False)
    self.myDOF = 7
    model.do()
    self.checkDOF()
    self.myEllipticArc = SketchAPI_EllipticArc(model.lastSubFeature(self.mySketch, "SketchEllipticArc"))
    self.myMajorRadius = self.myEllipticArc.majorRadius().value()
    self.myMinorRadius = self.myEllipticArc.minorRadius().value()

  def tearDown(self):
    self.checkDOF()
    self.checkPointOnEllipse(self.myEllipticArc.startPoint(), self.myEllipticArc)
    self.checkPointOnEllipse(self.myEllipticArc.endPoint(), self.myEllipticArc)
    model.end()

  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

  def checkPointCoordinates(self, thePoint, theCoordinates):
    aCoord = []
    if issubclass(type(theCoordinates), GeomAPI_Pnt2d):
      aCoord = [theCoordinates.x(), theCoordinates.y()]
    else:
      aCoord = theCoordinates
    DIGITS = 7 - math.ceil(math.log10(math.hypot(aCoord[0], aCoord[1])))
    self.assertAlmostEqual(thePoint.x(), aCoord[0], DIGITS)
    self.assertAlmostEqual(thePoint.y(), aCoord[1], DIGITS)

  def checkPointOnEllipse(self, theCoordinates, theEllipticArc):
    point = GeomAPI_Pnt2d(theCoordinates.x(), theCoordinates.y())
    firstFocus2d = GeomAPI_Pnt2d(theEllipticArc.firstFocus().x(), theEllipticArc.firstFocus().y())
    distPF1 = model.distancePointPoint(firstFocus2d,  point)
    secondFocus2d = GeomAPI_Pnt2d(theEllipticArc.secondFocus().x(), theEllipticArc.secondFocus().y())
    distPF2 = model.distancePointPoint(secondFocus2d,  point)
    DIGITS = 7 - math.ceil(math.log10(theEllipticArc.majorRadius().value()))
    self.assertAlmostEqual(distPF1 + distPF2, 2.0 * theEllipticArc.majorRadius().value(), DIGITS)

  def checkAlmostEqual(self, theValue1, theValue2):
    DIGITS = 7 - math.ceil(math.log10(theValue1))
    self.assertAlmostEqual(theValue1, theValue2, DIGITS)

  def fixMajorRadius(self):
    self.mySketch.setDistance(self.myEllipticArc.center(), self.myEllipticArc.majorAxisPositive(), self.myMajorRadius)
    self.myDOF -= 1
    model.do()
    self.checkDOF()

  def fixMinorRadius(self):
    self.mySketch.setDistance(self.myEllipticArc.center(), self.myEllipticArc.minorAxisPositive(), self.myMinorRadius)
    self.myDOF -= 1
    model.do()
    self.checkDOF()

  def fixPoint(self, thePoint):
    self.mySketch.setFixed(thePoint)
    self.myDOF -= 2
    model.do()
    self.checkDOF()


  def test_move_center_free_ellipse(self):
    """ Test 1. Movement of central point of a free elliptic arc
    """
    newPosition = [self.myCenter.x() + 20., self.myCenter.y() + 10.]
    self.mySketch.move(self.myEllipticArc.center(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myEllipticArc.center(), newPosition)

  def test_move_free_ellipse(self):
    """ Test 2. Movement of a free ellipse dragging the edge
    """
    newPosition = GeomAPI_Pnt2d(110., 80.)
    self.mySketch.move(self.myEllipticArc.defaultResult(), newPosition.x(), newPosition.y())
    model.do()
    self.checkPointCoordinates(self.myEllipticArc.center(), self.myCenter)
    self.checkPointOnEllipse(newPosition, self.myEllipticArc)
    self.assertNotEqual(self.myEllipticArc.minorRadius().value(), self.myMinorRadius)
    self.assertNotEqual(self.myEllipticArc.majorRadius().value(), self.myMajorRadius)

  def test_move_center_ellipse_fixed_major_radius(self):
    """ Test 3. Movement of central point of ellipse with fixed major radius
    """
    self.fixMajorRadius()

    newPosition = [self.myCenter.x() + 20., self.myCenter.y() + 10.]
    self.mySketch.move(self.myEllipticArc.center(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myEllipticArc.center(), newPosition)
    self.checkAlmostEqual(self.myEllipticArc.majorRadius().value(), self.myMajorRadius)

  def test_move_ellipse_fixed_major_radius(self):
    """ Test 4. Movement of ellipse with fixed major radius
    """
    self.fixMajorRadius()

    newPosition = GeomAPI_Pnt2d(80., 80.)
    self.mySketch.move(self.myEllipticArc.defaultResult(), newPosition.x(), newPosition.y())
    model.do()
    self.checkPointOnEllipse(newPosition, self.myEllipticArc)
    self.assertNotEqual(self.myEllipticArc.minorRadius().value(), self.myMinorRadius)
    self.checkAlmostEqual(self.myEllipticArc.majorRadius().value(), self.myMajorRadius)

  def test_move_center_ellipse_fixed_minor_radius(self):
    """ Test 5. Movement of central point of ellipse with fixed minor radius
    """
    self.fixMinorRadius()

    newPosition = [self.myCenter.x() + 20., self.myCenter.y() + 10.]
    self.mySketch.move(self.myEllipticArc.center(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myEllipticArc.center(), newPosition)
    self.checkAlmostEqual(self.myEllipticArc.minorRadius().value(), self.myMinorRadius)

  def test_move_ellipse_fixed_minor_radius(self):
    """ Test 6. Movement of ellipse with fixed minor radius
    """
    self.fixMinorRadius()

    newPosition = GeomAPI_Pnt2d(110., 80.)
    self.mySketch.move(self.myEllipticArc.defaultResult(), newPosition.x(), newPosition.y())
    model.do()
    self.checkPointOnEllipse(newPosition, self.myEllipticArc)
    self.checkAlmostEqual(self.myEllipticArc.minorRadius().value(), self.myMinorRadius)
    self.assertNotEqual(self.myEllipticArc.majorRadius().value(), self.myMajorRadius)

  def test_move_center_ellipse_fixed_center(self):
    """ Test 7. Movement of central point of ellipse with fixed center (nothing should be changed)
    """
    self.fixPoint(self.myEllipticArc.center())

    newPosition = [self.myCenter.x() + 20., self.myCenter.y() + 10.]
    self.mySketch.move(self.myEllipticArc.center(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myEllipticArc.center(), self.myCenter)
    self.checkAlmostEqual(self.myEllipticArc.minorRadius().value(), self.myMinorRadius)
    self.checkAlmostEqual(self.myEllipticArc.majorRadius().value(), self.myMajorRadius)

  def test_move_ellipse_fixed_center(self):
    """ Test 8. Movement of ellipse with fixed center
    """
    self.fixPoint(self.myEllipticArc.center())

    newPosition = GeomAPI_Pnt2d(110., 80.)
    self.mySketch.move(self.myEllipticArc.defaultResult(), newPosition.x(), newPosition.y())
    model.do()
    self.checkPointOnEllipse(newPosition, self.myEllipticArc)
    self.assertNotEqual(self.myEllipticArc.minorRadius().value(), self.myMinorRadius)
    self.assertNotEqual(self.myEllipticArc.majorRadius().value(), self.myMajorRadius)

  def test_move_center_ellipse_fixed_focus(self):
    """ Test 9. Movement of central point of ellipse with fixed focus
    """
    focus = [self.myEllipticArc.firstFocus().x(), self.myEllipticArc.firstFocus().y()]
    self.fixPoint(self.myEllipticArc.firstFocus())

    newPosition = GeomAPI_Pnt2d(self.myCenter.x() + 20., self.myCenter.y() + 10.)
    self.mySketch.move(self.myEllipticArc.center(), newPosition.x(), newPosition.y())
    model.do()
    self.checkPointCoordinates(self.myEllipticArc.center(), newPosition)
    self.checkPointCoordinates(self.myEllipticArc.firstFocus(), focus)
    self.assertNotEqual(self.myEllipticArc.minorRadius().value(), self.myMinorRadius)
    self.assertNotEqual(self.myEllipticArc.majorRadius().value(), self.myMajorRadius)

  def test_move_focus_ellipse_fixed_focus(self):
    """ Test 10. Movement of a focus point of ellipse with fixed focus (nothing should be changed)
    """
    focus = [self.myEllipticArc.firstFocus().x(), self.myEllipticArc.firstFocus().y()]
    self.fixPoint(self.myEllipticArc.firstFocus())

    newPosition = GeomAPI_Pnt2d(focus[0] + 10., focus[1] + 10.)
    self.mySketch.move(self.myEllipticArc.firstFocus(), newPosition.x(), newPosition.y())
    model.do()
    self.checkPointCoordinates(self.myEllipticArc.firstFocus(), focus)
    self.checkAlmostEqual(self.myEllipticArc.minorRadius().value(), self.myMinorRadius)
    self.checkAlmostEqual(self.myEllipticArc.majorRadius().value(), self.myMajorRadius)

  def test_move_ellipse_fixed_focus(self):
    """ Test 11. Movement of ellipse with fixed focus
    """
    focus = [self.myEllipticArc.firstFocus().x(), self.myEllipticArc.firstFocus().y()]
    self.fixPoint(self.myEllipticArc.firstFocus())

    newPosition = GeomAPI_Pnt2d(80., 90.)
    self.mySketch.move(self.myEllipticArc.defaultResult(), newPosition.x(), newPosition.y())
    model.do()
    self.checkPointOnEllipse(newPosition, self.myEllipticArc)
    self.checkPointCoordinates(self.myEllipticArc.firstFocus(), focus)
    self.assertNotEqual(self.myEllipticArc.minorRadius().value(), self.myMinorRadius)
    self.assertNotEqual(self.myEllipticArc.majorRadius().value(), self.myMajorRadius)

  def test_move_fixed_ellipse(self):
    """ Test 12. Trying to move fully fixed ellipse
    """
    self.mySketch.setFixed(self.myEllipticArc.results()[-1])
    self.myDOF -= 7
    model.do()
    self.checkDOF()

    newPosition = [110., 80.]
    self.mySketch.move(self.myEllipticArc.defaultResult(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myEllipticArc.center(), self.myCenter)
    self.checkPointCoordinates(self.myEllipticArc.majorAxisPositive(), self.myAxisPoint)
    self.checkPointCoordinates(self.myEllipticArc.startPoint(), self.myStartPoint)
    self.checkPointCoordinates(self.myEllipticArc.endPoint(), self.myEndPoint)
    self.checkAlmostEqual(self.myEllipticArc.majorRadius().value(), self.myMajorRadius)
    self.checkAlmostEqual(self.myEllipticArc.minorRadius().value(), self.myMinorRadius)

  def test_move_start_point_free_ellipse(self):
    """ Test 13. Trying to move start point of elliptic arc
    """
    newPosition = [self.myStartPoint.x() + 10., self.myStartPoint.y() + 10.]
    self.mySketch.move(self.myEllipticArc.startPoint(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myEllipticArc.startPoint(), newPosition)

  def test_move_end_point_free_ellipse(self):
    """ Test 14. Trying to move end point of elliptic arc
    """
    newPosition = [self.myEndPoint.x() - 10., self.myEndPoint.y() + 10.]
    self.mySketch.move(self.myEllipticArc.endPoint(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myEllipticArc.endPoint(), newPosition)

  def test_move_start_point_fixed_ellipse(self):
    """ Test 15. Trying to move start point of fully fixed ellipse
    """
    self.mySketch.setFixed(self.myEllipticArc.results()[-1])
    self.myDOF -= 7
    model.do()
    self.checkDOF()

    newPosition = [self.myStartPoint.x() + 10., self.myStartPoint.y() + 10.]
    self.mySketch.move(self.myEllipticArc.startPoint(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myEllipticArc.center(), self.myCenter)
    self.checkPointCoordinates(self.myEllipticArc.majorAxisPositive(), self.myAxisPoint)
    self.checkPointCoordinates(self.myEllipticArc.startPoint(), self.myStartPoint)
    self.checkPointCoordinates(self.myEllipticArc.endPoint(), self.myEndPoint)
    self.checkAlmostEqual(self.myEllipticArc.majorRadius().value(), self.myMajorRadius)
    self.checkAlmostEqual(self.myEllipticArc.minorRadius().value(), self.myMinorRadius)

  def test_move_end_point_fixed_ellipse(self):
    """ Test 16. Trying to move end point of fully fixed ellipse
    """
    self.mySketch.setFixed(self.myEllipticArc.results()[-1])
    self.myDOF -= 7
    model.do()
    self.checkDOF()

    newPosition = [self.myEndPoint.x() - 10., self.myEndPoint.y() + 10.]
    self.mySketch.move(self.myEllipticArc.endPoint(), newPosition[0], newPosition[1])
    model.do()
    self.checkPointCoordinates(self.myEllipticArc.center(), self.myCenter)
    self.checkPointCoordinates(self.myEllipticArc.majorAxisPositive(), self.myAxisPoint)
    self.checkPointCoordinates(self.myEllipticArc.startPoint(), self.myStartPoint)
    self.checkPointCoordinates(self.myEllipticArc.endPoint(), self.myEndPoint)
    self.checkAlmostEqual(self.myEllipticArc.majorRadius().value(), self.myMajorRadius)
    self.checkAlmostEqual(self.myEllipticArc.minorRadius().value(), self.myMinorRadius)


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
