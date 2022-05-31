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

# Test of move to the end and split on LinearCopy: check that order in result is correct

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "d", "8")
model.addParameter(Part_1_doc, "nb", "3")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-34, 28, 1)
SketchCircle_2 = Sketch_1.addCircle(-34, 28, 3)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchCircle_2.center())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchCircle_2_2f-SketchCircle_1_2r_wire_2")], model.selection(), 2, -1)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchCircle_2_2f-SketchCircle_1_2r_wire")], model.selection(), 5, 0)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_1"), model.selection("COMPOUND", "all-in-Extrusion_2")])
Group_1 = model.addGroup(Part_1_doc, "Solids", [model.selection("SOLID", "Partition_1_1_1")])
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("COMPSOLID", "Partition_1_1")], model.selection("EDGE", "PartSet/OX"), "d", "nb", model.selection("EDGE", "PartSet/OY"), "d", "nb")
model.do()
Part_1_doc.moveFeature(Group_1.feature(), LinearCopy_1.feature(), True)
model.end()

for a in range(9):
  aGroup = Part_1_doc.objectByName("Features", "Group_1_" + str(a + 1))
  aSelName = aGroup.data().selectionList("group_list").value(0).context().data().name()
  assert(aSelName == "LinearCopy_1_1_" + str(a + 1) + "_1")
