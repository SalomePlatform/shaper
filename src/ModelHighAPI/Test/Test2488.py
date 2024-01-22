# Copyright (C) 2017-2024  CEA, EDF
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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(0, 4, 6.2, 4)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_2.result())
SketchLine_3 = Sketch_1.addLine(6.2, 4, 6.2, 2.5)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(6.2, 2.5, 3.5, 2)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(3.5, 2, 3.5, 0)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_6 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.result())
SketchLine_7 = Sketch_1.addLine(3.5, 0, 0, 0)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_2).startPoint(), SketchLine_7.endPoint())
SketchLine_8 = Sketch_1.addLine(0, 4, 0, 0)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_2).startPoint(), SketchLine_8.endPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_7.result(), 3.5)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_3.result(), 1.5)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_1.result(), 6.2)
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_8.result(), 4)
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_5.result(), 2)
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_8f-SketchLine_7r-SketchLine_5r-SketchLine_4r-SketchLine_3r-SketchLine_1r")], model.selection("EDGE", "PartSet/OZ"), 360, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_1"))
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_1]__cc"), False)
SketchPoint_1 = SketchProjection_3.createdFeature()
SketchCircle_1 = Sketch_2.addCircle(0, 0, 4.75)
SketchCircle_1.setAuxiliary(True)
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
SketchCircle_2 = Sketch_2.addCircle(4.113620667973598, 2.375000000005359, 1.1)
SketchCircle_2.setName("SketchCircle_3")
SketchCircle_2.result().setName("SketchCircle_3")
SketchCircle_2.results()[1].setName("SketchCircle_3_2")
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchCircle_1.results()[1], SketchCircle_2.center())
SketchConstraintRadius_1 = Sketch_2.setRadius(SketchCircle_1.results()[1], 4.75)
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchCircle_2.results()[1], 1.1)
SketchLine_9 = Sketch_2.addLine(0, 0, 4.113620667973598, 2.375000000005359)
SketchLine_9.setAuxiliary(True)
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_9.startPoint())
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchCircle_2.center(), SketchLine_9.endPoint())
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_1"), False)
SketchLine_10 = SketchProjection_4.createdFeature()
SketchConstraintAngle_1 = Sketch_2.setAngleBackward(SketchLine_9.result(), SketchLine_10.result(), 30)
SketchMultiRotation_1 = Sketch_2.addRotation([SketchCircle_2.results()[1]], SketchCircle_1.center(), 120, 3)
[SketchCircle_3, SketchCircle_4] = SketchMultiRotation_1.rotated()
SketchCircle_4.setName("SketchCircle_5")
SketchCircle_4.result().setName("SketchCircle_5")
SketchCircle_4.results()[1].setName("SketchCircle_5_2")
SketchCircle_3.setName("SketchCircle_4")
SketchCircle_3.result().setName("SketchCircle_4")
SketchCircle_3.results()[1].setName("SketchCircle_4_2")
SketchCircle_5 = Sketch_2.addCircle(4.745479052513824, 0.2071920899853478, 0.65)
SketchCircle_5.setName("SketchCircle_6")
SketchCircle_5.result().setName("SketchCircle_6")
SketchCircle_5.results()[1].setName("SketchCircle_6_2")
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchCircle_1.results()[1], SketchCircle_5.center())
SketchCircle_6 = Sketch_2.addCircle(4.745479052513824, 0.2071920899853478, 0.8)
SketchCircle_6.setName("SketchCircle_7")
SketchCircle_6.result().setName("SketchCircle_7")
SketchCircle_6.results()[1].setName("SketchCircle_7_2")
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchCircle_5.center(), SketchCircle_6.center())
SketchCircle_7 = Sketch_2.addCircle(2.5521731400059, 4.006109367385169, 0.8)
SketchCircle_7.setName("SketchCircle_8")
SketchCircle_7.result().setName("SketchCircle_8")
SketchCircle_7.results()[1].setName("SketchCircle_8_2")
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchCircle_1.results()[1], SketchCircle_7.center())
SketchCircle_8 = Sketch_2.addCircle(2.5521731400059, 4.006109367385169, 0.65)
SketchCircle_8.setName("SketchCircle_9")
SketchCircle_8.result().setName("SketchCircle_9")
SketchCircle_8.results()[1].setName("SketchCircle_9_2")
SketchConstraintCoincidence_17 = Sketch_2.setCoincident(SketchCircle_1.results()[1], SketchCircle_8.center())
SketchConstraintCoincidence_18 = Sketch_2.setCoincident(SketchCircle_7.center(), SketchCircle_8.center())
SketchConstraintRadius_3 = Sketch_2.setRadius(SketchCircle_6.results()[1], 0.8)
SketchConstraintRadius_4 = Sketch_2.setRadius(SketchCircle_5.results()[1], 0.65)
SketchConstraintEqual_1 = Sketch_2.setEqual(SketchCircle_5.results()[1], SketchCircle_8.results()[1])
SketchConstraintEqual_2 = Sketch_2.setEqual(SketchCircle_7.results()[1], SketchCircle_6.results()[1])
SketchLine_11 = Sketch_2.addLine(0, 0, 4.745479052513824, 0.2071920899853478)
SketchLine_11.setAuxiliary(True)
SketchConstraintCoincidence_19 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_11.startPoint())
SketchConstraintCoincidence_20 = Sketch_2.setCoincident(SketchCircle_5.center(), SketchLine_11.endPoint())
SketchLine_12 = Sketch_2.addLine(2.5521731400059, 4.006109367385169, 0, 0)
SketchLine_12.setAuxiliary(True)
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchCircle_7.center(), SketchLine_12.startPoint())
SketchConstraintCoincidence_22 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_12.endPoint())
SketchConstraintAngle_2 = Sketch_2.setAngle(SketchLine_11.result(), SketchLine_9.result(), 27.5)
SketchConstraintAngle_3 = Sketch_2.setAngle(SketchLine_9.result(), SketchLine_12.result(), 27.5)
SketchMultiRotation_2 = Sketch_2.addRotation([SketchCircle_7.results()[1], SketchCircle_6.results()[1]], SketchCircle_1.center(), 120, 3)
[SketchCircle_9, SketchCircle_10, SketchCircle_11, SketchCircle_12] = SketchMultiRotation_2.rotated()
SketchCircle_12.setName("SketchCircle_13")
SketchCircle_12.result().setName("SketchCircle_13")
SketchCircle_12.results()[1].setName("SketchCircle_13_2")
SketchCircle_11.setName("SketchCircle_11")
SketchCircle_11.result().setName("SketchCircle_11")
SketchCircle_11.results()[1].setName("SketchCircle_11_2")
SketchCircle_10.setName("SketchCircle_12")
SketchCircle_10.result().setName("SketchCircle_12")
SketchCircle_10.results()[1].setName("SketchCircle_12_2")
SketchCircle_9.setName("SketchCircle_10")
SketchCircle_9.result().setName("SketchCircle_10")
SketchCircle_9.results()[1].setName("SketchCircle_10_2")
SketchMultiRotation_3 = Sketch_2.addRotation([SketchCircle_8.results()[1], SketchCircle_5.results()[1]], SketchAPI_Line(SketchLine_10).startPoint(), 120, 2)
[SketchCircle_13, SketchCircle_14] = SketchMultiRotation_3.rotated()
SketchCircle_14.setName("SketchCircle_15")
SketchCircle_14.result().setName("SketchCircle_15")
SketchCircle_14.results()[1].setName("SketchCircle_15_2")
SketchCircle_13.setName("SketchCircle_14")
SketchCircle_13.result().setName("SketchCircle_14")
SketchCircle_13.results()[1].setName("SketchCircle_14_2")
SketchCircle_15 = Sketch_2.addCircle(0, 5, 0.45)
SketchCircle_15.setName("SketchCircle_16")
SketchCircle_15.result().setName("SketchCircle_16")
SketchCircle_15.results()[1].setName("SketchCircle_16_2")
SketchCircle_16 = Sketch_2.addCircle(3.276608177155967, -2.294305745404186, 0.3)
SketchCircle_16.setName("SketchCircle_17")
SketchCircle_16.result().setName("SketchCircle_17")
SketchCircle_16.results()[1].setName("SketchCircle_17_2")
SketchCircle_17 = Sketch_2.addCircle(4.259590630302757, -2.982597469025442, 0.5)
SketchCircle_17.setName("SketchCircle_18")
SketchCircle_17.result().setName("SketchCircle_18")
SketchCircle_17.results()[1].setName("SketchCircle_18_2")
SketchLine_13 = Sketch_2.addLine(0, 0, 0, 5)
SketchLine_13.setAuxiliary(True)
SketchConstraintCoincidence_23 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_13.startPoint())
SketchConstraintCoincidence_24 = Sketch_2.setCoincident(SketchCircle_15.center(), SketchLine_13.endPoint())
SketchLine_14 = Sketch_2.addLine(0, 0, 4.259590630302757, -2.982597469025442)
SketchLine_14.setAuxiliary(True)
SketchConstraintCoincidence_25 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_14.startPoint())
SketchConstraintCoincidence_26 = Sketch_2.setCoincident(SketchCircle_17.center(), SketchLine_14.endPoint())
SketchConstraintCoincidence_27 = Sketch_2.setCoincident(SketchCircle_16.center(), SketchLine_14.result())
SketchConstraintRadius_5 = Sketch_2.setRadius(SketchCircle_16.results()[1], 0.3)
SketchConstraintRadius_6 = Sketch_2.setRadius(SketchCircle_17.results()[1], 0.5)
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchAPI_Line(SketchLine_10).startPoint(), SketchCircle_16.center(), 4, True)
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchCircle_1.center(), SketchCircle_17.center(), 5.2, True)
SketchConstraintLength_6 = Sketch_2.setLength(SketchLine_13.result(), 5)
SketchConstraintRadius_7 = Sketch_2.setRadius(SketchCircle_15.results()[1], 0.45)
SketchConstraintAngle_4 = Sketch_2.setAngleBackward(SketchLine_10.result(), SketchLine_14.result(), 35)
SketchProjection_5 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_15 = SketchProjection_5.createdFeature()
SketchConstraintCoincidence_28 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_15.result())
model.do()
ExtrusionCut_1_objects_1 = [model.selection("WIRE", "Sketch_2/Face-SketchCircle_3_2f_wire"), model.selection("FACE", "Sketch_2/Face-SketchCircle_9_2f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_6_2f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_18_2f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_17_2f"), model.selection("WIRE", "Sketch_2/Face-SketchCircle_14_2f_wire"), model.selection("FACE", "Sketch_2/Face-SketchCircle_15_2f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_5_2f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_4_2f")]
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, ExtrusionCut_1_objects_1, model.selection(), model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7"), 0, model.selection(), 0, [model.selection("SOLID", "Revolution_1_1")])
ExtrusionCut_2_objects_1 = [model.selection("WIRE", "Sketch_2/Face-SketchCircle_11_2f-SketchCircle_15_2r_wire"), model.selection("WIRE", "Sketch_2/Face-SketchCircle_10_2f-SketchCircle_14_2r_wire"), model.selection("WIRE", "Sketch_2/Face-SketchCircle_8_2f-SketchCircle_9_2r_wire"), model.selection("WIRE", "Sketch_2/Face-SketchCircle_7_2f-SketchCircle_6_2r_wire")]
ExtrusionCut_2 = model.addExtrusionCut(Part_1_doc, ExtrusionCut_2_objects_1, model.selection(), 0, 0.75, [model.selection("SOLID", "ExtrusionCut_1_1")])
model.do()
model.end()

assert(model.checkPythonDump())
