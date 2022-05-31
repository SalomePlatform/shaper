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
import sys
import salome

salome.standalone()
salome.salome_init()
import salome_notebook
###
### SHAPER component
###
from SketchAPI import *
from salome.shaper import model
model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(30, 0, 0, 0)
SketchLine_1.setAuxiliary(True)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 26)
SketchLine_2.setAuxiliary(True)
SketchLine_3 = Sketch_1.addLine(0, 26, 30, 26)
SketchLine_3.setAuxiliary(True)
SketchLine_4 = Sketch_1.addLine(30, 26, 30, 0)
SketchLine_4.setAuxiliary(True)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 30)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_2.result(), 26)
SketchBSplinePeriodic_1 = Sketch_1.addSpline(poles = [(5, 26), (-1.697788041973476e-26, 13), (5, 0), (30, 13)], periodic = True)
[SketchPoint_2, SketchPoint_3, SketchPoint_4, SketchPoint_5] = SketchBSplinePeriodic_1.controlPoles(auxiliary = [0, 1, 2, 3])
[SketchLine_5, SketchLine_6, SketchLine_7, SketchLine_8] = SketchBSplinePeriodic_1.controlPolygon(auxiliary = [0, 1, 2, 3])
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_6).startPoint(), SketchLine_2.result())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_3.result())
SketchConstraintCoincidence_7.setName("SketchConstraintCoincidence_8")
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_7).endPoint(), SketchLine_4.result())
SketchConstraintCoincidence_8.setName("SketchConstraintCoincidence_9")
SketchConstraintMiddle_1 = Sketch_1.setMiddlePoint(SketchAPI_Line(SketchLine_7).endPoint(), SketchLine_4.result())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_6).endPoint(), SketchLine_1.result())
SketchConstraintCoincidence_9.setName("SketchConstraintCoincidence_10")
SketchConstraintMiddle_2 = Sketch_1.setMiddlePoint(SketchAPI_Line(SketchLine_6).startPoint(), SketchLine_2.result())
SketchLine_9 = Sketch_1.addLine(5, 26, 5, 0)
SketchLine_9.setAuxiliary(True)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10.setName("SketchConstraintCoincidence_11")
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_4).coordinates(), SketchLine_9.endPoint())
SketchConstraintCoincidence_11.setName("SketchConstraintCoincidence_12")
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_9.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchAPI_Line(SketchLine_6).endPoint(), SketchLine_2.result(), 5, True)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchBSplinePeriodic_1f")], model.selection(), 1, 0)
model.end()

###
### SHAPERSTUDY component
###
model.publishToShaperStudy()

# from the issue scenario: make fillet and two publications (caused by dump and save)
model.begin()
Fillet_1 = model.addFillet(Part_1_doc, [model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchBSplinePeriodic_1][Extrusion_1_1/To_Face]")], 0.4999, keepSubResults = True)
model.end()

model.publishToShaperStudy()
model.publishToShaperStudy()
