# Copyright (C) 2021-2022  CEA/DEN, EDF R&D
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
from GeomAPI import GeomAPI_Shape

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

Point_1 = model.addPoint(Part_1_doc, 0, 0, 0)
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Point_1")])
Point_2 = model.addPoint(Part_1_doc, 1, 0, 0)
Vertex_2 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Point_2")])
model.do()

Partition_1_objects = [model.selection("VERTEX", "Vertex_1_1"), model.selection("VERTEX", "Vertex_2_1")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects, keepSubResults = True)
model.end()

model.testNbResults(Partition_1, 1)
model.testNbSubResults(Partition_1, [2])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.VERTEX, [2])

model.undo()
model.begin()
Fuse_1_objects = [model.selection("VERTEX", "Vertex_1_1"), model.selection("VERTEX", "Vertex_2_1")]
Fuse_1 = model.addFuse(Part_1_doc, Fuse_1_objects, True, keepSubResults = True)
model.end()

model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [2])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [2])

model.undo()
model.begin()
LimitTolerance_1 = model.addLimitTolerance(Part_1_doc, model.selection("VERTEX", "Vertex_1_1"), 0.6)
LimitTolerance_2 = model.addLimitTolerance(Part_1_doc, model.selection("VERTEX", "Vertex_2_1"), 0.6)
model.do()

Partition_2_objects = [model.selection("VERTEX", "LimitTolerance_1_1"), model.selection("VERTEX", "LimitTolerance_2_1")]
Partition_2 = model.addPartition(Part_1_doc, Partition_2_objects, keepSubResults = True)
model.end()

model.testNbResults(Partition_2, 1)
model.testNbSubResults(Partition_2, [0])
model.testNbSubShapes(Partition_2, GeomAPI_Shape.VERTEX, [1])

model.undo()
model.begin()
Fuse_2_objects = [model.selection("VERTEX", "LimitTolerance_1_1"), model.selection("VERTEX", "LimitTolerance_2_1")]
Fuse_2 = model.addFuse(Part_1_doc, Fuse_2_objects, True, keepSubResults = True)
model.end()

model.testNbResults(Fuse_2, 1)
model.testNbSubResults(Fuse_2, [0])
model.testNbSubShapes(Fuse_2, GeomAPI_Shape.VERTEX, [1])
