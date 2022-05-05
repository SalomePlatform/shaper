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

# Parameters
model.addParameter(Part_1_doc, "rmin", "10")
model.addParameter(Part_1_doc, "rmax", "12")
model.addParameter(Part_1_doc, "h", "20.5")

# Tests
Tube_1 = model.addTube(Part_1_doc, 3, 5, 12)
Tube_2 = model.addTube(Part_1_doc, "rmin", "rmax", "h")
Tube_3 = model.addTube(Part_1_doc, 5, 3, 12)
Tube_4 = model.addTube(Part_1_doc, -2.5, 5, 12)
Tube_5 = model.addTube(Part_1_doc, 3, 5, -10)

model.do()
model.end()

# Checks
from GeomAPI import GeomAPI_Shape

model.testNbResults(Tube_1, 1)
model.testNbSubResults(Tube_1, [0])
model.testNbSubShapes(Tube_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Tube_1, GeomAPI_Shape.FACE, [4])
model.testHaveNamingFaces(Tube_1, model, Part_1_doc)

model.testNbResults(Tube_2, 1)
model.testNbSubResults(Tube_2, [0])
model.testNbSubShapes(Tube_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Tube_2, GeomAPI_Shape.FACE, [4])
model.testHaveNamingFaces(Tube_2, model, Part_1_doc)

model.testNbResults(Tube_3, 0)
print(Tube_3.feature().error())
assert(Tube_3.feature().error() == "Tube builder :: rmin is greater then or equal to rmax.")

model.testNbResults(Tube_4, 0)
print(Tube_4.feature().error())
assert(Tube_4.feature().error() == "Tube builder :: rmin is negative.")

model.testNbResults(Tube_5, 0)
print(Tube_5.feature().error())
assert(Tube_5.feature().error() == "Tube builder :: z is negative or null.")
