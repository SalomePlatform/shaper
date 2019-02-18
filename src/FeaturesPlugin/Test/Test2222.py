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

# check that fac partitioned by plane has correctly named modified edges

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(173, 265, -254, 265)
SketchLine_2 = Sketch_1.addLine(-254, 265, -254, 118)
SketchLine_3 = Sketch_1.addLine(-254, 118, 173, 118)
SketchLine_4 = Sketch_1.addLine(173, 118, 173, 265)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Face_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_4")]
Face_1 = model.addFace(Part_1_doc, Face_1_objects)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/YOZ"), 1, False)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("FACE", "Face_1_1"), model.selection("FACE", "Plane_1")])
Group_1 = model.addGroup(Part_1_doc, [model.selection("EDGE", "Partition_1_1_2/Modified_Edge&Face_1_1/Edge_4")])
model.do()
model.end()

# check that resulting group selection is valid
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Group_1.feature()))
assert(Group_1.groupList().size() == 1)
assert(Group_1.groupList().value(0).value().shapeTypeStr() == "EDGE")
assert(len(Group_1.groupList().value(0).namingName()) > 0)

assert(model.checkPythonDump())
