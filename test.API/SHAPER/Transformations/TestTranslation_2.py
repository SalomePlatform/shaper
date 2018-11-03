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
Test case for Translation feature.
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
Box_21 = model.addBox(Part_1_doc, 10, 10, 10)
Box_22 = model.addBox(Part_1_doc, 10, 10, 10)
Box_23 = model.addBox(Part_1_doc, 10, 10, 10)
Box_24 = model.addBox(Part_1_doc, 10, 10, 10)
Box_25 = model.addBox(Part_1_doc, 10, 10, 10)
Box_26 = model.addBox(Part_1_doc, 10, 10, 10)
Box_27 = model.addBox(Part_1_doc, 10, 10, 10)
Box_28 = model.addBox(Part_1_doc, 10, 10, 10)
Box_29 = model.addBox(Part_1_doc, 10, 10, 10)
Box_30 = model.addBox(Part_1_doc, 10, 10, 10)
Box_31 = model.addBox(Part_1_doc, 10, 10, 10)
Box_32 = model.addBox(Part_1_doc, 10, 10, 10)
Box_33 = model.addBox(Part_1_doc, 10, 10, 10)
Box_34 = model.addBox(Part_1_doc, 10, 10, 10)
Box_35 = model.addBox(Part_1_doc, 10, 10, 10)
Box_36 = model.addBox(Part_1_doc, 10, 10, 10)

####################
# Sketches
# Sketch contenant un triangle
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(105.8795836373178, 106.9780351893485, -55.30451701345322, -118.6797057217308)
SketchLine_2 = Sketch_1.addLine(-55.30451701345322, -118.6797057217308, -134.9484255703047, 86.11891628160174)
SketchLine_3 = Sketch_1.addLine(-134.9484255703047, 86.11891628160174, 105.8795836373178, 106.9780351893485)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())

model.do()

# Edges
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_2")])

# Axis
Axis_1 = model.addAxis(Part_1_doc, 10, 10, 10)

# Parameters
model.addParameter(Part_1_doc, "d", "15")

# Translation 27
Translation_27_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "PartSet/OX"), 5)
Intersection_1 = model.addIntersection(Part_1_doc, [model.selection("SOLID", "Box_2_1"), model.selection("SOLID", "Translation_1_1")])
Translation_27 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "Intersection_1_1")], model.selection("EDGE", "PartSet/OX"), 15)

# Translation 28
Translation_28_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_3_1")], model.selection("EDGE", "PartSet/OX"), 5)
Intersection_2 = model.addIntersection(Part_1_doc, [model.selection("SOLID", "Box_4_1"), model.selection("SOLID", "Translation_3_1")])
Translation_28 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "Intersection_2_1")], model.selection("EDGE", "PartSet/OX"), 0)

# Translation 29
Translation_29_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_5_1")], model.selection("EDGE", "PartSet/OX"), 5)
Intersection_3 = model.addIntersection(Part_1_doc, [model.selection("SOLID", "Box_6_1"), model.selection("SOLID", "Translation_5_1")])
Translation_29 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "Intersection_3_1")], model.selection("EDGE", "PartSet/OX"), -15)

# Translation 30
Translation_30_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_7_1")], model.selection("EDGE", "PartSet/OX"), 5)
Intersection_4 = model.addIntersection(Part_1_doc, [model.selection("SOLID", "Box_8_1"), model.selection("SOLID", "Translation_7_1")])
Translation_30 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "Intersection_4_1")], model.selection("EDGE", "InvalidName"), -15)

# Translation 31
Translation_31_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_9_1")], model.selection("EDGE", "PartSet/OX"), 5)
Intersection_5 = model.addIntersection(Part_1_doc, [model.selection("SOLID", "Box_10_1"), model.selection("SOLID", "Translation_9_1")])
Translation_31 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "Intersection_5_1")], model.selection("EDGE", "PartSet/OY"), "d")

# Translation 32
Translation_32_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_11_1")], model.selection("EDGE", "PartSet/OX"), 5)
Intersection_6 = model.addIntersection(Part_1_doc, [model.selection("SOLID", "Box_12_1"), model.selection("SOLID", "Translation_11_1")])
Translation_32 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "Intersection_6_1")], model.selection("EDGE", "Sketch_1/SketchLine_1"), 15)

# Translation 33
Translation_33_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_13_1")], model.selection("EDGE", "PartSet/OX"), 5)
Intersection_7 = model.addIntersection(Part_1_doc, [model.selection("SOLID", "Box_14_1"), model.selection("SOLID", "Translation_13_1")])
Translation_33 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "Intersection_7_1")], model.selection("EDGE", "Edge_1_1"), 15)

# Translation 34
Translation_34_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_15_1")], model.selection("EDGE", "PartSet/OX"), 5)
Intersection_8 = model.addIntersection(Part_1_doc, [model.selection("SOLID", "Box_16_1"), model.selection("SOLID", "Translation_15_1")])
Translation_34 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "Intersection_8_1")], model.selection("EDGE", "Intersection_8_1_6"), 15)

