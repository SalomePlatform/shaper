# Copyright (C) 2020-2021  CEA/DEN, EDF R&D
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

from GeomAPI import *
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "h", "1")
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 0.5, "h")
Cylinder_2 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 0.1, "h")
Cylinder_3 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 0.4, "h")
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), 0.1, False)
Partition_1_objects = [model.selection("SOLID", "Cylinder_1_1"), model.selection("SOLID", "Cylinder_2_1"), model.selection("SOLID", "Cylinder_3_1"), model.selection("FACE", "Plane_1"), model.selection("FACE", "PartSet/YOZ"), model.selection("FACE", "PartSet/XOZ")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects, keepSubResults = True)
Box_1 = model.addBox(Part_1_doc, 0.7, 0.1, 0.05)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], 0, -0.05, "0.75*h")
Recover_1 = model.addRecover(Part_1_doc, Partition_1, [Cylinder_1.result()])
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Translation_1_1")], [model.selection("SOLID", "Recover_1_1")], keepSubResults = True)
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Partition_1_1_14/Modified_Face&PartSet/XOZ/XOZ")])
Recover_2 = model.addRecover(Part_1_doc, Face_1, [Partition_1.result()], True)
Partition_2 = model.addPartition(Part_1_doc, [model.selection("COMPSOLID", "Recover_2_1"), model.selection("SOLID", "Cut_1_1")], keepSubResults = True)
model.end()

Fuse_objects = [model.selection("SOLID", "Partition_2_1_14"), model.selection("SOLID", "Partition_2_1_25"), model.selection("SOLID", "Partition_2_1_24")]


import time

tStart = time.time()

model.begin()
Union_1 = model.addUnion(Part_1_doc, Fuse_objects, keepSubResults = True)
model.do()

tUnion = time.time() - tStart

model.testNbResults(Union_1, 1)
model.testNbSubResults(Union_1, [23])
model.testNbSubShapes(Union_1, GeomAPI_Shape.SOLID, [23])
model.testNbSubShapes(Union_1, GeomAPI_Shape.FACE, [139])
model.testNbSubShapes(Union_1, GeomAPI_Shape.EDGE, [550])
model.testNbSubShapes(Union_1, GeomAPI_Shape.VERTEX, [1100])
model.testResultsVolumes(Union_1, [0.78640233633346512398532])

model.begin()
Part_1_doc.removeFeature(Union_1.feature())
model.end()

tStart = time.time()

model.begin()
Fuse_1 = model.addFuse(Part_1_doc, Fuse_objects, keepSubResults = True)
model.do()

tFuse = time.time() - tStart

model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [23])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.SOLID, [23])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.FACE, [139])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.EDGE, [550])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [1100])
model.testResultsVolumes(Fuse_1, [0.78640233633346512398532])

print("Elapsed time of Union: {}".format(tUnion))
print("Elapsed time of Fuse: {}".format(tFuse))

assert(tFuse <= tUnion * 1.5)
