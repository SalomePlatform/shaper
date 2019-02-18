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
Test case for MultiRotation feature.
Written on High API.
"""

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

#Init
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Box_3 = model.addBox(Part_1_doc, 10, 10, 10)
Box_4 = model.addBox(Part_1_doc, 10, 10, 10)
Box_5 = model.addBox(Part_1_doc, 10, 10, 10)
Box_6 = model.addBox(Part_1_doc, 10, 10, 10)
Axis_1 = model.selection("EDGE", "edge")

# Tests
MultiRotation_1 = model.addMultiRotation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "PartSet/OZ"), 5)
MultiRotation_2 = model.addMultiRotation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], model.selection("EDGE", "PartSet/OZ"), 0)
MultiRotation_3 = model.addMultiRotation(Part_1_doc, [model.selection("SOLID", "Box_3_1")], model.selection("EDGE", "PartSet/OZ"), -3)
MultiRotation_4 = model.addMultiRotation(Part_1_doc, [model.selection("SOLID", "Box_4_1")], model.selection("EDGE", "PartSet/OY"), 90, 4)
MultiRotation_5 = model.addMultiRotation(Part_1_doc, [model.selection("SOLID", "Box_5_1")], model.selection("EDGE", "PartSet/OY"), -75, 4)
MultiRotation_6 = model.addMultiRotation(Part_1_doc, [model.selection("SOLID", "Box_6_1")], Axis_1, 4)

model.do()
model.end()

# Check
from GeomAPI import GeomAPI_Shape

model.testNbResults(MultiRotation_1, 1)
model.testNbSubResults(MultiRotation_1, [5])
model.testNbSubShapes(MultiRotation_1, GeomAPI_Shape.COMPOUND, [1])
model.testNbSubShapes(MultiRotation_1, GeomAPI_Shape.SOLID, [5])
model.testNbSubShapes(MultiRotation_1, GeomAPI_Shape.FACE, [30])
model.testHaveNamingFaces(MultiRotation_1, model, Part_1_doc)

model.testNbResults(MultiRotation_4, 1)
model.testNbSubResults(MultiRotation_4, [4])
model.testNbSubShapes(MultiRotation_4, GeomAPI_Shape.COMPOUND, [1])
model.testNbSubShapes(MultiRotation_4, GeomAPI_Shape.SOLID, [4])
model.testNbSubShapes(MultiRotation_4, GeomAPI_Shape.FACE, [24])
model.testHaveNamingFaces(MultiRotation_4, model, Part_1_doc)

model.testNbResults(MultiRotation_5, 1)
model.testNbSubResults(MultiRotation_5, [4])
model.testNbSubShapes(MultiRotation_5, GeomAPI_Shape.COMPOUND, [1])
model.testNbSubShapes(MultiRotation_5, GeomAPI_Shape.SOLID, [4])
model.testNbSubShapes(MultiRotation_5, GeomAPI_Shape.FACE, [24])
model.testHaveNamingFaces(MultiRotation_5, model, Part_1_doc)

model.testNbResults(MultiRotation_2, 0)
assert(MultiRotation_2.feature().error() == "Multirotation builder :: the number of copies for the angular direction is null or negative.")

model.testNbResults(MultiRotation_3, 0)
assert(MultiRotation_3.feature().error() == "Multirotation builder :: the number of copies for the angular direction is null or negative.")

model.testNbResults(MultiRotation_6, 0)
assert(MultiRotation_6.feature().error() == "Attribute \"axis_angular\" is not initialized.")
