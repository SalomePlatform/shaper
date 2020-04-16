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

# Test of deep nested results history. Scale operation.
# Check the groups of initial solids moved to the end contain the corresponding results.

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-15, 30, 30)
SketchCircle_2 = Sketch_1.addCircle(15, 25, 25)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
Group_1_objects = [model.selection("SOLID", "Extrusion_1_1_3"), model.selection("SOLID", "Extrusion_1_1_1"), model.selection("SOLID", "Extrusion_1_1_2")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
Group_2_objects = [model.selection("FACE", "Extrusion_1_1_3/Generated_Face&Sketch_1/SketchCircle_2_2"), model.selection("FACE", "Extrusion_1_1_3/Generated_Face&Sketch_1/SketchCircle_1_2"), model.selection("FACE", "Extrusion_1_1_3/From_Face"), model.selection("FACE", "Extrusion_1_1_3/To_Face"), model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1"), model.selection("FACE", "Extrusion_1_1_1/To_Face"), model.selection("FACE", "Extrusion_1_1_1/From_Face"), model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_2_2"), model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2"), model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_1"), model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1"), model.selection("FACE", "Extrusion_1_1_2/From_Face"), model.selection("FACE", "Extrusion_1_1_2/To_Face"), model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2"), model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_2")]
Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
Group_3_objects = [model.selection("EDGE", "[Extrusion_1_1_3/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1_3/Generated_Face&Sketch_1/SketchCircle_2_2][weak_name_1]"), model.selection("EDGE", "[Extrusion_1_1_3/Generated_Face&Sketch_1/SketchCircle_2_2][Extrusion_1_1_3/From_Face]"), model.selection("EDGE", "[Extrusion_1_1_3/Generated_Face&Sketch_1/SketchCircle_2_2][Extrusion_1_1_3/To_Face]"), model.selection("EDGE", "[Extrusion_1_1_3/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1_3/From_Face]"), model.selection("EDGE", "[Extrusion_1_1_3/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1_3/To_Face]"), model.selection("EDGE", "[Extrusion_1_1_3/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1_3/Generated_Face&Sketch_1/SketchCircle_2_2][weak_name_2]"), model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_2_2]"), model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_1/From_Face]"), model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_1/To_Face]"), model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_2_2][Extrusion_1_1_1/From_Face]"), model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2]"), model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_2_2][Extrusion_1_1_1/To_Face]"), model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_1/To_Face]"), model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_1/From_Face]"), model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_2_2]"), model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_1]"), model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_1][Extrusion_1_1_2/From_Face]"), model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_1][Extrusion_1_1_2/To_Face]"), model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_2/To_Face]"), model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_2/From_Face]"), model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2]"), model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_2/To_Face]"), model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_2/From_Face]"), model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_2][Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2]"), model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_2][Extrusion_1_1_2/To_Face]"), model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_2][Extrusion_1_1_2/From_Face]"), model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_1][Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_2]")]
Group_3 = model.addGroup(Part_1_doc, Group_3_objects)
Group_4_objects = [model.selection("VERTEX", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_1][Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_2][Extrusion_1_1_2/To_Face]"), model.selection("VERTEX", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_1][Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_2][Extrusion_1_1_2/From_Face]"), model.selection("VERTEX", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_2_2][Extrusion_1_1_1/To_Face]"), model.selection("VERTEX", "[_weak_name_5_Extrusion_1_1_3]e[_weak_name_2_Extrusion_1_1_3]e[_weak_name_4_Extrusion_1_1_3]e"), model.selection("VERTEX", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_1][Extrusion_1_1_2/To_Face]"), model.selection("VERTEX", "[_weak_name_5_Extrusion_1_1_3]e[_weak_name_1_Extrusion_1_1_3]e[_weak_name_3_Extrusion_1_1_3]e"), model.selection("VERTEX", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_2_2][Extrusion_1_1_1/From_Face]"), model.selection("VERTEX", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_1][Extrusion_1_1_2/From_Face]"), model.selection("VERTEX", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_1/To_Face]"), model.selection("VERTEX", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_2/To_Face]"), model.selection("VERTEX", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_2/From_Face]"), model.selection("VERTEX", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_1/From_Face]"), model.selection("VERTEX", "[_weak_name_6_Extrusion_1_1_3]e[_weak_name_2_Extrusion_1_1_3]e[_weak_name_4_Extrusion_1_1_3]e"), model.selection("VERTEX", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_2][Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_2/To_Face]"), model.selection("VERTEX", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_2_2][Extrusion_1_1_1/To_Face]"), model.selection("VERTEX", "[_weak_name_6_Extrusion_1_1_3]e[_weak_name_1_Extrusion_1_1_3]e[_weak_name_3_Extrusion_1_1_3]e"), model.selection("VERTEX", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_2][Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_2/From_Face]"), model.selection("VERTEX", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_2_2][Extrusion_1_1_1/From_Face]")]
Group_4 = model.addGroup(Part_1_doc, Group_4_objects)
Scale_1 = model.addScale(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1_1")] , model.selection("VERTEX", "PartSet/Origin"), 2)
model.do()
# move groups to the end
Part_1_doc.moveFeature(Group_1.feature(), Scale_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
Part_1_doc.moveFeature(Group_3.feature(), Group_2.feature())
Part_1_doc.moveFeature(Group_4.feature(), Group_3.feature())
model.end()

from ModelAPI import *

aFactory = ModelAPI_Session.get().validators()

selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 1)
assert(aFactory.validate(Group_1.feature()))

selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 5)
assert(aFactory.validate(Group_2.feature()))

selectionList = Group_3.feature().selectionList("group_list")
assert(selectionList.size() == 9)
assert(aFactory.validate(Group_3.feature()))

selectionList = Group_4.feature().selectionList("group_list")
assert(selectionList.size() == 6)
assert(aFactory.validate(Group_4.feature()))

assert(model.checkPythonDump())
