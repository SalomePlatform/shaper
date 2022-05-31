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

# Test that removed vertex, selected in the group makes empty group => invalid one

from salome.shaper import model
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 20)
Plane_4 = model.addPlane(Part_1_doc, model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Right][Box_1_1/Top]"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Top]"), model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Bottom]"))
Group_1 = model.addGroup(Part_1_doc, [model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Top]")])
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("FACE", "Plane_1")])
Remove_SubShapes_1 = model.addRemoveSubShapes(Part_1_doc, model.selection("COMPSOLID", "Partition_1_1"))
Remove_SubShapes_1.setSubShapesToKeep([model.selection("SOLID", "Partition_1_1_2")])
model.do()
# move group
Part_1_doc.moveFeature(Group_1.feature(), Remove_SubShapes_1.feature())
model.end()

# Check group
aFactory = ModelAPI_Session.get().validators()
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 0)
assert(aFactory.validate(Group_1.feature()) == False)
