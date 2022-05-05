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

# Test that box partitioned is splitted: group with edge becomes 2 edges group,
# group with not-touched vertex keeps this vertex.

from salome.shaper import model
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 20, 10, 10)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Back"), 10, True)
Group_1 = model.addGroup(Part_1_doc, [model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]")])
Group_2 = model.addGroup(Part_1_doc, [model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Top]")])
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("FACE", "Plane_1")])
model.do()
# move groups
Part_1_doc.moveFeature(Group_1.feature(), Partition_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
model.end()

# Check groups
aFactory = ModelAPI_Session.get().validators()
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 2)
assert(aFactory.validate(Group_1.feature()))

selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 1)
assert(aFactory.validate(Group_2.feature()))
