# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
from GeomAPI import GeomAPI_Shape


model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Param_fuzzy = model.addParameter(Part_1_doc, "fuzzy", '1e-07')

### Create Point
Point_2 = model.addPoint(Part_1_doc, 5, 9.9999, 0)

### Create Point
Point_3 = model.addPoint(Part_1_doc, 10, 10, 0)

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(5, 0, 0, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 9.9999)
SketchLine_3 = Sketch_1.addLine(0, 9.9999, 5, 9.9999)
SketchLine_4 = Sketch_1.addLine(5, 9.9999, 5, 0)
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
Sketch_1.setHorizontal(SketchLine_1.result())
Sketch_1.setVertical(SketchLine_2.result())
Sketch_1.setHorizontal(SketchLine_3.result())
Sketch_1.setVertical(SketchLine_4.result())
SketchProjection_2 = Sketch_1.addProjection(model.selection("VERTEX", "Point_1"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchPoint_2.result())
SketchProjection_3 = Sketch_1.addProjection(model.selection("VERTEX", "Point_2"), False)
SketchPoint_3 = SketchProjection_3.createdFeature()
model.do()

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_5 = Sketch_2.addLine(10, 0, 0, 0)
SketchProjection_4 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_4 = SketchProjection_4.createdFeature()
Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchPoint_4.result())
SketchLine_6 = Sketch_2.addLine(0, 0, 0, 10)
SketchLine_7 = Sketch_2.addLine(0, 10, 10, 10)
SketchLine_8 = Sketch_2.addLine(10, 10, 10, 0)
Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
Sketch_2.setHorizontal(SketchLine_5.result())
Sketch_2.setVertical(SketchLine_6.result())
Sketch_2.setHorizontal(SketchLine_7.result())
Sketch_2.setVertical(SketchLine_8.result())
SketchProjection_5 = Sketch_2.addProjection(model.selection("VERTEX", "Point_2"), False)
SketchPoint_5 = SketchProjection_5.createdFeature()
Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchPoint_5.result())
model.do()

### Create Face
Face_1 = model.addFace(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")])
Face_1.result().setColor(255, 0, 0)

### Create Face
Face_2 = model.addFace(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")])
Face_2.result().setColor(0, 255, 0)

### Create Cut
Cut_1 = model.addCut(Part_1_doc, [model.selection("FACE", "Face_2_1")], [model.selection("FACE", "Face_1_1")], fuzzyParam = "fuzzy", keepSubResults = True)
model.do()

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [0])
model.testNbUniqueSubShapes(Cut_1, GeomAPI_Shape.FACE, [1])
model.testNbUniqueSubShapes(Cut_1, GeomAPI_Shape.EDGE, [6])
model.testNbUniqueSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [6])

### Set a higher fuzzy value
Param_fuzzy.setValue(1.e-4)
model.do()

model.end()

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [0])
model.testNbUniqueSubShapes(Cut_1, GeomAPI_Shape.FACE, [1])
model.testNbUniqueSubShapes(Cut_1, GeomAPI_Shape.EDGE, [4])
model.testNbUniqueSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [4])