# Translation 35
Translation_35_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_17_1")], model.selection("EDGE", "PartSet/OX"), 5)
Intersection_9 = model.addIntersection(Part_1_doc, [model.selection("SOLID", "Box_18_1"), model.selection("SOLID", "Translation_17_1")])
Translation_35 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "Intersection_9_1")], model.selection("EDGE", "Axis_1"), 15)

#Translation 36
Translation_36_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_19_1")], model.selection("EDGE", "PartSet/OX"), 5)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_20_1"), model.selection("SOLID", "Translation_19_1")])
Translation_36 = model.addTranslation(Part_1_doc, [model.selection("COMPSOLID", "Partition_1_1")], model.selection("EDGE", "PartSet/OX"), 15)

## Translation 37
Translation_37_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_21_1")], model.selection("EDGE", "PartSet/OX"), 5)
Partition_2 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_22_1"), model.selection("SOLID", "Translation_21_1")])
Translation_37 = model.addTranslation(Part_1_doc, [model.selection("COMPSOLID", "Partition_2_1")], model.selection("EDGE", "PartSet/OX"), 0)

## Translation 38
Translation_38_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_23_1")], model.selection("EDGE", "PartSet/OX"), 5)
Partition_3 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_24_1"), model.selection("SOLID", "Translation_23_1")])
Translation_38 = model.addTranslation(Part_1_doc, [model.selection("COMPSOLID", "Partition_3_1")], model.selection("EDGE", "PartSet/OX"), 0)

## Translation 39
Translation_39_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_25_1")], model.selection("EDGE", "PartSet/OX"), 5)
Partition_4 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_26_1"), model.selection("SOLID", "Translation_25_1")])
Translation_39 = model.addTranslation(Part_1_doc, [model.selection("COMPSOLID", "Partition_4_1")], model.selection("EDGE", "InvalidName"), 0)

## Translation 40
Translation_40_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_27_1")], model.selection("EDGE", "PartSet/OX"), 5)
Partition_5 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_28_1"), model.selection("SOLID", "Translation_27_1")])
Translation_40 = model.addTranslation(Part_1_doc, [model.selection("COMPSOLID", "Partition_5_1")], model.selection("EDGE", "PartSet/OX"), "d")

# Translation 41
Translation_41_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_29_1")], model.selection("EDGE", "PartSet/OX"), 5)
Partition_6 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_30_1"), model.selection("SOLID", "Translation_29_1")])
Translation_41 = model.addTranslation(Part_1_doc, [model.selection("COMPSOLID", "Partition_6_1")], model.selection("EDGE", "Sketch_1/SketchLine_1"), 15)

# Translation 42
Translation_42_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_31_1")], model.selection("EDGE", "PartSet/OX"), 5)
Partition_7 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_32_1"), model.selection("SOLID", "Translation_31_1")])
Translation_42 = model.addTranslation(Part_1_doc, [model.selection("COMPSOLID", "Partition_7_1")], model.selection("EDGE", "Edge_1_1"), 15)

# Translation 43
Translation_43_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_33_1")], model.selection("EDGE", "PartSet/OX"), 5)
Partition_8 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_34_1"), model.selection("SOLID", "Translation_33_1")])
Translation_43 = model.addTranslation(Part_1_doc, [model.selection("COMPSOLID", "Partition_8_1")], model.selection("EDGE", "Partition_8_1_1/Modified_Face_2_5&Partition_8_1_1/Modified_Face_2_2"), 15)

# Translation 44
Translation_44_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_35_1")], model.selection("EDGE", "PartSet/OX"), 5)
Partition_9 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_36_1"), model.selection("SOLID", "Translation_35_1")])
Translation_44 = model.addTranslation(Part_1_doc, [model.selection("COMPSOLID", "Partition_9_1")], model.selection("EDGE", "Axis_1"), 15)
model.do()
model.end()

# Checks
from GeomAPI import GeomAPI_Shape

# Translation 27
model.testNbResults(Translation_27, 1)
model.testNbSubResults(Translation_27, [12])
model.testNbSubShapes(Translation_27, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_27, GeomAPI_Shape.EDGE, [12])
model.testHaveNamingEdges(Translation_27, model, Part_1_doc)

# Translation 28
model.testNbResults(Translation_28, 1)
model.testNbSubResults(Translation_28, [12])
model.testNbSubShapes(Translation_28, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_28, GeomAPI_Shape.EDGE, [12])
model.testHaveNamingEdges(Translation_28, model, Part_1_doc)

# Translation 29
model.testNbResults(Translation_29, 1)
model.testNbSubResults(Translation_29, [12])
model.testNbSubShapes(Translation_29, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_29, GeomAPI_Shape.EDGE, [12])
model.testHaveNamingEdges(Translation_29, model, Part_1_doc)

