# Copyright (C) 2020-2024  CEA, EDF
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
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_2_1"), model.selection("SOLID", "Cylinder_1_1")], keepSubResults = True)
Box_3 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_2 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_3_1")], model.selection("EDGE", "PartSet/OY"), 50)
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("SOLID", "Translation_2_1")], model.selection("EDGE", "PartSet/OZ"), 30, 3)
model.end()

TOLERANCE = 1.e-7

def symmetryOZ(thePoint):
    return GeomAPI_Pnt(-thePoint.x(), -thePoint.y(), thePoint.z())


model.begin()
Symmetry_1 = model.addSymmetry(Part_1_doc, [model.selection("VERTEX", "Vertex_1_1")], model.selection("EDGE", "PartSet/OZ"), keepOriginal = False, keepSubResults = True)
model.testNbResults(Symmetry_1, 1)
model.testNbSubResults(Symmetry_1, [0])
model.testNbSubShapes(Symmetry_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Symmetry_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Symmetry_1, GeomAPI_Shape.EDGE, [0])
model.testNbSubShapes(Symmetry_1, GeomAPI_Shape.VERTEX, [1])
model.testResultsVolumes(Symmetry_1, [0])
model.testResultsAreas(Symmetry_1, [0])
refPoint = symmetryOZ(Vertex_1.defaultResult().shape().middlePoint())
midPoint = Symmetry_1.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)

Symmetry_2 = model.addSymmetry(Part_1_doc, [model.selection("EDGE", "Edge_1_1")], model.selection("EDGE", "PartSet/OZ"), keepOriginal = False, keepSubResults = True)
model.testNbResults(Symmetry_2, 1)
model.testNbSubResults(Symmetry_2, [0])
model.testNbSubShapes(Symmetry_2, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Symmetry_2, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Symmetry_2, GeomAPI_Shape.EDGE, [1])
model.testNbSubShapes(Symmetry_2, GeomAPI_Shape.VERTEX, [2])
model.testResultsVolumes(Symmetry_2, [0])
model.testResultsAreas(Symmetry_2, [0])
refPoint = symmetryOZ(Edge_1.defaultResult().shape().middlePoint())
midPoint = Symmetry_2.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)

Symmetry_3 = model.addSymmetry(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], model.selection("EDGE", "PartSet/OZ"), keepOriginal = False, keepSubResults = True)
model.testNbResults(Symmetry_3, 1)
model.testNbSubResults(Symmetry_3, [0])
model.testNbSubShapes(Symmetry_3, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Symmetry_3, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Symmetry_3, GeomAPI_Shape.EDGE, [2])
model.testNbSubShapes(Symmetry_3, GeomAPI_Shape.VERTEX, [4])
model.testResultsVolumes(Symmetry_3, [0])
model.testResultsAreas(Symmetry_3, [0])
refPoint = symmetryOZ(Wire_1.defaultResult().shape().middlePoint())
midPoint = Symmetry_3.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)

Symmetry_4 = model.addSymmetry(Part_1_doc, [model.selection("FACE", "Face_1_1")], model.selection("EDGE", "PartSet/OZ"), keepOriginal = False, keepSubResults = True)
model.testNbResults(Symmetry_4, 1)
model.testNbSubResults(Symmetry_4, [0])
model.testNbSubShapes(Symmetry_4, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Symmetry_4, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Symmetry_4, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Symmetry_4, GeomAPI_Shape.VERTEX, [8])
model.testResultsVolumes(Symmetry_4, [0])
model.testResultsAreas(Symmetry_4, [109.154152964914914])
refPoint = symmetryOZ(Face_1.defaultResult().shape().middlePoint())
midPoint = Symmetry_4.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)

Symmetry_5 = model.addSymmetry(Part_1_doc, [model.selection("SHELL", "Shell_1_1")], model.selection("EDGE", "PartSet/OZ"), keepOriginal = False, keepSubResults = True)
model.testNbResults(Symmetry_5, 1)
model.testNbSubResults(Symmetry_5, [0])
model.testNbSubShapes(Symmetry_5, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Symmetry_5, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Symmetry_5, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Symmetry_5, GeomAPI_Shape.VERTEX, [16])
model.testResultsVolumes(Symmetry_5, [0])
model.testResultsAreas(Symmetry_5, [182.822012116])
refPoint = symmetryOZ(Shell_1.defaultResult().shape().middlePoint())
midPoint = Symmetry_5.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)

Symmetry_6 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Translation_1_1")], model.selection("EDGE", "PartSet/OZ"), keepOriginal = False, keepSubResults = True)
model.testNbResults(Symmetry_6, 1)
model.testNbSubResults(Symmetry_6, [0])
model.testNbSubShapes(Symmetry_6, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Symmetry_6, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Symmetry_6, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Symmetry_6, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(Symmetry_6, [1000])
refPoint = symmetryOZ(Translation_1.defaultResult().shape().middlePoint())
midPoint = Symmetry_6.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)

Symmetry_7 = model.addSymmetry(Part_1_doc, [model.selection("COMPSOLID", "Partition_1_1")], model.selection("EDGE", "PartSet/OZ"), keepOriginal = False, keepSubResults = True)
model.testNbResults(Symmetry_7, 1)
model.testNbSubResults(Symmetry_7, [3])
model.testNbSubShapes(Symmetry_7, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(Symmetry_7, GeomAPI_Shape.FACE, [17])
model.testNbSubShapes(Symmetry_7, GeomAPI_Shape.EDGE, [66])
model.testNbSubShapes(Symmetry_7, GeomAPI_Shape.VERTEX, [132])
model.testResultsVolumes(Symmetry_7, [1589.0486226])
refPoint = symmetryOZ(Partition_1.defaultResult().shape().middlePoint())
midPoint = Symmetry_7.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)

Symmetry_8 = model.addSymmetry(Part_1_doc, [model.selection("COMPOUND", "AngularCopy_1_1")], model.selection("EDGE", "PartSet/OZ"), keepOriginal = False, keepSubResults = True)
model.testNbResults(Symmetry_8, 1)
model.testNbSubResults(Symmetry_8, [3])
model.testNbSubShapes(Symmetry_8, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(Symmetry_8, GeomAPI_Shape.FACE, [18])
model.testNbSubShapes(Symmetry_8, GeomAPI_Shape.EDGE, [72])
model.testNbSubShapes(Symmetry_8, GeomAPI_Shape.VERTEX, [144])
model.testResultsVolumes(Symmetry_8, [3000])
refPoint = symmetryOZ(AngularCopy_1.defaultResult().shape().middlePoint())
midPoint = Symmetry_8.defaultResult().shape().middlePoint()
assert(midPoint.distance(refPoint) < TOLERANCE)

model.end()

assert(model.checkPythonDump())
