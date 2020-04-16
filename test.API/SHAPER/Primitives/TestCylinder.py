# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
Test case for Primitive Cylinder feature.
Written on High API.
"""
#from ModelAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Init
Point_1 = model.addPoint(Part_1_doc, 10, 10, 10).result()
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Point_1")])

# Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(40.1917811348149, 87.63526969661267, 92.63493386188256, 31.1341153558487)
SketchLine_2 = Sketch_1.addLine(92.63493386188256, 31.1341153558487, 44.70109718071649, 32.40093347950982)
SketchLine_3 = Sketch_1.addLine(44.70109718071649, 32.40093347950982, 40.1917811348149, 87.63526969661267)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
model.do()

# Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection(), 100, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchLine_3r-SketchLine_2r-SketchLine_1r_wire")], model.selection(), 100, 0)

# Edge
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_2")])
Edge_2 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_3")])

# Axis
Axis_1 = model.addAxis(Part_1_doc, 0, 10, 0)

# Parameters
model.addParameter(Part_1_doc, "r", "6")
model.addParameter(Part_1_doc, "h", "20")

model.addParameter(Part_1_doc, "r2", "6")
model.addParameter(Part_1_doc, "h2", "15")
model.addParameter(Part_1_doc, "angle", "270")

#Tests
Cylinder_1 = model.addCylinder(Part_1_doc, 5, 10)
Cylinder_2 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "Point_1"), model.selection("EDGE", "PartSet/OX"), 5, 10)
Cylinder_3 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "Point_1"), model.selection("EDGE", "Axis_1"), 7, 12)
Cylinder_4 = model.addCylinder(Part_1_doc, 0, 10)
Cylinder_5 = model.addCylinder(Part_1_doc, -5, 10)
Cylinder_6 = model.addCylinder(Part_1_doc, 5, 0)
Cylinder_7 = model.addCylinder(Part_1_doc, 5, -10)
Cylinder_8 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "InvalidName"), model.selection("EDGE", "PartSet/OX"), 5, 10)
Cylinder_9 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "InvalidName"), 5, 10)
Cylinder_10 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "Sketch_1/SketchLine_2_EndVertex"), model.selection("EDGE", "Sketch_1/SketchLine_3"), 50, 120)
Cylinder_11 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "Vertex_1_1"), model.selection("EDGE", "Edge_1_1"), 60, 100)
Cylinder_12 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/To_Face]"), model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/To_Face]"), 5, 10)
Cylinder_13 = model.addCylinder(Part_1_doc, "r", "h")
Cylinder_14 = model.addCylinder(Part_1_doc, 5, 10, 45)
Cylinder_15 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "Point_1"), model.selection("EDGE", "PartSet/OX"), 5, 10, 90)
Cylinder_16 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "Point_1"), model.selection("EDGE", "Axis_1"), 7, 12, 120)
Cylinder_17 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "Point_1"), model.selection("EDGE", "PartSet/OY"), 5, 10, 360)
Cylinder_18 = model.addCylinder(Part_1_doc, 0, 10, 45)
Cylinder_19 = model.addCylinder(Part_1_doc, -5, 10, 45)
Cylinder_20 = model.addCylinder(Part_1_doc, 5, 0, 45)
Cylinder_21 = model.addCylinder(Part_1_doc, 5, -10, 45)
Cylinder_22 = model.addCylinder(Part_1_doc, 5, 10, 0)
Cylinder_23 = model.addCylinder(Part_1_doc, 5, 10, -45)
Cylinder_24 = model.addCylinder(Part_1_doc, 5, 10, 450)
Cylinder_25 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "InvalidName"), model.selection("EDGE", "PartSet/OX"), 5, 10, 90)
Cylinder_26 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "InvalidName"), 5, 10, 90)
Cylinder_27 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "Sketch_1/SketchLine_1_EndVertex"), model.selection("EDGE", "Sketch_1/SketchLine_3"), 50, 120, 90)
Cylinder_28 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "Vertex_1_1"), model.selection("EDGE", "Edge_2_1"), 60, 100, 180)
Cylinder_29 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "[Extrusion_2_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_2_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_2_1/To_Face]"), model.selection("EDGE", "[Extrusion_2_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_2_1/To_Face]"), 5, 10, 120)
Cylinder_30 = model.addCylinder(Part_1_doc, "r2", "h2", "angle")

model.do()
model.end()

# Checks
from GeomAPI import GeomAPI_Shape

model.testNbResults(Cylinder_1, 1)
model.testNbSubResults(Cylinder_1, [0])
model.testNbSubShapes(Cylinder_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cylinder_1, GeomAPI_Shape.FACE, [3])
model.testHaveNamingFaces(Cylinder_1, model, Part_1_doc)

model.testNbResults(Cylinder_2, 1)
model.testNbSubResults(Cylinder_2, [0])
model.testNbSubShapes(Cylinder_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cylinder_2, GeomAPI_Shape.FACE, [3])
model.testHaveNamingFaces(Cylinder_2, model, Part_1_doc)

model.testNbResults(Cylinder_3, 1)
model.testNbSubResults(Cylinder_3, [0])
model.testNbSubShapes(Cylinder_3, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cylinder_3, GeomAPI_Shape.FACE, [3])
model.testHaveNamingFaces(Cylinder_3, model, Part_1_doc)

model.testNbResults(Cylinder_10, 1)
model.testNbSubResults(Cylinder_10, [0])
model.testNbSubShapes(Cylinder_10, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cylinder_10, GeomAPI_Shape.FACE, [3])
model.testHaveNamingFaces(Cylinder_10, model, Part_1_doc)

model.testNbResults(Cylinder_11, 1)
model.testNbSubResults(Cylinder_11, [0])
model.testNbSubShapes(Cylinder_11, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cylinder_11, GeomAPI_Shape.FACE, [3])
model.testHaveNamingFaces(Cylinder_11, model, Part_1_doc)

model.testNbResults(Cylinder_12, 1)
model.testNbSubResults(Cylinder_12, [0])
model.testNbSubShapes(Cylinder_12, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cylinder_12, GeomAPI_Shape.FACE, [3])
model.testHaveNamingFaces(Cylinder_12, model, Part_1_doc)

model.testNbResults(Cylinder_13, 1)
model.testNbSubResults(Cylinder_13, [0])
model.testNbSubShapes(Cylinder_13, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cylinder_13, GeomAPI_Shape.FACE, [3])
model.testHaveNamingFaces(Cylinder_13, model, Part_1_doc)

model.testNbResults(Cylinder_14, 1)
model.testNbSubResults(Cylinder_14, [0])
model.testNbSubShapes(Cylinder_14, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cylinder_14, GeomAPI_Shape.FACE, [5])
model.testHaveNamingFaces(Cylinder_14, model, Part_1_doc)

model.testNbResults(Cylinder_15, 1)
model.testNbSubResults(Cylinder_15, [0])
model.testNbSubShapes(Cylinder_15, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cylinder_15, GeomAPI_Shape.FACE, [5])
model.testHaveNamingFaces(Cylinder_15, model, Part_1_doc)

model.testNbResults(Cylinder_16, 1)
model.testNbSubResults(Cylinder_16, [0])
model.testNbSubShapes(Cylinder_16, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cylinder_16, GeomAPI_Shape.FACE, [5])
model.testHaveNamingFaces(Cylinder_16, model, Part_1_doc)

model.testNbResults(Cylinder_17, 1)
model.testNbSubResults(Cylinder_17, [0])
model.testNbSubShapes(Cylinder_17, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cylinder_17, GeomAPI_Shape.FACE, [3])
model.testHaveNamingFaces(Cylinder_17, model, Part_1_doc)

model.testNbResults(Cylinder_27, 1)
model.testNbSubResults(Cylinder_27, [0])
model.testNbSubShapes(Cylinder_27, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cylinder_27, GeomAPI_Shape.FACE, [5])
model.testHaveNamingFaces(Cylinder_27, model, Part_1_doc)

model.testNbResults(Cylinder_28, 1)
model.testNbSubResults(Cylinder_28, [0])
model.testNbSubShapes(Cylinder_28, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cylinder_28, GeomAPI_Shape.FACE, [5])
model.testHaveNamingFaces(Cylinder_28, model, Part_1_doc)

model.testNbResults(Cylinder_29, 1)
model.testNbSubResults(Cylinder_29, [0])
model.testNbSubShapes(Cylinder_29, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cylinder_29, GeomAPI_Shape.FACE, [5])
model.testHaveNamingFaces(Cylinder_29, model, Part_1_doc)

model.testNbResults(Cylinder_30, 1)
model.testNbSubResults(Cylinder_30, [0])
model.testNbSubShapes(Cylinder_30, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cylinder_30, GeomAPI_Shape.FACE, [5])
model.testHaveNamingFaces(Cylinder_30, model, Part_1_doc)

model.testNbResults(Cylinder_4, 0)
assert(Cylinder_4.feature().error() == "Cylinder builder :: radius is negative or null.")

model.testNbResults(Cylinder_5, 0)
assert(Cylinder_5.feature().error() == "Cylinder builder :: radius is negative or null.")

model.testNbResults(Cylinder_6, 0)
assert(Cylinder_6.feature().error() == "Cylinder builder :: height is negative or null.")

model.testNbResults(Cylinder_7, 0)
assert(Cylinder_7.feature().error() == "Cylinder builder :: height is negative or null.")

# To uncomment when #2044 will be performed.
#model.testNbResults(Cylinder_8, 0)
#assert(Cylinder_8.feature().error() == "Attribute \"base_point\" is not initialized")

# To uncomment when #2044 will be performed.
#model.testNbResults(Cylinder_9, 0)
#assert(Cylinder_9.feature().error() == "Attribute \"axis\" is not initialized")

model.testNbResults(Cylinder_18, 0)
assert(Cylinder_18.feature().error() == "Cylinder builder :: radius is negative or null.")

model.testNbResults(Cylinder_19, 0)
assert(Cylinder_19.feature().error() == "Cylinder builder :: radius is negative or null.")

model.testNbResults(Cylinder_20, 0)
assert(Cylinder_20.feature().error() == "Cylinder builder :: height is negative or null.")

model.testNbResults(Cylinder_21, 0)
assert(Cylinder_21.feature().error() == "Cylinder builder :: height is negative or null.")

model.testNbResults(Cylinder_22, 0)
assert(Cylinder_22.feature().error() == "Cylinder builder :: angle is negative or null.")

model.testNbResults(Cylinder_23, 0)
assert(Cylinder_23.feature().error() == "Cylinder builder :: angle is negative or null.")

model.testNbResults(Cylinder_24, 0)
assert(Cylinder_24.feature().error() == "Cylinder builder :: angle greater than 360 degrees.")

# To uncomment when #2044 will be performed.
#model.testNbResults(Cylinder_25, 0)
#assert(Cylinder_25.feature().error() == "Attribute \"base_point\" is not initialized")

# To uncomment when #2044 will be performed.
#model.testNbResults(Cylinder_26, 0)
#assert(Cylinder_26.feature().error() == "Attribute \"axis\" is not initialized")
