# Copyright (C) 2014-2024  CEA, EDF
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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "D", "50")
model.addParameter(Part_1_doc, "L", "30")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-6, 29.93619865618197, 18, 47.93619865618197)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
SketchLine_3 = Sketch_1.addLine(18, 47.93619865618197, 0, 71.93619865618197)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_2.result())
SketchLine_4 = Sketch_1.addLine(0, 71.93619865618197, -24, 53.93619865618197)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(-24, 53.93619865618197, -6, 29.93619865618197)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.endPoint())
SketchLine_6 = Sketch_1.addLine(-24, 53.93619865618197, 18, 47.93619865618197)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_6.endPoint())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchAPI_Line(SketchLine_2).startPoint(), SketchLine_6.result(), "D", True)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), "L")
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_3.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_3.result(), SketchLine_4.result())
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchLine_4.result(), SketchLine_5.result())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_1.result(), SketchLine_3.result())
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_5.endPoint(), SketchLine_2.result(), "L/5", True)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_2.createdFeature()
SketchLine_7 = Sketch_2.addLine(50, -39.61510263929618, 50, 43.87280058651025)
SketchLine_7.setName("SketchLine_8")
SketchLine_7.result().setName("SketchLine_8")
SketchLine_7.setAuxiliary(True)
SketchConstraintVertical_1 = Sketch_2.setVertical(SketchLine_7.result())
SketchConstraintDistance_3 = Sketch_2.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_7.result(), "D", True)
SketchLine_8 = Sketch_2.addLine(50, -3.170182618622045, 80, -14.17077918941143)
SketchLine_8.setName("SketchLine_9")
SketchLine_8.result().setName("SketchLine_9")
SketchLine_9 = Sketch_2.addLine(80, -14.17077918941143, 64.99999999999999, 12.18920647716607)
SketchLine_9.setName("SketchLine_11")
SketchLine_9.result().setName("SketchLine_11")
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_8.setName("SketchConstraintCoincidence_9")
SketchLine_10 = Sketch_2.addLine(64.99999999999999, 12.18920647716607, 50, 12.18920647716607)
SketchLine_10.setName("SketchLine_12")
SketchLine_10.result().setName("SketchLine_12")
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_9.setName("SketchConstraintCoincidence_10")
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_7.result())
SketchConstraintCoincidence_10.setName("SketchConstraintCoincidence_11")
SketchConstraintDistance_4 = Sketch_2.setDistance(SketchLine_8.endPoint(), SketchLine_7.result(), "L", True)
SketchConstraintDistance_5 = Sketch_2.setDistance(SketchLine_9.endPoint(), SketchLine_7.result(), "L/2", True)
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_10.result())
SketchConstraintMirror_1_objects = [SketchLine_8.result(), SketchLine_9.result(), SketchLine_10.result()]
SketchConstraintMirror_1 = Sketch_2.addMirror(SketchLine_7.result(), SketchConstraintMirror_1_objects)
[SketchLine_11, SketchLine_12, SketchLine_13] = SketchConstraintMirror_1.mirrored()
SketchLine_13.setName("SketchLine_15")
SketchLine_13.result().setName("SketchLine_15")
SketchLine_12.setName("SketchLine_14")
SketchLine_12.result().setName("SketchLine_14")
SketchLine_11.setName("SketchLine_13")
SketchLine_11.result().setName("SketchLine_13")
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_3_EndVertex"), False)
SketchPoint_2 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_9.result())
SketchConstraintCoincidence_11.setName("SketchConstraintCoincidence_12")
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_8.startPoint(), SketchLine_7.result())
SketchConstraintCoincidence_12.setName("SketchConstraintCoincidence_13")
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchLine_1r-SketchLine_6r-SketchLine_5f_wire")])
Face_2 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchLine_3f-SketchLine_4f-SketchLine_6f_wire")])
Shell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Face_1_1"), model.selection("FACE", "Face_2_1")])
Face_3 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchLine_9f-SketchLine_11f-SketchLine_12f-SketchLine_15r-SketchLine_14r-SketchLine_13r_wire")])
Split_1 = model.addSplit(Part_1_doc, [model.selection("SHELL", "Shell_1_1")], [model.selection("FACE", "Face_3_1")])
model.do()

model.checkResult(Split_1, model, 1, [0], [0], [4], [13], [26])
model.testHaveNamingSubshapes(Split_1, model, Part_1_doc)

model.end()

assert(model.checkPythonDump())
