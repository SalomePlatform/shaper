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

# Test of deep nested results history.
# Faces and edges of a box has been collected to groups. After that, the box has been copied.
# Check that groups moved to the end contain corresponding results.

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Group_1 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Box_1_1")])
Group_2_objects = [model.selection("FACE", "Box_1_1/Back"), model.selection("FACE", "Box_1_1/Top"), model.selection("FACE", "Box_1_1/Right"), model.selection("FACE", "Box_1_1/Left"), model.selection("FACE", "Box_1_1/Bottom"), model.selection("FACE", "Box_1_1/Front")]
Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
Group_3_objects = [model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Right]"), model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Left]"), model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]")]
Group_3 = model.addGroup(Part_1_doc, Group_3_objects)
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "PartSet/OX"), 15, 2)
model.do()
# move groups to the end
Part_1_doc.moveFeature(Group_1.feature(), LinearCopy_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
Part_1_doc.moveFeature(Group_3.feature(), Group_2.feature())
model.end()

from ModelAPI import *

aFactory = ModelAPI_Session.get().validators()
# check group 1: all solids in compound should be selected
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 2)
assert(aFactory.validate(Group_1.feature()))
# check group 2: number of faces is multiplied twice than original
selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 12)
assert(aFactory.validate(Group_2.feature()))
# check group 3: number of edges is multiplied twice than original
selectionList = Group_3.feature().selectionList("group_list")
assert(selectionList.size() == 24)
assert(aFactory.validate(Group_3.feature()))

model.begin()
LinearCopy_2 = model.addMultiTranslation(Part_1_doc, [model.selection("COMPOUND", "LinearCopy_1_1")], model.selection("EDGE", "PartSet/OY"), 15, 2, model.selection("EDGE", "PartSet/OZ"), 15, 2)
# move groups to the end
Part_1_doc.moveFeature(Group_1.feature(), LinearCopy_2.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
Part_1_doc.moveFeature(Group_3.feature(), Group_2.feature())
model.end()

# check group 1: all solids in compound should be selected
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 8)
assert(aFactory.validate(Group_1.feature()))
# check group 2: number of faces is multiplied twice than original
selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 48)
assert(aFactory.validate(Group_2.feature()))
# check group 3: number of edges is multiplied twice than original
selectionList = Group_3.feature().selectionList("group_list")
assert(selectionList.size() == 96)
assert(aFactory.validate(Group_3.feature()))

model.begin()
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("SOLID", "LinearCopy_2_1_1_1"), model.selection("SOLID", "LinearCopy_2_1_1_2")], model.selection("EDGE", "PartSet/OZ"), 2)
# move groups to the end
Part_1_doc.moveFeature(Group_1.feature(), AngularCopy_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
Part_1_doc.moveFeature(Group_3.feature(), Group_2.feature())
model.end()

# check group 1: all solids in compound should be selected
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 4)
assert(aFactory.validate(Group_1.feature()))
# check group 2: number of faces is multiplied twice than original
selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 24)
assert(aFactory.validate(Group_2.feature()))
# check group 3: number of edges is multiplied twice than original
selectionList = Group_3.feature().selectionList("group_list")
assert(selectionList.size() == 48)
assert(aFactory.validate(Group_3.feature()))

assert(model.checkPythonDump())
