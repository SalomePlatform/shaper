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

# Test that the history of Common operation works correctly after movement of groups after this Common feature

# -*- coding: utf-8 -*-

from salome.shaper import model
from ModelAPI import *
from GeomAPI import *
from SketchAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(5, 5, 7.071067811865464)
SketchCircle_2 = Sketch_1.addCircle(0, 10, 7.071067811865464)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchCircle_2.results()[1])
SketchCircle_3 = Sketch_1.addCircle(-5, 5, 7.071067811865464)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchCircle_2.center(), SketchCircle_3.results()[1])
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchCircle_3.center(), SketchCircle_2.results()[1])
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchCircle_1.results()[1], SketchCircle_2.center())
SketchCircle_4 = Sketch_1.addCircle(0, 0, 7.071067811865464)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchCircle_4.center(), SketchCircle_3.results()[1])
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchCircle_4.results()[1], SketchCircle_3.center())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchCircle_4.results()[1], SketchCircle_1.center())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchCircle_1.center(), SketchCircle_3.center(), 10, True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchCircle_2.center(), SketchCircle_4.center(), 10, True)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchCircle_4.center(), SketchAPI_Point(SketchPoint_1).coordinates())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_1 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchCircle_2.center(), SketchLine_1.result())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_2 = Sketch_2.addLine(-5, 5, 0, 0)
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchCircle_3_2__cc"), False)
SketchPoint_2 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_2.startPoint(), SketchPoint_2.result())
SketchProjection_4 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchCircle_4_2__cc"), False)
SketchPoint_3 = SketchProjection_4.createdFeature()
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchPoint_3.result())
SketchLine_3 = Sketch_2.addLine(0, 0, 5, 5)
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchProjection_5 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchCircle_1_2__cc"), False)
SketchPoint_4 = SketchProjection_5.createdFeature()
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchPoint_4.result())
SketchLine_4 = Sketch_2.addLine(-5, 5, 5, 5)
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchLine_2.startPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.endPoint())
SketchArc_1 = Sketch_2.addArc(-1.474514954580286e-16, 5, -5, 5, 5, 5, True)
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchLine_4.result(), SketchArc_1.center())
SketchConstraintCoincidence_17 = Sketch_2.setCoincident(SketchLine_2.startPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_18 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchArc_1.endPoint())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection(), 10, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_2")], model.selection(), 5, 5)
Group_1_objects = [model.selection("SOLID", "Extrusion_1_1_13"), model.selection("SOLID", "Extrusion_1_1_10"), model.selection("SOLID", "Extrusion_1_1_7"), model.selection("SOLID", "Extrusion_1_1_9"), model.selection("SOLID", "Extrusion_1_1_1"), model.selection("SOLID", "Extrusion_1_1_5"), model.selection("SOLID", "Extrusion_1_1_4"), model.selection("SOLID", "Extrusion_1_1_12"), model.selection("SOLID", "Extrusion_1_1_11"), model.selection("SOLID", "Extrusion_1_1_8"), model.selection("SOLID", "Extrusion_1_1_3"), model.selection("SOLID", "Extrusion_1_1_6"), model.selection("SOLID", "Extrusion_1_1_2")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
Group_2 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Extrusion_2_1_2"), model.selection("SOLID", "Extrusion_2_1_1")])
Group_3_objects = [model.selection("FACE", "Extrusion_1_1_7/From_Face"), model.selection("FACE", "Extrusion_1_1_1/From_Face"), model.selection("FACE", "Extrusion_1_1_9/From_Face"), model.selection("FACE", "Extrusion_1_1_12/From_Face"), model.selection("FACE", "Extrusion_1_1_4/From_Face"), model.selection("FACE", "Extrusion_1_1_5/From_Face"), model.selection("FACE", "Extrusion_1_1_8/From_Face"), model.selection("FACE", "Extrusion_1_1_11/From_Face"), model.selection("FACE", "Extrusion_1_1_6/From_Face"), model.selection("FACE", "Extrusion_1_1_2/From_Face"), model.selection("FACE", "Extrusion_1_1_3/From_Face"), model.selection("FACE", "Extrusion_1_1_10/From_Face")]
Group_3 = model.addGroup(Part_1_doc, Group_3_objects)
Group_4_objects = [model.selection("FACE", "Extrusion_2_1_2/Generated_Face&Sketch_2/SketchLine_4"), model.selection("FACE", "Extrusion_2_1_2/Generated_Face&Sketch_2/SketchArc_1_2"), model.selection("FACE", "Extrusion_2_1_2/From_Face"), model.selection("FACE", "Extrusion_2_1_2/To_Face"), model.selection("FACE", "Extrusion_2_1_1/Generated_Face&Sketch_2/SketchLine_2"), model.selection("FACE", "Extrusion_2_1_1/To_Face"), model.selection("FACE", "Extrusion_2_1_1/From_Face"), model.selection("FACE", "Extrusion_2_1_1/Generated_Face&Sketch_2/SketchLine_4"), model.selection("FACE", "Extrusion_2_1_1/Generated_Face&Sketch_2/SketchLine_3")]
Group_4 = model.addGroup(Part_1_doc, Group_4_objects)
Common_1 = model.addCommon(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_1_1"), model.selection("COMPSOLID", "Extrusion_2_1")])
Common_1.result().subResult(0).setColor(254, 254, 127)
Common_1.result().subResult(1).setColor(204, 102, 204)
Common_1.result().subResult(2).setColor(51, 102, 102)
Common_1.result().subResult(3).setColor(0, 153, 0)
Common_1.result().subResult(4).setColor(254, 0, 0)
Common_1.result().subResult(5).setColor(204, 0, 204)
Common_1.result().subResult(6).setColor(102, 0, 0)
Common_1.result().subResult(7).setColor(0, 0, 153)
Common_1.result().subResult(8).setColor(0, 0, 204)
Common_1.result().subResult(9).setColor(0, 0, 204)
Common_1.result().subResult(10).setColor(0, 153, 0)
Common_1.result().subResult(11).setColor(127, 127, 254)
model.do()
# move groups after the Common
Part_1_doc.moveFeature(Group_1.feature(), Common_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
Part_1_doc.moveFeature(Group_3.feature(), Group_2.feature())
Part_1_doc.moveFeature(Group_4.feature(), Group_3.feature())
model.end()

# check groups are correct
aFactory = ModelAPI_Session.get().validators()
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 12)
assert(aFactory.validate(Group_1.feature()))
for i in range(12):
  assert(Group_1.groupList().value(i).value().shapeType() == GeomAPI_Shape.SOLID)

selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 12)
assert(aFactory.validate(Group_2.feature()))
for i in range(12):
  assert(Group_2.groupList().value(i).value().shapeType() == GeomAPI_Shape.SOLID)

selectionList = Group_3.feature().selectionList("group_list")
assert(selectionList.size() == 12)
assert(aFactory.validate(Group_3.feature()))
for i in range(12):
  assert(Group_3.groupList().value(i).value().shapeType() == GeomAPI_Shape.FACE)

selectionList = Group_4.feature().selectionList("group_list")
assert(selectionList.size() == 22)
assert(aFactory.validate(Group_4.feature()))
for i in range(22):
  assert(Group_4.groupList().value(i).value().shapeType() == GeomAPI_Shape.FACE)

assert(model.checkPythonDump(model.ModelHighAPI.CHECK_NAMING))
