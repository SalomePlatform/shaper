# Copyright (C) 2020-2021  CEA/DEN, EDF R&D
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

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Box
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)

model.do()

### Create Part
Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()

### Create ImportResult
ImportResult_1 = model.addImportResult(Part_2_doc, [model.selection("SOLID", (5.000000000000001, 5.000000000000001, 5))])

model.end()

from GeomAPI import *

model.testNbResults(Part_1, 1)
model.testNbSubResults(Part_1, [0])
model.testNbSubShapes(Part_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Part_1, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Part_1, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Part_1, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(Part_1, [1000])

model.testNbResults(Part_2, 1)
model.testNbSubResults(Part_2, [0])
model.testNbSubShapes(Part_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Part_2, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Part_2, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Part_2, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(Part_2, [1000])

assert(model.checkPythonDump())
