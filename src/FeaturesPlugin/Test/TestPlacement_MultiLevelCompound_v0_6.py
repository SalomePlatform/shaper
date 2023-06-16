# Copyright (C) 2020-2023  CEA/DEN, EDF R&D
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

# collect reference data
Z_SHIFT = 10
REFERENCE = []
TOLERANCE = 1.e-7
for ind in range(0, Compound_1.result().numberOfSubs()):
    p = Compound_1.result().subResult(ind).resultSubShapePair()[1].middlePoint()
    p.setZ(p.z() + Z_SHIFT)
    REFERENCE.append(p)

index = 0

model.begin()
Placement_1 = model.addPlacement(Part_1_doc, [model.selection("VERTEX", "Compound_1_1_1")], model.selection("FACE", "Compound_1_1_6/Modified_Face&Box_1_1/Bottom"), model.selection("FACE", "Compound_1_1_6/Modified_Face&Box_1_1/Top"), False, False)
model.testNbResults(Placement_1, 1)
model.testNbSubResults(Placement_1, [0])
model.testNbSubShapes(Placement_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Placement_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Placement_1, GeomAPI_Shape.EDGE, [0])
model.testNbSubShapes(Placement_1, GeomAPI_Shape.VERTEX, [1])
model.testResultsVolumes(Placement_1, [0])
model.testResultsAreas(Placement_1, [0])
midPoint = Placement_1.defaultResult().shape().middlePoint()
assert(midPoint.distance(REFERENCE[index]) < TOLERANCE)
index += 1

Recover_1 = model.addRecover(Part_1_doc, Placement_1, [Compound_1.result()], True)
Placement_2 = model.addPlacement(Part_1_doc, [model.selection("EDGE", "Recover_1_1_2")], model.selection("FACE", "Recover_1_1_6/Modified_Face&Box_1_1/Bottom"), model.selection("FACE", "Recover_1_1_6/Modified_Face&Box_1_1/Top"), False, False)
model.testNbResults(Placement_2, 1)
model.testNbSubResults(Placement_2, [0])
model.testNbSubShapes(Placement_2, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Placement_2, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Placement_2, GeomAPI_Shape.EDGE, [1])
model.testNbSubShapes(Placement_2, GeomAPI_Shape.VERTEX, [2])
model.testResultsVolumes(Placement_2, [0])
model.testResultsAreas(Placement_2, [0])
midPoint = Placement_2.defaultResult().shape().middlePoint()
assert(midPoint.distance(REFERENCE[index]) < TOLERANCE)
index += 1

Recover_2 = model.addRecover(Part_1_doc, Placement_2, [Recover_1.result()])
Placement_3 = model.addPlacement(Part_1_doc, [model.selection("WIRE", "Recover_2_1_3")], model.selection("FACE", "Recover_2_1_6/Modified_Face&Box_1_1/Bottom"), model.selection("FACE", "Recover_2_1_6/Modified_Face&Box_1_1/Top"), False, False)
model.testNbResults(Placement_3, 1)
model.testNbSubResults(Placement_3, [0])
model.testNbSubShapes(Placement_3, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Placement_3, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Placement_3, GeomAPI_Shape.EDGE, [2])
model.testNbSubShapes(Placement_3, GeomAPI_Shape.VERTEX, [4])
model.testResultsVolumes(Placement_3, [0])
model.testResultsAreas(Placement_3, [0])
midPoint = Placement_3.defaultResult().shape().middlePoint()
assert(midPoint.distance(REFERENCE[index]) < TOLERANCE)
index += 1

Recover_3 = model.addRecover(Part_1_doc, Placement_3, [Recover_2.result()])
Placement_4 = model.addPlacement(Part_1_doc, [model.selection("FACE", "Recover_3_1_4")], model.selection("FACE", "Recover_3_1_6/Modified_Face&Box_1_1/Bottom"), model.selection("FACE", "Recover_3_1_6/Modified_Face&Box_1_1/Top"), False, False)
model.testNbResults(Placement_4, 1)
model.testNbSubResults(Placement_4, [0])
model.testNbSubShapes(Placement_4, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Placement_4, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Placement_4, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Placement_4, GeomAPI_Shape.VERTEX, [8])
model.testResultsVolumes(Placement_4, [0])
model.testResultsAreas(Placement_4, [109.154152964914914])
midPoint = Placement_4.defaultResult().shape().middlePoint()
assert(midPoint.distance(REFERENCE[index]) < TOLERANCE)
index += 1

