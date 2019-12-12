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

# Test of move to the end and split of the group of whole feature selected and no new results appeared

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(45.19889502762433, 34.03867403314917, -47.9889502762431, 34.03867403314917)
SketchLine_2 = Sketch_1.addLine(-47.9889502762431, 34.03867403314917, -47.9889502762431, -32.64364640883979)
SketchLine_3 = Sketch_1.addLine(-47.9889502762431, -32.64364640883979, 45.19889502762433, -32.64364640883979)
SketchLine_4 = Sketch_1.addLine(45.19889502762433, -32.64364640883979, 45.19889502762433, 34.03867403314917)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchCircle_1 = Sketch_1.addCircle(-0.27900552486187, -3.9060773480663, 16.61196177134834)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f-SketchCircle_1_2r")])
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("FACE", "Face_1_1")], model.selection("EDGE", "PartSet/OX"), 100)
Recover_1 = model.addRecover(Part_1_doc, Translation_1, [Face_1.result()])
Symmetry_1 = model.addSymmetry(Part_1_doc, [model.selection("FACE", "Translation_1_1")], model.selection("EDGE", "PartSet/OZ"), True)
Extrusion_1_objects = [model.selection("FACE", "Recover_1_1"), model.selection("FACE", "Symmetry_1_1_1"), model.selection("FACE", "Symmetry_1_1_2")]
Extrusion_1 = model.addExtrusion(Part_1_doc, Extrusion_1_objects, model.selection(), 50, 0)
Group_3 = model.addGroup(Part_1_doc, "Edges", [model.selection("COMPOUND", "all-in-Extrusion_1")])
Group_5 = model.addGroup(Part_1_doc, "Faces", [model.selection("COMPOUND", "all-in-Extrusion_1")])
Group_6 = model.addGroup(Part_1_doc, "Solids", [model.selection("COMPOUND", "all-in-Extrusion_1")])
LinearCopy_1_objects = [model.selection("SOLID", "Extrusion_1_3"), model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_1_2")]
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, LinearCopy_1_objects, model.selection("EDGE", "PartSet/OY"), 80, 2, model.selection("EDGE", "PartSet/OZ"), 70, 3)
model.do()
Part_1_doc.moveFeature(Group_3.feature(), LinearCopy_1.feature(), True)
Part_1_doc.moveFeature(Group_5.feature(), LinearCopy_1.feature(), True)
Part_1_doc.moveFeature(Group_6.feature(), LinearCopy_1.feature(), True)
model.end()

assert(Part_1_doc.size("Groups") == 3) # equal to the number of original groups

# check names of results
from ModelAPI import *

aFactory = ModelAPI_Session.get().validators()
for group in [Group_3, Group_5, Group_6]:
  assert(aFactory.validate(group.feature()))
  selectionList = group.feature().selectionList("group_list")
  assert(selectionList.size() == 1)

assert(model.checkPythonDump())
