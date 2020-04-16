# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Group_1 = model.addGroup(Part_1_doc, [model.selection("FACE", "Box_1_1/Top"), model.selection("FACE", "Box_1_1/Left")])
Group_2 = model.addGroup(Part_1_doc, [model.selection("FACE", "Box_1_1/Front"), model.selection("FACE", "Box_1_1/Top")])
GroupAddition_1 = model.addGroupAddition(Part_1_doc, [model.selection("COMPOUND", "Group_1"), model.selection("COMPOUND", "Group_2")])
model.end()

from GeomAPI import *

model.testNbResults(GroupAddition_1, 1)
model.testNbSubResults(GroupAddition_1, [0])
model.testNbSubShapes(GroupAddition_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(GroupAddition_1, GeomAPI_Shape.FACE, [3])
model.testNbSubShapes(GroupAddition_1, GeomAPI_Shape.EDGE, [12])
model.testNbSubShapes(GroupAddition_1, GeomAPI_Shape.VERTEX, [24])
model.testResultsVolumes(GroupAddition_1, [300])

assert(model.checkPythonDump())
