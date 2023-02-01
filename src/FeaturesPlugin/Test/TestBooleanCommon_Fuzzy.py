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
Param_fuzzy = model.addParameter(Part_1_doc, "fuzzy", '1e-06')

### Create Sphere
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), 5)

### Create Point
Point_2 = model.addPoint(Part_1_doc, 9.999995, 0, 0)

### Create Sphere
Sphere_2 = model.addSphere(Part_1_doc, model.selection("VERTEX", "Point_1"), 5)

### Create Common
Common_1 = model.addCommon(Part_1_doc, [model.selection("SOLID", "Sphere_2_1"), model.selection("SOLID", "Sphere_1_1")], fuzzyParam = "fuzzy", keepSubResults = True)
model.do()

model.testNbResults(Common_1, 1)
model.testNbSubResults(Common_1, [0])
model.testNbUniqueSubShapes(Common_1, GeomAPI_Shape.SOLID, [1])
model.testNbUniqueSubShapes(Common_1, GeomAPI_Shape.FACE, [3])
model.testNbUniqueSubShapes(Common_1, GeomAPI_Shape.EDGE, [3])
model.testNbUniqueSubShapes(Common_1, GeomAPI_Shape.VERTEX, [2])

### Set a higher fuzzy value
Param_fuzzy.setValue(1.e-5)
model.do()

model.end()

model.testNbResults(Common_1, 0)

model.end()
