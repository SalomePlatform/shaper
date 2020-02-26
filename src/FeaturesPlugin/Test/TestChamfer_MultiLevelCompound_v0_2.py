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
from GeomAPI import *
from GeomAlgoAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchPoint_1 = Sketch_1.addPoint(44.29784155360136, 17.09942588468031)
SketchArc_1 = Sketch_1.addArc(44.29784155360136, 17.09942588468031, 47.1668727423061, 12.27945348765633, 45.38299232715926, 22.60269052200972, False)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchArc_1.center())
SketchLine_1 = Sketch_1.addLine(42.5764228403785, 22.14892077680068, 39.70739165167375, 16.41085839939117)
SketchLine_2 = Sketch_1.addLine(39.70739165167375, 16.41085839939117, 43.03546783057126, 12.04993099255995)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(28.57555063949931, 12.96802097294547, 15.72229091410204, 12.96802097294547)
SketchLine_4 = Sketch_1.addLine(15.72229091410204, 12.96802097294547, 15.72229091410204, 21.46035329151154)
SketchLine_5 = Sketch_1.addLine(15.72229091410204, 21.46035329151154, 28.57555063949931, 21.46035329151154)
SketchLine_6 = Sketch_1.addLine(28.57555063949931, 21.46035329151154, 28.57555063949931, 12.96802097294547)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_6.result())
SketchLine_7 = Sketch_1.addLine(-10.67279602198167, 14.78814178154371, -28.34602814440294, 14.78814178154371)
SketchLine_8 = Sketch_1.addLine(-28.34602814440294, 14.78814178154371, -28.34602814440294, 25.13271321305362)
SketchLine_9 = Sketch_1.addLine(-28.34602814440294, 25.13271321305362, -10.67279602198167, 25.13271321305362)
SketchLine_10 = Sketch_1.addLine(-10.67279602198167, 25.13271321305362, -10.67279602198167, 14.78814178154371)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_8.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_9.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_10.result())
SketchLine_11 = Sketch_1.addLine(-17.67323212242127, 25.13271321305362, -21.80463703415611, 14.78814178154371)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_11.startPoint(), SketchLine_9.result())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_7.result())
model.do()
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_1/SketchArc_1")], False)
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchArc_1_2")], False)
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2")], False)
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_6r-SketchLine_5r-SketchLine_4r-SketchLine_3r")])
Shell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_10r-SketchLine_9r-SketchLine_11f-SketchLine_7r"), model.selection("FACE", "Sketch_1/Face-SketchLine_11r-SketchLine_9r-SketchLine_8r-SketchLine_7r")])
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "PartSet/OX"), 50)
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_2_1"), model.selection("SOLID", "Cylinder_1_1")])
Box_3 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_2 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_3_1")], model.selection("EDGE", "PartSet/OY"), 50)
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("SOLID", "Translation_2_1")], model.selection("EDGE", "PartSet/OZ"), 30, 3)
Compound_1_objects = [model.selection("VERTEX", "Vertex_1_1"), model.selection("EDGE", "Edge_1_1"), model.selection("WIRE", "Wire_1_1"), model.selection("FACE", "Face_1_1"), model.selection("SHELL", "Shell_1_1"), model.selection("SOLID", "Translation_1_1"), model.selection("COMPSOLID", "Partition_1_1"), model.selection("COMPOUND", "AngularCopy_1_1")]
Compound_1 = model.addCompound(Part_1_doc, Compound_1_objects)
model.end()

TOLERANCE = 1.e-7
DISTANCE_1 = 2
DISTANCE_2 = 1

model.begin()
Chamfer_1 = model.addChamfer(Part_1_doc, [model.selection("EDGE", "Compound_1_1_2")], True, DISTANCE_1, DISTANCE_2)
model.end()
assert(Chamfer_1.feature().error() != "")
model.undo()

model.begin()
Chamfer_2 = model.addChamfer(Part_1_doc, [model.selection("EDGE", "Compound_1_1_3/Modified_Edge&Sketch_1/SketchLine_1")], True, DISTANCE_1, DISTANCE_2)
model.end()
assert(Chamfer_2.feature().error() != "")
model.undo()

model.begin()
Chamfer_3 = model.addChamfer(Part_1_doc, [model.selection("EDGE", "Compound_1_1_4/Modified_Edge&Sketch_1/SketchLine_3")], True, DISTANCE_1, DISTANCE_2)
model.end()
assert(Chamfer_3.feature().error() != "")
model.undo()

