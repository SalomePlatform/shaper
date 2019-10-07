# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

# Tests python API for the whole result in group selection

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-16, -4, -4, 29)
SketchLine_2 = Sketch_1.addLine(-4, 29, 21, -9)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(21, -9, -16, -4)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection(), 10, 0)
Group_1 = model.addGroup(Part_1_doc, "EDGE", [model.selection("SOLID", "Extrusion_1_1")])
model.end()

# check the group result: it must be compound of 9 edges
assert(Group_1.groupList().selectionType() == "EDGE")
assert(len(Group_1.results()) == 1)
assert(Group_1.result().shapeType() == "COMPOUND")

from GeomAPI import GeomAPI_ShapeIterator
aResultShape = Group_1.feature().firstResult().shape()
anIter = GeomAPI_ShapeIterator(aResultShape)
aNum = 0
while anIter.more():
  anEdge = anIter.current()
  assert(anEdge.isEdge())
  aNum = aNum + 1
  anIter.next()

assert(aNum == 9)

assert(model.checkPythonDump())
