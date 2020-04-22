# Copyright (C) 2020  CEA/DEN, EDF R&D
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
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Box
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)

### Create Wire
Wire_1_objects = [model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]"), model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Right]"), model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Left]")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects, False)

### Create Fillet1D
Fillet1D_1 = model.addFillet(Part_1_doc, [model.selection("VERTEX", "[Wire_1_1/Edge_4]e[Wire_1_1/Edge_6]e"), model.selection("VERTEX", "[Wire_1_1/Edge_2]e[Wire_1_1/Edge_4]e"), model.selection("VERTEX", "[Wire_1_1/Edge_1]e[Wire_1_1/Edge_3]e")], 2)

model.testHaveNamingByType(Fillet1D_1, model, Part_1_doc, GeomAPI_Shape.VERTEX)
model.testHaveNamingByType(Fillet1D_1, model, Part_1_doc, GeomAPI_Shape.EDGE)
model.end()

model.testNbResults(Fillet1D_1, 1)
model.testNbSubResults(Fillet1D_1, [0])
model.testNbSubShapes(Fillet1D_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Fillet1D_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Fillet1D_1, GeomAPI_Shape.EDGE, [11])
model.testNbSubShapes(Fillet1D_1, GeomAPI_Shape.VERTEX, [22])
model.testResultsVolumes(Fillet1D_1, [0])

assert(model.checkPythonDump(model.ModelHighAPI.CHECK_NAMING))
