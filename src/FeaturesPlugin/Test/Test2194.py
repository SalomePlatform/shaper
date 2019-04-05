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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchCircle_1 = Sketch_1.addCircle(-79.57211859899775, 93.0358363165245, 69.46562387481423)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection(), 10, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/To_Face"))
SketchCircle_2 = Sketch_2.addCircle(-111.8968837135755, 98.96901887198163, 6.57145775571253)
SketchCircle_3 = Sketch_2.addCircle(-41.97827733564338, 89.35676702647821, 6.102067916018811)
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2r"), model.selection("FACE", "Sketch_2/Face-SketchCircle_3_2f")], model.selection(), 10, 0)
Smash_1 = model.addSmash(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], [model.selection("SOLID", "Extrusion_2_1"), model.selection("SOLID", "Extrusion_2_2")])
Group_1_objects = [model.selection("FACE", "Extrusion_2_2/To_Face"), model.selection("FACE", "Extrusion_2_2/From_Face"), model.selection("FACE", "Extrusion_2_2/Generated_Face&Sketch_2/SketchCircle_3_2"), model.selection("FACE", "Extrusion_2_1/From_Face"), model.selection("FACE", "(Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2)(Extrusion_2_1/From_Face)(Extrusion_2_2/From_Face)_Smash_1_1_1"), model.selection("FACE", "Extrusion_1_1/From_Face"), model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2"), model.selection("FACE", "Extrusion_2_2/From_Face"), model.selection("FACE", "Extrusion_2_1/Generated_Face&Sketch_2/SketchCircle_2_2"), model.selection("FACE", "Extrusion_2_1/From_Face"), model.selection("FACE", "Extrusion_2_1/To_Face")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
model.do()
model.end()

groupFeature = Group_1.feature()
groupSelectionList = groupFeature.selectionList("group_list")
assert(groupSelectionList.size() == 11)
for index in range(0, groupSelectionList.size()):
  attrSelection = groupSelectionList.value(index)
  shape = attrSelection.value()
  name = attrSelection.namingName()
  assert(shape.isFace())
  assert(name != ""), "Empty shape name"

assert(model.checkPythonDump())
