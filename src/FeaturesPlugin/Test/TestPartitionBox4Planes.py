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
Box_1 = model.addBox(Part_1_doc, 100, 100, 400)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), 100, False)
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "Plane_1"), 100, False)
Plane_6 = model.addPlane(Part_1_doc, model.selection("FACE", "Plane_2"), 100, False)
Plane_7 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/YOZ"), 50, False)
Partition_1_objects = [model.selection("FACE", "Plane_1"), model.selection("FACE", "Plane_2"), model.selection("FACE", "Plane_3"), model.selection("FACE", "Plane_4"), model.selection("SOLID", "Box_1_1")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
model.do()

model.checkResult(Partition_1,model,1,[8],[8],[48],[192],[384])
model.testHaveNamingSubshapes(Partition_1,model,Part_1_doc)

model.end()

assert(model.checkPythonDump())
