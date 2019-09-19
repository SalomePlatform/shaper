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
    Test constraint "Collinear" applied for ellipse's axes
"""

import unittest
import math

from salome.shaper import model

from GeomAPI import *
from SketchAPI import *

__updated__ = "2019-09-18"

class TestCollinearEllipse(unittest.TestCase):
  def setUp(self):
    axisStart = GeomAPI_Pnt2d(30., 60.)
    axisEnd = GeomAPI_Pnt2d(80., 50.)
    passedPoint = GeomAPI_Pnt2d(60., 60.)

    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    macroEllipse = self.mySketch.addEllipse(axisStart, axisEnd, passedPoint, False)
    self.myDOF = 5
    self.myOX = self.mySketch.addLine("OX")
    model.do()
    self.myEllipse = SketchAPI_Ellipse(model.lastSubFeature(self.mySketch, "SketchEllipse"))
    self.myCenter = macroEllipse.center()
    self.myFocus1 = macroEllipse.focus1()
    self.myFocus2 = macroEllipse.focus2()
    self.myMajorAxis = macroEllipse.majorAxis()
    self.myMajorStart = macroEllipse.majorAxisStart()
    self.myMajorEnd = macroEllipse.majorAxisEnd()
    self.myMinorAxis = macroEllipse.minorAxis()
    self.myMinorStart = macroEllipse.minorAxisStart()
    self.myMinorEnd = macroEllipse.minorAxisEnd()


  def tearDown(self):
    model.end()
    self.checkDOF()
    self.assertPoints(self.myCenter.coordinates(), self.myEllipse.center())
    self.assertPoints(self.myFocus1.coordinates(), self.myEllipse.firstFocus())
    self.assertPoints(self.myFocus2.coordinates(), self.myEllipse.secondFocus())
    self.assertPoints(self.myMajorStart.coordinates(), self.myEllipse.majorAxisNegative())
    self.assertPoints(self.myMajorEnd.coordinates(), self.myEllipse.majorAxisPositive())
    self.assertPoints(self.myMajorAxis.startPoint(), self.myEllipse.majorAxisNegative())
    self.assertPoints(self.myMajorAxis.endPoint(), self.myEllipse.majorAxisPositive())
    self.assertPoints(self.myMinorStart.coordinates(), self.myEllipse.minorAxisNegative())
    self.assertPoints(self.myMinorEnd.coordinates(), self.myEllipse.minorAxisPositive())
    self.assertPoints(self.myMinorAxis.startPoint(), self.myEllipse.minorAxisNegative())
    self.assertPoints(self.myMinorAxis.endPoint(), self.myEllipse.minorAxisPositive())
    model.testNbSubFeatures(self.mySketch, "SketchPoint", 7)
    model.testNbSubFeatures(self.mySketch, "SketchLine", 3)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", 1)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidenceInternal", 11)


  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

  def assertPoints(self, thePoint1, thePoint2):
    self.assertAlmostEqual(thePoint1.x(), thePoint2.x())
    self.assertAlmostEqual(thePoint1.y(), thePoint2.y())


  def test_collinear_major_axis(self):
    """ Test 1. Make major axis of ellipse collinear with OX
    """
    self.mySketch.setCollinear(self.myOX.result(), self.myMajorAxis.result())
    self.myDOF -= 2
    model.do()
    self.assertAlmostEqual(self.myMajorAxis.startPoint().y(), 0)
    self.assertAlmostEqual(self.myMajorAxis.endPoint().y(), 0)
    self.assertAlmostEqual(self.myMajorStart.coordinates().y(), 0)
    self.assertAlmostEqual(self.myMajorEnd.coordinates().y(), 0)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCollinear", 1)

  def test_collinear_minor_axis(self):
    """ Test 2. Make minor axis of ellipse collinear with OX
    """
    self.mySketch.setCollinear(self.myOX.result(), self.myMinorAxis.result())
    self.myDOF -= 2
    model.end()
    # solver shows wrong result
    assert(self.mySketch.solverError() != ""), "PlaneGCS limitation: if you see this message, then PlaneGCS has solved the set of constraints correctly"
    model.undo()

    model.begin()
    self.mySketch.move(self.myMinorStart.coordinates(), 0, 70)
    self.mySketch.move(self.myMinorEnd.coordinates(), 50, 70)
    model.do()
    self.mySketch.setCollinear(self.myOX.result(), self.myMinorAxis.result())
    model.do()
    self.assertAlmostEqual(self.myMinorAxis.startPoint().y(), 0)
    self.assertAlmostEqual(self.myMinorAxis.endPoint().y(), 0)
    self.assertAlmostEqual(self.myMinorStart.coordinates().y(), 0)
    self.assertAlmostEqual(self.myMinorEnd.coordinates().y(), 0)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCollinear", 1)

  def test_collinear_ellipse_axes(self):
    """ Test 3. Set collinear axes of an ellipse. Check conflicting constraints.
    """
    self.mySketch.setCollinear(self.myEllipse.majorAxis(), self.myEllipse.minorAxis())
    model.end()
    self.assertNotEqual(self.mySketch.solverError(), "")
    model.undo()
    model.begin()


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    assert model.checkPythonDump()
