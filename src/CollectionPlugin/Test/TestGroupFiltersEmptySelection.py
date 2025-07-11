# Copyright (C) 2025  CEA, EDF
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

### Create Group 1 (selection empty -- old behaviour)
Group_1 = model.addGroup(
    Part_1_doc,
    "Faces",
    [
        model.selection("FACE", "Box_1_1/Top"),
        model.filters(
            Part_1_doc,
            [
                model.addFilter(name="OnPlane", args=[model.selection()]),
                model.addFilter(name="VerticalFaces"),
            ],
        ),
    ],
)

### Create Group 2 (selection not empty)
Group_2 = model.addGroup(
    Part_1_doc,
    "Faces",
    [
        model.selection("FACE", "Box_1_1/Front"),
        model.filters(
            Part_1_doc,
            [
                model.addFilter(
                    name="OnPlane",
                    args=[
                        model.selection("FACE", "Box_1_1/Top"),
                        model.selection("FACE", "Box_1_1/Front"),
                    ],
                ),
                model.addFilter(name="VerticalFaces"),
            ],
        ),
    ],
)

model.end()

aFactory = ModelAPI_Session.get().validators()

### Check if no filters in group 1 since selection is empty in one of the filters (filters are removed)
GroupFeature1 = Group_1.feature()
assert aFactory.validate(GroupFeature1)
assert GroupFeature1.selectionList("group_list").size() == 1
assert not GroupFeature1.selectionList("group_list").filters()

### Check if filters are defined in group 2 since selection is not empty
GroupFeature2 = Group_2.feature()
assert aFactory.validate(GroupFeature2)
assert GroupFeature2.selectionList("group_list").size() == 1
assert GroupFeature2.selectionList("group_list").filters().filters().size() == 2
assert GroupFeature2.selectionList("group_list").filters().filters()[0] == 'OnPlane'
assert GroupFeature2.selectionList("group_list").filters().filters()[1] == 'VerticalFaces'