# Copyright (C) 2020-2021  CEA/DEN, EDF R&D
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
    Test creation of periodic B-spline curve
"""

import unittest
from salome.shaper import model

from GeomAPI import *
from SketchAPI import *

__updated__ = "2020-01-24"

class TestBSplinePeriodic(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myPoles = [GeomAPI_Pnt2d(50., 50.), GeomAPI_Pnt2d(70., 70.), GeomAPI_Pnt2d(80., 30.), GeomAPI_Pnt2d(50., 10.), GeomAPI_Pnt2d(10., -30.)]
    self.myPolesCoordinates = [(50., 50.), (70., 70.), (80., 30.), (50., 10.), (10., -30.)]
    self.myDegree = 3;
    self.myDOF = 0
    self.myNbPoints = 0
    self.myNbLines = 0
    self.myNbSplines = 0

  def tearDown(self):
    self.checkDOF()
    model.end()
    model.testNbSubFeatures(self.mySketch, "SketchPoint", self.myNbPoints)
    model.testNbSubFeatures(self.mySketch, "SketchLine", self.myNbLines)
    model.testNbSubFeatures(self.mySketch, "SketchBSplinePeriodic", self.myNbSplines)


  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)


  def test_bspline_by_coordinates(self):
    """ Test 1. Create B-spline curve by coordinates of its poles
    """
    self.mySpline = self.mySketch.addSpline(poles = self.myPolesCoordinates, periodic = True)
    self.myDOF += len(self.myPolesCoordinates) * 2
    self.myNbSplines += 1
    model.do()

    assert(self.mySpline.feature())
    assert(self.mySpline.feature().error() == "")
    assert(self.mySpline.degree().value() == self.myDegree)

  def test_bspline_by_poles(self):
    """ Test 2. Create B-spline curve by poles
    """
    self.mySpline = self.mySketch.addSpline(poles = self.myPoles, periodic = True)
    self.myDOF += len(self.myPoles) * 2
    self.myNbSplines += 1
    model.do()

    assert(self.mySpline.feature())
    assert(self.mySpline.feature().error() == "")
    assert(self.mySpline.degree().value() == self.myDegree)

  def test_bspline_by_degree_and_poles(self):
    """ Test 3. Create B-spline curve by poles and degree
    """
    self.myDegree = 4
    self.mySpline = self.mySketch.addSpline(degree = self.myDegree, poles = self.myPoles, periodic = True)
    self.myDOF += len(self.myPoles) * 2
    self.myNbSplines += 1
    model.do()

    assert(self.mySpline.feature())
    assert(self.mySpline.feature().error() == "")
    assert(self.mySpline.degree().value() == self.myDegree)

  def test_bspline_by_poles_and_weights(self):
    """ Test 4. Create B-spline curve by poles and weights
    """
    self.mySpline = self.mySketch.addSpline(poles = self.myPoles, weights = [1, 2, 3, 2, 1], periodic = True)
    self.myDOF += len(self.myPoles) * 2
    self.myNbSplines += 1
    model.do()

    assert(self.mySpline.feature())
    assert(self.mySpline.feature().error() == "")
    assert(self.mySpline.degree().value() == self.myDegree)

  def test_bspline_by_parametric(self):
    """ Test 5. Create B-spline curve by whole set of parameters
    """
    self.myDegree = 3
    self.myPolesCoordinates = [(-10, 0), (-20, 20), (0, 10), (20, 20),
                               (10, 0), (20, -20), (0, -10), (-20, -20)
                              ]
    self.mySpline = self.mySketch.addSpline(degree = self.myDegree,
                                            poles = self.myPolesCoordinates,
                                            weights = [1, 1, 1, 1, 1, 1, 1, 1],
                                            knots = [0, 1, 2, 3, 4, 5, 6, 7, 8],
                                            multiplicities = [1, 1, 1, 1, 1, 1, 1, 1, 1],
                                            periodic = True)
    self.myDOF += len(self.myPolesCoordinates) * 2
    self.myNbSplines += 1
    model.do()

    assert(self.mySpline.feature())
    assert(self.mySpline.feature().error() == "")
    assert(self.mySpline.degree().value() == self.myDegree)

  def test_bspline_linear(self):
    """ Test 6. Create B-spline curve by 2 poles
    """
    self.myDegree = 1
    self.mySpline = self.mySketch.addSpline(poles = self.myPoles[:2], periodic = True)
    self.myDOF += 4
    self.myNbSplines += 1
    model.do()

    assert(self.mySpline.feature())
    assert(self.mySpline.feature().error() == "")
    assert(self.mySpline.degree().value() == self.myDegree)

  def test_bspline_parabola(self):
    """ Test 7. Create B-spline curve by 3 poles
    """
    self.myDegree = 2
    self.mySpline = self.mySketch.addSpline(poles = self.myPoles[:3], periodic = True)
    self.myDOF += 6
    self.myNbSplines += 1
    model.do()

    assert(self.mySpline.feature())
    assert(self.mySpline.feature().error() == "")
    assert(self.mySpline.degree().value() == self.myDegree)

  def test_bspline_with_poles(self):
    """ Test 8. Create B-spline curve and points coincident with its poles
    """
    self.mySpline = self.mySketch.addSpline(poles = self.myPoles, periodic = True)
    self.mySpline.controlPoles(regular = [0, 2], auxiliary = [1, 3])
    self.myDOF += len(self.myPoles) * 2
    self.myNbSplines += 1
    self.myNbPoints += 4
    model.do()

    assert(self.mySpline.feature())
    assert(self.mySpline.feature().error() == "")
    assert(self.mySpline.degree().value() == self.myDegree)

  def test_bspline_with_polygon(self):
    """ Test 9. Create B-spline curve and its control polygon
    """
    self.mySpline = self.mySketch.addSpline(poles = self.myPoles, periodic = True)
    self.mySpline.controlPolygon(regular = [0, 2], auxiliary = [1, 3])
    self.myDOF += len(self.myPoles) * 2
    self.myNbSplines += 1
    self.myNbLines += 4
    model.do()

    assert(self.mySpline.feature())
    assert(self.mySpline.feature().error() == "")
    assert(self.mySpline.degree().value() == self.myDegree)

if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    assert model.checkPythonDump()
