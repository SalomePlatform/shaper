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
from GeomAlgoAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "d", "8")
model.addParameter(Part_1_doc, "nb", "3")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-34.12026714434272, 28.71281409244911, 1.619211673722419)
SketchCircle_2 = Sketch_1.addCircle(-34.12026714434272, 28.71281409244911, 3.180589706721861)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchCircle_2.center())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE_2", (-35.73947881806514, 28.71281409244911, 0))], model.selection(), 2, -1)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", (-37.30085685106458, 28.71281409244911, 0))], model.selection(), 5, 0)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_1"), model.selection("COMPOUND", "all-in-Extrusion_2")])
Group_1 = model.addGroup(Part_1_doc, [model.selection("SOLID", (-34.12026714434272, 28.71281409244911, 1.5))])
Group_1.setName("Group_dechets")
Group_1.result().setName("Group_dechets")
Group_2 = model.addGroup(Part_1_doc, [model.selection("SOLID", (-34.12026714434272, 28.71281409244911, 2.554668601838412))])
Group_2.setName("Group_colis")
Group_2.result().setName("Group_colis")
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("COMPSOLID", (-34.12026714434272, 28.71281409244911, 2.5))], model.selection("EDGE", "PartSet/OX"), "d", "nb", model.selection("EDGE", "PartSet/OY"), "d", "nb")
model.do()
# move groups to the end
Part_1_doc.moveFeature(Group_1.feature(), LinearCopy_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
model.end()

from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()

# number of solids should be multiplyed by linear copy
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 9)
assert(aFactory.validate(Group_1.feature()))
# checkthat solids in the first group are small, like the origin
for i in range(9):
  delta = GeomAlgoAPI_ShapeTools_volume(selectionList.value(i).value()) - 8.236773528312948
  assert(fabs(delta) < 1.e-5)

# same-plane faces should be merged
selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 9)
assert(aFactory.validate(Group_2.feature()))

# checkthat solids in the secong group are big, like the origin
for i in range(9):
  delta = GeomAlgoAPI_ShapeTools_volume(selectionList.value(i).value()) - 150.66735294710597
  assert(fabs(delta) < 1.e-5)

assert(model.checkPythonDump())
