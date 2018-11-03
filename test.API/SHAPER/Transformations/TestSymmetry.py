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

"""
Test case for Symmetry feature.
Written on High API.
"""
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Boxes
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Box_3 = model.addBox(Part_1_doc, 10, 10, 10)
Box_4 = model.addBox(Part_1_doc, 10, 10, 10)
Box_5 = model.addBox(Part_1_doc, 10, 10, 10)
Box_6 = model.addBox(Part_1_doc, 10, 10, 10)
Box_7 = model.addBox(Part_1_doc, 10, 10, 10)
Box_8 = model.addBox(Part_1_doc, 10, 10, 10)
Box_9 = model.addBox(Part_1_doc, 10, 10, 10)
Box_10 = model.addBox(Part_1_doc, 10, 10, 10)
Box_11 = model.addBox(Part_1_doc, 10, 10, 10)
Box_12 = model.addBox(Part_1_doc, 10, 10, 10)
Box_13 = model.addBox(Part_1_doc, 10, 10, 10)
Box_14 = model.addBox(Part_1_doc, 10, 10, 10)
Box_15 = model.addBox(Part_1_doc, 10, 10, 10)
Box_16 = model.addBox(Part_1_doc, 10, 10, 10)
Box_17 = model.addBox(Part_1_doc, 10, 10, 10)
Box_18 = model.addBox(Part_1_doc, 10, 10, 10)
Box_19 = model.addBox(Part_1_doc, 10, 10, 10)
Box_20 = model.addBox(Part_1_doc, 10, 10, 10)

# Points
Point_1 = model.addPoint(Part_1_doc, -10, -10, -10)

# Axes
Axis_1 = model.addAxis(Part_1_doc, 10, 10, 20)

# Sketches
# Sketch comportant une ligne n'ayant dont les extremites ne coincident pas avec l'origine
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-60.61265429508839, -83.21124437581197, 133.0036727560886, 114.0582209216138)

# Sketch comportant une contour ferme de 4 lignes
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_2 = Sketch_2.addLine(82.71019756173548, 167.8442841418389, -114.8596207554555, -2.66979472427181)
SketchLine_3 = Sketch_2.addLine(-114.8596207554555, -2.66979472427181, 102.2154980962352, -58.03968011252918)
SketchLine_4 = Sketch_2.addLine(102.2154980962352, -58.03968011252918, 240.6402115668787, 65.28415552495312)
SketchLine_5 = Sketch_2.addLine(240.6402115668787, 65.28415552495312, 82.71019756173548, 167.8442841418389)
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_2.startPoint(), SketchLine_5.endPoint())

# Sketch comportant un contour ferme de 5 lignes
Sketch_3 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_6 = Sketch_3.addLine(26.85462764597397, 67.26291526312015, -53.39765785606262, 39.86755675840885)
SketchLine_7 = Sketch_3.addLine(-53.39765785606262, 39.86755675840885, -33.73745939974039, -8.799491879372397)
SketchLine_8 = Sketch_3.addLine(-33.73745939974039, -8.799491879372397, 33.30059435296487, -32.6495686952387)
SketchLine_9 = Sketch_3.addLine(33.30059435296487, -32.6495686952387, 19.44176593293446, 18.5958666253389)
SketchLine_10 = Sketch_3.addLine(19.44176593293446, 18.5958666253389, 26.85462764597397, 67.26291526312015)
SketchConstraintCoincidence_1 = Sketch_3.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_2 = Sketch_3.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_3 = Sketch_3.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_4 = Sketch_3.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_5 = Sketch_3.setCoincident(SketchLine_6.startPoint(), SketchLine_10.endPoint())

model.do()

# Vertexes
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_1/SketchLine_1_EndVertex")])

# Edges
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1")])

# Wire & Extrusion
Wire_1 = model.addWire(Part_1_doc, [model.selection("WIRE", "Sketch_3/Face-SketchLine_6r-SketchLine_7f-SketchLine_8f-SketchLine_9f-SketchLine_10f_wire")])
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], model.selection(), 50, 0)

