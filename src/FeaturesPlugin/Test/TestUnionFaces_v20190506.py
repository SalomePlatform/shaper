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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-5.197255574614073, 0.1732418524871273, 67.586529314451)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchCircle_1_2")])
Partition_1_objects = [model.selection("FACE", "Face_1_1"), model.selection("FACE", "PartSet/YOZ"), model.selection("FACE", "PartSet/XOZ")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
Union_1 = model.addUnion(Part_1_doc, [model.selection("FACE", "Partition_1_1_1"), model.selection("FACE", "Partition_1_1_2")], keepSubResults = True)
model.do()
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Union_1, 1)
model.testNbSubResults(Union_1, [3])
model.testNbSubShapes(Union_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Union_1, GeomAPI_Shape.FACE, [3])
model.testNbSubShapes(Union_1, GeomAPI_Shape.EDGE, [10])
model.testNbSubShapes(Union_1, GeomAPI_Shape.VERTEX, [20])

assert(model.checkPythonDump())
