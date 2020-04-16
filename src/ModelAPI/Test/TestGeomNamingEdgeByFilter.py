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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 15, 15, 15)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Bottom"), 5, True)
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Bottom"), 10, True)
Plane_6 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Back"), 5, True)
Plane_7 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Back"), 10, True)
Plane_8 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Left"), 5, True)
Plane_9 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Left"), 10, True)
Partition_1_objects = [model.selection("SOLID", "Box_1_1"), model.selection("FACE", "Plane_1"), model.selection("FACE", "Plane_2"), model.selection("FACE", "Plane_5"), model.selection("FACE", "Plane_6"), model.selection("FACE", "Plane_3"), model.selection("FACE", "Plane_4")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
Box_2 = model.addBox(Part_1_doc, 2, 2, 10)
Placement_1 = model.addPlacement(Part_1_doc, [model.selection("SOLID", "Box_2_1")], model.selection("FACE", "Box_2_1/Bottom"), model.selection("FACE", "_weak_name_4_Partition_1_1_14"), False, True)
model.do()
# update the selection of the placement
Placement_1.setEndShape(model.selection("EDGE", "Partition_1_1_5/Generated_Edge&Plane_3/Plane_3&Plane_2/Plane_2"))
model.end()

# check that placement is valid
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Placement_1.feature()))

assert(model.checkPythonDump(model.ModelHighAPI.CHECK_NAMING))
