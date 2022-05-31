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
    Test projection without the reference to the source geometry.
"""

import unittest

from salome.shaper import model
from GeomAPI import *
from ModelAPI import *

__updated__ = "2020-07-07"

CURVES = []
PLANE = None

class TestProjectionWithoutRef(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(partSet, model.selection("FACE", PLANE.name()))
    self.myDOF = 0
    self.myNbPoints = 1
    self.myNbLines = 1
    self.myNbCircles = 0
    self.myNbArcs = 0
    self.myNbEllipses = 2
    self.myNbEllipticArcs = 2
    self.myNbSplines = 1
    self.myNbPeriodicSplines = 1
    self.myNbProjections = 0
    self.myNbFixedConstraints = 0
    self.myNbEdgesInSketch = 0

  def tearDown(self):
    self.checkDOF()
    model.end()
    model.testNbSubFeatures(self.mySketch, "SketchPoint", self.myNbPoints)
    model.testNbSubFeatures(self.mySketch, "SketchLine", self.myNbLines)
    model.testNbSubFeatures(self.mySketch, "SketchCircle", self.myNbCircles)
    model.testNbSubFeatures(self.mySketch, "SketchArc", self.myNbArcs)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", self.myNbEllipses)
    model.testNbSubFeatures(self.mySketch, "SketchEllipticArc", self.myNbEllipticArcs)
    model.testNbSubFeatures(self.mySketch, "SketchBSpline", self.myNbSplines)
    model.testNbSubFeatures(self.mySketch, "SketchBSplinePeriodic", self.myNbPeriodicSplines)
    model.testNbSubFeatures(self.mySketch, "SketchProjection", self.myNbProjections)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintRigid", self.myNbFixedConstraints)
    nbEdges = 0
    exp = GeomAPI_ShapeExplorer(self.mySketch.defaultResult().shape(), GeomAPI_Shape.EDGE)
    while exp.more(): nbEdges += 1; exp.next()
    self.assertEqual(self.myNbEdgesInSketch, nbEdges)

  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)


  def test_projection_withref_includeintoresult(self):
    """ Test 1. Projection with the reference to the original shapes. Projected curves are composed into the sketch result.
    """
    for c in CURVES:
      self.mySketch.addProjection(c, keepResult = True)
    self.myNbProjections = len(CURVES)
    self.myNbEdgesInSketch = len(CURVES) - 1

  def test_projection_withref_notincludeintoresult(self):
    """ Test 2. Projection with the reference to the original shapes. Projected curves are NOT included into the sketch result.
    """
    for c in CURVES:
      self.mySketch.addProjection(c, keepResult = False)
    self.myNbProjections = len(CURVES)

  def test_projection_withoutref_noconstraints(self):
    """ Test 3. Projection without the reference to the original shapes. No additional constraints applied.
    """
    for c in CURVES:
      self.mySketch.addProjection(c, keepRefToOriginal = False)
    model.do()
    self.myNbEdgesInSketch = len(CURVES) - 1
    self.myDOF += 2 + 4 + 5 + 7 + 5 + 7 + 6 * 2 + 6 * 2

  def test_projection_withoutref_fixed(self):
    """ Test 4. Projection without the reference to the original shapes. Additionally, Fixed constraints applied.
    """
    model.end()
    # use the low-level API to access the necessary attributes
    session = ModelAPI_Session.get()
    for c in CURVES:
      session.startOperation()
      proj = featureToCompositeFeature(self.mySketch.feature()).addFeature("SketchProjection")
      proj.boolean("IncludeToResult").setValue(False)
      proj.string("keep_reference").setValue("False")
      proj.boolean("make_fixed").setValue(True)
      c.fillAttribute(proj.selection("ExternalFeature"))
      session.finishOperation()
    self.myNbEdgesInSketch = len(CURVES) - 1
    self.myNbFixedConstraints = len(CURVES)
    model.begin()


if __name__ == "__main__":
    model.begin()
    partSet = model.moduleDocument()
    Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
    SketchPoint_1 = Sketch_1.addPoint(35, -40)
    CURVES.append(model.selection("VERTEX", Sketch_1.name() + "/" + SketchPoint_1.name()))
    SketchLine_1 = Sketch_1.addLine(20, -15, 40, 15)
    CURVES.append(model.selection("EDGE", Sketch_1.name() + "/" + SketchLine_1.name()))
    SketchCircle_1 = Sketch_1.addCircle(65, -30, 20)
    CURVES.append(model.selection("EDGE", Sketch_1.name() + "/" + SketchCircle_1.defaultResult().data().name()))
    SketchArc_1 = Sketch_1.addArc(60, 15, 80, 0, 50, 33, False)
    CURVES.append(model.selection("EDGE", Sketch_1.name() + "/" + SketchArc_1.defaultResult().data().name()))
    SketchEllipse_1 = Sketch_1.addEllipse(25, 30, 40, 30, 10)
    CURVES.append(model.selection("EDGE", Sketch_1.name() + "/" + SketchEllipse_1.defaultResult().data().name()))
    SketchEllipticArc_1 = Sketch_1.addEllipticArc(40, 70, 55, 70, 45, 50, 25, 56, False)
    CURVES.append(model.selection("EDGE", Sketch_1.name() + "/" + SketchEllipticArc_1.defaultResult().data().name()))
    SketchBSpline_1_poles = [(95, -50), (130, -10), (100, 10), (125, 45), (90, 70), (55, 45)]
    SketchBSpline_1 = Sketch_1.addSpline(poles = SketchBSpline_1_poles)
    CURVES.append(model.selection("EDGE", Sketch_1.name() + "/" + SketchBSpline_1.name()))
    SketchBSplinePeriodic_1_poles = [(95, 80), (135, 90), (145, 55), (130, 30), (125, 70), (105, 60)]
    SketchBSplinePeriodic_1 = Sketch_1.addSpline(poles = SketchBSplinePeriodic_1_poles, periodic = True)
    CURVES.append(model.selection("EDGE", Sketch_1.name() + "/" + SketchBSplinePeriodic_1.name()))
    model.do()
    PLANE = model.addPlane(partSet, model.selection("FACE", "XOY"), model.selection("EDGE", "OY"), 45)
    model.end()

    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    assert model.checkPythonDump()
