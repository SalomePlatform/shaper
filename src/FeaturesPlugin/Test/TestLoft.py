# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(65.56914119359536, 34.84279475982533, -62.33478893740904, 34.84279475982533)

### Create SketchLine
SketchLine_2 = Sketch_1.addLine(-62.33478893740904, 34.84279475982533, -62.33478893740904, -28.08005822416302)

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(-62.33478893740904, -28.08005822416302, 65.56914119359536, -28.08005822416302)

### Create SketchLine
SketchLine_4 = Sketch_1.addLine(65.56914119359536, -28.08005822416302, 65.56914119359536, 34.84279475982533)
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
Sketch_1.setHorizontal(SketchLine_1.result())
Sketch_1.setVertical(SketchLine_2.result())
Sketch_1.setHorizontal(SketchLine_3.result())
Sketch_1.setVertical(SketchLine_4.result())
model.do()

### Create Plane
Plane_4 = model.addPlane(Part_1_doc, model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("VERTEX", "Sketch_1/SketchLine_3_StartVertex"), False)

### Create Plane
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f"), 100, False)

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))

### Create SketchCircle
SketchCircle_1 = Sketch_2.addCircle(60.9581866489876, 39.76685108527785, 26.86156014153439)
model.do()

### Create Plane
Plane_6 = model.addPlane(Part_1_doc, model.selection("FACE", "Sketch_2/Face-SketchCircle_1_2r"), 100, False)

### Create Box
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)

### Create Box
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)

### Create Translation
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "all-in-Box_2")], axis = model.selection("EDGE", "PartSet/OY"), distance = 100, keepSubResults = True)

### Create Wire
Wire_1_objects = [model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]"),
                  model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Top]"),
                  model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Right]"),
                  model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Bottom]")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects, False)

### Create Wire
Wire_2_objects = [model.selection("EDGE", "[Translation_1_1_1/MF:Translated&Box_2_1/Left][Translation_1_1_1/MF:Translated&Box_2_1/Top]"),
                  model.selection("EDGE", "[Translation_1_1_1/MF:Translated&Box_2_1/Front][Translation_1_1_1/MF:Translated&Box_2_1/Left]"),
                  model.selection("EDGE", "[Translation_1_1_1/MF:Translated&Box_2_1/Left][Translation_1_1_1/MF:Translated&Box_2_1/Bottom]"),
                  model.selection("EDGE", "[Translation_1_1_1/MF:Translated&Box_2_1/Back][Translation_1_1_1/MF:Translated&Box_2_1/Left]")]
Wire_2 = model.addWire(Part_1_doc, Wire_2_objects, False)

### Create Box
Box_3 = model.addBox(Part_1_doc, 10, 10, 10)

### Create Wire
Wire_3 = model.addWire(Part_1_doc, [model.selection("EDGE", "[Box_3_1/Left][Box_3_1/Top]"), model.selection("EDGE", "[Box_3_1/Front][Box_3_1/Left]")], False)

### Create Box
Box_4 = model.addBox(Part_1_doc, 10, 10, 10)

### Create Translation
Translation_2 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "all-in-Box_4")], axis = model.selection("EDGE", "PartSet/OY"), distance = 40, keepSubResults = True)

### Create Wire
Wire_4 = model.addWire(Part_1_doc, [model.selection("EDGE", "[Translation_2_1_1/MF:Translated&Box_4_1/Left][Translation_2_1_1/MF:Translated&Box_4_1/Top]"), model.selection("EDGE", "[Translation_2_1_1/MF:Translated&Box_4_1/Front][Translation_2_1_1/MF:Translated&Box_4_1/Left]")], False)

### Create Loft
Loft_1 = model.addLoft(Part_1_doc, model.selection("FACE", "Plane_2"), model.selection("FACE", "Plane_1"))

### Create Loft
Loft_2 = model.addLoft(Part_1_doc, model.selection("WIRE", "Wire_2_1"), model.selection("WIRE", "Wire_1_1"))

### Create Loft
Loft_3 = model.addLoft(Part_1_doc, model.selection("WIRE", "Wire_3_1"), model.selection("WIRE", "Wire_4_1"))

