# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

"""
Test case for Rotation feature.
Written on High API.
"""
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Create Boxes
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

# Create Points
Point_1 = model.addPoint(Part_1_doc, 20, 0, 0)
Point_2 = model.addPoint(Part_1_doc, 20, 20, 0)
Point_3 = model.addPoint(Part_1_doc, -10, 0, 0)

# Create Parts
Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Box_16 = model.addBox(Part_2_doc, 10, 10, 10)
Cylinder_1 = model.addCylinder(Part_2_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)

Part_3 = model.addPart(partSet)
Part_3_doc = Part_3.document()
Box_17 = model.addBox(Part_3_doc, 10, 10, 10)
Cylinder_1 = model.addCylinder(Part_3_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Point_4 = model.addPoint(partSet, -10, 0, 0)
Point_5 = model.addPoint(partSet, 20, 20, 0)

model.do()

# Create Rotations
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "PartSet/OZ"), 45)
Rotation_2 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], model.selection("EDGE", "PartSet/OZ"), 0)
Rotation_3 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Box_3_1")], model.selection("EDGE", "PartSet/OZ"), 360)
Rotation_4 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Box_4_1")], model.selection("EDGE", "PartSet/OZ"), -360)
Rotation_5 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Box_5_1")], model.selection("EDGE", "PartSet/OZ"), 450)
Rotation_6 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Box_6_1")], model.selection("EDGE", "PartSet/OZ"), -450)
Rotation_7 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Box_7_1")], model.selection("EDGE", "InvalidName"), 180)

Rotation_9 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Box_8_1")], model.selection("VERTEX", "PartSet/Origin"), model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_2"))
Rotation_10 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Box_9_1")], model.selection("VERTEX", "Point_3"), model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_2"))
Rotation_11 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Box_10_1")], model.selection("VERTEX", "Point_3"), model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_1"))
Rotation_12 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Box_11_1")], model.selection("VERTEX", "Point_3"), model.selection("VERTEX", "Point_3"), model.selection("VERTEX", "Point_1"))
Rotation_13 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Box_12_1")], model.selection("VERTEX", "Point_3"), model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_3"))
Rotation_14 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Box_13_1")], model.selection("VERTEX", "PartSet/Origin"), model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "InvalidName"))
Rotation_15 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Box_14_1")], model.selection("VERTEX", "InvalidName"), model.selection("VERTEX", "Point_3"), model.selection("VERTEX", "Point_2"))
Rotation_16 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Box_15_1")], model.selection("VERTEX", "PartSet/Origin"), model.selection("VERTEX", "InvalidName"), model.selection("VERTEX", "Point_1"))

Rotation_18 = model.addRotation(partSet, [model.selection("COMPOUND", "Part_2/")], model.selection("EDGE", "OY"), 90)
Rotation_19 = model.addRotation(partSet, [model.selection("COMPOUND", "Part_3/")], model.selection("VERTEX", "Origin"), model.selection("VERTEX", "Point_2"), model.selection("VERTEX", "Point_3"))

model.do()
model.end()

# Checks
from GeomAPI import GeomAPI_Shape

model.testNbResults(Rotation_1, 1)
model.testNbSubResults(Rotation_1, [0])
model.testNbSubShapes(Rotation_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Rotation_1, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Rotation_1, model, Part_1_doc)

model.testNbResults(Rotation_2, 1)
model.testNbSubResults(Rotation_2, [0])
model.testNbSubShapes(Rotation_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Rotation_2, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Rotation_2, model, Part_1_doc)

model.testNbResults(Rotation_3, 1)
model.testNbSubResults(Rotation_3, [0])
model.testNbSubShapes(Rotation_3, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Rotation_3, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Rotation_3, model, Part_1_doc)

model.testNbResults(Rotation_4, 1)
model.testNbSubResults(Rotation_4, [0])
model.testNbSubShapes(Rotation_4, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Rotation_4, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Rotation_4, model, Part_1_doc)

model.testNbResults(Rotation_5, 1)
model.testNbSubResults(Rotation_5, [0])
model.testNbSubShapes(Rotation_5, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Rotation_5, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Rotation_5, model, Part_1_doc)

model.testNbResults(Rotation_6, 1)
model.testNbSubResults(Rotation_6, [0])
model.testNbSubShapes(Rotation_6, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Rotation_6, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Rotation_6, model, Part_1_doc)

model.testNbResults(Rotation_9, 1)
model.testNbSubResults(Rotation_9, [0])
model.testNbSubShapes(Rotation_9, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Rotation_9, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Rotation_9, model, Part_1_doc)

model.testNbResults(Rotation_10, 1)
model.testNbSubResults(Rotation_10, [0])
model.testNbSubShapes(Rotation_10, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Rotation_10, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Rotation_10, model, Part_1_doc)

model.testNbResults(Rotation_18, 1)
model.testNbSubResults(Rotation_18, [0])

model.testNbResults(Rotation_19, 1)
model.testNbSubResults(Rotation_19, [0])


# This test is OK because we accept angles greater than 360
#model.testNbResults(Rotation_5, 0)
#assert(Rotation_5.feature().error() == 'Rotation builder :: angle greater than 360 degrees.')

# This test is OK because we accept angles smaller than 360
#model.testNbResults(Rotation_6, 0)
#assert(Rotation_6.feature().error() == 'Rotation builder :: angle smaller than -360 degrees.')

model.testNbResults(Rotation_7, 0)
assert(Rotation_7.feature().error() == 'Attribute "axis_object" is not initialized.')

model.testNbResults(Rotation_11, 0)
assert(Rotation_11.feature().error() == "The feature uses equal shapes.")

model.testNbResults(Rotation_12, 0)
assert(Rotation_12.feature().error() == "The feature uses equal shapes.")

model.testNbResults(Rotation_13, 0)
assert(Rotation_13.feature().error() == "The feature uses equal shapes.")

model.testNbResults(Rotation_14, 0)
assert(Rotation_14.feature().error() == 'Attribute "end_point" is not initialized.')

model.testNbResults(Rotation_15, 0)
assert(Rotation_15.feature().error() == 'Attribute "center_point" is not initialized.')

model.testNbResults(Rotation_16, 0)
assert(Rotation_16.feature().error() == 'Attribute "start_point" is not initialized.')




#Rotation_8 = model.addRotation(Part_1_doc, [model.selection("SOLID", "InvalidName")], model.selection("EDGE", "PartSet/OZ"), 180)
#Rotation_17 = model.addRotation(Part_1_doc, [model.selection("SOLID", "InvalidName")], model.selection("VERTEX", "PartSet/Origin"), model.selection("VERTEX", "Point_3"), model.selection("VERTEX", "Point_2"))

#model.testNbResults(Rotation_8, 0)
#assert(Rotation_8.feature().error() == 'Attribute "main_object" is not initialized.')

#model.testNbResults(Rotation_17, 0)
#assert(Rotation_17.feature().error() == 'Attribute "main_object" is not initialized.')
