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

# Test that the history of Smash operation works correctly after movement of groups after this Smash feature

# -*- coding: utf-8 -*-

from salome.shaper import model
from ModelAPI import *
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(26.49384236453203, 25.42805779478775, -23.25985221674878, 25.42805779478775)
SketchLine_2 = Sketch_1.addLine(-23.25985221674878, 25.42805779478775, -23.25985221674878, -22.79086009461325)
SketchLine_3 = Sketch_1.addLine(-23.25985221674878, -22.79086009461325, 26.49384236453203, -22.79086009461325)
SketchLine_4 = Sketch_1.addLine(26.49384236453203, -22.79086009461325, 26.49384236453203, 25.42805779478775)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(-23.25985221674878, -22.79086009461325, -4.602216748768477, 25.42805779478775)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_1.result())
SketchLine_6 = Sketch_1.addLine(-4.602216748768477, 25.42805779478775, 8.458128078817737, -22.79086009461324)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_3.result())
SketchLine_7 = Sketch_1.addLine(8.458128078817737, -22.79086009461324, 26.49384236453203, 25.42805779478775)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_7.endPoint())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_8 = Sketch_2.addLine(38.18596059113301, 15.79679802955666, -30.91800131851275, 15.79679802955666)
SketchLine_9 = Sketch_2.addLine(-30.91800131851275, 15.79679802955666, -30.91800131851275, -10.07512315270936)
SketchLine_10 = Sketch_2.addLine(-30.91800131851275, -10.07512315270936, 38.18596059113301, -10.07512315270936)
SketchLine_11 = Sketch_2.addLine(38.18596059113301, -10.07512315270936, 38.18596059113301, 15.79679802955666)
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_8.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_9.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_10.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_11.result())
SketchLine_12 = Sketch_2.addLine(38.18596059113301, 15.79679802955666, -30.91800131851275, 1.368226600985215)
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchLine_8.startPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_9.result())
SketchLine_13 = Sketch_2.addLine(-30.91800131851275, 1.368226600985215, 38.18596059113301, -10.07512315270936)
SketchConstraintCoincidence_17 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_18 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_13.endPoint())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection(), 10, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_2")], model.selection(), 5, 5)
Group_1_objects = [model.selection("SOLID", "Extrusion_1_1_1"), model.selection("SOLID", "Extrusion_1_1_4"), model.selection("SOLID", "Extrusion_1_1_2"), model.selection("SOLID", "Extrusion_1_1_3")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
Group_2_objects = [model.selection("SOLID", "Extrusion_2_1_2"), model.selection("SOLID", "Extrusion_2_1_3"), model.selection("SOLID", "Extrusion_2_1_1")]
Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
Group_3_objects = [model.selection("FACE", "Extrusion_1_1_4/From_Face"), model.selection("FACE", "Extrusion_1_1_2/From_Face"), model.selection("FACE", "Extrusion_1_1_4/Generated_Face&Sketch_1/SketchLine_3"), model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_1"), model.selection("FACE", "Extrusion_1_1_4/To_Face"), model.selection("FACE", "Extrusion_1_1_2/To_Face")]
Group_3 = model.addGroup(Part_1_doc, Group_3_objects)
Group_4_objects = [model.selection("FACE", "Extrusion_2_1_2/To_Face"), model.selection("FACE", "Extrusion_2_1_3/To_Face"), model.selection("FACE", "Extrusion_2_1_1/To_Face")]
Group_4 = model.addGroup(Part_1_doc, Group_4_objects)
Smash_1_objects_1 = [model.selection("SOLID", "Extrusion_1_1_1"), model.selection("SOLID", "Extrusion_1_1_4"), model.selection("SOLID", "Extrusion_1_1_2")]
Smash_1 = model.addSmash(Part_1_doc, Smash_1_objects_1, [model.selection("SOLID", "Extrusion_2_1_1"), model.selection("SOLID", "Extrusion_2_1_2")])
model.do()
# move groups after the Smash
Part_1_doc.moveFeature(Group_1.feature(), Smash_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
Part_1_doc.moveFeature(Group_3.feature(), Group_2.feature())
Part_1_doc.moveFeature(Group_4.feature(), Group_3.feature())
model.end()

# check groups are correct
aFactory = ModelAPI_Session.get().validators()
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 4)
assert(aFactory.validate(Group_1.feature()))
for i in range(4):
  assert(Group_1.groupList().value(i).value().shapeType() == GeomAPI_Shape.SOLID)

selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 2)
assert(aFactory.validate(Group_2.feature()))
for i in range(2):
  assert(Group_2.groupList().value(i).value().shapeType() == GeomAPI_Shape.SOLID)

selectionList = Group_3.feature().selectionList("group_list")
assert(selectionList.size() == 10)
assert(aFactory.validate(Group_3.feature()))
for i in range(10):
  assert(Group_3.groupList().value(i).value().shapeType() == GeomAPI_Shape.FACE)

selectionList = Group_4.feature().selectionList("group_list")
assert(selectionList.size() == 10)
assert(aFactory.validate(Group_4.feature()))
for i in range(10):
  assert(Group_4.groupList().value(i).value().shapeType() == GeomAPI_Shape.FACE)

assert(model.checkPythonDump())
