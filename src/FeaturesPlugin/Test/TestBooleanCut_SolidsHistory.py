# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

# Test that the history of Cut operation works correctly after movement of groups after this Cut feature:
# cut tools are deleted, cut objects are correctly transformed to and divided if needed

from salome.shaper import model
from ModelAPI import *
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-10.57884322822754, 25.90966115484096, 9.63424990252037)
SketchLine_1 = Sketch_1.addLine(-18.29572175958338, 20.14177099321736, -2.288684850537554, 30.81802598002054)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchCircle_1.results()[1])
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_2 = Sketch_2.addLine(7.424373233594292, 20.04016872303595, -10.19950738916256, 28.60837438423646)
SketchLine_3 = Sketch_2.addLine(-10.19950738916256, 28.60837438423646, 10.7835313495703, 29.11992811073657)
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_2.addLine(10.7835313495703, 29.11992811073657, 7.424373233594292, 20.04016872303595)
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_2.startPoint(), SketchLine_4.endPoint())
SketchLine_5 = Sketch_2.addLine(-10.19950738916256, 28.60837438423646, 9.111243103183778, 24.59975538297626)
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1"), model.selection("COMPOUND", "all-in-Sketch_2")], model.selection(), 10, 0)
Group_1_objects = [model.selection("SOLID", "Extrusion_1_1_2"), model.selection("SOLID", "Extrusion_1_1_1"), model.selection("SOLID", "Extrusion_1_2_2"), model.selection("SOLID", "Extrusion_1_2_1")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
Group_2_objects = [model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_1"), model.selection("FACE", "Extrusion_1_1_2/To_Face"), model.selection("FACE", "Extrusion_1_2_2/To_Face")]
Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1_2")], [model.selection("SOLID", "Extrusion_1_2_2")])
model.do()
# move groups after the Cut
Part_1_doc.moveFeature(Group_1.feature(), Cut_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
model.end()

# check groups are correct
aFactory = ModelAPI_Session.get().validators()
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 2)
assert(aFactory.validate(Group_1.feature()))
for i in range(2):
  assert(Group_1.groupList().value(i).value().shapeType() == GeomAPI_Shape.SOLID)

selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 3)
assert(aFactory.validate(Group_2.feature()))
for i in range(3):
  assert(Group_2.groupList().value(i).value().shapeType() == GeomAPI_Shape.FACE)

assert(model.checkPythonDump())
