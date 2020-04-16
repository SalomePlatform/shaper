# Copyright (C) 2017-2020  CEA/DEN, EDF R&D
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
model.addParameter(Part_1_doc, "r", "10")
model.addParameter(Part_1_doc, "ring", "3")

# Tests
Torus_1 = model.addTorus(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 15, 3)
Torus_2 = model.addTorus(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Torus_3 = model.addTorus(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 0, 3)
Torus_4 = model.addTorus(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), -10, 3)
Torus_5 = model.addTorus(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 15, 0)
Torus_6 = model.addTorus(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 15, -3)
Torus_7 = model.addTorus(Part_1_doc, Point_1, model.selection("EDGE", "PartSet/OX"), 12, 7)
Torus_8 = model.addTorus(Part_1_doc, Point_1, Axis_1, 15, 4)
Torus_9 = model.addTorus(Part_1_doc, Point_2, model.selection("EDGE", "PartSet/OZ"), 15, 4)
Torus_10 = model.addTorus(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), Axis_2, 15, 4)
Torus_11 = model.addTorus(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), "r", "ring")
Torus_12 = model.addTorus(Part_1_doc, 15, 4)

model.do()
model.end()

# Checks
from GeomAPI import GeomAPI_Shape

model.testNbResults(Torus_1, 1)
model.testNbSubResults(Torus_1, [0])
model.testNbSubShapes(Torus_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Torus_1, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Torus_1, model, Part_1_doc)

model.testNbResults(Torus_7, 1)
model.testNbSubResults(Torus_7, [0])
model.testNbSubShapes(Torus_7, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Torus_7, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Torus_7, model, Part_1_doc)

model.testNbResults(Torus_8, 1)
model.testNbSubResults(Torus_8, [0])
model.testNbSubShapes(Torus_8, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Torus_8, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Torus_8, model, Part_1_doc)

model.testNbResults(Torus_11, 1)
model.testNbSubResults(Torus_11, [0])
model.testNbSubShapes(Torus_11, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Torus_11, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Torus_11, model, Part_1_doc)

model.testNbResults(Torus_12, 1)
model.testNbSubResults(Torus_12, [0])
model.testNbSubShapes(Torus_12, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Torus_12, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Torus_12, model, Part_1_doc)

model.testNbResults(Torus_2, 0)
assert(Torus_2.feature().error() == "Torus builder :: ring radius is greater than the radius.")

model.testNbResults(Torus_3, 0)
assert(Torus_3.feature().error() == "Torus builder :: radius is negative or null.")

model.testNbResults(Torus_4, 0)
assert(Torus_4.feature().error() == "Torus builder :: radius is negative or null.")

model.testNbResults(Torus_5, 0)
assert(Torus_5.feature().error() == "Torus builder :: ring radius is negative or null.")

model.testNbResults(Torus_6, 0)
assert(Torus_6.feature().error() == "Torus builder :: ring radius is negative or null.")

model.testNbResults(Torus_9, 0)
assert(Torus_9.feature().error() == "Attribute \"base_point\" is not initialized.")

model.testNbResults(Torus_10, 0)
assert(Torus_10.feature().error() == "Attribute \"axis\" is not initialized.")
