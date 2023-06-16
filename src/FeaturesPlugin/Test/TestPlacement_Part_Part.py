# Copyright (C) 2018-2023  CEA/DEN, EDF R&D
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
model.do()
Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Cylinder_1 = model.addCylinder(Part_2_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
model.do()
Placement_1 = model.addPlacement(partSet, [model.selection("COMPOUND", "Part_2/")], model.selection("FACE", "Part_2/Cylinder_1_1/Face_2"), model.selection("FACE", "Part_1/Box_1_1/Front"), False, True)
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Placement_1, 1)
model.testNbSubResults(Placement_1, [0])
model.testNbSubShapes(Placement_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Placement_1, GeomAPI_Shape.FACE, [3])
model.testNbSubShapes(Placement_1, GeomAPI_Shape.EDGE, [6])
model.testNbSubShapes(Placement_1, GeomAPI_Shape.VERTEX, [12])
model.testResultsVolumes(Placement_1, [785.398163397448342948337085545])
