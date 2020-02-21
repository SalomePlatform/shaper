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
SCALE = 2


model.begin()
Scale_1 = model.addScale(Part_1_doc, [model.selection("VERTEX", "Compound_1_1_1")], model.selection("VERTEX", "PartSet/Origin"), SCALE)
model.testNbResults(Scale_1, 1)
model.testNbSubResults(Scale_1, [0])
model.testNbSubShapes(Scale_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Scale_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Scale_1, GeomAPI_Shape.EDGE, [0])
model.testNbSubShapes(Scale_1, GeomAPI_Shape.VERTEX, [1])
model.testResultsVolumes(Scale_1, [0])
refPoint = GeomAPI_Pnt(Compound_1.result().subResult(0).resultSubShapePair()[0].shape().middlePoint().xyz().multiplied(SCALE))
midPoint = Scale_1.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)

Recover_1 = model.addRecover(Part_1_doc, Scale_1, [Compound_1.result()], True)
Scale_2 = model.addScale(Part_1_doc, [model.selection("EDGE", "Recover_1_1_2")], model.selection("VERTEX", "PartSet/Origin"), SCALE)
model.testNbResults(Scale_2, 1)
model.testNbSubResults(Scale_2, [0])
model.testNbSubShapes(Scale_2, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Scale_2, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Scale_2, GeomAPI_Shape.EDGE, [1])
model.testNbSubShapes(Scale_2, GeomAPI_Shape.VERTEX, [2])
model.testResultsVolumes(Scale_2, [0])
refPoint = GeomAPI_Pnt(Compound_1.result().subResult(1).resultSubShapePair()[0].shape().middlePoint().xyz().multiplied(SCALE))
midPoint = Scale_2.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)

Recover_2 = model.addRecover(Part_1_doc, Scale_2, [Recover_1.result()])
Scale_3 = model.addScale(Part_1_doc, [model.selection("WIRE", "Recover_2_1_3")], model.selection("VERTEX", "PartSet/Origin"), SCALE)
model.testNbResults(Scale_3, 1)
model.testNbSubResults(Scale_3, [0])
model.testNbSubShapes(Scale_3, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Scale_3, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Scale_3, GeomAPI_Shape.EDGE, [2])
model.testNbSubShapes(Scale_3, GeomAPI_Shape.VERTEX, [4])
model.testResultsVolumes(Scale_3, [0])
refPoint = GeomAPI_Pnt(Compound_1.result().subResult(2).resultSubShapePair()[0].shape().middlePoint().xyz().multiplied(SCALE))
midPoint = Scale_3.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)

Recover_3 = model.addRecover(Part_1_doc, Scale_3, [Recover_2.result()])
Scale_4 = model.addScale(Part_1_doc, [model.selection("FACE", "Recover_3_1_4")], model.selection("VERTEX", "PartSet/Origin"), SCALE)
model.testNbResults(Scale_4, 1)
model.testNbSubResults(Scale_4, [0])
model.testNbSubShapes(Scale_4, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Scale_4, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Scale_4, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Scale_4, GeomAPI_Shape.VERTEX, [8])
model.testResultsVolumes(Scale_4, [109.154152964914914 * (SCALE**2)])
refPoint = GeomAPI_Pnt(Compound_1.result().subResult(3).resultSubShapePair()[0].shape().middlePoint().xyz().multiplied(SCALE))
midPoint = Scale_4.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)

Recover_4 = model.addRecover(Part_1_doc, Scale_4, [Recover_3.result()])
Scale_5 = model.addScale(Part_1_doc, [model.selection("SHELL", "Recover_4_1_5")], model.selection("VERTEX", "PartSet/Origin"), SCALE)
model.testNbResults(Scale_5, 1)
model.testNbSubResults(Scale_5, [0])
model.testNbSubShapes(Scale_5, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Scale_5, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Scale_5, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Scale_5, GeomAPI_Shape.VERTEX, [16])
model.testResultsVolumes(Scale_5, [182.822012116 * (SCALE**2)])
refPoint = GeomAPI_Pnt(Compound_1.result().subResult(4).resultSubShapePair()[0].shape().middlePoint().xyz().multiplied(SCALE))
midPoint = Scale_5.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)

Recover_5 = model.addRecover(Part_1_doc, Scale_5, [Recover_4.result()])
Scale_6 = model.addScale(Part_1_doc, [model.selection("SOLID", "Recover_5_1_6")], model.selection("VERTEX", "PartSet/Origin"), SCALE)
model.testNbResults(Scale_6, 1)
model.testNbSubResults(Scale_6, [0])
model.testNbSubShapes(Scale_6, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Scale_6, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Scale_6, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Scale_6, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(Scale_6, [1000 * (SCALE**3)])
refPoint = GeomAPI_Pnt(Compound_1.result().subResult(5).resultSubShapePair()[0].shape().middlePoint().xyz().multiplied(SCALE))
midPoint = Scale_6.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)

Recover_6 = model.addRecover(Part_1_doc, Scale_6, [Recover_5.result()])
Scale_7 = model.addScale(Part_1_doc, [model.selection("COMPSOLID", "Recover_6_1_7")], model.selection("VERTEX", "PartSet/Origin"), SCALE)
model.testNbResults(Scale_7, 1)
model.testNbSubResults(Scale_7, [3])
model.testNbSubShapes(Scale_7, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(Scale_7, GeomAPI_Shape.FACE, [17])
model.testNbSubShapes(Scale_7, GeomAPI_Shape.EDGE, [66])
model.testNbSubShapes(Scale_7, GeomAPI_Shape.VERTEX, [132])
model.testResultsVolumes(Scale_7, [1589.0486226 * (SCALE**3)])
refPoint = GeomAPI_Pnt(Compound_1.result().subResult(6).resultSubShapePair()[0].shape().middlePoint().xyz().multiplied(SCALE))
midPoint = Scale_7.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)

Recover_7 = model.addRecover(Part_1_doc, Scale_7, [Recover_6.result()])
Scale_8 = model.addScale(Part_1_doc, [model.selection("COMPOUND", "Recover_7_1_8")], model.selection("VERTEX", "PartSet/Origin"), SCALE)
model.testNbResults(Scale_8, 1)
model.testNbSubResults(Scale_8, [3])
model.testNbSubShapes(Scale_8, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(Scale_8, GeomAPI_Shape.FACE, [18])
model.testNbSubShapes(Scale_8, GeomAPI_Shape.EDGE, [72])
model.testNbSubShapes(Scale_8, GeomAPI_Shape.VERTEX, [144])
model.testResultsVolumes(Scale_8, [3000 * (SCALE**3)])
refPoint = GeomAPI_Pnt(Compound_1.result().subResult(7).resultSubShapePair()[0].shape().middlePoint().xyz().multiplied(SCALE))
midPoint = Scale_8.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)

model.end()

assert(model.checkPythonDump())
