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

# Test based on DBC mail 04.03.2019. If the selected solid was not modified by
# the following Fill operation, this selection disappears after movement to the end.

from GeomAPI import *
from ModelAPI import *
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Front"), 2, True)
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Front"), 7, True)
Fill_1 = model.addFill(Part_1_doc, [model.selection("SOLID", "Box_1_1")], [model.selection("FACE", "Plane_2")])
Group_1 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Fill_1_1_1")])
Fill_2 = model.addFill(Part_1_doc, [model.selection("COMPSOLID", "Fill_1_1")], [model.selection("FACE", "Plane_1")])
model.do()

# move group
Part_1_doc.moveFeature(Group_1.feature(), Fill_2.feature())
model.end()

# Check group: result must be four faces
aFactory = ModelAPI_Session.get().validators()
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 1)
assert(aFactory.validate(Group_1.feature()))
assert(Group_1.groupList().value(0).value().shapeType() == GeomAPI_Shape.SOLID)

assert(model.checkPythonDump())
