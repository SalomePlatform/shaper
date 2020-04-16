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

# Test that splitted edges in the group are still correct edges in the group
# Based on the CEA report mail 04.12.2018, page 4

from SketchAPI import *
from ModelAPI import *
from GeomAPI import *
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
param_p = model.addParameter(Part_1_doc, "r", "15")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchArc_1 = Sketch_1.addArc(7.5, 7.5, 22.5, 7.499992770095306, 7.499999582962127, 22.5, False)
SketchLine_1 = Sketch_1.addLine(7.499999582962127, 22.5, 7.499999582962127, 52.5)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.startPoint())
SketchLine_2 = Sketch_1.addLine(22.5, 7.499992770095306, 52.5, 7.499992770095306)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_2.startPoint())
SketchArc_2 = Sketch_1.addArc(7.5, 7.5, 52.5, 7.499992770095306, 7.499999582962127, 52.5, False)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.center(), SketchArc_2.center())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_1.endPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), "r*2")
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_2.result())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], "r")
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_2.results()[1], "3*r")
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchArc_2.center(), "r/2")
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchArc_1.center(), "r/2")
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_2f-SketchArc_2_2f-SketchLine_1r-SketchArc_1_2r")])
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Sketch_1/Face-SketchLine_2f-SketchArc_2_2f-SketchLine_1r-SketchArc_1_2r"))
SketchCircle_1 = Sketch_2.addCircle(27.71638597484288, 11.4805029721516, 3)
SketchCircle_2 = Sketch_2.addCircle(45.03912720992525, 18.6558173277981, 3)
SketchLine_3 = Sketch_2.addLine(0, 0, 45.03912720992525, 18.6558173277981)
SketchLine_3.setAuxiliary(True)
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_3.startPoint(), SketchPoint_2.result())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchCircle_2.center(), SketchLine_3.endPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_3.result(), SketchCircle_1.center())
SketchConstraintRadius_3 = Sketch_2.setRadius(SketchCircle_1.results()[1], "r/5")
SketchConstraintRadius_4 = Sketch_2.setRadius(SketchCircle_2.results()[1], "r/5")
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchLine_3.startPoint(), SketchCircle_1.center(), "2*r", True)
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchCircle_1.center(), SketchLine_3.endPoint(), "r*1.25", True)
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_4 = SketchProjection_3.createdFeature()
SketchConstraintAngle_1 = Sketch_2.setAngle(SketchLine_4.result(), SketchLine_3.result(), "r*1.5")
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection(), 10, 0)
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_1")], model.selection("EDGE", "PartSet/OZ"), "90.-r*3", 2)
Cut_1 = model.addCut(Part_1_doc, [model.selection("FACE", "Face_1_1")], [model.selection("COMPOUND", "AngularCopy_1_1")])
Group_1 = model.addGroup(Part_1_doc, [model.selection("EDGE", "Face_1_1/Modified_Edge&Sketch_1/SketchArc_1_2")])
Group_1.setName("edge_int")
Group_1.result().setName("edge_int")
Group_2 = model.addGroup(Part_1_doc, [model.selection("EDGE", "Face_1_1/Modified_Edge&Sketch_1/SketchLine_2")])
Group_2.setName("edge_bottom")
Group_2.result().setName("edge_bottom")
Group_3 = model.addGroup(Part_1_doc, [model.selection("EDGE", "Face_1_1/Modified_Edge&Sketch_1/SketchLine_1")])
Group_3.setName("edge_left")
Group_3.result().setName("edge_left")
Group_4 = model.addGroup(Part_1_doc, [model.selection("EDGE", "Face_1_1/Modified_Edge&Sketch_1/SketchArc_2_2")])
Group_4.setName("edge_ext")
Group_4.result().setName("edge_ext")
model.do()
# change parameter "p" to make one ylinder split bottom and left edges
param_p.setValue(12)
model.end()

# check that int and ext groups are still edges, bottom and left groups are compounds of edges
aFactory = ModelAPI_Session.get().validators()

for group in [Group_1, Group_4]:
  assert(aFactory.validate(group.feature()))
  selectionList = group.feature().selectionList("group_list")
  assert(selectionList.size() == 1)
  assert(group.groupList().value(0).value().shapeType() == GeomAPI_Shape.EDGE)

for group in [Group_2, Group_3]:
  assert(aFactory.validate(group.feature()))
  selectionList = group.feature().selectionList("group_list")
  assert(selectionList.size() == 2)
  assert(group.groupList().value(0).value().shapeType() == GeomAPI_Shape.EDGE)
  assert(group.groupList().value(1).value().shapeType() == GeomAPI_Shape.EDGE)

assert(model.checkPythonDump())
