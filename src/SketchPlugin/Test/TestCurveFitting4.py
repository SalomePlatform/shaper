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
    Test creation of B-spline curve passing through the list of points
"""

import math
import unittest

from salome.shaper import model

from GeomAPI import *
from ModelAPI import *
from ModelHighAPI import *
from SketchAPI import *
from GeomAlgoAPI import GeomAlgoAPI_ShapeTools as shapetools

__updated__ = "2020-06-28"

TOLERANCE = 1.e-7
POINT_NAMES = []

class TestApproximationExternal(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myPoints = []
    self.myPointRefs = []
    for name in POINT_NAMES:
      proj = self.mySketch.addProjection(model.selection("VERTEX", name), False)
      self.myPoints.append(SketchAPI_Point(proj.createdFeature()))
      self.myPointRefs.append(self.myPoints[-1].coordinates())
    self.myDOF = 0
    self.myNbPoints = len(self.myPoints)
    self.myNbLines = 0
    self.myNbSplines = 0
    self.myNbPeriodicSplines = 0

  def tearDown(self):
    self.checkDOF()
    model.end()
    model.testNbSubFeatures(self.mySketch, "SketchPoint", self.myNbPoints)
    model.testNbSubFeatures(self.mySketch, "SketchLine", self.myNbLines)
    model.testNbSubFeatures(self.mySketch, "SketchBSpline", self.myNbSplines)
    model.testNbSubFeatures(self.mySketch, "SketchBSplinePeriodic", self.myNbPeriodicSplines)


  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

  def checkPointsOnCurve(self, theCurve, theTolerance = 1.e-3):
    for p in self.myPoints:
      shape = p.defaultResult().shape()
      self.assertTrue(shapetools.minimalDistance(shape, theCurve) < theTolerance)


  def test_approximation_default(self):
    """ Test 1. Create B-spline curve approximating a set of points with the default precision
    """
    self.mySpline = self.mySketch.addApproximation(self.myPointRefs)
    edge = GeomAPI_Edge(self.mySpline.defaultResult().shape())
    curve = GeomAPI_BSpline(GeomAPI_Curve(edge))
    self.myDOF += len(curve.poles()) * 2 - 4
    self.myNbSplines += 1
    model.do()

    self.assertTrue(self.mySpline.feature())
    self.assertTrue(self.mySpline.feature().error() == "")
    self.assertTrue(self.mySpline.degree().value() == 4)
    self.assertTrue(curve.isPeriodic() == False)
    EXPECTED_LEN = 189.17832318
    self.assertTrue(math.fabs(edge.length() - EXPECTED_LEN) < TOLERANCE, "Curve length: {}; expected: {}".format(edge.length(), EXPECTED_LEN))
    # check points lying of the created curve
    self.checkPointsOnCurve(edge)

  def test_approximation_custom(self):
    """ Test 2. Create B-spline curve approximating a set of points with the custom precision
    """
    PRECISION = 10
    self.mySpline = self.mySketch.addApproximation(self.myPointRefs, precision = PRECISION)
    edge = GeomAPI_Edge(self.mySpline.defaultResult().shape())
    curve = GeomAPI_BSpline(GeomAPI_Curve(edge))
    self.myDOF += len(curve.poles()) * 2 - 4
    self.myNbSplines += 1
    model.do()

    self.assertTrue(self.mySpline.feature())
    self.assertTrue(self.mySpline.feature().error() == "")
    self.assertTrue(self.mySpline.degree().value() == 3)
    self.assertTrue(curve.isPeriodic() == False)
    EXPECTED_LEN = 166.97400096
    self.assertTrue(math.fabs(edge.length() - EXPECTED_LEN) < TOLERANCE, "Curve length: {}; expected: {}".format(edge.length(), EXPECTED_LEN))
    # check points lying of the created curve
    self.checkPointsOnCurve(edge, PRECISION)

  def test_approximation_periodic_default(self):
    """ Test 3. Create periodic B-spline curve approximating set of points with the default precision
    """
    self.mySpline = self.mySketch.addApproximation(self.myPointRefs, periodic = True)
    edge = GeomAPI_Edge(self.mySpline.defaultResult().shape())
    curve = GeomAPI_BSpline(GeomAPI_Curve(edge))
    self.myDOF += len(curve.poles()) * 2
    self.myNbPeriodicSplines += 1
    model.do()

    self.assertTrue(self.mySpline.feature())
    self.assertTrue(self.mySpline.feature().error() == "")
    self.assertTrue(self.mySpline.degree().value() == 3)
    self.assertTrue(curve.isPeriodic() == True)
    EXPECTED_LEN = 390.98013747
    self.assertTrue(math.fabs(edge.length() - EXPECTED_LEN) < TOLERANCE, "Curve length: {}; expected: {}".format(edge.length(), EXPECTED_LEN))
    # check points lying of the created curve
    self.checkPointsOnCurve(edge)

  def test_approximation_periodic_custom(self):
    """ Test 4. Create periodic B-spline curve approximating set of points with the custom precision
    """
    PRECISION = 10
    self.mySpline = self.mySketch.addApproximation(self.myPointRefs, precision = PRECISION, periodic = True)
    edge = GeomAPI_Edge(self.mySpline.defaultResult().shape())
    curve = GeomAPI_BSpline(GeomAPI_Curve(edge))
    self.myDOF += len(curve.poles()) * 2
    self.myNbPeriodicSplines += 1
    model.do()

    self.assertTrue(self.mySpline.feature())
    self.assertTrue(self.mySpline.feature().error() == "")
    self.assertTrue(self.mySpline.degree().value() == 3)
    self.assertTrue(curve.isPeriodic() == True)
    EXPECTED_LEN = 390.98013747
    self.assertTrue(math.fabs(edge.length() - EXPECTED_LEN) < TOLERANCE, "Curve length: {}; expected: {}".format(edge.length(), EXPECTED_LEN))
    # check points lying of the created curve
    self.checkPointsOnCurve(edge, PRECISION)

  def test_approximation_closed_default(self):
    """ Test 5. Create closed but not periodic B-spline curve approximating set of points with the default precision
    """
    self.mySpline = self.mySketch.addApproximation(self.myPointRefs, closed = True)
    edge = GeomAPI_Edge(self.mySpline.defaultResult().shape())
    curve = GeomAPI_BSpline(GeomAPI_Curve(edge))
    self.myDOF += len(curve.poles()) * 2 - 4
    self.myNbSplines += 1
    model.do()

    self.assertTrue(self.mySpline.feature())
    self.assertTrue(self.mySpline.feature().error() == "")
    self.assertTrue(self.mySpline.degree().value() == 5)
    self.assertTrue(curve.isPeriodic() == False)
    self.assertTrue(curve.poles()[0].distance(curve.poles()[-1]) < TOLERANCE)
    EXPECTED_LEN = 413.18489109
    self.assertTrue(math.fabs(edge.length() - EXPECTED_LEN) < TOLERANCE, "Curve length: {}; expected: {}".format(edge.length(), EXPECTED_LEN))
    # check points lying of the created curve
    self.checkPointsOnCurve(edge)

  def test_approximation_closed_custom(self):
    """ Test 6. Create closed but not periodic B-spline curve approximating set of points with the custom precision
    """
    PRECISION = 10
    self.mySpline = self.mySketch.addApproximation(self.myPointRefs, precision = PRECISION, closed = True)
    edge = GeomAPI_Edge(self.mySpline.defaultResult().shape())
    curve = GeomAPI_BSpline(GeomAPI_Curve(edge))
    self.myDOF += len(curve.poles()) * 2 - 4
    self.myNbSplines += 1
    model.do()

    self.assertTrue(self.mySpline.feature())
    self.assertTrue(self.mySpline.feature().error() == "")
    self.assertTrue(self.mySpline.degree().value() == 3)
    self.assertTrue(curve.isPeriodic() == False)
    self.assertTrue(curve.poles()[0].distance(curve.poles()[-1]) < TOLERANCE)
    EXPECTED_LEN = 276.27724118
    self.assertTrue(math.fabs(edge.length() - EXPECTED_LEN) < TOLERANCE, "Curve length: {}; expected: {}".format(edge.length(), EXPECTED_LEN))
    # check points lying of the created curve
    self.checkPointsOnCurve(edge, PRECISION)

  def test_approximation_reorder(self):
    """ Test 7. Check reordering of points works properly
    """
    PRECISION = 10
    model.do()
    # use low-level API to create feature
    curveFitting = featureToCompositeFeature(self.mySketch.feature()).addFeature("SketchCurveFitting")
    curveFitting.string("type").setValue("approximation_type")
    curveFitting.real("precision").setValue(PRECISION)
    curveFitting.boolean("periodic").setValue(False)
    curveFitting.boolean("closed").setValue(False)
    pointsAttr = curveFitting.refattrlist("points")
    for ind in [0, 3, 4, 2, 1]:
      pointsAttr.append(self.myPointRefs[ind])
    # perform reordering
    curveFitting.customAction("reorder_points")
    model.do()

    self.mySpline = SketchAPI_BSpline(model.lastSubFeature(self.mySketch, "SketchBSpline"))
    edge = GeomAPI_Edge(self.mySpline.defaultResult().shape())
    curve = GeomAPI_BSpline(GeomAPI_Curve(edge))
    self.myDOF += len(curve.poles()) * 2 - 4
    self.myNbSplines += 1
    model.do()

    self.assertTrue(self.mySpline.feature())
    self.assertTrue(self.mySpline.feature().error() == "")
    self.assertTrue(self.mySpline.degree().value() == 3)
    self.assertTrue(curve.isPeriodic() == False)
    EXPECTED_LEN = 166.97400096
    self.assertTrue(math.fabs(edge.length() - EXPECTED_LEN) < TOLERANCE, "Curve length: {}; expected: {}".format(edge.length(), EXPECTED_LEN))
    # check points lying of the created curve
    self.checkPointsOnCurve(edge, PRECISION)


if __name__ == "__main__":
    # Create a sketch with the external points
    model.begin()
    document = model.moduleDocument()
    coordinates = [(50., 50.), (70., 70.), (80., 30.), (50., 10.), (10., -30.)]
    for p in coordinates:
      point3D = model.addPoint(document, p[0], p[1], 0)
      POINT_NAMES.append(point3D.name())
    model.end()

    # Run testing
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    assert model.checkPythonDump()
