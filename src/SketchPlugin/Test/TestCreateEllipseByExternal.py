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
    Test creation of ellipse by external feature
"""

import unittest
from salome.shaper import model

from GeomAPI import *
from SketchAPI import *

__updated__ = "2019-09-12"

# reference data
CENTER_POINT = GeomAPI_Pnt2d(50., 50.)
FOCUS_POINT = GeomAPI_Pnt2d(70., 60.)
MINOR_RADIUS = 10.

class TestEllipse(unittest.TestCase):
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


  def test_ellipse_by_external_name(self):
    """ Test 1. Create ellipse by name of external edge
    """
    self.myEllipse = self.mySketch.addEllipse("Sketch_1/SketchEllipse_1")
    model.do()

    # check ellipse parameters
    anEllipse = self.myEllipse.defaultResult().shape().edge().ellipse()
    self.checkPointsEqual(anEllipse.center(), CENTER_POINT)
    self.checkPointsEqual(anEllipse.firstFocus(), FOCUS_POINT)
    self.assertAlmostEqual(anEllipse.minorRadius(), MINOR_RADIUS)

  def test_ellipse_by_external_selection(self):
    """ Test 2. Create ellipse by selected edge
    """
    self.myEllipse = self.mySketch.addEllipse(ELLIPSE.results()[-1])
    model.do()

    # check ellipse parameters
    anEllipse = self.myEllipse.defaultResult().shape().edge().ellipse()
    self.checkPointsEqual(anEllipse.center(), CENTER_POINT)
    self.checkPointsEqual(anEllipse.firstFocus(), FOCUS_POINT)
    self.assertAlmostEqual(anEllipse.minorRadius(), MINOR_RADIUS)


if __name__ == "__main__":
    model.begin()
    aDocument = model.moduleDocument()
    aSketch = model.addSketch(aDocument, model.defaultPlane("XOY"))
    ELLIPSE = aSketch.addEllipse(CENTER_POINT, FOCUS_POINT, MINOR_RADIUS)
    model.end()

    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    assert model.checkPythonDump()
