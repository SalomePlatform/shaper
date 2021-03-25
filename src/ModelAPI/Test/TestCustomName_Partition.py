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
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Cylinder_1.result().setName("cylinder")
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_1.result().setName("box")
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "box"), model.selection("SOLID", "cylinder")])
model.do()

PartitionResult = Partition_1.result()
PartitionResultName = PartitionResult.name()
assert(PartitionResultName == Box_1.result().name()), "Partition name '{}' != '{}'".format(PartitionResultName, Box_1.result().name())
# check sub-result names
PartitionName = Partition_1.name() + "_1"
for i in range(0, PartitionResult.numberOfSubs()):
  refName = PartitionName + '_' + str(i + 1)
  subResult = PartitionResult.subResult(i)
  assert(subResult.name() == refName), "Partition sub-result {} name '{}' != '{}'".format(i, subResult.name(), refName)

model.end()

assert(model.checkPythonDump())
