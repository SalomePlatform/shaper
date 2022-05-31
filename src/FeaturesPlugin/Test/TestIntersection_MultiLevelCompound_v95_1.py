# Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "PartSet/OX"), 20, 2, keepSubResults = True)
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 5)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), model.selection("EDGE", "PartSet/OX"), 30)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1"), model.selection("FACE", "Plane_1")], keepSubResults = True)
Intersection_1 = model.addIntersection(Part_1_doc, [model.selection("SOLID", "LinearCopy_1_1_1"), model.selection("SOLID", "Partition_1_1_1")], keepSubResults = True)
model.end()

from GeomAPI import *

model.testNbResults(Intersection_1, 1)
model.testNbSubResults(Intersection_1, [2])
model.testNbSubShapes(Intersection_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Intersection_1, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Intersection_1, GeomAPI_Shape.EDGE, [30])
model.testNbSubShapes(Intersection_1, GeomAPI_Shape.VERTEX, [60])
model.testResultsVolumes(Intersection_1, [1000])

assert(model.checkPythonDump())
