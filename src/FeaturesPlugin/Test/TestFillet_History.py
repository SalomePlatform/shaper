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

# Test that the history of Fillet operation works correctly after movement of groups after this Fillet feature:
# Faces are modified, edges that were used for fillet are removed

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
SketchLine_1 = Sketch_1.addLine(23.0955987576144, 26.85924638608351, -23.50862068965518, 26.85924638608351)
SketchLine_2 = Sketch_1.addLine(-23.50862068965518, 26.85924638608351, -23.50862068965518, -11.11713209726581)
SketchLine_3 = Sketch_1.addLine(-23.50862068965518, -11.11713209726581, 23.0955987576144, -11.11713209726581)
SketchLine_4 = Sketch_1.addLine(23.0955987576144, -11.11713209726581, 23.0955987576144, 26.85924638608351)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_3.setName("SketchConstraintCoincidence_4")
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(2.024890803123894, -11.11713209726581, 23.0955987576144, 26.85924638608351)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.endPoint())
SketchConstraintCoincidence_4.setName("SketchConstraintCoincidence_6")
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_3.result())
SketchConstraintCoincidence_5.setName("SketchConstraintCoincidence_5")
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_6.setName("SketchConstraintCoincidence_7")
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_3f-SketchLine_4f-SketchLine_5r"), model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_5f")], model.selection(), 10, 0)
Group_1_objects = [model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_2"), model.selection("FACE", "Extrusion_1_1_2/To_Face"), model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_3")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
Group_2_objects = [model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_3]"), model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1_2/To_Face]"), model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_2]"), model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_5][Extrusion_1_1_2/To_Face]")]
Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
Fillet_1_objects = [model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1_2/To_Face]"), model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_2]"), model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1_2/From_Face]")]
Fillet_1 = model.addFillet(Part_1_doc, Fillet_1_objects, 2)
model.do()
# move groups after the Fillet
Part_1_doc.moveFeature(Group_1.feature(), Fillet_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
model.end()

# check groups are correct
aFactory = ModelAPI_Session.get().validators()
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 3)
assert(aFactory.validate(Group_1.feature()))
for i in range(3):
  assert(Group_1.groupList().value(i).value().shapeType() == GeomAPI_Shape.FACE)

selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 2)
assert(aFactory.validate(Group_2.feature()))
for i in range(2):
  assert(Group_2.groupList().value(i).value().shapeType() == GeomAPI_Shape.EDGE)

assert(model.checkPythonDump())
