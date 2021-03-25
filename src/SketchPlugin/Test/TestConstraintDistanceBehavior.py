# Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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

from salome.shaper import model
from SketchAPI import *
import math

TOLERANCE = 1.e-7

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
DistanceParam = model.addParameter(Part_1_doc, "distance", "10.")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchRectangle_1 = Sketch_1.addRectangle(20., 20., 70., 50.)
[SketchLine_1, SketchLine_2, SketchLine_3, SketchLine_4] = SketchRectangle_1.lines()
firstPoint = SketchAPI_Line(SketchLine_2).startPoint()
secondPoint = SketchAPI_Line(SketchLine_3).endPoint()
model.do()

# =============================================================================
# Test 1.
# =============================================================================
# horizontal distance constraint
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(firstPoint, secondPoint, 10)
SketchConstraintDistanceHorizontal_1.feature().real("ConstraintValue").setText(DistanceParam.name().value())
model.do()

# changing the parameter
for param in range(-31, 30, 2):
    if param == 0:
        continue
    DistanceParam.setValue(param)
    model.do()
    dist = secondPoint.x() - firstPoint.x()
    assert math.fabs(dist - param) < TOLERANCE, "Incorrect horizontal distance {}, expected {}".format(dist, param)

model.testNbSubFeatures(Sketch_1, "SketchLine", 4)
model.testNbSubFeatures(Sketch_1, "SketchConstraintDistanceHorizontal", 1)

# remove horizontal distance constraint
Part_1_doc.removeFeature(SketchConstraintDistanceHorizontal_1.feature())
model.do()

# =============================================================================
# Test 2.
# =============================================================================
# Vertical distance constraint
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(firstPoint, secondPoint, 10)
SketchConstraintDistanceVertical_1.feature().real("ConstraintValue").setText(DistanceParam.name().value())
model.do()

# changing the parameter
for param in range(-31, 30, 2):
    if param == 0:
        continue
    DistanceParam.setValue(param)
    model.do()
    dist = secondPoint.y() - firstPoint.y()
    assert math.fabs(dist - param) < TOLERANCE, "Incorrect vertical distance {}, expected {}".format(dist, param)

model.testNbSubFeatures(Sketch_1, "SketchLine", 4)
model.testNbSubFeatures(Sketch_1, "SketchConstraintDistanceVertical", 1)

# remove verticel distance constraint
Part_1_doc.removeFeature(SketchConstraintDistanceVertical_1.feature())
model.do()

# =============================================================================
# Test 3.
# =============================================================================
# distance constraint
SketchConstraintDistance_1 = Sketch_1.setDistance(firstPoint, secondPoint, "distance")
model.do()

# changing the parameter
for param in range(-30, 31, 2):
    DistanceParam.setValue(param)
    model.do()
    if param < 0:
        assert SketchConstraintDistance_1.feature().error() != '', "ERROR: Sketch should not be valid due to negative distance value"
    elif param == 0: # constraint is valid, but lead to degenerated geometry
        assert Sketch_1.feature().error() != '', "ERROR: Sketch should not be valid due to negative distance value"
    else:
        dist = model.distancePointPoint(firstPoint, secondPoint)
        assert math.fabs(dist - math.fabs(param)) < TOLERANCE, "Incorrect distance {}, expected {}".format(dist, math.fabs(param))

model.testNbSubFeatures(Sketch_1, "SketchLine", 4)
model.testNbSubFeatures(Sketch_1, "SketchConstraintDistance", 1)
# leave distance constraint alive

model.end()
