# Copyright (C) 2020  CEA/DEN, EDF R&D
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

from GeomAlgoAPI import *
from SketchAPI import *

from salome.shaper import model

import math

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(50, 0, 41.0188620508502, 41.01886205085074)

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_2.result())

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(41.0188620508502, 41.01886205085074, 0, 50)
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.startPoint())

### Create SketchProjection
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_4 = SketchProjection_2.createdFeature()
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.result())

### Create SketchLine
SketchLine_5 = Sketch_1.addLine(41.0188620508502, 41.01886205085074, 70.71067811865088, 70.71067811866516)
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_5.startPoint())

### Create SketchCircle
SketchCircle_1 = Sketch_1.addCircle(0, 0, 100)
SketchCircle_1.setAuxiliary(True)
Sketch_1.setCoincident(SketchAPI_Line(SketchLine_2).startPoint(), SketchCircle_1.center())
Sketch_1.setCoincident(SketchAPI_Line(SketchLine_2).endPoint(), SketchCircle_1.results()[1])
Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchCircle_1.results()[1])
Sketch_1.setMiddlePoint(SketchLine_1.startPoint(), SketchLine_2.result())
Sketch_1.setMiddlePoint(SketchLine_3.endPoint(), SketchLine_4.result())
Sketch_1.setEqual(SketchLine_1.result(), SketchLine_3.result())
Sketch_1.setEqual(SketchLine_1.result(), SketchLine_5.result())
Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_5.result())

### Create SketchMultiRotation
SketchMultiRotation_1_objects = [SketchLine_1.result(), SketchLine_3.result(), SketchLine_5.result()]
SketchMultiRotation_1 = Sketch_1.addRotation(SketchMultiRotation_1_objects, SketchCircle_1.center(), 360, 4, True)
[SketchLine_6, SketchLine_7, SketchLine_8, SketchLine_9, SketchLine_10, SketchLine_11, SketchLine_12, SketchLine_13, SketchLine_14] = SketchMultiRotation_1.rotatedList()
model.do()

### Create Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 100, 0, "Edges")
model.end()

### Store volumes of sub-shapes as a reference
TOLERANCE = 1.e-7
REFERENCE = []
resultExtrusion_1 = Extrusion_1.result()
for ind in range(resultExtrusion_1.numberOfSubs()):
  REFERENCE.append(GeomAlgoAPI_ShapeTools.volume(resultExtrusion_1.subResult(ind).resultSubShapePair()[0].shape()))

### Add new edges to Sketch_1 then check the Extrusion and update reference data
model.begin()
SketchLine_6 = Sketch_1.addLine(70.71067811865088, 70.71067811866516, 77.78174593052023, 77.78174593052023)
Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
Sketch_1.setCollinear(SketchLine_5.result(), SketchLine_6.result())
Sketch_1.setLength(SketchLine_6.result(), 10)

SketchMultiRotation_1.rotationList().append(SketchLine_6.defaultResult())
model.end()

resultExtrusion_1 = Extrusion_1.result()
for ind in range(resultExtrusion_1.numberOfSubs()):
  area = GeomAlgoAPI_ShapeTools.volume(resultExtrusion_1.subResult(ind).resultSubShapePair()[0].shape())
  if (ind < len(REFERENCE)):
    assert(math.fabs(REFERENCE[ind] - area) < TOLERANCE)
  else:
    REFERENCE.append(area)

assert(model.checkPythonDump(model.CHECK_NAMING))

### Check results after dump
resultExtrusion_1 = Extrusion_1.result()
assert(len(REFERENCE) == resultExtrusion_1.numberOfSubs())
for ind in range(resultExtrusion_1.numberOfSubs()):
  area = GeomAlgoAPI_ShapeTools.volume(resultExtrusion_1.subResult(ind).resultSubShapePair()[0].shape())
  assert(math.fabs(REFERENCE[ind] - area) < TOLERANCE)
