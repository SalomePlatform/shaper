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
    Test removing ellipse and its construstion elements
"""

from salome.shaper import model
from ModelAPI import *

def assertNbSubs(theSketch, theNbPoints, theNbLines, theNbEllipses, theNbInternalConstraints):
    model.testNbSubFeatures(theSketch, "SketchPoint", theNbPoints)
    model.testNbSubFeatures(theSketch, "SketchLine", theNbLines)
    model.testNbSubFeatures(theSketch, "SketchEllipse", theNbEllipses)
    model.testNbSubFeatures(theSketch, "SketchConstraintCoincidenceInternal", theNbInternalConstraints)

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchEllipse_1 = Sketch_1.addEllipse(40, 30, 70, 40, 20)
[Center, Focus1, Focus2, MajorAxisStart, MajorAxisEnd, MinorAxisStart, MinorAxisEnd, MajorAxisLine, MinorAxisLine] = SketchEllipse_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
model.do()
model.end()

DEFAULT_DOF = 5
DEFAULT_POINTS = 7
DEFAULT_LINES = 2
DEFAULT_ELLIPSES = 1
DEAFULT_INTERNALS = 11

assertNbSubs(Sketch_1, DEFAULT_POINTS, DEFAULT_LINES, DEFAULT_ELLIPSES, DEAFULT_INTERNALS)
assert(model.dof(Sketch_1) == DEFAULT_DOF)

# Test 1. Remove auxiliary points one by one.
points = [Center, Focus1, Focus2, MajorAxisStart, MajorAxisEnd, MinorAxisStart, MinorAxisEnd]
for pnt in points:
    model.begin()
    removeFeaturesAndReferences(FeatureSet([pnt.feature()]))
    model.end()

    assertNbSubs(Sketch_1, DEFAULT_POINTS - 1, DEFAULT_LINES, DEFAULT_ELLIPSES, DEAFULT_INTERNALS - 1)
    assert(model.dof(Sketch_1) == DEFAULT_DOF)
    model.undo()
    assertNbSubs(Sketch_1, DEFAULT_POINTS, DEFAULT_LINES, DEFAULT_ELLIPSES, DEAFULT_INTERNALS)
    assert(model.dof(Sketch_1) == DEFAULT_DOF)

# Test 2. Remove auxiliary axes one by one.
lines = [MajorAxisLine, MinorAxisLine]
for ln in lines:
    model.begin()
    removeFeaturesAndReferences(FeatureSet([ln.feature()]))
    model.end()

    assertNbSubs(Sketch_1, DEFAULT_POINTS, DEFAULT_LINES - 1, DEFAULT_ELLIPSES, DEAFULT_INTERNALS - 2)
    assert(model.dof(Sketch_1) == DEFAULT_DOF)
    model.undo()
    assertNbSubs(Sketch_1, DEFAULT_POINTS, DEFAULT_LINES, DEFAULT_ELLIPSES, DEAFULT_INTERNALS)
    assert(model.dof(Sketch_1) == DEFAULT_DOF)

# Test 3.  Remove the ellipse.
model.begin()
removeFeaturesAndReferences(FeatureSet([SketchEllipse_1.feature()]))
model.end()

assertNbSubs(Sketch_1, 0, 0, 0, 0)
assert(model.dof(Sketch_1) == 0)
model.undo()
assertNbSubs(Sketch_1, DEFAULT_POINTS, DEFAULT_LINES, DEFAULT_ELLIPSES, DEAFULT_INTERNALS)
assert(model.dof(Sketch_1) == DEFAULT_DOF)

# Test 4. Remove some construction elements, make non-auxiliary a couple of the rest and check the dumping.
model.begin()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchEllipse_2 = Sketch_2.addEllipse(40, -30, 70, 0, 10)
[Center, Focus1, Focus2, MajorAxisStart, MajorAxisEnd, MinorAxisStart, MinorAxisEnd, MajorAxisLine, MinorAxisLine] = SketchEllipse_2.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
model.do()
model.end()

model.begin()
removeFeaturesAndReferences(FeatureSet([MajorAxisLine.feature(), Focus2.feature()]))
Focus1.setAuxiliary(False)
MinorAxisEnd.setAuxiliary(False)
model.end()

assertNbSubs(Sketch_2, DEFAULT_POINTS - 1, DEFAULT_LINES - 1, DEFAULT_ELLIPSES, DEAFULT_INTERNALS - 3)
assert(model.dof(Sketch_2) == DEFAULT_DOF)

assert(model.checkPythonDump())