# Planes
Plane_1 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_14_1/Top"), 10, False)
Plane_2 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOZ"), 40, False)

# Faces
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchLine_2r-SketchLine_3f-SketchLine_4f-SketchLine_5f_wire")])


# Symmetries
Symmetry_1 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("VERTEX", "PartSet/Origin"))
Symmetry_2 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_2_1")], model.selection("VERTEX", "Point_1"))
Symmetry_3 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_3_1")], model.selection("VERTEX", "[Box_3_1/Front][Box_3_1/Right][Box_3_1/Top]"))
Symmetry_4 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_4_1")], model.selection("VERTEX", "Sketch_1/SketchLine_1_EndVertex"))
Symmetry_5 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_5_1")], model.selection("VERTEX", "Vertex_1_1"))
Symmetry_6 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_6_1")], model.selection("VERTEX", "InvalidName"))
Symmetry_7 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_7_1")], model.selection("VERTEX", "PartSet/Origin"), False)

Symmetry_8 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_8_1")], model.selection("EDGE", "PartSet/OZ"))
Symmetry_9 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_9_1")], model.selection("EDGE", "Axis_1"))
Symmetry_10 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_10_1")], model.selection("EDGE", "[Box_9_1/Front][Box_9_1/Top]"))
Symmetry_11 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_11_1")], model.selection("EDGE", "Sketch_1/SketchLine_1"))
Symmetry_12 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_12_1")], model.selection("EDGE", "Edge_1_1"))
Symmetry_13 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_13_1")], model.selection("EDGE", "InvalidName"))
Symmetry_14 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_14_1")], model.selection("EDGE", "PartSet/OZ"), False)

Symmetry_15 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_15_1")], model.selection("FACE", "PartSet/XOY"))
Symmetry_16 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_16_1")], model.selection("FACE", "Plane_1"))
Symmetry_17 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_17_1")], model.selection("FACE", "Box_15_1/Front"))
Symmetry_18 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_18_1")], model.selection("FACE", "Face_1_1"))
Symmetry_19 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_19_1")], model.selection("FACE", "InvalidName"))
Symmetry_20 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Box_20_1")], model.selection("FACE", "PartSet/XOY"), False)
Symmetry_21 = model.addSymmetry(Part_1_doc, [model.selection("SHELL", "Extrusion_1_1")], model.selection("FACE", "Plane_2"))

model.do()
model.end()

# Checks
from GeomAPI import GeomAPI_Shape

model.testNbResults(Symmetry_1, 1)
model.testNbSubResults(Symmetry_1, [2])
model.testNbSubShapes(Symmetry_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Symmetry_1, GeomAPI_Shape.FACE, [12])
model.testHaveNamingFaces(Symmetry_1, model, Part_1_doc)

model.testNbResults(Symmetry_2, 1)
model.testNbSubResults(Symmetry_2, [2])
model.testNbSubShapes(Symmetry_2, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Symmetry_2, GeomAPI_Shape.FACE, [12])
model.testHaveNamingFaces(Symmetry_2, model, Part_1_doc)

model.testNbResults(Symmetry_3, 1)
model.testNbSubResults(Symmetry_3, [2])
model.testNbSubShapes(Symmetry_3, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Symmetry_3, GeomAPI_Shape.FACE, [12])
model.testHaveNamingFaces(Symmetry_3, model, Part_1_doc)

model.testNbResults(Symmetry_4, 1)
model.testNbSubResults(Symmetry_4, [2])
model.testNbSubShapes(Symmetry_4, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Symmetry_4, GeomAPI_Shape.FACE, [12])
model.testHaveNamingFaces(Symmetry_4, model, Part_1_doc)

