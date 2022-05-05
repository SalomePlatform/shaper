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

from SketchAPI import *
from ModelAPI import *
from GeomAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "l", "200")
model.addParameter(Part_1_doc, "b", "100")
model.addParameter(Part_1_doc, "h", "25")
model.addParameter(Part_1_doc, "r", "25")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(200, 0, 0, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 100)
SketchLine_3 = Sketch_1.addLine(0, 100, 200, 100)
SketchLine_4 = Sketch_1.addLine(200, 100, 200, 0)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_1.setName("SketchConstraintCoincidence_2")
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_2.setName("SketchConstraintCoincidence_3")
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3.setName("SketchConstraintCoincidence_4")
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4.setName("SketchConstraintCoincidence_5")
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), "l")
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_2.result(), "b")
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchAPI_Point(SketchPoint_1).coordinates())
SketchConstraintCoincidence_5.setName("SketchConstraintCoincidence_6")
SketchCircle_1 = Sketch_1.addCircle(50, 50, 25)
SketchCircle_2 = Sketch_1.addCircle(150, 50, 25)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], "r")
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchCircle_1.results()[1], SketchCircle_2.results()[1])
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_2.result(), "l/4", True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_1.result(), "b/2", True)
SketchLine_5 = Sketch_1.addLine(50, 50, 150, 50)
SketchLine_5.setAuxiliary(True)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6.setName("SketchConstraintCoincidence_7")
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchCircle_2.center(), SketchLine_5.endPoint())
SketchConstraintCoincidence_7.setName("SketchConstraintCoincidence_8")
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_5.endPoint(), SketchLine_4.result(), "l/4", True)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), "h", 0)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/YOZ"), 100, False)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_1_1"), model.selection("FACE", "Plane_1")])
Group_1 = model.addGroup(Part_1_doc, [model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_2")])
Group_1.setName("Inlet")
Group_1.result().setName("Inlet")
Group_2 = model.addGroup(Part_1_doc, [model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_4")])
Group_2.setName("Outlet")
Group_2.result().setName("Outlet")
Group_3_objects = [model.selection("FACE", "Partition_1_1_1/Modified_Face&Extrusion_1_1_1/To_Face"), model.selection("FACE", "Partition_1_1_1/Modified_Face&Sketch_1/SketchLine_1"), model.selection("FACE", "Partition_1_1_1/Modified_Face&Extrusion_1_1_1/From_Face"), model.selection("FACE", "Partition_1_1_1/Modified_Face&Sketch_1/SketchLine_3"), model.selection("FACE", "Partition_1_1_2/Modified_Face&Sketch_1/SketchLine_3"), model.selection("FACE", "Partition_1_1_2/Modified_Face&Extrusion_1_1_1/From_Face"), model.selection("FACE", "Partition_1_1_2/Modified_Face&Sketch_1/SketchLine_1"), model.selection("FACE", "Partition_1_1_2/Modified_Face&Extrusion_1_1_1/To_Face")]
Group_3 = model.addGroup(Part_1_doc, Group_3_objects)
Group_3.setName("Wall")
Group_3.result().setName("Wall")
Group_4 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Partition_1_1_3"), model.selection("SOLID", "Partition_1_1_4")])
Group_4.setName("Cylinders")
Group_4.result().setName("Cylinders")
Group_4.result().setColor(170, 0, 0)
Group_5 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Partition_1_1_1"), model.selection("SOLID", "Partition_1_1_2")])
Group_5.setName("Fluide")
Group_5.result().setName("Fluide")
Group_5.result().setColor(0, 0, 255)
Group_6 = model.addGroup(Part_1_doc, [model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2"), model.selection("FACE", "Extrusion_1_1_3/Generated_Face&Sketch_1/SketchCircle_2_2")])
Group_6.setName("Faces_cyl")
Group_6.result().setName("Faces_cyl")
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), 10, False)
Partition_2 = model.addPartition(Part_1_doc, [model.selection("COMPSOLID", "Partition_1_1"), model.selection("FACE", "Plane_2")])
model.do()
# move groups after the latest partitiion
Part_1_doc.moveFeature(Group_1.feature(), Partition_2.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
Part_1_doc.moveFeature(Group_3.feature(), Group_2.feature())
Part_1_doc.moveFeature(Group_4.feature(), Group_3.feature())
Part_1_doc.moveFeature(Group_5.feature(), Group_4.feature())
Part_1_doc.moveFeature(Group_6.feature(), Group_5.feature())
model.end()

# check groups are correct: group name, number of elements, elements type
Group_Info = [
(Group_1, 2, GeomAPI_Shape.FACE), # Inlet
(Group_2, 2, GeomAPI_Shape.FACE), # Outlet
(Group_3, 12, GeomAPI_Shape.FACE), # Wall
(Group_4, 4, GeomAPI_Shape.SOLID), # Cylinders
(Group_5, 4, GeomAPI_Shape.SOLID), # Fluide
(Group_6, 4, GeomAPI_Shape.FACE)] # Faces_cyl

aFactory = ModelAPI_Session.get().validators()
for group in Group_Info:
  selectionList = group[0].feature().selectionList("group_list")
  assert(selectionList.size() == group[1])
  assert(aFactory.validate(group[0].feature()))
  for i in range(group[1]):
    assert(group[0].groupList().value(i).value().shapeType() == group[2])

assert(model.checkPythonDump())
