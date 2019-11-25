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
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Left"), 5, True)
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Front"), 5, True)
Partition_1_objects = [model.selection("FACE", "Plane_1"), model.selection("FACE", "Plane_2"), model.selection("SOLID", "Box_1_1")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
Union_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Partition_1_1_3"), model.selection("SOLID", "Partition_1_1_1")], False, 20190506)
Union_2 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Fuse_1_1_2"), model.selection("SOLID", "Fuse_1_1_1")], False, 20190506)
model.do()
model.end()

from GeomAPI import GeomAPI_Shape
assert(Part_1_doc.size("Bodies") == 1)
model.testNbSubShapes(Union_2, GeomAPI_Shape.SOLID, [2])

assert(model.checkPythonDump())