Recover_4 = model.addRecover(Part_1_doc, Placement_4, [Recover_3.result()])
Placement_5 = model.addPlacement(Part_1_doc, [model.selection("SHELL", "Recover_4_1_5")], model.selection("FACE", "Recover_4_1_6/Modified_Face&Box_1_1/Bottom"), model.selection("FACE", "Recover_4_1_6/Modified_Face&Box_1_1/Top"), False, False)
model.testNbResults(Placement_5, 1)
model.testNbSubResults(Placement_5, [0])
model.testNbSubShapes(Placement_5, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Placement_5, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Placement_5, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Placement_5, GeomAPI_Shape.VERTEX, [16])
model.testResultsVolumes(Placement_5, [0])
model.testResultsAreas(Placement_5, [182.822012116])
midPoint = Placement_5.defaultResult().shape().middlePoint()
assert(midPoint.distance(REFERENCE[index]) < TOLERANCE)
index += 1

Recover_5 = model.addRecover(Part_1_doc, Placement_5, [Recover_4.result()])
Placement_6 = model.addPlacement(Part_1_doc, [model.selection("SOLID", "Recover_5_1_6")], model.selection("FACE", "Recover_5_1_6/Modified_Face&Box_1_1/Bottom"), model.selection("FACE", "Recover_5_1_6/Modified_Face&Box_1_1/Top"), False, False)
model.testNbResults(Placement_6, 1)
model.testNbSubResults(Placement_6, [0])
model.testNbSubShapes(Placement_6, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Placement_6, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Placement_6, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Placement_6, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(Placement_6, [1000])
midPoint = Placement_6.defaultResult().shape().middlePoint()
assert(midPoint.distance(REFERENCE[index]) < TOLERANCE)
index += 1

Recover_6 = model.addRecover(Part_1_doc, Placement_6, [Recover_5.result()])
Placement_7 = model.addPlacement(Part_1_doc, [model.selection("COMPSOLID", "Recover_6_1_7")], model.selection("FACE", "Recover_6_1_6/Modified_Face&Box_1_1/Bottom"), model.selection("FACE", "Recover_6_1_6/Modified_Face&Box_1_1/Top"), False, False)
model.testNbResults(Placement_7, 1)
model.testNbSubResults(Placement_7, [3])
model.testNbSubShapes(Placement_7, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(Placement_7, GeomAPI_Shape.FACE, [17])
model.testNbSubShapes(Placement_7, GeomAPI_Shape.EDGE, [66])
model.testNbSubShapes(Placement_7, GeomAPI_Shape.VERTEX, [132])
model.testResultsVolumes(Placement_7, [1589.0486226])
midPoint = Placement_7.defaultResult().shape().middlePoint()
assert(midPoint.distance(REFERENCE[index]) < TOLERANCE)
index += 1

Recover_7 = model.addRecover(Part_1_doc, Placement_7, [Recover_6.result()])
Placement_8 = model.addPlacement(Part_1_doc, [model.selection("COMPOUND", "Recover_7_1_8")], model.selection("FACE", "Recover_7_1_6/Modified_Face&Box_1_1/Bottom"), model.selection("FACE", "Recover_7_1_6/Modified_Face&Box_1_1/Top"), False, False)
model.testNbResults(Placement_8, 1)
model.testNbSubResults(Placement_8, [3])
model.testNbSubShapes(Placement_8, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(Placement_8, GeomAPI_Shape.FACE, [18])
model.testNbSubShapes(Placement_8, GeomAPI_Shape.EDGE, [72])
model.testNbSubShapes(Placement_8, GeomAPI_Shape.VERTEX, [144])
model.testResultsVolumes(Placement_8, [3000])
midPoint = Placement_8.defaultResult().shape().middlePoint()
assert(midPoint.distance(REFERENCE[index]) < TOLERANCE)

model.end()

assert(model.checkPythonDump())
