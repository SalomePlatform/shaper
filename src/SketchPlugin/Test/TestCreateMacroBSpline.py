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
    Test creation of B-spline curve by references to another features
"""

import unittest
from salome.shaper import model

from GeomAPI import *
from GeomDataAPI import *
from ModelAPI import *
from SketchAPI import *

__updated__ = "2020-01-31"

class TestMacroBSpline(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myPoint = self.mySketch.addPoint(50., 50.)
    self.myLine = self.mySketch.addLine(30., -1., 70., 19.)

    self.myPoles = [[GeomAPI_Pnt2d(50., 50.), self.myPoint.coordinates()],
                    GeomAPI_Pnt2d(70., 70.),
                    (80., 30.),
                    [GeomAPI_Pnt2d(50., 10.), self.myLine.result()],
                    GeomAPI_Pnt2d(20., 10.)
                   ]

    self.myDegree = 3;
    self.myDOF = 6
    self.myNbPoints = 1
    self.myNbLines = 1
    self.myNbSplines = 0
    self.myNbSplinesP = 0
    self.myNbCoincidences = 0
    self.myNbInternal = 0

  def tearDown(self):
    self.checkDOF()
    model.end()
    model.testNbSubFeatures(self.mySketch, "SketchPoint", self.myNbPoints)
    model.testNbSubFeatures(self.mySketch, "SketchLine", self.myNbLines)
    model.testNbSubFeatures(self.mySketch, "SketchBSpline", self.myNbSplines)
    model.testNbSubFeatures(self.mySketch, "SketchBSplinePeriodic", self.myNbSplinesP)
    model.testNbSubFeatures(self.mySketch, "SketchMacroBSpline", 0)
    model.testNbSubFeatures(self.mySketch, "SketchMacroBSplinePeriodic", 0)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", self.myNbCoincidences)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidenceInternal", self.myNbInternal)


  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)


  def test_bspline_non_periodic(self):
    """ Test 1. Create B-spline curve by poles and references to other shapes
    """
    self.mySpline = self.mySketch.addSpline(poles = self.myPoles)
    self.myDOF += len(self.myPoles) * 2 - 3
    self.myNbSplines += 1
    self.myNbPoints += len(self.myPoles)
    self.myNbLines += len(self.myPoles) - 1
    self.myNbCoincidences += 2
    self.myNbInternal += len(self.myPoles) * 3 - 2
    model.do()

    assert(self.mySpline.feature())
    assert(self.mySpline.feature().error() == "")
    assert(self.mySpline.degree().value() == self.myDegree)

  def test_bspline_periodic(self):
    """ Test 2. Create periodic B-spline curve by poles and references to other shapes
    """
    self.mySpline = self.mySketch.addSpline(poles = self.myPoles, periodic = True)
    self.myDOF += len(self.myPoles) * 2 - 3
    self.myNbSplinesP += 1
    self.myNbPoints += len(self.myPoles)
    self.myNbLines += len(self.myPoles)
    self.myNbCoincidences += 2
    self.myNbInternal += len(self.myPoles) * 3
    model.do()

    assert(self.mySpline.feature())
    assert(self.mySpline.feature().error() == "")
    assert(self.mySpline.degree().value() == self.myDegree)

  def test_bspline_lowlevel(self):
    """ Test 3. Create macro B-spline on low-level to test attributeRefAttrList
    """
    model.end()
    session = ModelAPI_Session.get()
    sketch = featureToCompositeFeature(self.mySketch.feature())
    session.startOperation()
    bspline = sketch.addFeature("SketchMacroBSpline")
    poles = geomDataAPI_Point2DArray(bspline.attribute("poles"))
    weights = bspline.data().realArray("weights")
    polesRef = bspline.data().refattrlist("poles_ref")
    bspline.boolean("need_control_poly").setValue(False)

    poles.setSize(3); weights.setSize(3)
    poles.setPnt(0, GeomAPI_Pnt2d(50., 50.)); weights.setValue(0, 1.0); polesRef.append(self.myPoint.coordinates())
    poles.setPnt(1, GeomAPI_Pnt2d(50., 10.)); weights.setValue(1, 1.0); polesRef.append(self.myLine.defaultResult())
    poles.setPnt(2, GeomAPI_Pnt2d(20., 10.)); weights.setValue(2, 1.0); polesRef.append(None)

    assert(polesRef.isInList(self.myPoint.coordinates()))
    assert(not polesRef.isInList(self.myLine.startPoint()))
    assert(polesRef.isInList(self.myLine.defaultResult()))
    assert(not polesRef.isInList(self.myPoint.defaultResult()))

    assert(polesRef.isAttribute(0))
    assert(not polesRef.isAttribute(1))
    assert(not polesRef.isAttribute(2))
    assert(not polesRef.isAttribute(3))

    assert(polesRef.attribute(0) is not None)
    assert(polesRef.attribute(1) is None)
    assert(polesRef.attribute(2) is None)
    assert(polesRef.attribute(3) is None)
    assert(polesRef.object(0) is not None)
    assert(polesRef.object(1) is not None)
    assert(polesRef.object(2) is None)
    assert(polesRef.object(3) is None)

    polesRef.remove(self.myPoint.coordinates())
    polesRef.remove(self.myLine.defaultResult())
    assert(polesRef.size() == 1)
    polesRef.removeLast()

    polesRef.append(self.myPoint.coordinates())
    polesRef.append(None)
    polesRef.append(self.myLine.defaultResult())
    polesRef.removeLast()
    polesRef.append(self.myLine.defaultResult())

    session.finishOperation()
    model.begin()
    self.myDOF += 6
    self.myNbSplines += 1

if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    assert model.checkPythonDump()
