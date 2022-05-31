# Copyright (C) 2017-2022  CEA/DEN, EDF R&D
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
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Init
Point_1 = model.addPoint(Part_1_doc, 10, 10, 10).result()
Point_2 = model.selection("VERTEX", "pnt")
Axis_1 = model.addAxis(Part_1_doc, 0, 10, 0).result()
Axis_2 = model.selection("EDGE", "edge")

# Parameters
model.addParameter(Part_1_doc, "baseRadius", "10")
model.addParameter(Part_1_doc, "topRadius", "7")
model.addParameter(Part_1_doc, "h", "15")

# Tests
Cone_1 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10., 5., 10.)
Cone_2 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5., 10., 10.)
Cone_3 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 0., 5., 10.)
Cone_4 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), -3., 5., 10.)
Cone_5 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10., 0., 10.)
Cone_6 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10., -5., 10.)
Cone_7 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10., 5., 0.)
Cone_8 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10., 5., -10.)
Cone_9 = model.addCone(Part_1_doc, Point_1, model.selection("EDGE", "PartSet/OZ"), 10., 7., 15.)
Cone_10 = model.addCone(Part_1_doc, Point_1, Axis_1, 10., 7., 15.)
Cone_11 = model.addCone(Part_1_doc, Point_2, model.selection("EDGE", "PartSet/OZ"), 10., 7., 15.)
Cone_12= model.addCone(Part_1_doc, Point_1, Axis_2, 10., 7., 15.)
Cone_13 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 0., 0., 15.)
Cone_14 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10., 10., 15.)
Cone_15 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), "baseRadius", "topRadius", "h")

Cone_16 = model.addCone(Part_1_doc, 5., 10., 15.)
Cone_16.setRadius(10., 15.)

model.do()
model.end()

# Checks
from GeomAPI import GeomAPI_Shape

model.testNbResults(Cone_1, 1)
model.testNbSubResults(Cone_1, [0])
model.testNbSubShapes(Cone_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cone_1, GeomAPI_Shape.FACE, [3])
model.testHaveNamingFaces(Cone_1, model, Part_1_doc)

model.testNbResults(Cone_2, 1)
model.testNbSubResults(Cone_2, [0])
model.testNbSubShapes(Cone_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cone_2, GeomAPI_Shape.FACE, [3])
model.testHaveNamingFaces(Cone_2, model, Part_1_doc)

model.testNbResults(Cone_3, 1)
model.testNbSubResults(Cone_3, [0])
model.testNbSubShapes(Cone_3, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cone_3, GeomAPI_Shape.FACE, [2])
model.testHaveNamingFaces(Cone_3, model, Part_1_doc)

model.testNbResults(Cone_5, 1)
model.testNbSubResults(Cone_5, [0])
model.testNbSubShapes(Cone_5, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cone_5, GeomAPI_Shape.FACE, [2])
model.testHaveNamingFaces(Cone_5, model, Part_1_doc)

model.testNbResults(Cone_9, 1)
model.testNbSubResults(Cone_9, [0])
model.testNbSubShapes(Cone_9, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cone_9, GeomAPI_Shape.FACE, [3])
model.testHaveNamingFaces(Cone_9, model, Part_1_doc)

model.testNbResults(Cone_10, 1)
model.testNbSubResults(Cone_10, [0])
model.testNbSubShapes(Cone_10, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cone_10, GeomAPI_Shape.FACE, [3])
model.testHaveNamingFaces(Cone_10, model, Part_1_doc)

model.testNbResults(Cone_15, 1)
model.testNbSubResults(Cone_15, [0])
model.testNbSubShapes(Cone_15, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cone_15, GeomAPI_Shape.FACE, [3])
model.testHaveNamingFaces(Cone_15, model, Part_1_doc)

model.testNbResults(Cone_16, 1)
model.testNbSubResults(Cone_16, [0])
model.testNbSubShapes(Cone_16, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Cone_16, GeomAPI_Shape.FACE, [3])
model.testHaveNamingFaces(Cone_16, model, Part_1_doc)

model.testNbResults(Cone_4, 0)
assert(Cone_4.feature().error() == "Cone builder :: base radius is negative.")

model.testNbResults(Cone_6, 0)
assert(Cone_6.feature().error() == "Cone builder :: top radius is negative.")

model.testNbResults(Cone_7, 0)
assert(Cone_7.feature().error() == "Cone builder :: height is negative or null.")

model.testNbResults(Cone_8, 0)
assert(Cone_8.feature().error() == "Cone builder :: height is negative or null.")

model.testNbResults(Cone_11, 0)
assert(Cone_11.feature().error() == "Attribute \"base_point\" is not initialized.")

model.testNbResults(Cone_12, 0)
assert(Cone_12.feature().error() == "Attribute \"axis\" is not initialized.")

model.testNbResults(Cone_13, 0)
assert(Cone_13.feature().error() == "Cone builder :: base radius and top radius are negative or null.")

model.testNbResults(Cone_14, 0)
assert(Cone_14.feature().error() == "Cone builder :: base radius and top radius are too close.")
