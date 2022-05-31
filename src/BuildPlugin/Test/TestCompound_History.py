# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

# Test that the history of compound works correctly after movement of groups after this compound feature

from salome.shaper import model
from ModelAPI import *
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchPoint_1 = Sketch_1.addPoint(-19.03078817733991, 40.92241379310347)
SketchLine_1 = Sketch_1.addLine(-0.7463054187192111, 38.55911330049261, -18.03571428571429, 28.48399014778325)
SketchCircle_1 = Sketch_1.addCircle(-2.238916256157633, 23.13546798029557, 5.523556488740459)
model.do()
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_1/SketchPoint_1")])
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1")])
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f")], model.selection(), 10, 0)
Group_1_objects = [model.selection("VERTEX", "Vertex_1_1"), model.selection("VERTEX", "Edge_1_1/Modified_Vertex&Sketch_1/SketchLine_1_EndVertex"), model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1/To_Face]")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
Group_2 = model.addGroup(Part_1_doc, [model.selection("EDGE", "Edge_1_1"), model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1/To_Face]")])
Group_3 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")])
Compound_1_objects = [model.selection("VERTEX", "Vertex_1_1"), model.selection("EDGE", "Edge_1_1"), model.selection("SOLID", "Extrusion_1_1")]
Compound_1 = model.addCompound(Part_1_doc, Compound_1_objects)
model.do()
# move groups after the compound
Part_1_doc.moveFeature(Group_1.feature(), Compound_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
Part_1_doc.moveFeature(Group_3.feature(), Group_2.feature())
model.end()

# check groups are correct
aFactory = ModelAPI_Session.get().validators()
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 3)
assert(aFactory.validate(Group_1.feature()))
for i in range(3):
  assert(Group_1.groupList().value(i).value().shapeType() == GeomAPI_Shape.VERTEX)

selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 2)
assert(aFactory.validate(Group_2.feature()))
for i in range(2):
  assert(Group_2.groupList().value(i).value().shapeType() == GeomAPI_Shape.EDGE)

selectionList = Group_3.feature().selectionList("group_list")
assert(selectionList.size() == 1)
assert(aFactory.validate(Group_3.feature()))
assert(Group_3.groupList().value(0).value().shapeType() == GeomAPI_Shape.SOLID)

assert(model.checkPythonDump())
