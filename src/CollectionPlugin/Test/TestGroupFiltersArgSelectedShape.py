# Copyright (C) 2014-2025  CEA, EDF
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
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Box
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)

### Create Plane
Plane_4 = model.addPlane(
    Part_1_doc,
    model.selection("FACE", "Box_1_1/Left"),
    model.selection("FACE", "Box_1_1/Right"),
)

### Create Partition
Partition_1 = model.addPartition(
    Part_1_doc,
    [
        model.selection("COMPOUND", "all-in-Box_1"),
        model.selection("FACE", "all-in-Plane_1"),
    ],
    keepSubResults=True,
)

### Create Group
Group_1_objects = [
    model.selection("FACE", "Box_1_1/Right"),
    model.selection("FACE", "Partition_1_1_2/Modified_Face&Box_1_1/Front"),
    model.filters(
        Part_1_doc,
        [
            model.addFilter(
                name="TopoConnectedFaces",
                args=[
                    model.selection(
                        "VERTEX",
                        "[Partition_1_1_2/Modified_Face&Box_1_1/Top][Box_1_1/Right][Partition_1_1_2/Modified_Face&Box_1_1/Front]",
                    ),
                    False,
                ],
            ),
            model.addFilter(name="VerticalFaces"),
        ],
    ),
]
Group_1 = model.addGroup(Part_1_doc, "Faces", Group_1_objects)

model.end()

aFactory = ModelAPI_Session.get().validators()

GroupFeature = Group_1.feature()
assert aFactory.validate(GroupFeature)
assert GroupFeature.selectionList("group_list").size() == 2
assert GroupFeature.selectionList("group_list").filters().filters().size() == 2
assert GroupFeature.selectionList("group_list").filters().filters()[0] == 'TopoConnectedFaces'
assert GroupFeature.selectionList("group_list").filters().filters()[1] == 'VerticalFaces'
aNamedShape = GroupFeature.selectionList("group_list").filters().data().selection("TopoConnectedFaces__Shape").namingName()
assert len(aNamedShape) > 0
assert model.checkPythonDump()