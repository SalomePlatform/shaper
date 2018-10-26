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
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "D", "50")
model.addParameter(Part_1_doc, "L", "30")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 28.78679656440358, 21.21320343559643, 50)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_2.result())
SketchLine_3 = Sketch_1.addLine(21.21320343559643, 50, 0, 71.21320343559643)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_2.result())
SketchLine_4 = Sketch_1.addLine(0, 71.21320343559643, -21.21320343559642, 50)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(-21.21320343559642, 50, 0, 28.78679656440358)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.endPoint())
SketchLine_6 = Sketch_1.addLine(-21.21320343559642, 50, 21.21320343559643, 50)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_6.endPoint())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchAPI_Line(SketchLine_2).startPoint(), SketchLine_6.result(), "D", True)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), "L")
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_3.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_3.result(), SketchLine_4.result())
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchLine_4.result(), SketchLine_5.result())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_1.result(), SketchLine_3.result())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_2.createdFeature()
SketchLine_7 = Sketch_2.addLine(50, -39.61510263929618, 50, 43.87280058651025)
SketchLine_7.setName("SketchLine_8")
SketchLine_7.result().setName("SketchLine_8")
SketchLine_7.setAuxiliary(True)
SketchConstraintVertical_1 = Sketch_2.setVertical(SketchLine_7.result())
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_7.result(), "D", True)
SketchLine_8 = Sketch_2.addLine(57.5, 0, 65, -10.81485284151855)
SketchLine_8.setName("SketchLine_9")
SketchLine_8.result().setName("SketchLine_9")
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_9 = SketchProjection_3.createdFeature()
SketchLine_9.setName("SketchLine_10")
SketchLine_9.result().setName("SketchLine_10")
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_8.startPoint(), SketchLine_9.result())
SketchLine_10 = Sketch_2.addLine(65, -10.81485284151855, 65, 11.50710132369052)
SketchLine_10.setName("SketchLine_11")
SketchLine_10.result().setName("SketchLine_11")
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_10.startPoint())
SketchLine_11 = Sketch_2.addLine(65, 11.50710132369052, 50, 11.50710132369052)
SketchLine_11.setName("SketchLine_12")
SketchLine_11.result().setName("SketchLine_12")
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_7.result())
SketchConstraintDistance_3 = Sketch_2.setDistance(SketchLine_8.startPoint(), SketchLine_7.result(), "L/4", True)
SketchConstraintDistance_4 = Sketch_2.setDistance(SketchLine_8.endPoint(), SketchLine_7.result(), "L/2", True)
SketchConstraintDistance_5 = Sketch_2.setDistance(SketchLine_10.endPoint(), SketchLine_7.result(), "L/2", True)
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_11.result())
SketchConstraintMirror_1_objects = [SketchLine_8.result(), SketchLine_10.result(), SketchLine_11.result()]
SketchConstraintMirror_1 = Sketch_2.addMirror(SketchLine_7.result(), SketchConstraintMirror_1_objects)
[SketchLine_12, SketchLine_13, SketchLine_14] = SketchConstraintMirror_1.mirrored()
SketchLine_14.setName("SketchLine_15")
SketchLine_14.result().setName("SketchLine_15")
SketchLine_13.setName("SketchLine_14")
SketchLine_13.result().setName("SketchLine_14")
SketchLine_12.setName("SketchLine_13")
SketchLine_12.result().setName("SketchLine_13")
SketchLine_15 = Sketch_2.addLine(42.5, 0, 57.5, 0)
SketchLine_15.setName("SketchLine_16")
SketchLine_15.result().setName("SketchLine_16")
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchAPI_Line(SketchLine_12).startPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchLine_15.endPoint(), SketchLine_8.startPoint())
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchLine_1r-SketchLine_6r-SketchLine_5f_wire")])
Face_2 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchLine_3f-SketchLine_4f-SketchLine_6f_wire")])
Shell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Face_1_1"), model.selection("FACE", "Face_2_1")])
Wire_1 = model.addWire(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchLine_9f-SketchLine_11f-SketchLine_12f-SketchLine_15r-SketchLine_14r-SketchLine_13r-SketchLine_16f_wire")])
Fill_1 = model.addFill(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], [model.selection("SHELL", "Shell_1_1")])
model.do()

model.checkResult(Fill_1, model, 1, [0], [0], [0], [10], [20])
model.testHaveNamingSubshapes(Fill_1, model, Part_1_doc)

model.end()

assert(model.checkPythonDump())
