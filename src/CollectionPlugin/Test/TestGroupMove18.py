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

# Test of deep nested results history. Shell operation.
# Check the initial groups moved to the end contain the corresponding results.

from salome.shaper import model
from ModelAPI import *

aFactory = ModelAPI_Session.get().validators()

def moveGroupsAndVerify(thePart, theLastFeature, theRefNumInGroups):
    # move groups to the end
    model.begin()
    LastFeature = theLastFeature
    for i in range(thePart.size("Groups")):
        GroupFeature = thePart.feature(objectToResult(thePart.object("Groups", 0))) # move always the very first group
        Part_1_doc.moveFeature(GroupFeature, LastFeature)
        LastFeature = GroupFeature
    model.end()

    a = 0
    for i in range(thePart.size("Groups")):
        GroupFeature = thePart.feature(objectToResult(thePart.object("Groups", i)))
        assert(aFactory.validate(GroupFeature))
        assert(GroupFeature.selectionList("group_list").size() == theRefNumInGroups[a])
        a = a + 1


model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Group_1_objects = [model.selection("FACE", "Box_1_1/Back"), model.selection("FACE", "Box_1_1/Top"), model.selection("FACE", "Box_1_1/Right"), model.selection("FACE", "Box_1_1/Left"), model.selection("FACE", "Box_1_1/Bottom"), model.selection("FACE", "Box_1_1/Front")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
Group_2_objects = [model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Right]"), model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Left]"), model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]")]
Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
Group_3_objects = [model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Top]"), model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Top]"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Top]"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Bottom]"), model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Right][Box_1_1/Top]"), model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Bottom]"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Bottom]"), model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Right][Box_1_1/Bottom]")]
Group_3 = model.addGroup(Part_1_doc, Group_3_objects)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Left"), model.selection("FACE", "Box_1_1/Right"))
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Front"), model.selection("FACE", "Box_1_1/Back"))
Partition_1_objects = [model.selection("SOLID", "Box_1_1"), model.selection("FACE", "Plane_1"), model.selection("FACE", "Plane_2")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
Shell_1_objects = [model.selection("FACE", "Partition_1_1_1/Modified_Face&Box_1_1/Back"), model.selection("FACE", "Partition_1_1_1/Modified_Face&Box_1_1/Left"), model.selection("FACE", "Partition_1_1_1/Modified_Face&Box_1_1/Top"), model.selection("FACE", "Partition_1_1_2/Modified_Face&Box_1_1/Back"), model.selection("FACE", "Partition_1_1_4/Modified_Face&Box_1_1/Front"), model.selection("FACE", "Partition_1_1_4/Modified_Face&Box_1_1/Right")]
Shell_1 = model.addShell(Part_1_doc, Shell_1_objects)
model.do()
model.end()

num_in_groups = [6, 13, 6]
moveGroupsAndVerify(Part_1_doc, Shell_1.feature(), num_in_groups)

assert(model.checkPythonDump())
