# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
from GeomAPI import GeomAPI_Shape


model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Param_offset = model.addParameter(Part_1_doc, "offset", '5e-5')
Param_fuzzy = model.addParameter(Part_1_doc, "fuzzy", '1e-05')

### Create Point
Point_2 = model.addPoint(Part_1_doc, "offset", "5", "5")

### Create Box
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_1.result().setColor(255, 0, 0)

### Create Cylinder
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "Point_1"), model.selection("EDGE", "PartSet/OX"), 6, 13)
Cylinder_1.result().setColor(255, 255, 0)
Cylinder_1.result().setTransparency(0.6)

### Create Cut
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Box_1_1")], [model.selection("SOLID", "Cylinder_1_1")], fuzzyParam = "fuzzy", keepSubResults = True)
model.do()

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [0])
model.testNbUniqueSubShapes(Cut_1, GeomAPI_Shape.SOLID, [1])
model.testNbUniqueSubShapes(Cut_1, GeomAPI_Shape.FACE, [14])
model.testNbUniqueSubShapes(Cut_1, GeomAPI_Shape.EDGE, [36])
model.testNbUniqueSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [24])
model.testResultsVolumes(Cut_1, [49.093623770546])

### Set a higher fuzzy value
Param_fuzzy.setValue(5.e-5)
model.do()

model.end()

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [4])
model.testNbUniqueSubShapes(Cut_1, GeomAPI_Shape.SOLID, [4])
model.testNbUniqueSubShapes(Cut_1, GeomAPI_Shape.FACE, [20])
model.testNbUniqueSubShapes(Cut_1, GeomAPI_Shape.EDGE, [36])
model.testNbUniqueSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [24])
model.testResultsVolumes(Cut_1, [49.088834629314])