### Create Loft
Loft_4 = model.addLoft(Part_1_doc, model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchLine_4"))

# # Errors tests
# Wire and Edge
Box_5 = model.addBox(Part_1_doc, 10, 10, 10)
Wire_5_objects = [model.selection("EDGE", "[Box_5_1/Back][Box_5_1/Left]"),
                  model.selection("EDGE", "[Box_5_1/Left][Box_5_1/Top]"),
                  model.selection("EDGE", "[Box_5_1/Front][Box_5_1/Left]"),
                  model.selection("EDGE", "[Box_5_1/Left][Box_5_1/Bottom]")]
Wire_5 = model.addWire(Part_1_doc, Wire_5_objects, False)
Box_6 = model.addBox(Part_1_doc, 10, 10, 10)
Loft_5 = model.addLoft(Part_1_doc, model.selection("WIRE", "Wire_5_1"), model.selection("EDGE", "[Box_6_1/Right][Box_6_1/Top]"))
assert(Loft_5.feature().error() == "Error: the shapes have different type")

# Wire and Face
Wire_6_objects = [model.selection("EDGE", "[Loft_1_1/Loft_Face_1][Loft_1_1/Loft_Face_5]"),
                  model.selection("EDGE", "[Loft_1_1/Loft_Face_1][Loft_1_1/Loft_Face_4]"),
                  model.selection("EDGE", "[Loft_1_1/Loft_Face_1][Loft_1_1/Loft_Face_2]"),
                  model.selection("EDGE", "[Loft_1_1/Loft_Face_1][Loft_1_1/Loft_Face_6]")]
Wire_6 = model.addWire(Part_1_doc, Wire_6_objects, False)
Loft_6 = model.addLoft(Part_1_doc, model.selection("WIRE", "Wire_6_1"), model.selection("EDGE", "Plane_3"))
assert(Loft_6.feature().error() == "Error: the shapes have different type")

# Only one element
Box_7 = model.addBox(Part_1_doc, 10, 10, 10)
Loft_7 = model.addLoft(Part_1_doc, model.selection("SOLID", "Box_7_1"), model.selection())
assert(Loft_7.feature().error() == 'Error: Could not get "second_object" attribute.')

# Two faces with different number of edges
Cone_1 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10, 5, 10)
Loft_8 = model.addLoft(Part_1_doc, model.selection("FACE", "Cone_1_1/Face_2"), model.selection("EDGE", "Plane_3"))

model.end()

from GeomAPI import GeomAPI_Shape

#test loft with two face
model.testNbResults(Loft_1, 1)
model.testNbSubResults(Loft_1, [0])
model.testNbSubShapes(Loft_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Loft_1, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Loft_1, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Loft_1, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(Loft_1, [996988.58940])

#test loft with two wire
model.testNbResults(Loft_2, 1)
model.testNbSubResults(Loft_2, [0])
model.testNbSubShapes(Loft_2, GeomAPI_Shape.SHELL, [1])
model.testNbSubShapes(Loft_2, GeomAPI_Shape.FACE, [4])
model.testNbSubShapes(Loft_2, GeomAPI_Shape.EDGE, [16])
model.testNbSubShapes(Loft_2, GeomAPI_Shape.VERTEX, [32])
model.testResultsVolumes(Loft_2, [0.0])

#test loft with two wire
model.testNbResults(Loft_3, 1)
model.testNbSubResults(Loft_3, [0])
model.testNbSubShapes(Loft_3, GeomAPI_Shape.SHELL, [1])
model.testNbSubShapes(Loft_3, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Loft_3, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Loft_3, GeomAPI_Shape.VERTEX, [16])
model.testResultsVolumes(Loft_3, [0.0])

#test loft with two edge
model.testNbResults(Loft_4, 1)
model.testNbSubResults(Loft_4, [0])
model.testNbSubShapes(Loft_4, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Loft_4, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Loft_4, GeomAPI_Shape.VERTEX, [8])
model.testResultsVolumes(Loft_4, [0.0])

#test loft with faces with different number of edges
model.testNbResults(Loft_8, 1)
model.testNbSubResults(Loft_8, [0])
model.testNbSubShapes(Loft_8, GeomAPI_Shape.FACE, [7])
model.testNbSubShapes(Loft_8, GeomAPI_Shape.EDGE, [30])
model.testNbSubShapes(Loft_8, GeomAPI_Shape.VERTEX, [60])
model.testResultsVolumes(Loft_8, [151868.70541])




