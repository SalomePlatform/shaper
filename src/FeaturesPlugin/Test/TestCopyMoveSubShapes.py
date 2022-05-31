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

# Checks selection of the sub-shapes move to the end with combination of 2 Copy features in history

from salome.shaper import model
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Group_1 = model.addGroup(Part_1_doc, "Edges", [model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]")])
Copy_1 = model.addCopy(Part_1_doc, [model.selection("FACE", "Box_1_1/Front")], 2)
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), 0, 5, [model.selection("SOLID", "Box_1_1")])
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Front"))
SketchCircle_1 = Sketch_1.addCircle(9.650212071680357, 9.344990586582618, 1.565166813054581)
ExtrusionCut_1.setNestedSketch(Sketch_1)
ExtrusionCut_1.result().setColor(225, 0, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1_1"))
SketchCircle_2 = Sketch_2.addCircle(0.7603686814133139, 9.06793355634084, 1.630854194501576)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2r")])
Cut_1 = model.addCut(Part_1_doc, [model.selection("FACE", "Box_1_1_1")], [model.selection("FACE", "Face_1_1")], keepSubResults = True)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("FACE", "Box_1_1_2")], model.selection("EDGE", "PartSet/OX"), 5)
Copy_2 = model.addCopy(Part_1_doc, [model.selection("FACE", "Box_1_1_2")], 2)
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("FACE", "Box_1_1_2_1")], model.selection("EDGE", "PartSet/OZ"), 10)
Rotation_2 = model.addRotation(Part_1_doc, [model.selection("FACE", "Box_1_1_2_2")], model.selection("EDGE", "PartSet/OZ"), 20)
model.do()
# move the group feature to the end - through 2 copies and many modifications of the selected edge
Part_1_doc.moveFeature(Group_1.feature(), Rotation_2.feature())
model.end()

# result is 5 edges: cut, translation, 2 rotations, extrusion cut
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Group_1.feature()))
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 5)

assert(model.checkPythonDump())
