# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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
LinearCopy_1 = model.addMultiTranslation(partSet, [model.selection("COMPOUND", "Part_1/")], model.selection("EDGE", "OZ"), 20, 2)
LinearCopy_2 = model.addMultiTranslation(partSet, [model.selection("COMPOUND", "LinearCopy_1/")], model.selection("EDGE", "OX"), 20, 2, model.selection("EDGE", "OY"), 20, 2)
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(LinearCopy_1, 2)
model.testNbSubResults(LinearCopy_1, [0, 0])
model.testNbSubShapes(LinearCopy_1, GeomAPI_Shape.SOLID, [1, 1])
model.testNbSubShapes(LinearCopy_1, GeomAPI_Shape.FACE, [6, 6])
model.testNbSubShapes(LinearCopy_1, GeomAPI_Shape.EDGE, [24, 24])
model.testNbSubShapes(LinearCopy_1, GeomAPI_Shape.VERTEX, [48, 48])
model.testResultsVolumes(LinearCopy_1, [1000, 1000])

model.testNbResults(LinearCopy_2, 4)
model.testNbSubResults(LinearCopy_2, [0, 0, 0, 0])
model.testNbSubShapes(LinearCopy_2, GeomAPI_Shape.SOLID, [1, 1, 1, 1])
model.testNbSubShapes(LinearCopy_2, GeomAPI_Shape.FACE, [6, 6, 6, 6])
model.testNbSubShapes(LinearCopy_2, GeomAPI_Shape.EDGE, [24, 24, 24, 24])
model.testNbSubShapes(LinearCopy_2, GeomAPI_Shape.VERTEX, [48, 48, 48, 48])
model.testResultsVolumes(LinearCopy_2, [1000, 1000, 1000, 1000])
