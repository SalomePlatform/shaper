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
Group_1_objects = [model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"), model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Left]")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
Group_2_objects = [model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]")]
Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
Group_3_objects = [model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]")]
Group_3 = model.addGroup(Part_1_doc, Group_3_objects)
Group_4_objects = [model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Bottom]"), model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Bottom]")]
Group_4 = model.addGroup(Part_1_doc, Group_4_objects)
GroupSubstraction_1 = model.addGroupSubstraction(Part_1_doc, [model.selection("COMPOUND", "Group_1"), model.selection("COMPOUND", "Group_2")], [model.selection("COMPOUND", "Group_3"), model.selection("COMPOUND", "Group_4")])
model.end()

from GeomAPI import *

model.testNbResults(GroupSubstraction_1, 1)
model.testNbSubResults(GroupSubstraction_1, [0])
model.testNbSubShapes(GroupSubstraction_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(GroupSubstraction_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(GroupSubstraction_1, GeomAPI_Shape.EDGE, [3])
model.testNbSubShapes(GroupSubstraction_1, GeomAPI_Shape.VERTEX, [6])

assert(model.checkPythonDump())
