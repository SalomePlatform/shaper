# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

# Tests python API for the whole feature (all results) in group selection
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Extrusion_1 = model.addExtrusion(Part_1_doc, [], model.selection(), 10, 0)
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(0, 0, 10)
Extrusion_1.setNestedSketch(Sketch_1)
Group_1 = model.addGroup(Part_1_doc, "Vertices", [model.selection("COMPOUND", "all-in-Extrusion_1")])
Group_1.setName("CylVertices")
Group_1.result().setName("CylVertices")
Group_2 = model.addGroup(Part_1_doc, "Edges", [model.selection("COMPOUND", "all-in-Extrusion_1")])
Group_2.setName("CylEdges")
Group_2.result().setName("CylEdges")
Group_3 = model.addGroup(Part_1_doc, "Faces", [model.selection("COMPOUND", "all-in-Extrusion_1")])
Group_3.setName("CylFaces")
Group_3.result().setName("CylFaces")
Group_4 = model.addGroup(Part_1_doc, "Solids", [model.selection("COMPOUND", "all-in-Extrusion_1")])
Group_4.setName("CylSolid")
Group_4.result().setName("CylSolid")
model.end()

from GeomAPI import GeomAPI_ShapeIterator

# group variable, type of selection, number of expected sub-shapes, sub-shapes type
test_list = [(Group_1, "Vertices", 2, 7), (Group_2, "Edges", 3, 6), (Group_3, "Faces", 3, 4), (Group_4, "Solids", 1, 2)]
for test in test_list:
  assert(test[0].groupList().selectionType() == test[1])
  assert(len(test[0].results()) == 1)
  assert(test[0].result().shapeType() == "COMPOUND")

  aResultShape = test[0].feature().firstResult().shape()
  anIter = GeomAPI_ShapeIterator(aResultShape)
  aNum = 0
  while anIter.more():
    aShape = anIter.current()
    assert(aShape.shapeType() == test[3])
    aNum = aNum + 1
    anIter.next()

  assert(aNum == test[2])

assert(model.checkPythonDump())
