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

# Test of deep nested results history. FuseFaces operation.
# Check the groups of initial solids moved to the end contain the corresponding
# results, but merged faces.

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1"), model.selection("SOLID", "Box_1_1")])
Union_1_objects = [model.selection("SOLID", "Partition_1_1_1"), model.selection("SOLID", "Partition_1_1_2"), model.selection("SOLID", "Partition_1_1_3")]
Union_1 = model.addUnion(Part_1_doc, Union_1_objects)
Group_1 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Union_1_1")])
Group_2_objects = [model.selection("FACE", "Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_1"), model.selection("FACE", "Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_2"), model.selection("FACE", "Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_3"), model.selection("FACE", "Partition_1_1_2/Modified_Face&Box_1_1/Bottom&Cylinder_1_1/Face_3"), model.selection("FACE", "Partition_1_1_2/Modified_Face&Box_1_1/Top&Cylinder_1_1/Face_2"), model.selection("FACE", "Partition_1_1_3/Modified_Face&Box_1_1/Back"), model.selection("FACE", "Partition_1_1_3/Modified_Face&Box_1_1/Bottom"), model.selection("FACE", "Partition_1_1_3/Modified_Face&Box_1_1/Top"), model.selection("FACE", "Box_1_1/Right"), model.selection("FACE", "Partition_1_1_3/Modified_Face&Box_1_1/Left"), model.selection("FACE", "Box_1_1/Front")]
Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
Group_3_objects = [model.selection("EDGE", "[Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_1][Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_3]"), model.selection("EDGE", "[Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_2][Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_1]"), model.selection("EDGE", "([Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_1][Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_3])(Partition_1_1_3/Generated_Edge&Box_1_1/Back&Cylinder_1_1/Face_1)([Partition_1_1_3/Modified_Face&Box_1_1/Back][Partition_1_1_3/Modified_Face&Box_1_1/Bottom])([Partition_1_1_3/Modified_Face&Box_1_1/Bottom][Partition_1_1_2/Modified_Face&Box_1_1/Bottom&Cylinder_1_1/Face_3])_Union_1_1"), model.selection("EDGE", "[Partition_1_1_3/Modified_Face&Box_1_1/Back][Partition_1_1_3/Modified_Face&Box_1_1/Bottom]"), model.selection("EDGE", "[Partition_1_1_3/Modified_Face&Box_1_1/Back][Box_1_1/Right]"), model.selection("EDGE", "Partition_1_1_3/Generated_Edge&Box_1_1/Back&Cylinder_1_1/Face_1"), model.selection("EDGE", "([Partition_1_1_2/Modified_Face&Box_1_1/Top&Cylinder_1_1/Face_2][Partition_1_1_3/Modified_Face&Box_1_1/Top])([Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_2][Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_1])([Partition_1_1_3/Modified_Face&Box_1_1/Top][Partition_1_1_3/Modified_Face&Box_1_1/Back])(Partition_1_1_3/Generated_Edge&Box_1_1/Back&Cylinder_1_1/Face_1)_Union_1_1"), model.selection("EDGE", "[Partition_1_1_3/Modified_Face&Box_1_1/Top][Partition_1_1_3/Modified_Face&Box_1_1/Back]"), model.selection("EDGE", "([Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_1][Partition_1_1_3/Modified_Face&Box_1_1/Left])([Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_1][Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_3])([Partition_1_1_3/Modified_Face&Box_1_1/Left][Partition_1_1_3/Modified_Face&Box_1_1/Bottom])([Partition_1_1_3/Modified_Face&Box_1_1/Bottom][Partition_1_1_2/Modified_Face&Box_1_1/Bottom&Cylinder_1_1/Face_3])_Union_1_1"), model.selection("EDGE", "[Partition_1_1_3/Modified_Face&Box_1_1/Bottom][Partition_1_1_2/Modified_Face&Box_1_1/Bottom&Cylinder_1_1/Face_3]"), model.selection("EDGE", "[Partition_1_1_2/Modified_Face&Box_1_1/Top&Cylinder_1_1/Face_2][Partition_1_1_3/Modified_Face&Box_1_1/Top]"), model.selection("EDGE", "([Partition_1_1_2/Modified_Face&Box_1_1/Top&Cylinder_1_1/Face_2][Partition_1_1_3/Modified_Face&Box_1_1/Top])([Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_2][Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_1])([Partition_1_1_3/Modified_Face&Box_1_1/Top][Partition_1_1_3/Modified_Face&Box_1_1/Left])([Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_1][Partition_1_1_3/Modified_Face&Box_1_1/Left])_Union_1_1"), model.selection("EDGE", "[Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_1][Partition_1_1_3/Modified_Face&Box_1_1/Left]"), model.selection("EDGE", "[Partition_1_1_3/Modified_Face&Box_1_1/Top][Box_1_1/Right]"), model.selection("EDGE", "[Box_1_1/Right][Partition_1_1_3/Modified_Face&Box_1_1/Bottom]"), model.selection("EDGE", "[Partition_1_1_3/Modified_Face&Box_1_1/Left][Partition_1_1_3/Modified_Face&Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Front][Partition_1_1_3/Modified_Face&Box_1_1/Left]"), model.selection("EDGE", "[Partition_1_1_3/Modified_Face&Box_1_1/Top][Partition_1_1_3/Modified_Face&Box_1_1/Left]"), model.selection("EDGE", "[Box_1_1/Front][Partition_1_1_3/Modified_Face&Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Front]"), model.selection("EDGE", "[Partition_1_1_3/Modified_Face&Box_1_1/Top][Box_1_1/Front]")]
Group_3 = model.addGroup(Part_1_doc, Group_3_objects)
Group_4_objects = [model.selection("VERTEX", "[Partition_1_1_3/Modified_Face&Box_1_1/Top][Box_1_1/Front][Partition_1_1_3/Modified_Face&Box_1_1/Left]"), model.selection("VERTEX", "[Partition_1_1_2/Modified_Face&Box_1_1/Top&Cylinder_1_1/Face_2][Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_2][Partition_1_1_3/Modified_Face&Box_1_1/Top][Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_1][Partition_1_1_3/Modified_Face&Box_1_1/Left]"), model.selection("VERTEX", "[Box_1_1/Front][Partition_1_1_3/Modified_Face&Box_1_1/Left][Partition_1_1_3/Modified_Face&Box_1_1/Bottom]"), model.selection("VERTEX", "[_weak_name_10_Union_1_1]e[_weak_name_4_Union_1_1]e"), model.selection("VERTEX", "[Partition_1_1_3/Modified_Face&Box_1_1/Top][Box_1_1/Right][Box_1_1/Front]"), model.selection("VERTEX", "[Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_1][Partition_1_1_3/Modified_Face&Box_1_1/Left][Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_3][Partition_1_1_3/Modified_Face&Box_1_1/Bottom][Partition_1_1_2/Modified_Face&Box_1_1/Bottom&Cylinder_1_1/Face_3]"), model.selection("VERTEX", "[Partition_1_1_2/Modified_Face&Box_1_1/Top&Cylinder_1_1/Face_2][Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_2][Partition_1_1_3/Modified_Face&Box_1_1/Top][Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_1][Partition_1_1_3/Modified_Face&Box_1_1/Back]"), model.selection("VERTEX", "[Box_1_1/Right][Box_1_1/Front][Partition_1_1_3/Modified_Face&Box_1_1/Bottom]"), model.selection("VERTEX", "[_weak_name_3_Union_1_1]e[_weak_name_9_Union_1_1]e"), model.selection("VERTEX", "[Partition_1_1_3/Modified_Face&Box_1_1/Top][Partition_1_1_3/Modified_Face&Box_1_1/Back][Box_1_1/Right]"), model.selection("VERTEX", "[Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_1][Partition_1_1_3/Modified_Face&Box_1_1/Back][Partition_1_1_1/Modified_Face&Cylinder_1_1/Face_3][Partition_1_1_3/Modified_Face&Box_1_1/Bottom][Partition_1_1_2/Modified_Face&Box_1_1/Bottom&Cylinder_1_1/Face_3]"), model.selection("VERTEX", "[Partition_1_1_3/Modified_Face&Box_1_1/Back][Box_1_1/Right][Partition_1_1_3/Modified_Face&Box_1_1/Bottom]")]
Group_4 = model.addGroup(Part_1_doc, Group_4_objects)
FusionFaces_1 = model.addFusionFaces(Part_1_doc, model.selection("SOLID", "Union_1_1"))
model.do()
# move groups to the end
Part_1_doc.moveFeature(Group_1.feature(), FusionFaces_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
Part_1_doc.moveFeature(Group_3.feature(), Group_2.feature())
Part_1_doc.moveFeature(Group_4.feature(), Group_3.feature())
model.end()

from ModelAPI import *

aFactory = ModelAPI_Session.get().validators()

# number of solids should be the same
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 1)
assert(aFactory.validate(Group_1.feature()))

# same-plane faces should be merged
selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 7)
assert(aFactory.validate(Group_2.feature()))

# shared edges of merged faces should be deleted
selectionList = Group_3.feature().selectionList("group_list")
assert(selectionList.size() == 15)
assert(aFactory.validate(Group_3.feature()))

selectionList = Group_4.feature().selectionList("group_list")
assert(selectionList.size() == 10)
assert(aFactory.validate(Group_4.feature()))

assert(model.checkPythonDump())
