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

# Test of movement of the group of whole result with different results shape types: only one shape type must be in the moved group
from salome.shaper import model
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(17.20575221238938, 2.793141592920342, 8.375699762004958)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_2.addLine(24.25155410709516, 6.639479956172791, 20.59251568321609, -12.31511385947313)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")])
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchLine_1")])
Group_1 = model.addGroup(Part_1_doc, "Edges", [model.selection("EDGE", "Edge_1_1")])
Partition_1 = model.addPartition(Part_1_doc, [model.selection("FACE", "Face_1_1"), model.selection("EDGE", "Edge_1_1")], keepSubResults = True)
model.do()
# move group after the partition
Part_1_doc.moveFeature(Group_1.feature(), Partition_1.feature())
model.end()

# check that only two edges are in the group result, no face
assert(len(Group_1.feature().results())==1)
model.testNbSubShapes(Group_1, GeomAPI_Shape.EDGE, [2])

assert(model.checkPythonDump())