model.testNbResults(Symmetry_5, 1)
model.testNbSubResults(Symmetry_5, [2])
model.testNbSubShapes(Symmetry_5, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Symmetry_5, GeomAPI_Shape.FACE, [12])
model.testHaveNamingFaces(Symmetry_5, model, Part_1_doc)

model.testNbResults(Symmetry_7, 1)
model.testNbSubResults(Symmetry_7, [0])
model.testNbSubShapes(Symmetry_7, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Symmetry_7, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Symmetry_7, model, Part_1_doc)

model.testNbResults(Symmetry_8, 1)
model.testNbSubResults(Symmetry_8, [2])
model.testNbSubShapes(Symmetry_8, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Symmetry_8, GeomAPI_Shape.FACE, [12])
model.testHaveNamingFaces(Symmetry_8, model, Part_1_doc)

model.testNbResults(Symmetry_9, 1)
model.testNbSubResults(Symmetry_9, [2])
model.testNbSubShapes(Symmetry_9, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Symmetry_9, GeomAPI_Shape.FACE, [12])
model.testHaveNamingFaces(Symmetry_9, model, Part_1_doc)

model.testNbResults(Symmetry_10, 0)

model.testNbResults(Symmetry_11, 1)
model.testNbSubResults(Symmetry_11, [2])
model.testNbSubShapes(Symmetry_11, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Symmetry_11, GeomAPI_Shape.FACE, [12])
model.testHaveNamingFaces(Symmetry_11, model, Part_1_doc)

model.testNbResults(Symmetry_12, 1)
model.testNbSubResults(Symmetry_12, [2])
model.testNbSubShapes(Symmetry_12, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Symmetry_12, GeomAPI_Shape.FACE, [12])
model.testHaveNamingFaces(Symmetry_12, model, Part_1_doc)

model.testNbResults(Symmetry_14, 1)
model.testNbSubResults(Symmetry_14, [0])
model.testNbSubShapes(Symmetry_14, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Symmetry_14, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Symmetry_14, model, Part_1_doc)

model.testNbResults(Symmetry_15, 1)
model.testNbSubResults(Symmetry_15, [2])
model.testNbSubShapes(Symmetry_15, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Symmetry_15, GeomAPI_Shape.FACE, [12])
model.testHaveNamingFaces(Symmetry_15, model, Part_1_doc)

model.testNbResults(Symmetry_16, 1)
model.testNbSubResults(Symmetry_16, [2])
model.testNbSubShapes(Symmetry_16, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Symmetry_16, GeomAPI_Shape.FACE, [12])
model.testHaveNamingFaces(Symmetry_16, model, Part_1_doc)

model.testNbResults(Symmetry_17, 0)

model.testNbResults(Symmetry_18, 1)
model.testNbSubResults(Symmetry_18, [2])
model.testNbSubShapes(Symmetry_18, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Symmetry_18, GeomAPI_Shape.FACE, [12])
model.testHaveNamingFaces(Symmetry_18, model, Part_1_doc)

model.testNbResults(Symmetry_20, 1)
model.testNbSubResults(Symmetry_20, [0])
model.testNbSubShapes(Symmetry_20, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Symmetry_20, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Symmetry_20, model, Part_1_doc)

model.testNbResults(Symmetry_6, 0)
assert(Symmetry_6.feature().error() == 'Attribute "point_object" is not initialized.')

model.testNbResults(Symmetry_13, 0)
assert(Symmetry_13.feature().error() == 'Attribute "axis_object" is not initialized.')

model.testNbResults(Symmetry_19, 0)
assert(Symmetry_19.feature().error() == 'Attribute "plane_object" is not initialized.')

model.testNbResults(Symmetry_21, 1)
model.testNbSubResults(Symmetry_21, [2])
model.testNbSubShapes(Symmetry_21, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Symmetry_21, GeomAPI_Shape.SHELL, [2])
model.testNbSubShapes(Symmetry_21, GeomAPI_Shape.FACE, [10])
model.testHaveNamingFaces(Symmetry_21, model, Part_1_doc)
