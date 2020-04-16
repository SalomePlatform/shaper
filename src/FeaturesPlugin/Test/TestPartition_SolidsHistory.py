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

# Test that partition of solid works correctly after movement of groups after this partition

from salome.shaper import model
from ModelAPI import *
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-16.04556650246306, 45.15147783251232, -31.59359605911331, 45.15147783251232)
SketchLine_2 = Sketch_1.addLine(-31.59359605911331, 45.15147783251232, -31.59359605911331, 29.10591133004927)
SketchLine_3 = Sketch_1.addLine(-31.59359605911331, 29.10591133004927, -16.04556650246306, 29.10591133004927)
SketchLine_4 = Sketch_1.addLine(-16.04556650246306, 29.10591133004927, -16.04556650246306, 45.15147783251232)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(12.4384236453202, 13.06034482758621, -5.846059113300496, 13.06034482758621)
SketchLine_6 = Sketch_1.addLine(-5.846059113300496, 13.06034482758621, -5.846059113300496, -6.965517241379309)
SketchLine_7 = Sketch_1.addLine(-5.846059113300496, -6.965517241379309, 12.4384236453202, -6.965517241379309)
SketchLine_8 = Sketch_1.addLine(12.4384236453202, -6.965517241379309, 12.4384236453202, 13.06034482758621)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_8.result())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_2.addCircle(-10.81448298196523, 20.3003568426161, 15.58088381891068)
SketchLine_9 = Sketch_2.addLine(-19.86849367207068, 32.98061042849822, 0.2083042015703297, 9.288449501673265)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_9.startPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchCircle_1.results()[1])
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1"), model.selection("COMPOUND", "Sketch_2")], model.selection(), 10, 0)
Group_1 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_1_3_2")])
Group_2_objects = [model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4"), model.selection("FACE", "Extrusion_1_3_2/Generated_Face&Sketch_2/SketchCircle_1_2"), model.selection("FACE", "Extrusion_1_3_1/Generated_Face&Sketch_2/SketchCircle_1_2&weak_name_1"), model.selection("FACE", "Extrusion_1_2/To_Face")]
Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
Partition_1_objects = [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_1_2"), model.selection("COMPSOLID", "Extrusion_1_3")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
model.do()
# move groups after the partitiion
Part_1_doc.moveFeature(Group_1.feature(), Partition_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
model.end()

# check groups are correct
aFactory = ModelAPI_Session.get().validators()
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 5)
assert(aFactory.validate(Group_1.feature()))
for i in range(5):
  assert(Group_1.groupList().value(i).value().shapeType() == GeomAPI_Shape.SOLID)

selectionList = Group_2.feature().selectionList("group_list")
assert(selectionList.size() == 10)
assert(aFactory.validate(Group_2.feature()))
for i in range(10):
  assert(Group_2.groupList().value(i).value().shapeType() == GeomAPI_Shape.FACE)

assert(model.checkPythonDump())
