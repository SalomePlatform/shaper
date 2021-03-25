# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-1154.716981132076, -199.5283018867925, -849.0566037735849, 128.7735849056603)
SketchLine_2 = Sketch_1.addLine(-849.0566037735849, 128.7735849056603, -563.2075471698113, -69.33962264150945)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-563.2075471698113, -69.33962264150945, -70.75471698113215, 326.8867924528301)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
model.do()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Wire_1_objects = [model.selection("EDGE", "PartSet/Sketch_1/SketchLine_1"), model.selection("EDGE", "PartSet/Sketch_1/SketchLine_2"), model.selection("EDGE", "PartSet/Sketch_1/SketchLine_3")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects)
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], model.selection(), 100, 0)
Group_1_objects = [model.selection("FACE", "Extrusion_1_1/Generated_Face&PartSet/Sketch_1/SketchLine_1"), model.selection("FACE", "Extrusion_1_1/Generated_Face&PartSet/Sketch_1/SketchLine_2"), model.selection("FACE", "Extrusion_1_1/Generated_Face&PartSet/Sketch_1/SketchLine_3")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
model.do()
model.end()

groupFeature = Group_1.feature()
groupSelectionList = groupFeature.selectionList("group_list")
assert(groupSelectionList.size() == 3)
for index in range(0, groupSelectionList.size()):
  attrSelection = groupSelectionList.value(index)
  shape = attrSelection.value()
  name = attrSelection.namingName()
  assert(shape.isFace())
  assert(name != ""), "Empty shape name"
