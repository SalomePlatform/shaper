# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

# Checks selection of the whole shape and move to the end of the simple copy wihtout the
# next modifications applied.

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(38.31034482758622, 31.49775664633739, 7.836206896551726, 31.49775664633739)
SketchLine_2 = Sketch_1.addLine(7.836206896551726, 31.49775664633739, 7.836206896551726, 8.984209848307833)
SketchLine_3 = Sketch_1.addLine(7.836206896551726, 8.984209848307833, 38.31034482758622, 8.984209848307833)
SketchLine_4 = Sketch_1.addLine(38.31034482758622, 8.984209848307833, 38.31034482758622, 31.49775664633739)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(21.64285714285715, 31.4977566463374, 25.0012315270936, 8.984209848307833)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_1.result())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_3.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_5r"), model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_5f-SketchLine_3f-SketchLine_4f")], model.selection(), 7, 0)
Extrusion_1.result().setName("Origin")
Group_1 = model.addGroup(Part_1_doc, "Solids", [model.selection("SOLID", "Extrusion_1_1_1")])
Copy_1 = model.addCopy(Part_1_doc, [model.selection("COMPSOLID", "Origin")], 1)
Copy_1.result().setName("Origin_1")
Fillet_1 = model.addFillet(Part_1_doc, [model.selection("EDGE", "[Origin_1_1/Copy_2][Origin_1_1/Copy_6]"), model.selection("EDGE", "[Origin_1_2/Copy_4][Origin_1_2/Copy_6]")], 2)
Fillet_1.result().setName("CopyCompound")
model.do()
# move the group feature to the end - through copy and fillet on this copy (to distinguish the origin and the copy)
Part_1_doc.moveFeature(Group_1.feature(), Fillet_1.feature())
model.end()

from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Group_1.feature()))
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 2) # still the same solid + the copied and filleted

assert(selectionList.value(0).namingName() == "Extrusion_1_1_1")
assert(selectionList.value(1).namingName() == "Fillet_1_1_1")

assert(model.checkPythonDump())
