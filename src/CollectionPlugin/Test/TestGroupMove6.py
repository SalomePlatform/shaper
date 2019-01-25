## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

# Test of deep nested results history.
# Copied boxes are involved to Fill operation.
# Check the groups of initial boxes moved to the end contain the corresponding
# results, but divided.

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Axis_4 = model.addAxis(Part_1_doc, model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Bottom]"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Top]"))
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "Axis_1"), 10, 2)
Group_1 = model.addGroup(Part_1_doc, [model.selection("SOLID", "LinearCopy_1_1_1")])
Group_2_objects = [model.selection("FACE", "LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Back"), model.selection("FACE", "LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Top"), model.selection("FACE", "LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Right"), model.selection("FACE", "LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Left"), model.selection("FACE", "LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Bottom"), model.selection("FACE", "LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Front")]
Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
Group_3_objects = [model.selection("EDGE", "[LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Back][LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Bottom]"), model.selection("EDGE", "[LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Back][LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Right]"), model.selection("EDGE", "[LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Back][LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Left]"), model.selection("EDGE", "[LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Back][LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Top]"), model.selection("EDGE", "[LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Right][LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Bottom]"), model.selection("EDGE", "[LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Left][LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Top]"), model.selection("EDGE", "[LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Left][LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Bottom]"), model.selection("EDGE", "[LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Right][LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Top]"), model.selection("EDGE", "[LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Front][LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Bottom]"), model.selection("EDGE", "[LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Front][LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Left]"), model.selection("EDGE", "[LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Front][LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Right]"), model.selection("EDGE", "[LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Front][LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Top]")]
Group_3 = model.addGroup(Part_1_doc, Group_3_objects)
Group_4 = model.addGroup(Part_1_doc, [model.selection("SOLID", "LinearCopy_1_1_2")])
Group_5_objects = [model.selection("FACE", "LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Back"), model.selection("FACE", "LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Top"), model.selection("FACE", "LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Right"), model.selection("FACE", "LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Left"), model.selection("FACE", "LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Bottom"), model.selection("FACE", "LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Front")]
Group_5 = model.addGroup(Part_1_doc, Group_5_objects)
Group_6_objects = [model.selection("EDGE", "[LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Back][LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Bottom]"), model.selection("EDGE", "[LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Back][LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Right]"), model.selection("EDGE", "[LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Back][LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Left]"), model.selection("EDGE", "[LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Back][LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Top]"), model.selection("EDGE", "[LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Right][LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Bottom]"), model.selection("EDGE", "[LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Left][LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Top]"), model.selection("EDGE", "[LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Left][LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Bottom]"), model.selection("EDGE", "[LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Right][LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Top]"), model.selection("EDGE", "[LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Front][LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Bottom]"), model.selection("EDGE", "[LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Front][LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Left]"), model.selection("EDGE", "[LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Front][LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Right]"), model.selection("EDGE", "[LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Front][LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Top]")]
Group_6 = model.addGroup(Part_1_doc, Group_6_objects)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Left"), model.selection("FACE", "LinearCopy_1_1_1/MF:Translated_Face&Box_1_1/Right"))
Fill_1 = model.addFill(Part_1_doc, [model.selection("SOLID", "LinearCopy_1_1_1")], [model.selection("SOLID", "LinearCopy_1_1_2"), model.selection("FACE", "Plane_1")])
model.do()
# move groups to the end
Part_1_doc.moveFeature(Group_1.feature(), Fill_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
Part_1_doc.moveFeature(Group_3.feature(), Group_2.feature())
Part_1_doc.moveFeature(Group_4.feature(), Group_3.feature())
Part_1_doc.moveFeature(Group_5.feature(), Group_4.feature())
Part_1_doc.moveFeature(Group_6.feature(), Group_5.feature())
model.end()

from ModelAPI import *

aFactory = ModelAPI_Session.get().validators()

# groups related to original box should be split
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 3)
assert(aFactory.validate(Group_1.feature()))

selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 13)
assert(aFactory.validate(Group_2.feature()))

selectionList = Group_3.feature().selectionList("group_list")
assert(selectionList.size() == 19)
assert(aFactory.validate(Group_3.feature()))

# groups related to the copied box should stay untouched
selectionList = Group_4.feature().selectionList("group_list")
assert(selectionList.size() == 1)
assert(aFactory.validate(Group_4.feature()))

selectionList = Group_5.feature().selectionList("group_list")
assert(selectionList.size() == 6)
assert(aFactory.validate(Group_5.feature()))

selectionList = Group_6.feature().selectionList("group_list")
assert(selectionList.size() == 12)
assert(aFactory.validate(Group_6.feature()))


model.begin()
Filling_1 = model.addFilling(Part_1_doc, [model.selection("EDGE", "Fill_1_1_2/Generated_Edge&Plane_1/Plane_1&Box_1_1/Top"), model.selection("EDGE", "[Fill_1_1_2/Modified_Face&Box_1_1/Right][(Fill_1_1_2/Modified_Face&Box_1_1/Right)(Fill_1_1_2/Modified_Face&Plane_1/Plane_1)(Fill_1_1_2/Modified_Face&Box_1_1/Front)(Fill_1_1_2/Modified_Face&Box_1_1/Top)2(Fill_1_1_2/Modified_Face&Box_1_1/Left)2]")])
Fill_2 = model.addFill(Part_1_doc, [model.selection("SOLID", "Fill_1_1_2")], [model.selection("FACE", "Filling_1_1")])
model.do()
# move groups to the end once again
Part_1_doc.moveFeature(Group_1.feature(), Fill_2.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
Part_1_doc.moveFeature(Group_3.feature(), Group_2.feature())
Part_1_doc.moveFeature(Group_4.feature(), Group_3.feature())
Part_1_doc.moveFeature(Group_5.feature(), Group_4.feature())
Part_1_doc.moveFeature(Group_6.feature(), Group_5.feature())
model.end()

# groups related to original box should be split
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 4)
assert(aFactory.validate(Group_1.feature()))

selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 16)
assert(aFactory.validate(Group_2.feature()))

selectionList = Group_3.feature().selectionList("group_list")
assert(selectionList.size() == 19)
assert(aFactory.validate(Group_3.feature()))

# groups related to the copied box should stay untouched
selectionList = Group_4.feature().selectionList("group_list")
assert(selectionList.size() == 1)
assert(aFactory.validate(Group_4.feature()))

selectionList = Group_5.feature().selectionList("group_list")
assert(selectionList.size() == 6)
assert(aFactory.validate(Group_5.feature()))

selectionList = Group_6.feature().selectionList("group_list")
assert(selectionList.size() == 12)
assert(aFactory.validate(Group_6.feature()))

assert(model.checkPythonDump())