# Translation 31
model.testNbResults(Translation_31, 1)
model.testNbSubResults(Translation_31, [12])
model.testNbSubShapes(Translation_31, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_31, GeomAPI_Shape.EDGE, [12])
model.testHaveNamingEdges(Translation_31, model, Part_1_doc)

# Translation 32
model.testNbResults(Translation_32, 1)
model.testNbSubResults(Translation_32, [12])
model.testNbSubShapes(Translation_32, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_32, GeomAPI_Shape.EDGE, [12])
model.testHaveNamingEdges(Translation_32, model, Part_1_doc)

# Translation 33
model.testNbResults(Translation_33, 1)
model.testNbSubResults(Translation_33, [12])
model.testNbSubShapes(Translation_33, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_33, GeomAPI_Shape.EDGE, [12])
model.testHaveNamingEdges(Translation_33, model, Part_1_doc)

# Translation 34
model.testNbResults(Translation_34, 1)
model.testNbSubResults(Translation_34, [12])
model.testNbSubShapes(Translation_34, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_34, GeomAPI_Shape.EDGE, [12])
model.testHaveNamingEdges(Translation_34, model, Part_1_doc)

# Translation 35
model.testNbResults(Translation_35, 1)
model.testNbSubResults(Translation_35, [12])
model.testNbSubShapes(Translation_35, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_35, GeomAPI_Shape.EDGE, [12])
model.testHaveNamingEdges(Translation_35, model, Part_1_doc)

# Translation 36
model.testNbResults(Translation_36, 1)
model.testNbSubResults(Translation_36, [3])
model.testNbSubShapes(Translation_36, GeomAPI_Shape.COMPSOLID, [1])
model.testNbSubShapes(Translation_36, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(Translation_36, GeomAPI_Shape.FACE, [18])
model.testHaveNamingFaces(Translation_36, model, Part_1_doc)

# Translation 37
model.testNbResults(Translation_37, 1)
model.testNbSubResults(Translation_37, [3])
model.testNbSubShapes(Translation_37, GeomAPI_Shape.COMPSOLID, [1])
model.testNbSubShapes(Translation_37, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(Translation_37, GeomAPI_Shape.FACE, [18])
model.testHaveNamingFaces(Translation_37, model, Part_1_doc)

# Translation 38
model.testNbResults(Translation_38, 1)
model.testNbSubResults(Translation_38, [3])
model.testNbSubShapes(Translation_38, GeomAPI_Shape.COMPSOLID, [1])
model.testNbSubShapes(Translation_38, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(Translation_38, GeomAPI_Shape.FACE, [18])
model.testHaveNamingFaces(Translation_38, model, Part_1_doc)

# Translation 40
model.testNbResults(Translation_40, 1)
model.testNbSubResults(Translation_40, [3])
model.testNbSubShapes(Translation_40, GeomAPI_Shape.COMPSOLID, [1])
model.testNbSubShapes(Translation_40, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(Translation_40, GeomAPI_Shape.FACE, [18])
model.testHaveNamingFaces(Translation_40, model, Part_1_doc)

# Translation 41
model.testNbResults(Translation_41, 1)
model.testNbSubResults(Translation_41, [3])
model.testNbSubShapes(Translation_41, GeomAPI_Shape.COMPSOLID, [1])
model.testNbSubShapes(Translation_41, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(Translation_41, GeomAPI_Shape.FACE, [18])
model.testHaveNamingFaces(Translation_41, model, Part_1_doc)

# Translation 42
model.testNbResults(Translation_42, 1)
model.testNbSubResults(Translation_42, [3])
model.testNbSubShapes(Translation_42, GeomAPI_Shape.COMPSOLID, [1])
model.testNbSubShapes(Translation_42, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(Translation_42, GeomAPI_Shape.FACE, [18])
model.testHaveNamingFaces(Translation_42, model, Part_1_doc)

# Translation 43
# It is concealed by Partition_9
#model.testNbResults(Translation_43, 1)
#model.testNbSubResults(Translation_43, [3])
#model.testNbSubShapes(Translation_43, GeomAPI_Shape.COMPSOLID, [1])
#model.testNbSubShapes(Translation_43, GeomAPI_Shape.SOLID, [3])
#model.testNbSubShapes(Translation_43, GeomAPI_Shape.FACE, [18])
#model.testHaveNamingFaces(Translation_43, model, Part_1_doc)

# Translation 44
model.testNbResults(Translation_44, 1)
model.testNbSubResults(Translation_44, [3])
model.testNbSubShapes(Translation_44, GeomAPI_Shape.COMPSOLID, [1])
model.testNbSubShapes(Translation_44, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(Translation_44, GeomAPI_Shape.FACE, [18])
model.testHaveNamingFaces(Translation_44, model, Part_1_doc)

# Translation 30
model.testNbResults(Translation_30, 0)
assert(Translation_30.feature().error() == 'Attribute "axis_object" is not initialized.')

# Translation 39
model.testNbResults(Translation_39, 0)
assert(Translation_39.feature().error() == 'Attribute "axis_object" is not initialized.')