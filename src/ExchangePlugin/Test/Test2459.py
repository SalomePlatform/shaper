## Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchLine_1 = Sketch_1.addLine(-7.500000000000001, -17.0293863659264, -7.500000000000001, -27)
SketchLine_1.setName("SketchLine_2")
SketchLine_1.result().setName("SketchLine_2")
SketchLine_2 = Sketch_1.addLine(-4.500000000000001, -30, 4.500000000000001, -30)
SketchLine_2.setName("SketchLine_3")
SketchLine_2.result().setName("SketchLine_3")
SketchLine_3 = Sketch_1.addLine(7.500000000000001, -27, 7.500000000000001, -17.0293863659264)
SketchLine_3.setName("SketchLine_4")
SketchLine_3.result().setName("SketchLine_4")
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_3.result())
SketchArc_1 = Sketch_1.addArc(-4.500000000000001, -27, -7.500000000000001, -27, -4.500000000000001, -30, False)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_1.endPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_1.result())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_2.result())
SketchArc_2 = Sketch_1.addArc(4.500000000000001, -27, 4.500000000000001, -30, 7.500000000000001, -27, False)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_2.endPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintTangent_3 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_3.result())
SketchConstraintTangent_4 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_2.result())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 3)
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchArc_1.results()[1], SketchArc_2.results()[1])
SketchPoint_2 = Sketch_1.addPoint(-2.952241104523109e-048, -30)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_2.result())
SketchConstraintMiddle_1 = Sketch_1.setMiddlePoint(SketchPoint_2.coordinates(), SketchLine_2.result())
SketchArc_3 = Sketch_1.addArc(0, 0, 8.525641025641026, -15.2827826360878, 17.49809851618825, 0.2579696062919406, False)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchArc_3.center())
SketchArc_4 = Sketch_1.addArc(-9.500000000000002, -17.0293863659264, -7.500000000000001, -17.0293863659264, -8.525641025641027, -15.2827826360878, False)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchArc_4.startPoint(), SketchLine_1.startPoint())
SketchConstraintTangent_5 = Sketch_1.setTangent(SketchArc_4.results()[1], SketchLine_1.result())
SketchArc_5 = Sketch_1.addArc(9.5, -17.0293863659264, 8.525641025641026, -15.2827826360878, 7.500000000000001, -17.0293863659264, False)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchArc_5.startPoint(), SketchArc_3.startPoint())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchArc_5.endPoint(), SketchLine_3.endPoint())
SketchConstraintTangent_6 = Sketch_1.setTangent(SketchArc_5.results()[1], SketchLine_3.result())
SketchConstraintTangent_7 = Sketch_1.setTangent(SketchArc_5.results()[1], SketchArc_3.results()[1])
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_4.results()[1], 2)
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchArc_4.results()[1], SketchArc_5.results()[1])
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchArc_1.startPoint(), SketchLine_3.result(), 15, True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_2.result(), 30, True)
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_4 = SketchProjection_2.createdFeature()
SketchLine_4.setName("SketchLine_5")
SketchLine_4.result().setName("SketchLine_5")
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_4.result(), SketchPoint_2.coordinates())
SketchCircle_1 = Sketch_1.addCircle(0, -24, 4)
SketchMultiRotation_1_objects = [SketchArc_4.results()[1], SketchLine_1.result(), SketchArc_1.results()[1], SketchLine_2.result(), SketchArc_2.results()[1], SketchLine_3.result(), SketchArc_5.results()[1], SketchCircle_1.results()[1]]
SketchMultiRotation_1 = Sketch_1.addRotation(SketchMultiRotation_1_objects, SketchAPI_Line(SketchLine_4).startPoint(), "360/3", 3)
[SketchArc_6, SketchArc_7, SketchLine_5, SketchLine_6, SketchArc_8, SketchArc_9, SketchLine_7, SketchLine_8, SketchArc_10, SketchArc_11, SketchLine_9, SketchLine_10, SketchArc_12, SketchArc_13, SketchCircle_2, SketchCircle_3] = SketchMultiRotation_1.rotated()
SketchLine_10.setName("SketchLine_11")
SketchLine_10.result().setName("SketchLine_11")
SketchLine_9.setName("SketchLine_10")
SketchLine_9.result().setName("SketchLine_10")
SketchLine_8.setName("SketchLine_9")
SketchLine_8.result().setName("SketchLine_9")
SketchLine_7.setName("SketchLine_8")
SketchLine_7.result().setName("SketchLine_8")
SketchLine_6.setName("SketchLine_7")
SketchLine_6.result().setName("SketchLine_7")
SketchLine_5.setName("SketchLine_6")
SketchLine_5.result().setName("SketchLine_6")
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_4.result())
SketchConstraintRadius_3 = Sketch_1.setRadius(SketchCircle_1.results()[1], 4)
SketchProjection_3 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_11 = SketchProjection_3.createdFeature()
SketchLine_11.setName("SketchLine_12")
SketchLine_11.result().setName("SketchLine_12")
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_11.result(), 24, True)
SketchArc_14 = Sketch_1.addArc(0, 0, -17.49809851618825, 0.2579696062919513, -8.525641025641027, -15.2827826360878, False)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchArc_14.endPoint())
SketchConstraintTangent_8 = Sketch_1.setTangent(SketchArc_4.results()[1], SketchArc_14.results()[1])
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchArc_3.results()[1], SketchArc_14.results()[1])
SketchArc_15 = Sketch_1.addArc(0, 0, 8.97245749054723, 15.02481302979584, -8.972457490547235, 15.02481302979584, False)
SketchConstraintEqual_4 = Sketch_1.setEqual(SketchArc_3.results()[1], SketchArc_15.results()[1])
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchAPI_Arc(SketchArc_13).startPoint(), SketchArc_14.startPoint())
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchArc_15.endPoint(), SketchAPI_Arc(SketchArc_7).endPoint())
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchArc_15.startPoint(), SketchAPI_Arc(SketchArc_12).startPoint())
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchAPI_Arc(SketchArc_6).endPoint())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_2f-SketchArc_1_2f-SketchLine_3f-SketchArc_2_2f-SketchLine_4f-SketchArc_5_2r-SketchArc_3_2f-SketchArc_6_2r-SketchLine_6f-SketchArc_8_2f-SketchLine_8f-SketchArc_10_2f-SketchLine_10f-SketchArc_12_2r-SketchArc_15_2f-SketchArc_7_2r-SketchLine_7f-SketchArc_9_2f-SketchLine_9f-SketchArc_11_2f-SketchLine_11f-SketchArc_13_2r-SketchArc_14_2f-SketchArc_4_2r-SketchCircle_1_2r-SketchCircle_2_2r-SketchCircle_3_2r")], model.selection(), 10, 0)
model.do()
model.end()

assert(model.checkPythonDump())
