## Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Front"), 5, True)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("FACE", "Plane_1")])
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], 3, 3, 3)
Common_1 = model.addCommon(Part_1_doc, [model.selection("SOLID", "Partition_1_1_2")], [model.selection("SOLID", "Translation_1_1")])
model.do()

model.testHaveNamingSubshapes(Common_1, model, Part_1_doc)
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Common_1, 1)
model.testNbSubResults(Common_1, [2])
model.testNbSubShapes(Common_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Common_1, GeomAPI_Shape.FACE, [13])
model.testNbSubShapes(Common_1, GeomAPI_Shape.EDGE, [56])
model.testNbSubShapes(Common_1, GeomAPI_Shape.VERTEX, [112])
model.testResultsVolumes(Common_1, [745])

assert(model.checkPythonDump())
