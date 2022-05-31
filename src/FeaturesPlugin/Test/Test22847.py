# Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(22, 23, 60, 70)
SketchLine_2 = Sketch_1.addLine(60, 70, 90, 23)
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(90, 23, 22, 23)
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
Sketch_1.setHorizontal(SketchLine_3.result())
SketchLine_4 = Sketch_1.addLine(60, 72, 54, 23)
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_3.result())
Sketch_1.setCoincident(SketchLine_4.startPoint(), SketchLine_2.startPoint())
model.do()

Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection(), 10, 0, "Faces|Wires")
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_1_1")], model.selection("EDGE", "PartSet/OY"), 60, 2, model.selection("EDGE", "PartSet/OX"), 75, 2, keepSubResults = True)

### Create Group
Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "LinearCopy_1_1_1_2/MF:Translated&Sketch_1/SketchLine_2")])

### Create Remove_SubShapes
Remove_SubShapes_1 = model.addRemoveSubShapes(Part_1_doc, model.selection("COMPOUND", "LinearCopy_1_1"))
Remove_SubShapes_1.setSubShapesToRemove([model.selection("SOLID", "LinearCopy_1_1_4_2")])
model.end()

# Move the group to the end of history
model.begin()
Part_1_doc.moveFeature(Group_1.feature(), Remove_SubShapes_1.feature())
model.end()

# check that all groups are correct
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()

assert(aFactory.validate(Group_1.feature()))
assert(Group_1.groupList().size() == 1)
assert(Group_1.groupList().value(0).value().shapeTypeStr() == "FACE")
assert(len(Group_1.groupList().value(0).namingName()) > 0)

assert(model.checkPythonDump())
