# Copyright (C) 2019-2022  CEA/DEN, EDF R&D
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
    Test removing B-spline curve and its construstion elements
"""

from salome.shaper import model
from ModelAPI import *

def assertNbSubs(theSketch, theNbPoints, theNbLines, theNbSplines, theNbInternalConstraints):
    model.testNbSubFeatures(theSketch, "SketchPoint", theNbPoints)
    model.testNbSubFeatures(theSketch, "SketchLine", theNbLines)
    model.testNbSubFeatures(theSketch, "SketchBSpline", theNbSplines)
    model.testNbSubFeatures(theSketch, "SketchConstraintCoincidenceInternal", theNbInternalConstraints)

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchBSpline_1_poles = [(-30, -10), (-15, 20), (0, -10), (15, 20), (30, -10)]
SketchBSpline_1 = Sketch_1.addSpline(poles = SketchBSpline_1_poles)
controlPoles = SketchBSpline_1.controlPoles(auxiliary = [0, 1, 2, 3, 4])
controlLines = SketchBSpline_1.controlPolygon(auxiliary = [0, 1, 2, 3])
model.do()
model.end()

DEFAULT_DOF = len(SketchBSpline_1_poles) * 2
DEFAULT_POINTS = len(SketchBSpline_1_poles)
DEFAULT_LINES = len(SketchBSpline_1_poles) - 1
DEFAULT_BSPLINES = 1
DEAFULT_INTERNALS = len(controlPoles) + len(controlLines) * 2

assertNbSubs(Sketch_1, DEFAULT_POINTS, DEFAULT_LINES, DEFAULT_BSPLINES, DEAFULT_INTERNALS)
assert(model.dof(Sketch_1) == DEFAULT_DOF)

# Test 1. Remove auxiliary points one by one.
for pnt in controlPoles:
    model.begin()
    removeFeaturesAndReferences(FeatureSet([pnt.feature()]))
    model.end()

    assertNbSubs(Sketch_1, DEFAULT_POINTS - 1, DEFAULT_LINES, DEFAULT_BSPLINES, DEAFULT_INTERNALS - 1)
    assert(model.dof(Sketch_1) == DEFAULT_DOF)
    model.undo()
    assertNbSubs(Sketch_1, DEFAULT_POINTS, DEFAULT_LINES, DEFAULT_BSPLINES, DEAFULT_INTERNALS)
    assert(model.dof(Sketch_1) == DEFAULT_DOF)

# Test 2. Remove auxiliary lines one by one.
for ln in controlLines:
    model.begin()
    removeFeaturesAndReferences(FeatureSet([ln.feature()]))
    model.end()

    assertNbSubs(Sketch_1, DEFAULT_POINTS, DEFAULT_LINES - 1, DEFAULT_BSPLINES, DEAFULT_INTERNALS - 2)
    assert(model.dof(Sketch_1) == DEFAULT_DOF)
    model.undo()
    assertNbSubs(Sketch_1, DEFAULT_POINTS, DEFAULT_LINES, DEFAULT_BSPLINES, DEAFULT_INTERNALS)
    assert(model.dof(Sketch_1) == DEFAULT_DOF)

# Test 3. Remove the B-spline curve.
model.begin()
removeFeaturesAndReferences(FeatureSet([SketchBSpline_1.feature()]))
model.end()

assertNbSubs(Sketch_1, 0, 0, 0, 0)
assert(model.dof(Sketch_1) == 0)
model.undo()
assertNbSubs(Sketch_1, DEFAULT_POINTS, DEFAULT_LINES, DEFAULT_BSPLINES, DEAFULT_INTERNALS)
assert(model.dof(Sketch_1) == DEFAULT_DOF)

# Test 4. Remove some construction elements, make non-auxiliary a couple of the rest and check the dumping.
model.begin()
partSet = model.moduleDocument()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchBSpline_2_poles = [(-30, -10), (-15, -40), (0, -10), (15, -40), (30, -10)]
SketchBSpline_2 = Sketch_2.addSpline(poles = SketchBSpline_2_poles)
controlPoles2 = SketchBSpline_2.controlPoles(auxiliary = [0, 1, 2, 3, 4])
controlLines2 = SketchBSpline_2.controlPolygon(auxiliary = [0, 1, 2, 3])
model.do()
model.end()

model.begin()
controlPoles2[1].setAuxiliary(False)
controlLines2[2].setAuxiliary(False)
removeFeaturesAndReferences(FeatureSet([controlPoles2[2].feature(), controlLines2[0].feature()]))
model.end()

assertNbSubs(Sketch_2, DEFAULT_POINTS - 1, DEFAULT_LINES - 1, DEFAULT_BSPLINES, DEAFULT_INTERNALS - 3)

assert(model.checkPythonDump())
