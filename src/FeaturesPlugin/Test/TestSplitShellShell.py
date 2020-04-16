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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "D", "50")
model.addParameter(Part_1_doc, "L", "30")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-10, 32.04672553984553, 15.61552812808831, 47.66225366793383)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
SketchLine_3 = Sketch_1.addLine(15.61552812808831, 47.66225366793383, -4.988916520257207e-21, 73.27778179602213)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_2.result())
SketchLine_4 = Sketch_1.addLine(-4.988916520257207e-21, 73.27778179602213, -25.61552812810082, 57.66225366792627)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(-25.61552812810082, 57.66225366792627, -10, 32.04672553984553)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.endPoint())
SketchLine_6 = Sketch_1.addLine(-25.61552812810082, 57.66225366792627, 15.61552812808831, 47.66225366793383)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_6.endPoint())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchAPI_Line(SketchLine_2).startPoint(), SketchLine_6.result(), "D", True)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), "L")
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_3.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_3.result(), SketchLine_4.result())
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchLine_4.result(), SketchLine_5.result())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_1.result(), SketchLine_3.result())
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_5.endPoint(), SketchLine_2.result(), "L/3", True)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchLine_7 = Sketch_2.addLine(50, 15.56025031353752, 50, -15.54016280334104)
SketchLine_8 = Sketch_2.addLine(50, -15.54016280334104, 36.52981427174974, 0)
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_9 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.result())
SketchLine_10 = Sketch_2.addLine(36.52981427174974, 0, 50, 15.56025031353752)
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_7.startPoint(), SketchLine_10.endPoint())
SketchLine_11 = Sketch_2.addLine(50, 15.56025031353752, 73.27778179602213, 0)
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_7.startPoint(), SketchLine_11.startPoint())
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_3_EndVertex"), False)
SketchPoint_1 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchPoint_1.result())
SketchLine_12 = Sketch_2.addLine(73.27778179602213, 0, 50, -15.54016280334104)
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_12.endPoint())
SketchConstraintDistance_3 = Sketch_2.setDistance(SketchAPI_Line(SketchLine_9).startPoint(), SketchLine_7.result(), "D", True)
SketchConstraintVertical_1 = Sketch_2.setVertical(SketchLine_7.result())
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchLine_1r-SketchLine_6r-SketchLine_5f_wire")])
Face_2 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchLine_3f-SketchLine_4f-SketchLine_6f_wire")])
Face_3 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchLine_10r-SketchLine_8r-SketchLine_7r_wire")])
Face_4 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchLine_7r-SketchLine_12r-SketchLine_11r_wire")])
Shell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Face_1_1"), model.selection("FACE", "Face_2_1")])
Shell_2 = model.addShell(Part_1_doc, [model.selection("FACE", "Face_3_1"), model.selection("FACE", "Face_4_1")])
Split_1 = model.addSplit(Part_1_doc, [model.selection("SHELL", "Shell_1_1")], [model.selection("SHELL", "Shell_2_1")])
model.do()

model.checkResult(Split_1, model, 1, [0], [0], [4], [15], [30])
model.testHaveNamingSubshapes(Split_1, model, Part_1_doc)

model.end()

assert(model.checkPythonDump())
