# Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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
Symmetry_1 = model.addSymmetry(partSet, [model.selection("COMPOUND", "Part_1/")], model.selection("VERTEX", "Part_1/[Box_1_1/Back][Box_1_1/Left][Box_1_1/Bottom]"), False)
Symmetry_2 = model.addSymmetry(partSet, [model.selection("COMPOUND", "Symmetry_1/")], model.selection("EDGE", "Symmetry_1/[Box_1_1/Back][Box_1_1/Left]"), False)
Symmetry_3 = model.addSymmetry(partSet, [model.selection("COMPOUND", "Symmetry_2/")], model.selection("FACE", "Symmetry_2/Box_1_1/Bottom"), True)
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Symmetry_3, 2)
model.testNbSubResults(Symmetry_3, [0, 0])
model.testNbSubShapes(Symmetry_3, GeomAPI_Shape.SOLID, [1, 1])
model.testNbSubShapes(Symmetry_3, GeomAPI_Shape.FACE, [6, 6])
model.testNbSubShapes(Symmetry_3, GeomAPI_Shape.EDGE, [24, 24])
model.testNbSubShapes(Symmetry_3, GeomAPI_Shape.VERTEX, [48, 48])
model.testResultsVolumes(Symmetry_3, [1000, 1000])
