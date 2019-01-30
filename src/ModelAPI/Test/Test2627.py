## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

# -*- coding: utf-8 -*-

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
model.addParameter(partSet, "L", "10")
model.addParameter(partSet, "H", "8")
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(10, 0, 0, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 8)
SketchLine_3 = Sketch_1.addLine(0, 8, 10, 8)
SketchLine_4 = Sketch_1.addLine(10, 8, 10, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(2.5, -1.540743955509789e-33, 2.5, 8)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_1.result())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_3.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), "L")
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_2.result(), "H")
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_5.startPoint(), "L/4.")
model.do()
Shell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_5f-SketchLine_3r-SketchLine_2r-SketchLine_1r"), model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_5r-SketchLine_1r")])
Group_1 = model.addGroup(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_5f-SketchLine_3r-SketchLine_2r-SketchLine_1r")])
Group_1.setName("face_g")
Group_1.result().setName("face_g")
Group_2 = model.addGroup(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_5r-SketchLine_1r")])
Group_2.setName("face_d")
Group_2.result().setName("face_d")
Group_3 = model.addGroup(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_5f-SketchLine_3r-SketchLine_2r-SketchLine_1r"), model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_5r-SketchLine_1r")])
Group_3.setName("face_g_d")
Group_3.result().setName("face_g_d")
Group_4 = model.addGroup(Part_1_doc, [model.selection("EDGE", "Sketch_1/SubEdge_1&Sketch_1/SketchLine_1")])
Group_4.setName("ligne_g")
Group_4.result().setName("ligne_g")
Group_5 = model.addGroup(Part_1_doc, [model.selection("EDGE", "Sketch_1/SubEdge_2&Sketch_1/SketchLine_1")])
Group_5.setName("ligne_d")
Group_5.result().setName("ligne_d")
Group_6 = model.addGroup(Part_1_doc, [model.selection("EDGE", "Sketch_1/SubEdge_1&Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SubEdge_2&Sketch_1/SketchLine_1")])
Group_6.setName("ligne_g_d")
Group_6.result().setName("ligne_g_d")
Group_7 = model.addGroup(Part_1_doc, [model.selection("VERTEX", "Sketch_1/SubVertex_1&Sketch_1/SketchLine_2_EndVertex&Sketch_1/SketchLine_3_StartVertex")])
Group_7.setName("coin_g")
Group_7.result().setName("coin_g")
Group_8 = model.addGroup(Part_1_doc, [model.selection("VERTEX", "Sketch_1/SubVertex_2&Sketch_1/SketchLine_3_EndVertex&Sketch_1/SketchLine_4_StartVertex")])
Group_8.setName("coin_d")
Group_8.result().setName("coin_d")
Group_9 = model.addGroup(Part_1_doc, [model.selection("VERTEX", "Sketch_1/SubVertex_1&Sketch_1/SketchLine_2_EndVertex&Sketch_1/SketchLine_3_StartVertex"), model.selection("VERTEX", "Sketch_1/SubVertex_2&Sketch_1/SketchLine_3_EndVertex&Sketch_1/SketchLine_4_StartVertex")])
Group_9.setName("coins")
Group_9.result().setName("coins")
model.end()

# check all groups are correct
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
for i in range(Part_1_doc.size("Groups")):
  GroupFeature = Part_1_doc.feature(objectToResult(Part_1_doc.object("Groups", i)))
  assert(aFactory.validate(GroupFeature))

assert(model.checkPythonDump())
