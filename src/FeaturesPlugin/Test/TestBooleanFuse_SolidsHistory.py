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

# -*- coding: utf-8 -*-

from salome.shaper import model
from ModelAPI import *
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-9.826354679802961, 29.97660098522167, -30.22536945812809, 29.97660098522167)
SketchLine_2 = Sketch_1.addLine(-30.22536945812809, 29.97660098522167, -30.22536945812809, 11.69211822660099)
SketchLine_3 = Sketch_1.addLine(-30.22536945812809, 11.69211822660099, -9.826354679802961, 11.69211822660099)
SketchLine_4 = Sketch_1.addLine(-9.826354679802961, 11.69211822660099, -9.826354679802961, 29.97660098522167)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(-30.22536945812809, 11.69211822660099, -9.826354679802961, 29.97660098522167)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.endPoint())
SketchCircle_1 = Sketch_1.addCircle(9.328817733990148, 6.841133004926101, 8.342093824373531)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_2 = Sketch_2.addCircle(-5.846059113300496, 9.453201970443347, 11.01344978196573)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection(), 10, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection(), 11, 1)
Group_1_objects = [model.selection("SOLID", "Extrusion_1_1_1"), model.selection("SOLID", "Extrusion_1_1_2"), model.selection("SOLID", "Extrusion_2_1"), model.selection("SOLID", "Extrusion_1_2")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
Group_2_objects = [model.selection("FACE", "Extrusion_1_1_1/To_Face"), model.selection("FACE", "Extrusion_1_1_2/To_Face"), model.selection("FACE", "Extrusion_2_1/To_Face"), model.selection("FACE", "Extrusion_1_2/To_Face")]
Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
Fuse_1_objects_1 = [model.selection("SOLID", "Extrusion_1_1_2"), model.selection("SOLID", "Extrusion_2_1"), model.selection("SOLID", "Extrusion_1_2")]
Fuse_1 = model.addFuse(Part_1_doc, Fuse_1_objects_1)
model.do()
# move group
Part_1_doc.moveFeature(Group_1.feature(), Fuse_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
model.end()

# All solids are merged to one - result
aFactory = ModelAPI_Session.get().validators()
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 1)
assert(aFactory.validate(Group_1.feature()))
for i in range(1):
  assert(Group_1.groupList().value(i).value().shapeType() == GeomAPI_Shape.SOLID)

# Check group 2: 3 faces
selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 3)
assert(aFactory.validate(Group_2.feature()))
for i in range(3):
  assert(Group_2.groupList().value(i).value().shapeType() == GeomAPI_Shape.FACE)

assert(model.checkPythonDump())
