# Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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
    Test creation of B-spline curve
"""

import unittest
from salome.shaper import model

from GeomAPI import *
from SketchAPI import *

__updated__ = "2020-01-17"

class TestBSpline(unittest.TestCase):
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
    model.testNbSubFeatures(self.mySketch, "SketchBSpline", self.myNbSplines)


  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)


  def test_bspline_by_coordinates(self):
    """ Test 1. Create B-spline curve by coordinates of its poles
    """
    self.mySpline = self.mySketch.addSpline(poles = self.myPolesCoordinates)
    self.myDOF += len(self.myPolesCoordinates) * 2
    self.myNbSplines += 1
    model.do()

    assert(self.mySpline.feature())
    assert(self.mySpline.feature().error() == "")
    assert(self.mySpline.degree().value() == self.myDegree)

  def test_bspline_by_poles(self):
    """ Test 2. Create B-spline curve by poles
    """
    self.mySpline = self.mySketch.addSpline(poles = self.myPoles)
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
    self.mySpline = self.mySketch.addSpline(degree = self.myDegree, poles = self.myPoles)
    self.myDOF += len(self.myPoles) * 2
    self.myNbSplines += 1
    model.do()

    assert(self.mySpline.feature())
    assert(self.mySpline.feature().error() == "")
    assert(self.mySpline.degree().value() == self.myDegree)

  def test_bspline_by_poles_and_weights(self):
    """ Test 4. Create B-spline curve by poles and weights
    """
    self.mySpline = self.mySketch.addSpline(poles = self.myPoles, weights = [1, 2, 3, 2, 1])
    self.myDOF += len(self.myPoles) * 2
    self.myNbSplines += 1
    model.do()

    assert(self.mySpline.feature())
    assert(self.mySpline.feature().error() == "")
    assert(self.mySpline.degree().value() == self.myDegree)

  def test_bspline_by_parametric(self):
    """ Test 5. Create B-spline curve by whole set of parameters
    """
    self.myDegree = 5
    self.myPolesCoordinates = [(-79.8578274581199, 75.5284518447357),
                               (-64.6205376770376, 62.7428476092882),
                               (-49.3832478959552, 49.9572433738407),
                               (-34.1459581148729, 37.1716391383932),
                               (-18.9086683337906, 24.3860349029457),
                               (-3.55842111132817, 11.5056481200973),
                               (-3.37993197286247, 11.42995541724),
                               (-3.1778022626919, 11.4565662984205),
                               (-3.02498570721059, 11.575876223351),
                               (8.46852511720001, 27.9903107977019),
                               (19.8774589601206, 44.2839569245217),
                               (31.2863928030413, 60.5776030513415),
                               (42.6953266459619, 76.8712491781612),
                               (54.1042604888826, 93.164895304981)
                              ]
    self.mySpline = self.mySketch.addSpline(degree = self.myDegree,
                                            poles = self.myPolesCoordinates,
                                            weights = [1, 1, 1, 1, 1, 1, 0.957903314642061, 0.95790331464206, 1, 1, 1, 1, 1, 1],
                                            knots = [-494.543457494654, 500, 507.372773368102, 1501.91623086297],
                                            multiplicities = [6, 4, 4, 6])
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
    self.mySpline = self.mySketch.addSpline(poles = self.myPoles[:2])
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
    self.mySpline = self.mySketch.addSpline(poles = self.myPoles[:3])
    self.myDOF += 6
    self.myNbSplines += 1
    model.do()

    assert(self.mySpline.feature())
    assert(self.mySpline.feature().error() == "")
    assert(self.mySpline.degree().value() == self.myDegree)

  def test_bspline_with_poles(self):
    """ Test 8. Create B-spline curve and points coincident with its poles
    """
    self.mySpline = self.mySketch.addSpline(poles = self.myPoles)
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
    self.mySpline = self.mySketch.addSpline(poles = self.myPoles)
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
