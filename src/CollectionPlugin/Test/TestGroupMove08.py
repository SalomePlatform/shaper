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

# Test of deep nested results history. Fillet operation.
# Check the groups of initial solids moved to the end contain the corresponding
# results, but divided.

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Group_1 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Box_1_1")])
Group_2_objects = [model.selection("FACE", "Box_1_1/Back"), model.selection("FACE", "Box_1_1/Top"), model.selection("FACE", "Box_1_1/Right"), model.selection("FACE", "Box_1_1/Left"), model.selection("FACE", "Box_1_1/Bottom"), model.selection("FACE", "Box_1_1/Front")]
Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
Group_3_objects = [model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Right]"), model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Left]"), model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]")]
Group_3 = model.addGroup(Part_1_doc, Group_3_objects)
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Top]"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Group_4 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1")])
Group_5_objects = [model.selection("FACE", "Cylinder_1_1/Face_1"), model.selection("FACE", "Cylinder_1_1/Face_2"), model.selection("FACE", "Cylinder_1_1/Face_3")]
Group_5 = model.addGroup(Part_1_doc, Group_5_objects)
Group_6_objects = [model.selection("EDGE", "[Cylinder_1_1/Face_1][Cylinder_1_1/Face_2]"), model.selection("EDGE", "[Cylinder_1_1/Face_1][Cylinder_1_1/Face_3]"), model.selection("EDGE", "([Cylinder_1_1/Face_1][Cylinder_1_1/Face_2])([Cylinder_1_1/Face_1][Cylinder_1_1/Face_3])")]
Group_6 = model.addGroup(Part_1_doc, Group_6_objects)
Fillet_1 = model.addFillet(Part_1_doc, [model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]")], 2)
model.do()
# move groups to the end
Part_1_doc.moveFeature(Group_1.feature(), Fillet_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
Part_1_doc.moveFeature(Group_3.feature(), Group_2.feature())
model.end()

from ModelAPI import *

aFactory = ModelAPI_Session.get().validators()

# groups related to original box should be split
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 1)
assert(aFactory.validate(Group_1.feature()))

selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 6)
assert(aFactory.validate(Group_2.feature()))

selectionList = Group_3.feature().selectionList("group_list")
assert(selectionList.size() == 11)
assert(aFactory.validate(Group_3.feature()))

model.begin()
Fillet_2 = model.addFillet(Part_1_doc, [model.selection("FACE", "Cylinder_1_1/Face_2")], 2)
model.do()
# move groups to the end
Part_1_doc.moveFeature(Group_4.feature(), Fillet_2.feature())
Part_1_doc.moveFeature(Group_5.feature(), Group_4.feature())
Part_1_doc.moveFeature(Group_6.feature(), Group_5.feature())
model.end()

# groups related to original cylinder should be split
selectionList = Group_4.feature().selectionList("group_list")
assert(selectionList.size() == 1)
assert(aFactory.validate(Group_4.feature()))

selectionList = Group_5.feature().selectionList("group_list")
assert(selectionList.size() == 3)
assert(aFactory.validate(Group_5.feature()))

selectionList = Group_6.feature().selectionList("group_list")
assert(selectionList.size() == 2) # edge selected for the fillet become removed
assert(aFactory.validate(Group_6.feature()))

model.begin()
Fillet_3 = model.addFillet(Part_1_doc, [model.selection("EDGE", "[Fillet_1_1/MF:Fillet&Box_1_1/Front][Fillet_1_1/MF:Fillet&Box_1_1/Left]")], 1, 2)
model.do()
# move groups to the end
Part_1_doc.moveFeature(Group_1.feature(), Fillet_3.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
Part_1_doc.moveFeature(Group_3.feature(), Group_2.feature())
Part_1_doc.moveFeature(Group_4.feature(), Group_3.feature())
Part_1_doc.moveFeature(Group_5.feature(), Group_4.feature())
Part_1_doc.moveFeature(Group_6.feature(), Group_5.feature())
model.end()

# groups related to original box should be split
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 1)
assert(aFactory.validate(Group_1.feature()))

selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 6)
assert(aFactory.validate(Group_2.feature()))

selectionList = Group_3.feature().selectionList("group_list")
assert(selectionList.size() == 9)
assert(aFactory.validate(Group_3.feature()))

# groups related to original cylinder should stay untouched
selectionList = Group_4.feature().selectionList("group_list")
assert(selectionList.size() == 1)
assert(aFactory.validate(Group_4.feature()))

selectionList = Group_5.feature().selectionList("group_list")
assert(selectionList.size() == 3)
assert(aFactory.validate(Group_5.feature()))

selectionList = Group_6.feature().selectionList("group_list")
assert(selectionList.size() == 2)
assert(aFactory.validate(Group_6.feature()))

assert(model.checkPythonDump())
