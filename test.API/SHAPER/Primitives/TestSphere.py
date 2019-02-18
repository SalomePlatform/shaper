# Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

# Parameters
model.addParameter(Part_1_doc, "r", "5")

# Tests
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), 10)
Sphere_2 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), 0)
Sphere_3 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), -5)
Sphere_4 = model.addSphere(Part_1_doc, Point_1, 10)
Sphere_5 = model.addSphere(Part_1_doc, Point_2, 10)
Sphere_6 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), "r")

Sphere_7 = model.addSphere(Part_1_doc, 10)
Sphere_7.setCenterPoint(Point_1)

model.do()
model.end()

# Checks
from GeomAPI import GeomAPI_Shape

model.testNbResults(Sphere_1, 1)
model.testNbSubResults(Sphere_1, [0])
model.testNbSubShapes(Sphere_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Sphere_1, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Sphere_1, model, Part_1_doc)

model.testNbResults(Sphere_4, 1)
model.testNbSubResults(Sphere_4, [0])
model.testNbSubShapes(Sphere_4, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Sphere_4, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Sphere_4, model, Part_1_doc)

model.testNbResults(Sphere_6, 1)
model.testNbSubResults(Sphere_6, [0])
model.testNbSubShapes(Sphere_6, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Sphere_6, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Sphere_6, model, Part_1_doc)

model.testNbResults(Sphere_7, 1)
model.testNbSubResults(Sphere_7, [0])
model.testNbSubShapes(Sphere_7, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Sphere_7, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Sphere_7, model, Part_1_doc)

model.testNbResults(Sphere_2, 0)
assert(Sphere_2.feature().error() == "Sphere builder :: radius is negative or null.")

model.testNbResults(Sphere_3, 0)
assert(Sphere_3.feature().error() == "Sphere builder :: radius is negative or null.")

model.testNbResults(Sphere_5, 0)
assert(Sphere_5.feature().error() == "Attribute \"center_point\" is not initialized.")
