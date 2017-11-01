## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(50.45430931454487, 141.3050678403047, -211.8625790259634, 141.3050678403047)
SketchLine_2 = Sketch_1.addLine(-211.8625790259634, 141.3050678403047, -211.8625790259634, -163.3288837233227)
SketchLine_3 = Sketch_1.addLine(-211.8625790259634, -163.3288837233227, 50.45430931454487, -163.3288837233227)
SketchLine_4 = Sketch_1.addLine(50.45430931454487, -163.3288837233227, 50.45430931454487, 141.3050678403047)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(246.5736143115653, 48.78197815871301, -51.09047583637312, 48.78197815871301)
SketchLine_6 = Sketch_1.addLine(-51.09047583637312, 48.78197815871301, -51.09047583637312, -104.8260668928192)
SketchLine_7 = Sketch_1.addLine(-51.09047583637312, -104.8260668928192, 246.5736143115653, -104.8260668928192)
SketchLine_8 = Sketch_1.addLine(246.5736143115653, -104.8260668928192, 246.5736143115653, 48.78197815871301)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_8.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 100, 0)
Boolean_1 = model.addFill(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1_2")], [model.selection("FACE", "PartSet/YOZ")])
Group_1 = model.addGroup(Part_1_doc, [model.selection("FACE", "Boolean_1_1_4/Modified_3"), model.selection("FACE", "Boolean_1_1_4/Modified_Face_1"), model.selection("FACE", "Extrusion_1_1/Generated_Face_9"), model.selection("FACE", "Boolean_1_1_4/Modified_2"), model.selection("FACE", "Boolean_1_1_4/Modified_4"), model.selection("FACE", "Boolean_1_1_4/Modified_1"), model.selection("FACE", "Boolean_1_1_4/Modified_3"), model.selection("FACE", "Extrusion_1_1/Generated_Face_6"), model.selection("FACE", "Boolean_1_1_3/Modified_3"), model.selection("FACE", "Extrusion_1_1/Generated_Face_7"), model.selection("FACE", "Boolean_1_1_1/Modified_2"), model.selection("FACE", "Extrusion_1_1/Generated_Face_5"), model.selection("FACE", "Boolean_1_1_4/Modified_4"), model.selection("FACE", "Boolean_1_1_3/Modified_4"), model.selection("FACE", "Extrusion_1_1/Generated_Face_9"), model.selection("FACE", "Boolean_1_1_1/Modified_1"), model.selection("FACE", "Extrusion_1_1/Generated_Face_12"), model.selection("FACE", "Extrusion_1_1/Generated_Face_11"), model.selection("FACE", "Boolean_1_1_3/Modified_3"), model.selection("FACE", "Boolean_1_1_3/Modified_2"), model.selection("FACE", "Boolean_1_1_3/Modified_1"), model.selection("FACE", "Extrusion_1_1/Generated_Face_4"), model.selection("FACE", "Boolean_1_1_3/Modified_4"), model.selection("FACE", "Boolean_1_1_4/Modified_Face_1"), model.selection("FACE", "Extrusion_1_1/Generated_Face_4"), model.selection("FACE", "Extrusion_1_1/To_Face_3"), model.selection("FACE", "Extrusion_1_1/Generated_Face_3"), model.selection("FACE", "Extrusion_1_1/Generated_Face_2"), model.selection("FACE", "Extrusion_1_1/From_Face_3"), model.selection("FACE", "Extrusion_1_1/Generated_Face_1")])
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Boolean_1, 1)
model.testNbSubResults(Boolean_1, [4])
model.testNbSubShapes(Boolean_1, GeomAPI_Shape.SOLID, [4])
model.testNbSubShapes(Boolean_1, GeomAPI_Shape.FACE, [30])
model.testNbSubShapes(Boolean_1, GeomAPI_Shape.EDGE, [132])
model.testNbSubShapes(Boolean_1, GeomAPI_Shape.VERTEX, [264])
model.testResultsVolumes(Boolean_1, [11003613.329450136050581932067871094])

assert(model.checkPythonDump())
