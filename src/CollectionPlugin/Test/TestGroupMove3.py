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

# Test that partition divides cylinder into 4 faces, there is no edges in a group moved to the end
# Based on the CEA report mail 04.12.2018, page 2

from SketchAPI import *

from ModelAPI import *
from GeomAPI import *
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchArc_1 = Sketch_1.addArc(-1.103476974288834e-12, 24.99999999999979, 24.49489742783218, 30, 0, 50, False)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.result())
SketchLine_2 = Sketch_1.addLine(0, 50, -10, 50)
SketchLine_2.setAuxiliary(True)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_2.result())
SketchLine_3 = Sketch_1.addLine(24.49489742783218, 30, 24.49489742783218, 5)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(24.49489742783218, 5, 34.49489742783218, 5)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(34.49489742783218, 5, 34.49489742783218, 0)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_6 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.result())
SketchLine_7 = Sketch_1.addLine(34.49489742783218, 0, 0, 0)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_1).startPoint(), SketchLine_7.endPoint())
SketchLine_8 = Sketch_1.addLine(0, 50, 0, 0)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_1).startPoint(), SketchLine_8.endPoint())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_4.result(), 10)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_5.result(), 5)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_3.result(), 25)
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_8.result(), 50)
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_2.result(), 10)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 25)
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchArc_1_2r-SketchLine_8f-SketchLine_7r-SketchLine_5r-SketchLine_4r-SketchLine_3r")], model.selection("EDGE", "PartSet/OZ"), 360, 0)
Group_1 = model.addGroup(Part_1_doc, [model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3")])
Partition_1_objects = [model.selection("SOLID", "Revolution_1_1"), model.selection("FACE", "PartSet/XOZ"), model.selection("FACE", "PartSet/YOZ")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
model.do()

# move group
Part_1_doc.moveFeature(Group_1.feature(), Partition_1.feature())
model.end()

# Check group: result must be four faces
aFactory = ModelAPI_Session.get().validators()
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 4)
assert(aFactory.validate(Group_1.feature()))
for i in range(4):
  assert(Group_1.groupList().value(i).value().shapeType() == GeomAPI_Shape.FACE)

assert(model.checkPythonDump())
