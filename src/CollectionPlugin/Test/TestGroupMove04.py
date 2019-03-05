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

# Test of deep nested results history. Cylinders divided to two by cut, then each divided to
# two by partition by plane, then resulting compsolids are collected in compound.
# Checking that group on initial extrusion moved to the end contains the corresponding
# results, but divided.

from salome.shaper import model
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-4.602216748768477, 10.94581280788177, 9.660420057801511)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection(), 10, 0)
Group_1 = model.addGroup(Part_1_doc, [model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2")])
Group_2 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")])
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/YOZ"), 1, True)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchCircle_2 = Sketch_2.addCircle(-5.643073116097736, 11.91382008305256, 15.03576198961618)
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2r")], model.selection(), 2, -4)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], [model.selection("SOLID", "Extrusion_2_1")])
Partition_1 = model.addPartition(Part_1_doc, [model.selection("COMPOUND", "Cut_1_1"), model.selection("FACE", "Plane_1")])
Compound_1 = model.addCompound(Part_1_doc, [model.selection("COMPSOLID", "Partition_1_1"), model.selection("COMPSOLID", "Partition_1_2")])
model.do()
# move groups to the end
Part_1_doc.moveFeature(Group_1.feature(), Compound_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
model.end()

aFactory = ModelAPI_Session.get().validators()
# check group 1: cylindical face is divided to 6 (because of seam edge)
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 6)
assert(aFactory.validate(Group_1.feature()))

# check group 2: solid is divided to 4 solids
selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 4)
assert(aFactory.validate(Group_2.feature()))

assert(model.checkPythonDump())
