# Copyright (C) 2014-2026  CEA/DEN, EDF R&D
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

# Check the Move to the end and split feature after a Fuse feature; should create two groups of faces

from salome.shaper import model
from ModelAPI import *
from SketchAPI import *

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "L", '15')
model.addParameter(Part_1_doc, "l", '5')

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(7.5, -1.25, -7.5, -1.25)

### Create SketchLine
SketchLine_2 = Sketch_1.addLine(-7.5, -1.25, -7.5, 3.75)

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(-7.5, 3.75, 7.5, 3.75)

### Create SketchLine
SketchLine_4 = Sketch_1.addLine(7.5, 3.75, 7.5, -1.25)
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint(), True)
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint(), True)
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint(), True)
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint(), True)
Sketch_1.setPerpendicular(SketchLine_1.result(), SketchLine_2.result(), True)
Sketch_1.setPerpendicular(SketchLine_2.result(), SketchLine_3.result(), True)
Sketch_1.setPerpendicular(SketchLine_3.result(), SketchLine_4.result(), True)
Sketch_1.setLength(SketchLine_2.result(), "l", True)
Sketch_1.setLength(SketchLine_1.result(), "L", True)
Sketch_1.setHorizontal(SketchLine_1.result(), True)
Sketch_1.setFixed(SketchLine_2.endPoint(), True)
model.do()

### Create Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r_wire")], model.selection(), 10, 0, "Faces|Wires")
Extrusion_1.result().setColor(132, 255, 0)

### Create Group
Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Extrusion_1_1/To_Face")])
Group_1.setName("top")
Group_1.result().setName("top")
Group_1.result().setColor(255, 0, 0)

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchLine
SketchLine_5 = Sketch_2.addLine(-7.5, -0.25, 4.5, -0.25)

### Create SketchProjection
SketchProjection_1 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_2"), False)
SketchLine_6 = SketchProjection_1.createdFeature()
Sketch_2.setCoincident(SketchLine_5.startPoint(), SketchLine_6.result(), True)
Sketch_2.setHorizontal(SketchLine_5.result(), True)

### Create SketchLine
SketchLine_7 = Sketch_2.addLine(4.5, -0.25, 4.5, 3.75)
Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_7.startPoint(), True)

### Create SketchProjection
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_3"), False)
SketchLine_8 = SketchProjection_2.createdFeature()
Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.result(), True)
Sketch_2.setVertical(SketchLine_7.result(), True)
Sketch_2.setVerticalDistance(SketchAPI_Line(SketchLine_6).endPoint(), SketchLine_5.startPoint(), "l-1", True)
Sketch_2.setHorizontalDistance(SketchLine_5.startPoint(), SketchLine_5.endPoint(), "L-3", True)
model.do()

### Create Extrusion
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchLine_7"), model.selection("EDGE", "Sketch_2/SketchLine_5")], model.selection("EDGE", "PartSet/OZ"), 10, 0)

### Create Partition
Partition_1_objects = [model.selection("SOLID", "Extrusion_1_1"),
                       model.selection("FACE", "Extrusion_2_1"),
                       model.selection("FACE", "Extrusion_2_2")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects, keepSubResults = True)

### Create Fuse
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Partition_1_1_2"), model.selection("SOLID", "Partition_1_1_1")], keepSubResults = True)

model.end()

# Move the "top" group to the end of history and split 
model.begin()
Part_1_doc.moveFeature(Group_1.feature(), Fuse_1.feature(), True)
model.end()

# Check that 2 groups of faces are created after split, 1 face in each
assert Part_1_doc.size("Groups") == 2, "Number of groups obtained is different than 2"

for i in range(Part_1_doc.size("Groups")) :
  resGroup = objectToResult(Part_1_doc.object("Groups", i))
  resFeature = Part_1_doc.feature(resGroup)
  resShape = resGroup.shape()
  assert not resShape.isNull(), "Shape in group is null"
  assert resFeature.data().selectionList("group_list").size() == 1, "More than one attribute in group arguments"
  assert resFeature.data().selectionList("group_list").value(0).value().shapeTypeStr() == "FACE", "Shape type of attribute is not a face"
  aSelName = resFeature.data().selectionList("group_list").value(0).context().data().name()
  assert len(aSelName) > 0, "Selection attribute name is empty"
