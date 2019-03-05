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
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(396.8373493975904, 115.9638554216867, -225.1506024096385, 115.9638554216867)
SketchLine_2 = Sketch_1.addLine(-225.1506024096385, 115.9638554216867, -225.1506024096385, -149.0963855421687)
SketchLine_3 = Sketch_1.addLine(-225.1506024096385, -149.0963855421687, 396.8373493975904, -149.0963855421687)
SketchLine_4 = Sketch_1.addLine(396.8373493975904, -149.0963855421687, 396.8373493975904, 115.9638554216867)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_2.addCircle(-348.644578313253, 155.1204819277109, 207.6894050099908)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection(), 100, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_1_2r")], model.selection(), 100, 0)
Group_1 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Extrusion_2_1")])
Group_2 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")])
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_2_1")])
model.end()

# move groups
model.begin()
Part_1_doc.moveFeature(Group_1.feature(), Partition_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
model.end()

# check each group contain two results: one is related to original body only, another - common part

aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Group_1.feature()))
assert(aFactory.validate(Group_2.feature()))
aList1 = Group_1.feature().selectionList("group_list")
aList2 = Group_2.feature().selectionList("group_list")
assert(aList1.size() == 2)
assert(aList2.size() == 2)
assert(aList1.value(0).value().shapeTypeStr() == "SOLID")
assert(aList1.value(1).value().shapeTypeStr() == "SOLID")
assert(aList2.value(0).value().shapeTypeStr() == "SOLID")
assert(aList2.value(1).value().shapeTypeStr() == "SOLID")
assert(aList1.value(0).value().isSame(aList2.value(0).value()) or aList1.value(1).value().isSame(aList2.value(0).value()) or
aList1.value(0).value().isSame(aList2.value(1).value()) or aList1.value(1).value().isSame(aList2.value(1).value()))
