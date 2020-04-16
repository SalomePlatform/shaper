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
Box_1 = model.addBox(Part_1_doc, 100, 100, 100)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], -50, -50, -50)
Partition_1_objects = [model.selection("SOLID", "Translation_1_1"), model.selection("FACE", "PartSet/XOY"), model.selection("FACE", "PartSet/YOZ"), model.selection("FACE", "PartSet/XOZ")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects, keepSubResults = True)
CompSolid_1_objects = [model.selection("SOLID", "Partition_1_1_1"), model.selection("SOLID", "Partition_1_1_5"), model.selection("SOLID", "Partition_1_1_6"), model.selection("SOLID", "Partition_1_1_8")]
CompSolid_1 = model.addCompSolid(Part_1_doc, CompSolid_1_objects)
Recover_1 = model.addRecover(Part_1_doc, CompSolid_1, [Partition_1.result()], True)
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Part_1, 1)
model.testNbSubResults(Part_1, [0])
model.testNbSubShapes(Part_1, GeomAPI_Shape.SOLID, [12])
model.testNbSubShapes(Part_1, GeomAPI_Shape.FACE, [72])
model.testNbSubShapes(Part_1, GeomAPI_Shape.EDGE, [288])
model.testNbSubShapes(Part_1, GeomAPI_Shape.VERTEX, [576])
model.testResultsVolumes(Part_1, [1500000])

assert(model.checkPythonDump())
