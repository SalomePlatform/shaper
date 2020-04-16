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
    Test creation of elliptic arc by external feature
"""

import unittest
from salome.shaper import model

from GeomAPI import *
from SketchAPI import *

__updated__ = "2019-10-02"

# reference data
CENTER_POINT = GeomAPI_Pnt2d(50., 50.)
MAJOR_AXIS_POINT = GeomAPI_Pnt2d(70., 60.)
START_POINT = GeomAPI_Pnt2d(60., 65.)
END_POINT = GeomAPI_Pnt2d(60., 42.535751)
ARC_LENGTH_1 = 0
ARC_LENGTH_2 = 0

class TestEllipticArcByExternal(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myDOF = 0

  def tearDown(self):
    self.checkDOF()
    model.end()


  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

  def checkPointsEqual(self, thePoint1, thePoint2):
    self.assertAlmostEqual(thePoint1.x(), thePoint2.x(), 5)
    self.assertAlmostEqual(thePoint1.y(), thePoint2.y(), 5)


  def test_elliptic_arc_by_external_name_1(self):
    """ Test 1. Create elliptic arc by name of external edge (direct)
    """
    self.myEllipse = self.mySketch.addEllipticArc("Sketch_1/SketchEllipticArc_1")
    model.do()

    # check ellipse parameters
    anArcEdge = self.myEllipse.defaultResult().shape().edge()
    anEllipse = anArcEdge.ellipse()
    self.checkPointsEqual(anEllipse.center(), CENTER_POINT)
    self.checkPointsEqual(self.myEllipse.majorAxisPositive(), MAJOR_AXIS_POINT)
    self.checkPointsEqual(self.myEllipse.startPoint(), START_POINT)
    self.checkPointsEqual(self.myEllipse.endPoint(), END_POINT)
    self.assertAlmostEqual(anArcEdge.length(), ARC_LENGTH_1)

  def test_elliptic_arc_by_external_name_2(self):
    """ Test 2. Create elliptic arc by name of external edge (reversed)
    """
    self.myEllipse = self.mySketch.addEllipticArc("Sketch_1/SketchEllipticArc_2")
    model.do()

    # check ellipse parameters
    anArcEdge = self.myEllipse.defaultResult().shape().edge()
    anEllipse = anArcEdge.ellipse()
    self.checkPointsEqual(anEllipse.center(), CENTER_POINT)
    self.checkPointsEqual(self.myEllipse.majorAxisPositive(), MAJOR_AXIS_POINT)
    self.checkPointsEqual(anArcEdge.firstPoint(), END_POINT)
    self.checkPointsEqual(anArcEdge.lastPoint(), START_POINT)
    self.assertAlmostEqual(anArcEdge.length(), ARC_LENGTH_2)

  def test_elliptic_arc_by_external_selection_1(self):
    """ Test 3. Create elliptic arc by selected edge (direct)
    """
    self.myEllipse = self.mySketch.addEllipticArc(ELLIPTIC_ARC_1.results()[-1])
    model.do()

    # check ellipse parameters
    anArcEdge = self.myEllipse.defaultResult().shape().edge()
    anEllipse = anArcEdge.ellipse()
    self.checkPointsEqual(anEllipse.center(), CENTER_POINT)
    self.checkPointsEqual(self.myEllipse.majorAxisPositive(), MAJOR_AXIS_POINT)
    self.checkPointsEqual(self.myEllipse.startPoint(), START_POINT)
    self.checkPointsEqual(self.myEllipse.endPoint(), END_POINT)
    self.assertAlmostEqual(anArcEdge.length(), ARC_LENGTH_1)

  def test_elliptic_arc_by_external_selection_2(self):
    """ Test 4. Create elliptic arc by selected edge (reversed)
    """
    self.myEllipse = self.mySketch.addEllipticArc(ELLIPTIC_ARC_2.results()[-1])
    model.do()

    # check ellipse parameters
    anArcEdge = self.myEllipse.defaultResult().shape().edge()
    anEllipse = anArcEdge.ellipse()
    self.checkPointsEqual(anEllipse.center(), CENTER_POINT)
    self.checkPointsEqual(self.myEllipse.majorAxisPositive(), MAJOR_AXIS_POINT)
    self.checkPointsEqual(self.myEllipse.startPoint(), END_POINT)
    self.checkPointsEqual(self.myEllipse.endPoint(), START_POINT)
    self.assertAlmostEqual(anArcEdge.length(), ARC_LENGTH_2)


if __name__ == "__main__":
    model.begin()
    aDocument = model.moduleDocument()
    aSketch = model.addSketch(aDocument, model.defaultPlane("XOY"))
    aSketch.addEllipticArc(CENTER_POINT, MAJOR_AXIS_POINT, START_POINT, END_POINT, False)
    model.do()

    ELLIPTIC_ARC_1 = SketchAPI_EllipticArc(model.lastSubFeature(aSketch, "SketchEllipticArc"))
    ARC_LENGTH_1 = ELLIPTIC_ARC_1.defaultResult().shape().edge().length()

    aSketch.addEllipticArc(CENTER_POINT, MAJOR_AXIS_POINT, START_POINT, END_POINT, True)
    model.end()

    ELLIPTIC_ARC_2 = SketchAPI_EllipticArc(model.lastSubFeature(aSketch, "SketchEllipticArc"))
    ARC_LENGTH_2 = ELLIPTIC_ARC_2.defaultResult().shape().edge().length()

    # redefine end point
    END_POINT = ELLIPTIC_ARC_2.endPoint()

    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    assert model.checkPythonDump()
