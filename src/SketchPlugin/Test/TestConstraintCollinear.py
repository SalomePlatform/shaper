## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

"""
    TestConstraintCollinear.py
    Unit test of SketchPlugin_ConstraintCollinear class

    SketchPlugin_ConstraintCollinear
        static const std::string MY_CONSTRAINT_COLLINEAR_ID("SketchConstraintCollinear");
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());

"""

#=========================================================================
# Initialization of the test
#=========================================================================

from GeomDataAPI import *
from ModelAPI import *
import math
import unittest
from salome.shaper import model

__updated__ = "2017-03-06"

class TestConstraintCollinear(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myTolerance = 1.e-6
    self.myDOF = 0

  def tearDown(self):
    model.end()
    assert(model.checkPythonDump())

  def checkVectorCollinearity(self, theX1, theY1, theX2, theY2):
    aLen1 = math.hypot(theX1, theY1)
    aLen2 = math.hypot(theX2, theY2)
    aDot = theX1 * theX2 + theY1 * theY2
    self.assertTrue(math.fabs(math.fabs(aDot) - aLen1 * aLen2) < self.myTolerance**2, "Vectors ({0}, {1}) and ({2}, {3}) do not collinear".format(theX1, theY1, theX2, theY2))

  def checkLineCollinearity(self, theLine1, theLine2):
    aStartPoint1 = theLine1.startPoint()
    aEndPoint1   = theLine1.endPoint()
    aStartPoint2 = theLine2.startPoint()
    aEndPoint2   = theLine2.endPoint()

    aDir1x, aDir1y = aEndPoint1.x() - aStartPoint1.x(), aEndPoint1.y() - aStartPoint1.y()
    aDir2x, aDir2y = aEndPoint2.x() - aStartPoint1.x(), aEndPoint2.y() - aStartPoint1.y()
    aDir3x, aDir3y = aStartPoint2.x() - aStartPoint1.x(), aStartPoint2.y() - aStartPoint1.y()
    self.checkVectorCollinearity(aDir1x, aDir1y, aDir2x, aDir2y)
    self.checkVectorCollinearity(aDir1x, aDir1y, aDir3x, aDir3y)

  def moveLineAndCheckCollinearity(self, theLine1, theLine2):
    deltaX = deltaY = 10.

    theLine1.startPoint().setValue(theLine1.startPoint().x() + deltaX, theLine1.startPoint().y() + deltaY)
    model.do()
    self.checkLineCollinearity(theLine1, theLine2)
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

    theLine1.endPoint().setValue(theLine1.endPoint().x() - deltaX, theLine1.endPoint().y() - deltaY)
    model.do()
    self.checkLineCollinearity(theLine1, theLine2)
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

  def test_collinear_base(self):
    """ Test 1. Collinearity two independent lines
    """
    aSketchLineA = self.mySketch.addLine(0., 25., 85., 25.)
    aSketchLineB = self.mySketch.addLine(0., 50., 80., 75.)
    self.myDOF += 8
    self.assertEqual(model.dof(self.mySketch), self.myDOF)
    self.mySketch.setCollinear(aSketchLineA, aSketchLineB)
    self.myDOF -= 2
    model.do()
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

    self.checkLineCollinearity(aSketchLineA, aSketchLineB)
    self.moveLineAndCheckCollinearity(aSketchLineA, aSketchLineB)

  def test_collinear_connected_lines(self):
    """ Test 2. Collinearity of two lines in polyline
    """
    aSketchLineA = self.mySketch.addLine(10., 20., 30., 40.)
    aSketchLineB = self.mySketch.addLine(30., 40., 30., 70.)
    self.myDOF += 8
    self.assertEqual(model.dof(self.mySketch), self.myDOF)
    self.mySketch.setCoincident(aSketchLineA.endPoint(), aSketchLineB.startPoint())
    self.myDOF -= 2
    model.do()
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

    self.mySketch.setCollinear(aSketchLineA, aSketchLineB)
    self.myDOF -= 1
    model.do()
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

    self.checkLineCollinearity(aSketchLineA, aSketchLineB)
    self.moveLineAndCheckCollinearity(aSketchLineA, aSketchLineB)

  def test_collinear_point_on_line(self):
    """ Test 3. Collinearity for line which extremity is coincident with other line
    """
    aSketchLineA = self.mySketch.addLine(10., 20., 30., 40.)
    aSketchLineB = self.mySketch.addLine(20., 40., 30., 70.)
    self.myDOF += 8
    self.assertEqual(model.dof(self.mySketch), self.myDOF)
    self.mySketch.setCoincident(aSketchLineA.result(), aSketchLineB.startPoint())
    self.myDOF -= 1
    model.do()
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

    self.mySketch.setCollinear(aSketchLineA, aSketchLineB)
    self.myDOF -= 1
    model.do()
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

    self.checkLineCollinearity(aSketchLineA, aSketchLineB)
    self.moveLineAndCheckCollinearity(aSketchLineA, aSketchLineB)

  def test_already_collinear(self):
    """ Test 4. Collinearity two lines connected by extremities to each other
    """
    aSketchLineA = self.mySketch.addLine(10., 20., 40., 50.)
    aSketchLineB = self.mySketch.addLine(20., 40., 40., 70.)
    self.myDOF += 8
    self.assertEqual(model.dof(self.mySketch), self.myDOF)
    self.mySketch.setCoincident(aSketchLineA.result(), aSketchLineB.startPoint())
    self.mySketch.setCoincident(aSketchLineA.endPoint(), aSketchLineB.result())
    self.myDOF -= 2
    model.do()
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

    aCollinear = self.mySketch.setCollinear(aSketchLineA, aSketchLineB)
    model.do()
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

    # check error message and remove non valid constraint
    self.assertNotEqual(self.mySketch.solverError().value(), "")
    self.myDocument.removeFeature(aCollinear.feature())
    model.do()
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

  def test_already_collinear2(self):
    """ Test 5. Collinearity of two lines when one line is fully on other
    """
    aSketchLineA = self.mySketch.addLine(10., 20., 30., 40.)
    aSketchLineB = self.mySketch.addLine(20., 40., 30., 70.)
    self.myDOF += 8
    self.assertEqual(model.dof(self.mySketch), self.myDOF)
    self.mySketch.setCoincident(aSketchLineA.result(), aSketchLineB.startPoint())
    self.mySketch.setCoincident(aSketchLineA.result(), aSketchLineB.endPoint())
    self.myDOF -= 2
    model.do()
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

    aCollinear = self.mySketch.setCollinear(aSketchLineA, aSketchLineB)
    model.do()
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

    # check error message and remove non valid constraint
    self.assertNotEqual(self.mySketch.solverError().value(), "")
    self.myDocument.removeFeature(aCollinear.feature())
    model.do()
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
#=========================================================================
# End of test
#=========================================================================
