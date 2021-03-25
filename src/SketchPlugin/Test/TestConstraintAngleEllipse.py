# Copyright (C) 2019-2021  CEA/DEN, EDF R&D
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
    Test constraint "Angle" applied for ellipse's axes
"""

import unittest
import math

from salome.shaper import model

from GeomAPI import *
from SketchAPI import *

__updated__ = "2019-09-12"

class TestAngleEllipse(unittest.TestCase):
  def setUp(self):
    axisStart = GeomAPI_Pnt2d(30., 60.)
    axisEnd = GeomAPI_Pnt2d(80., 50.)
    passedPoint = GeomAPI_Pnt2d(60., 60.)
    self.myAngle = 60

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

  def assertAngle(self, theStart, theEnd, theAngle):
    dirX = (theEnd.x() - theStart.x()) / math.hypot(theEnd.x() - theStart.x(), theEnd.y() - theStart.y())
    self.assertAlmostEqual(math.fabs(dirX), math.cos(theAngle * math.pi / 180))


  def test_angle_major_axis(self):
    """ Test 1. Make angle between major axis of ellipse and OX
    """
    self.mySketch.setAngle(self.myOX.result(), self.myMajorAxis.result(), self.myAngle)
    self.myDOF -= 1
    model.do()
    self.assertAngle(self.myMajorAxis.startPoint(), self.myMajorAxis.endPoint(), self.myAngle)
    self.assertAngle(self.myMajorStart.coordinates(), self.myMajorEnd.coordinates(), self.myAngle)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintAngle", 1)

  def test_angle_minor_axis(self):
    """ Test 2. Make angle between minor axis of ellipse and OX
    """
    self.mySketch.setAngle(self.myOX.result(), self.myMinorAxis.result(), self.myAngle)
    self.myDOF -= 1
    model.do()
    self.assertAngle(self.myMinorAxis.startPoint(), self.myMinorAxis.endPoint(), self.myAngle)
    self.assertAngle(self.myMinorStart.coordinates(), self.myMinorEnd.coordinates(), self.myAngle)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintAngle", 1)

  def test_angle_ellipse_axes(self):
    """ Test 3. Set angle between axes of an ellipse. Check conflicting constraints.
    """
    self.mySketch.setAngle(self.myMajorAxis.result(), self.myMinorAxis.result(), self.myAngle)
    model.end()
    self.assertNotEqual(self.mySketch.solverError(), "")
    model.undo()
    model.begin()


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    assert model.checkPythonDump()
