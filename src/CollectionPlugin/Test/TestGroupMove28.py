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

# Check the Move to the end feature by moving a group after a Fuse feature (2)

from salome.shaper import model
from SketchAPI import *

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "h", '10')

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_1 = SketchProjection_1.createdFeature()

### Create SketchArc
SketchArc_1 = Sketch_1.addArc(0, 0, 50, 0, 0, 50, False)
Sketch_1.setCoincident(SketchLine_1.result(), SketchArc_1.center(), True)

### Create SketchProjection
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_2.result(), True)
Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.result(), True)

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(0, 50, 0, 150)
Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_3.startPoint(), True)
Sketch_1.setVertical(SketchLine_3.result(), True)

### Create SketchLine
SketchLine_4 = Sketch_1.addLine(0, 150, 250, 150)
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint(), True)
Sketch_1.setHorizontal(SketchLine_4.result(), True)

### Create SketchLine
SketchLine_5 = Sketch_1.addLine(250, 150, 250, 0)
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint(), True)
Sketch_1.setVertical(SketchLine_5.result(), True)

### Create SketchLine
SketchLine_6 = Sketch_1.addLine(250, 0, 50, 0)
Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint(), True)
Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_6.endPoint(), True)
Sketch_1.setHorizontal(SketchLine_6.result(), True)
Sketch_1.setPerpendicular(SketchArc_1.results()[1], SketchLine_6.result(), True)
Sketch_1.setLength(SketchLine_4.result(), 250, True)
Sketch_1.setLength(SketchLine_5.result(), 150, True)
Sketch_1.setRadius(SketchArc_1.results()[1], 50, True)
model.do()

### Create Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_6r-SketchLine_5r-SketchLine_4r-SketchLine_3r-SketchArc_1_2r")], model.selection(), "h", 0, "Faces|Wires")

### Create Group
Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Extrusion_1_1/To_Face")])
Group_1.setName("Top")
Group_1.result().setName("Top")
Group_1.result().setColor(91, 204, 206)

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))

### Create SketchLine
SketchLine_7 = Sketch_2.addLine(0, 80, 80, 80)

### Create SketchProjection
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_3"), False)
SketchLine_8 = SketchProjection_3.createdFeature()
Sketch_2.setCoincident(SketchLine_7.startPoint(), SketchLine_8.result(), True)
Sketch_2.setHorizontal(SketchLine_7.result(), True)

### Create SketchLine
SketchLine_9 = Sketch_2.addLine(80, 80, 80, 0)
Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_9.startPoint(), True)

### Create SketchProjection
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_6"), False)
SketchLine_10 = SketchProjection_4.createdFeature()
Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_10.result(), True)
Sketch_2.setVertical(SketchLine_9.result(), True)

### Create SketchLine
SketchLine_11 = Sketch_2.addLine(35.35533905932738, 35.35533905932738, 80, 80)

### Create SketchProjection
SketchProjection_5 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchArc_1_2"), False)
SketchArc_2 = SketchProjection_5.createdFeature()
Sketch_2.setCoincident(SketchLine_11.startPoint(), SketchArc_2.results()[1], True)
Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_11.endPoint(), True)

### Create SketchLine
SketchLine_12 = Sketch_2.addLine(80, 80, 250, 80)
Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_12.startPoint(), True)

### Create SketchProjection
SketchProjection_6 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_5"), False)
SketchLine_13 = SketchProjection_6.createdFeature()
Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_13.result(), True)
Sketch_2.setHorizontal(SketchLine_12.result(), True)

### Create SketchLine
SketchLine_14 = Sketch_2.addLine(80, 80, 80, 150)
Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_14.startPoint(), True)

### Create SketchProjection
SketchProjection_7 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_4"), False)
SketchLine_15 = SketchProjection_7.createdFeature()
Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchLine_15.result(), True)
Sketch_2.setVertical(SketchLine_14.result(), True)
Sketch_2.setLength(SketchLine_7.result(), 80, True)
Sketch_2.setLength(SketchLine_9.result(), 80, True)

### Create SketchProjection
SketchProjection_8 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_16 = SketchProjection_8.createdFeature()

### Create SketchConstraintAngle
Sketch_2.setAngle(SketchLine_16.result(), SketchLine_11.result(), 45, type = "Direct", is_active = True)
model.do()

### Create Extrusion
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection("EDGE", "PartSet/OZ"), "h", 0, "Edges")

### Create Partition
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("COMPOUND", "Extrusion_2_1")], keepSubResults = True)

### Create Fuse
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Partition_1_1_4"), model.selection("SOLID", "Partition_1_1_3")], removeEdges = True, keepSubResults = True)

model.end()

# Move the "top" group to the end of history
model.begin()
Part_1_doc.moveFeature(Group_1.feature(), Fuse_1.feature())
model.end()

# Check that the content of the top group is correct (4 faces)
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
 
assert(aFactory.validate(Group_1.feature()))
assert(Group_1.groupList().size() == 4)
assert(Group_1.groupList().value(0).value().shapeTypeStr() == "FACE")
assert(Group_1.groupList().value(1).value().shapeTypeStr() == "FACE")
assert(Group_1.groupList().value(2).value().shapeTypeStr() == "FACE")
assert(Group_1.groupList().value(3).value().shapeTypeStr() == "FACE")
assert(len(Group_1.groupList().value(0).namingName()) > 0)
assert(len(Group_1.groupList().value(1).namingName()) > 0)
assert(len(Group_1.groupList().value(2).namingName()) > 0)
assert(len(Group_1.groupList().value(3).namingName()) > 0)
