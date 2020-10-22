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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Box
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Top"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(7.071067811865538, 2.928932188134462, 2.928932188134676, 2.928932188134462)

### Create SketchLine
SketchLine_2 = Sketch_1.addLine(2.928932188134676, 2.928932188134462, 2.928932188134676, 7.071067811865324)

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(2.928932188134676, 7.071067811865324, 7.071067811865538, 7.071067811865324)

### Create SketchLine
SketchLine_4 = Sketch_1.addLine(7.071067811865538, 7.071067811865324, 7.071067811865538, 2.928932188134462)
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
Sketch_1.setHorizontal(SketchLine_1.result())
Sketch_1.setVertical(SketchLine_2.result())
Sketch_1.setHorizontal(SketchLine_3.result())
Sketch_1.setVertical(SketchLine_4.result())

### Create SketchLine
SketchLine_5 = Sketch_1.addLine(0, 0, 2.928932188134676, 2.928932188134462)

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Top]"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchPoint_1.result())
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_5.endPoint())

### Create SketchLine
SketchLine_6 = Sketch_1.addLine(10, 0, 7.071067811865538, 2.928932188134462)

### Create SketchProjection
SketchProjection_2 = Sketch_1.addProjection(model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Top]"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchPoint_2.result())
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_6.endPoint())

### Create SketchLine
SketchLine_7 = Sketch_1.addLine(10, 10, 7.071067811865538, 7.071067811865324)

### Create SketchProjection
SketchProjection_3 = Sketch_1.addProjection(model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Top]"), False)
SketchPoint_3 = SketchProjection_3.createdFeature()
Sketch_1.setCoincident(SketchLine_7.startPoint(), SketchPoint_3.result())
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_7.endPoint())

### Create SketchLine
SketchLine_8 = Sketch_1.addLine(0, 10, 2.928932188134676, 7.071067811865324)

### Create SketchProjection
SketchProjection_4 = Sketch_1.addProjection(model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Right][Box_1_1/Top]"), False)
SketchPoint_4 = SketchProjection_4.createdFeature()
Sketch_1.setCoincident(SketchLine_8.startPoint(), SketchPoint_4.result())
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_8.endPoint())
Sketch_1.setEqual(SketchLine_4.result(), SketchLine_3.result())
Sketch_1.setEqual(SketchLine_5.result(), SketchLine_6.result())
Sketch_1.setEqual(SketchLine_6.result(), SketchLine_7.result())
Sketch_1.setEqual(SketchLine_5.result(), SketchLine_2.result())
model.do()

### Create Revolution
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), 0, 90, "Edges")

### Create Split
Split_1 = model.addSplit(Part_1_doc, [model.selection("SOLID", "Box_1_1")], [model.selection("COMPOUND", "Revolution_1_1")], keepSubResults = True)

model.end()

from GeomAPI import *

model.testNbResults(Split_1, 1)
model.testNbSubResults(Split_1, [5])
model.testNbSubShapes(Split_1, GeomAPI_Shape.SOLID, [5])
model.testNbSubShapes(Split_1, GeomAPI_Shape.FACE, [32])
model.testNbSubShapes(Split_1, GeomAPI_Shape.EDGE, [124])
model.testNbSubShapes(Split_1, GeomAPI_Shape.VERTEX, [248])
model.testResultsVolumes(Split_1, [1000])

assert(model.checkPythonDump())
