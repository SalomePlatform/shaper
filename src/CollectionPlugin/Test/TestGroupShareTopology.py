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

from GeomAPI import *
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), 0, 3, [model.selection("SOLID", "Box_1_1")])
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Top"))
SketchCircle_1 = Sketch_1.addCircle(4.69238908106919, 4.989364948318125, 5.746414396356567)
ExtrusionCut_1.setNestedSketch(Sketch_1)
Group_1 = model.addGroup(Part_1_doc, [model.selection("FACE", "ExtrusionCut_1_1/Modified_Face&Box_1_1/Top&weak_name_1")])
model.do()
# enable geometrical selection mode => 4 faces must be selected as one compound
Group_1.groupList().setGeometricalSelection(True)
model.do()
assert(Group_1.groupList().size() == 1)
assert(Group_1.groupList().value(0).value().shapeType() == GeomAPI_Shape.COMPOUND)

# disable geometrical selection mode => 4 faces must be selected as 4 group elements
Group_1.groupList().setGeometricalSelection(False)
model.end()

assert(Group_1.groupList().size() == 4)
assert(Group_1.groupList().value(0).value().shapeType() == GeomAPI_Shape.FACE)
assert(Group_1.groupList().value(1).value().shapeType() == GeomAPI_Shape.FACE)
assert(Group_1.groupList().value(2).value().shapeType() == GeomAPI_Shape.FACE)
assert(Group_1.groupList().value(3).value().shapeType() == GeomAPI_Shape.FACE)
