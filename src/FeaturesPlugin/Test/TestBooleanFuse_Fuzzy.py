# Copyright (C) 2014-2024  CEA, EDF
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
Param_fuzzy = model.addParameter(Part_1_doc, "fuzzy", '1e-07')

Point_2 = model.addPoint(Part_1_doc, 10, 0, 0)
Point_3 = model.addPoint(Part_1_doc, 20, 10.0001, 10.0001)

### Create Box
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)

### Create Box
Box_2 = model.addBox(Part_1_doc, model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_2"))

### Create Fuse
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Box_2_1")], fuzzyParam = "fuzzy", keepSubResults = True)
model.do()

model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [0])
model.testNbUniqueSubShapes(Fuse_1, GeomAPI_Shape.SOLID, [1])
model.testNbUniqueSubShapes(Fuse_1, GeomAPI_Shape.FACE, [11])
model.testNbUniqueSubShapes(Fuse_1, GeomAPI_Shape.EDGE, [24])
model.testNbUniqueSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [15])
model.testResultsVolumes(Fuse_1, [2000.02000010])

### Set a higher fuzzy value
Param_fuzzy.setValue(1.e-4)
model.do()

model.end()

model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [0])
model.testNbUniqueSubShapes(Fuse_1, GeomAPI_Shape.SOLID, [1])
model.testNbUniqueSubShapes(Fuse_1, GeomAPI_Shape.FACE, [10])
model.testNbUniqueSubShapes(Fuse_1, GeomAPI_Shape.EDGE, [20])
model.testNbUniqueSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [12])
model.testResultsVolumes(Fuse_1, [2000.02166677])