model.begin()
Chamfer_4 = model.addChamfer(Part_1_doc, [model.selection("EDGE", "Compound_1_1_5/Modified_Edge&Sketch_1/SketchLine_10")], True, DISTANCE_1, DISTANCE_2)
model.end()
assert(Chamfer_4.feature().error() != "")
model.undo()

model.begin()
Chamfer_5 = model.addChamfer(Part_1_doc, [model.selection("EDGE", "[Compound_1_1_6/Modified_Face&Box_1_1/Front][Compound_1_1_6/Modified_Face&Box_1_1/Top]"), model.selection("FACE", "Compound_1_1_6/Modified_Face&Box_1_1/Back")], True, DISTANCE_1, DISTANCE_2)
model.testNbResults(Chamfer_5, 1)
model.testNbSubResults(Chamfer_5, [0])
model.testNbSubShapes(Chamfer_5, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Chamfer_5, GeomAPI_Shape.FACE, [11])
model.testNbSubShapes(Chamfer_5, GeomAPI_Shape.EDGE, [46])
model.testNbSubShapes(Chamfer_5, GeomAPI_Shape.VERTEX, [92])
model.testResultsVolumes(Chamfer_5, [955.333333333])
refPoint = GeomAPI_Pnt(55.1200279, 5, 4.954640614)
midPoint = Chamfer_5.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)

Recover_1 = model.addRecover(Part_1_doc, Chamfer_5, [Compound_1.result()], True)
Chamfer_6 = model.addChamfer(Part_1_doc, [model.selection("EDGE", "[Recover_1_1_7_2/Modified_Face&Box_2_1/Top][Recover_1_1_7_2/Modified_Face&Box_2_1/Front]"), model.selection("FACE", "Recover_1_1_7_2/Modified_Face&Box_2_1/Right")], True, DISTANCE_1, DISTANCE_2)
model.testNbResults(Chamfer_6, 1)
model.testNbSubResults(Chamfer_6, [3])
model.testNbSubShapes(Chamfer_6, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(Chamfer_6, GeomAPI_Shape.FACE, [23])
model.testNbSubShapes(Chamfer_6, GeomAPI_Shape.EDGE, [90])
model.testNbSubShapes(Chamfer_6, GeomAPI_Shape.VERTEX, [180])
model.testResultsVolumes(Chamfer_6, [1545.0486226])
refPoint = GeomAPI_Pnt(2.5, 2.5, 5)
midPoint = Chamfer_6.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)

Recover_2 = model.addRecover(Part_1_doc, Chamfer_6, [Compound_1.result()], True)
Chamfer_7 = model.addChamfer(Part_1_doc, [model.selection("EDGE", "[Recover_2_1_8_1/Modified_Face&Box_3_1/Back][Recover_2_1_8_1/Modified_Face&Box_3_1/Top]"), model.selection("FACE", "Recover_2_1_8_2/Modified_Face&Box_3_1/Left")], True, DISTANCE_1, DISTANCE_2)
model.testNbResults(Chamfer_7, 2)
model.testNbSubResults(Chamfer_7, [0, 0])
model.testNbSubShapes(Chamfer_7, GeomAPI_Shape.SOLID, [1, 1])
model.testNbSubShapes(Chamfer_7, GeomAPI_Shape.FACE, [7, 10])
model.testNbSubShapes(Chamfer_7, GeomAPI_Shape.EDGE, [30, 40])
model.testNbSubShapes(Chamfer_7, GeomAPI_Shape.VERTEX, [60, 80])
model.testResultsVolumes(Chamfer_7, [990, 965.33333333])

Recover_3 = model.addRecover(Part_1_doc, Chamfer_7, [Compound_1.result()], True)
Chamfer_8 = model.addChamfer(Part_1_doc, [model.selection("EDGE", "Recover_3_1_6/Modified_Edge&Chamfer_1_1/ChamferSelected_1"), model.selection("EDGE", "[Recover_3_1_8_2/Modified_Face&Box_3_1/Left][Recover_3_1_8_2/Modified_Face&Box_3_1/Top]")], True, DISTANCE_1, DISTANCE_2)
model.testNbResults(Chamfer_8, 2)
model.testNbSubResults(Chamfer_8, [0, 0])
model.testNbSubShapes(Chamfer_8, GeomAPI_Shape.SOLID, [1, 1])
model.testNbSubShapes(Chamfer_8, GeomAPI_Shape.FACE, [7, 7])
model.testNbSubShapes(Chamfer_8, GeomAPI_Shape.EDGE, [30, 30])
model.testNbSubShapes(Chamfer_8, GeomAPI_Shape.VERTEX, [60, 60])
model.testResultsVolumes(Chamfer_8, [990, 990])
model.end()

assert(model.checkPythonDump())
