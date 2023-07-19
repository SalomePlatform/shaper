# Copyright (C) 2020-2023  CEA, EDF
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

from GeomAPI import *
from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "cir2", "22.225")
model.addParameter(Part_1_doc, "ext1", "3")
model.addParameter(Part_1_doc, "rib1", "13")
model.addParameter(Part_1_doc, "rib2", "13")
model.addParameter(Part_1_doc, "rib3", "13")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(193.5527547665121, 217.4086484957226, 25.4)
SketchCircle_2 = Sketch_1.addCircle(193.5527547665121, 217.4086484957226, 38)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchCircle_2.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_2.results()[1], 38)
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchCircle_1.results()[1], 25.4)
SketchLine_1 = Sketch_1.addLine(231.5527547665122, 217.4085749004042, 231.5527547665122, 96.40864849572259)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchCircle_2.results()[1])
SketchLine_2 = Sketch_1.addLine(212.5527547665122, 77.40864849572259, 79.55275476651215, 77.40864849572259)
SketchLine_3 = Sketch_1.addLine(79.55275476651215, 77.40864849572259, -35.01044899266839, 119.578215062531)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchArc_1 = Sketch_1.addArc(-28.44724523348785, 137.4086484957226, -35.01044899266839, 119.578215062531, -28.44724523348786, 156.4086484957226, True)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchArc_1.startPoint())
SketchLine_4 = Sketch_1.addLine(-28.44724523348786, 156.4086484957226, 96.55275476651215, 156.4086484957226)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(193.5527547665121, 217.4086484957226, 193.5527547665121, 96.40864849572259)
SketchLine_5.setAuxiliary(True)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(193.5527547665121, 96.40864849572259, -186.9071792634899, 96.40864849572259)
SketchLine_6.setAuxiliary(True)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_4.result(), SketchArc_1.results()[1])
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchLine_3.result(), SketchArc_1.results()[1])
SketchCircle_3 = Sketch_1.addCircle(-28.44724523348785, 137.4086484957226, 7)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchArc_1.center(), SketchCircle_3.center())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_1.result())
SketchArc_2 = Sketch_1.addArc(212.5527547665122, 96.40864849572259, 212.5527547665122, 77.40864849572259, 231.5527547665122, 96.40864849572259, False)
SketchArc_2.setName("SketchArc_3")
SketchArc_2.result().setName("SketchArc_3")
SketchArc_2.results()[1].setName("SketchArc_3_2")
SketchPoint_1 = Sketch_1.addPoint(231.5527547665122, 77.4086484957226)
SketchPoint_1.setAuxiliary(True)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchLine_5.startPoint(), SketchPoint_1.coordinates(), 140)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_1.result())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_2.result())
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_1.endPoint())
SketchConstraintTangent_3 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_1.result())
SketchConstraintTangent_4 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_2.result())
SketchConstraintRadius_3 = Sketch_1.setRadius(SketchArc_2.results()[1], 19)
SketchCircle_4 = Sketch_1.addCircle(212.5527547665122, 96.40864849572259, 7)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchArc_2.center(), SketchCircle_4.center())
SketchCircle_5 = Sketch_1.addCircle(149.5527547665122, 137.4086484957226, 7)
SketchLine_7 = Sketch_1.addLine(149.5527547665122, 137.4086484957226, -36.17794869484125, 137.4086484957226)
SketchLine_7.setAuxiliary(True)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchCircle_5.center(), SketchLine_7.startPoint())
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_7.result(), SketchLine_6.result())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_7.result())
SketchCircle_6 = Sketch_1.addCircle(79.55275476651215, 96.4086484957226, 7)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_6.result(), SketchCircle_6.center())
SketchCircle_7 = Sketch_1.addCircle(-3.447245233487854, 124.4086484957226, 5)
SketchCircle_8 = Sketch_1.addCircle(174.5527547665122, 96.4086484957226, 5)
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_6.result(), SketchCircle_8.center())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchCircle_3.results()[1], SketchCircle_6.results()[1])
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchCircle_6.results()[1], SketchCircle_5.results()[1])
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchCircle_5.results()[1], SketchCircle_4.results()[1])
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchArc_1.center(), SketchCircle_4.center(), 241)
SketchConstraintRadius_4 = Sketch_1.setRadius(SketchCircle_3.results()[1], 7)
SketchConstraintEqual_4 = Sketch_1.setEqual(SketchCircle_8.results()[1], SketchCircle_7.results()[1])
SketchConstraintRadius_5 = Sketch_1.setRadius(SketchCircle_7.results()[1], 5)
SketchConstraintDistanceHorizontal_2 = Sketch_1.setHorizontalDistance(SketchLine_3.startPoint(), SketchCircle_3.center(), 108)
SketchConstraintDistanceHorizontal_3 = Sketch_1.setHorizontalDistance(SketchLine_3.startPoint(), SketchLine_7.startPoint(), 70)
SketchConstraintDistanceHorizontal_4 = Sketch_1.setHorizontalDistance(SketchCircle_8.center(), SketchLine_7.startPoint(), 25)
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_3.startPoint(), SketchLine_6.result(), 19, True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_7.result(), SketchCircle_6.center(), 41, True)
SketchLine_8 = Sketch_1.addLine(79.55275476651215, 77.40864849572259, 79.55275476651215, 115.4086484957226)
SketchLine_8.setAuxiliary(True)
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_8.startPoint())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_8.result())
SketchConstraintMiddle_1 = Sketch_1.setMiddlePoint(SketchLine_8.result(), SketchCircle_6.center())
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchCircle_7.center(), SketchLine_7.result(), 13, True)
SketchConstraintDistanceHorizontal_5 = Sketch_1.setHorizontalDistance(SketchArc_1.center(), SketchCircle_7.center(), 25)
SketchConstraintRadius_6 = Sketch_1.setRadius(SketchArc_1.results()[1], 19)
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintTangent_5 = Sketch_1.setTangent(SketchLine_1.result(), SketchCircle_2.results()[1])
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_4.result(), 125)
SketchArc_3 = Sketch_1.addArc(96.55275476651215, 215.5823348515451, 96.55275476651215, 156.4086484957226, 155.6041596876128, 219.3845360273737, False)
SketchArc_3.setName("SketchArc_5")
SketchArc_3.result().setName("SketchArc_5")
SketchArc_3.results()[1].setName("SketchArc_5_2")
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchArc_3.startPoint())
SketchConstraintTangent_6 = Sketch_1.setTangent(SketchLine_4.result(), SketchArc_3.results()[1])
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchCircle_2.results()[1], SketchArc_3.endPoint())
model.do()
Sketch_1.changeFacesOrder([[SketchCircle_1.results()[1]],
                           [SketchCircle_2.results()[1], SketchCircle_2.results()[1], SketchArc_3.results()[1], SketchCircle_2.results()[1], SketchCircle_1.results()[1]],
                           [SketchCircle_4.results()[1]],
                           [SketchCircle_5.results()[1]],
                           [SketchCircle_6.results()[1]],
                           [SketchCircle_7.results()[1]],
                           [SketchCircle_8.results()[1]],
                           [SketchCircle_2.results()[1], SketchArc_3.results()[1]],
                           [SketchCircle_3.results()[1]],
                           [SketchArc_3.results()[1], SketchLine_4.result(), SketchArc_1.results()[1], SketchLine_3.result(), SketchLine_2.result(), SketchArc_2.results()[1], SketchLine_1.result(), SketchCircle_2.results()[1], SketchCircle_3.results()[1], SketchCircle_4.results()[1], SketchCircle_5.results()[1], SketchCircle_6.results()[1], SketchCircle_7.results()[1], SketchCircle_8.results()[1]]
                          ])
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchArc_5_2r-SketchLine_4r-SketchArc_1_2f-SketchLine_3r-SketchLine_2r-SketchArc_3_2f-SketchLine_1r-SketchCircle_2_2r-SketchCircle_3_2r-SketchCircle_4_2r-SketchCircle_5_2r-SketchCircle_6_2r-SketchCircle_7_2r-SketchCircle_8_2r"), model.selection("FACE", "Sketch_1/Face-SketchCircle_2_2f-SketchCircle_2_2f-SketchArc_5_2r-SketchCircle_2_2f-SketchCircle_1_2r")], model.selection(), 19, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_4"))
SketchLine_9 = Sketch_2.addLine(41.55275476651212, -117.4829288507397, 41.55275476651212, -28.90149327102993)
SketchLine_9.setAuxiliary(True)
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_9.result())
SketchLine_10 = Sketch_2.addLine(96.55275476651215, -19, 75.65570703399533, -109.515058097161)
SketchLine_11 = Sketch_2.addLine(7.449802499028871, -109.515058097161, -13.44724523348792, -19)
SketchLine_12 = Sketch_2.addLine(96.55275476651215, -19, -13.44724523348792, -19)
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchLine_10.startPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_22 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.endPoint())
SketchArc_4 = Sketch_2.addArc(41.5527547665121, -101.6417711951257, 75.65570703399533, -109.515058097161, 7.449802499028871, -109.515058097161, True)
SketchArc_4.setName("SketchArc_4")
SketchArc_4.result().setName("SketchArc_4")
SketchArc_4.results()[1].setName("SketchArc_4_2")
SketchConstraintCoincidence_23 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchArc_4.startPoint())
SketchConstraintCoincidence_24 = Sketch_2.setCoincident(SketchLine_11.startPoint(), SketchArc_4.endPoint())
SketchConstraintTangent_7 = Sketch_2.setTangent(SketchArc_4.results()[1], SketchLine_10.result())
SketchConstraintTangent_8 = Sketch_2.setTangent(SketchArc_4.results()[1], SketchLine_11.result())
SketchConstraintEqual_5 = Sketch_2.setEqual(SketchLine_10.result(), SketchLine_11.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_12.result())
SketchConstraintAngle_1 = Sketch_2.setAngle(SketchLine_10.result(), SketchLine_12.result(), 283, type = "Backward")
SketchConstraintRadius_7 = Sketch_2.setRadius(SketchArc_4.results()[1], 35)
SketchProjection_1 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_4"), True)
SketchLine_13 = SketchProjection_1.createdFeature()
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_5_StartVertex"), True)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchPoint_3 = Sketch_2.addPoint(41.55275476651212, -19)
SketchConstraintCoincidence_25 = Sketch_2.setCoincident(SketchPoint_3.coordinates(), SketchLine_9.result())
SketchConstraintCoincidence_26 = Sketch_2.setCoincident(SketchLine_12.result(), SketchPoint_3.coordinates())
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_5_2][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_4][Extrusion_1_1_1/To_Face]"), False)
SketchPoint_4 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_27 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_4).coordinates(), SketchLine_12.startPoint())
SketchConstraintDistanceHorizontal_6 = Sketch_2.setHorizontalDistance(SketchAPI_Point(SketchPoint_2).coordinates(), SketchPoint_3.coordinates(), 152)
SketchConstraintMiddle_2 = Sketch_2.setMiddlePoint(SketchLine_12.result(), SketchPoint_3.coordinates())
SketchCircle_9 = Sketch_2.addCircle(41.5527547665121, -101.6417711951257, 22.225)
SketchCircle_9.setName("SketchCircle_12")
SketchCircle_9.result().setName("SketchCircle_12")
SketchCircle_9.results()[1].setName("SketchCircle_12_2")
SketchConstraintCoincidence_28 = Sketch_2.setCoincident(SketchArc_4.center(), SketchCircle_9.center())
SketchConstraintRadius_8 = Sketch_2.setRadius(SketchCircle_9.results()[1], 22.225)
model.do()
ExtrusionFuse_1 = model.addExtrusionFuse(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_12f-SketchLine_11r-SketchArc_4_2f-SketchLine_10r-SketchCircle_12_2r")], model.selection(), 0, 19, [model.selection("SOLID", "Extrusion_1_1_1")])
ExtrusionFuse_2 = model.addExtrusionFuse(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_2_2f-SketchCircle_2_2f-SketchArc_5_2r-SketchCircle_2_2f-SketchCircle_1_2r")], model.selection(), 64, 0, [model.selection("SOLID", "ExtrusionFuse_1_1_1")])
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "ExtrusionFuse_1_1_2/From_Face"))
SketchCircle_10 = Sketch_3.addCircle(41.5527547664253, 101.6417711951057, 22.225)
SketchCircle_10.setName("SketchCircle_10")
SketchCircle_10.result().setName("SketchCircle_10")
SketchCircle_10.results()[1].setName("SketchCircle_10_2")
SketchConstraintRadius_9 = Sketch_3.setRadius(SketchCircle_10.results()[1], "cir2")
SketchProjection_4 = Sketch_3.addProjection(model.selection("EDGE", "[ExtrusionFuse_1_1_2/Generated_Face&Sketch_2/SketchArc_4_2][ExtrusionFuse_1_1_2/From_Face]"), False)
SketchArc_5 = SketchProjection_4.createdFeature()
SketchArc_5.setName("SketchArc_6")
SketchArc_5.result().setName("SketchArc_6")
SketchArc_5.results()[1].setName("SketchArc_6_2")
SketchCircle_11 = Sketch_3.addCircle(41.5527547664253, 101.6417711951057, 35)
SketchCircle_11.setName("SketchCircle_11")
SketchCircle_11.result().setName("SketchCircle_11")
SketchCircle_11.results()[1].setName("SketchCircle_11_2")
SketchConstraintCoincidence_29 = Sketch_3.setCoincident(SketchCircle_10.center(), SketchCircle_11.center())
SketchConstraintTangent_9 = Sketch_3.setTangent(SketchArc_5.results()[1], SketchCircle_11.results()[1])
SketchConstraintRadius_10 = Sketch_3.setRadius(SketchCircle_11.results()[1], 35)
model.do()
ExtrusionFuse_3 = model.addExtrusionFuse(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchCircle_11_2f-SketchCircle_10_2r")], model.selection(), 0, 55, [model.selection("SOLID", "ExtrusionFuse_2_1_1")])
Sketch_4 = model.addSketch(Part_1_doc, model.selection("FACE", "ExtrusionFuse_2_1_2/Modified_Face&Extrusion_1_1_1/From_Face"))
SketchLine_14 = Sketch_4.addLine(96.55275476651215, -156.4086484957226, 231.5527547665121, -156.4086484957226)
SketchProjection_5 = Sketch_4.addProjection(model.selection("VERTEX", "Sketch_2/SketchProjection_1_EndVertex"), False)
SketchPoint_5 = SketchProjection_5.createdFeature()
SketchConstraintCoincidence_30 = Sketch_4.setCoincident(SketchLine_14.startPoint(), SketchPoint_5.result())
SketchProjection_6 = Sketch_4.addProjection(model.selection("EDGE", "[ExtrusionFuse_2_1_2/Modified_Face&Sketch_1/SketchLine_1][ExtrusionFuse_2_1_2/Modified_Face&Extrusion_1_1_1/From_Face]"), False)
SketchLine_15 = SketchProjection_6.createdFeature()
SketchConstraintCoincidence_31 = Sketch_4.setCoincident(SketchLine_14.endPoint(), SketchLine_15.result())
SketchConstraintPerpendicular_1 = Sketch_4.setPerpendicular(SketchLine_14.result(), SketchLine_15.result())
SketchProjection_7 = Sketch_4.addProjection(model.selection("EDGE", "[ExtrusionFuse_2_1_2/Generated_Face&Sketch_1/SketchCircle_2_2][ExtrusionFuse_2_1_2/Modified_Face&Extrusion_1_1_1/From_Face]"), True)
SketchArc_6 = SketchProjection_7.createdFeature()
SketchArc_6.setName("SketchArc_7")
SketchArc_6.result().setName("SketchArc_7")
SketchArc_6.results()[1].setName("SketchArc_7_2")
SketchLine_16 = Sketch_4.addLine(231.5527547665121, -156.4086484957226, 231.5527547665122, -217.4085749004042)
SketchConstraintCoincidence_32 = Sketch_4.setCoincident(SketchLine_14.endPoint(), SketchLine_16.startPoint())
SketchConstraintCoincidence_33 = Sketch_4.setCoincident(SketchAPI_Line(SketchLine_15).startPoint(), SketchLine_16.endPoint())
SketchProjection_8 = Sketch_4.addProjection(model.selection("EDGE", "Sketch_1/SketchArc_5_2"), True)
SketchArc_7 = SketchProjection_8.createdFeature()
SketchArc_7.setName("SketchArc_8")
SketchArc_7.result().setName("SketchArc_8")
SketchArc_7.results()[1].setName("SketchArc_8_2")
model.do()
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_4")], model.selection(), 0, "ext1", [model.selection("SOLID", "ExtrusionFuse_3_1_1")])
Sketch_5 = model.addSketch(Part_1_doc, model.selection("FACE", "ExtrusionFuse_2_1_4/To_Face"))
SketchProjection_9 = Sketch_5.addProjection(model.selection("VERTEX", "Sketch_1/SketchCircle_1"), True)
SketchPoint_6 = SketchProjection_9.createdFeature()
model.do()
Plane_4 = model.addPlane(Part_1_doc, model.selection("EDGE", "Sketch_1/SketchLine_5"), model.selection("VERTEX", "Sketch_5/SketchPoint_6"), False)
Sketch_6 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchLine_17 = Sketch_6.addLine(181.9085749004042, -52.16576914538749, 181.9085749004042, -19)
SketchLine_18 = Sketch_6.addLine(181.9085749004042, -19, 110.7843534440984, -19)
SketchConstraintCoincidence_34 = Sketch_6.setCoincident(SketchLine_17.endPoint(), SketchLine_18.startPoint())
SketchLine_19 = Sketch_6.addLine(110.7843534440984, -19, 181.9085749004042, -52.16576914538749)
SketchConstraintCoincidence_35 = Sketch_6.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
SketchConstraintCoincidence_36 = Sketch_6.setCoincident(SketchLine_17.startPoint(), SketchLine_19.endPoint())
SketchLine_20 = Sketch_6.addLine(179.4085749004042, -51, 179.4085749004042, -19)
SketchProjection_10 = Sketch_6.addProjection(model.selection("EDGE", "[(Extrusion_1_1_2/To_Face)(ExtrusionFuse_2_1_4/To_Face)(ExtrusionFuse_2_1_4/Generated_Face&Sketch_1/SketchArc_5_2)2(ExtrusionFuse_2_1_4/Generated_Face&Sketch_1/SketchCircle_1_2)2][ExtrusionFuse_2_1_4/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_1]"), False)
SketchLine_21 = SketchProjection_10.createdFeature()
SketchConstraintParallel_2 = Sketch_6.setParallel(SketchLine_20.result(), SketchLine_21.result())
SketchConstraintVertical_5 = Sketch_6.setVertical(SketchLine_17.result())
SketchConstraintHorizontal_5 = Sketch_6.setHorizontal(SketchLine_18.result())
SketchProjection_11 = Sketch_6.addProjection(model.selection("EDGE", "[(ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchArc_5_2)(ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchCircle_2_2)(ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchLine_4)(ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchLine_1)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_3_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_3)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_1_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_6_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_8_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_4_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_7_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_3_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_5_2)][ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchLine_1]"), False)
SketchLine_22 = SketchProjection_11.createdFeature()
SketchConstraintCoincidence_37 = Sketch_6.setCoincident(SketchLine_22.result(), SketchLine_18.startPoint())
SketchConstraintCoincidence_38 = Sketch_6.setCoincident(SketchLine_20.startPoint(), SketchLine_19.result())
SketchConstraintCoincidence_39 = Sketch_6.setCoincident(SketchLine_20.endPoint(), SketchLine_18.result())
SketchConstraintDistanceHorizontal_7 = Sketch_6.setHorizontalDistance(SketchLine_20.startPoint(), SketchLine_19.endPoint(), 2.5)
SketchConstraintAngle_2 = Sketch_6.setAngle(SketchLine_18.result(), SketchLine_19.result(), 335, type = "Backward")
SketchConstraintLength_2 = Sketch_6.setLength(SketchLine_20.result(), 32)
SketchConstraintDistanceHorizontal_8 = Sketch_6.setHorizontalDistance(SketchLine_20.startPoint(), SketchAPI_Line(SketchLine_21).endPoint(), 38)
model.do()
ExtrusionFuse_4 = model.addExtrusionFuse(Part_1_doc, [model.selection("WIRE", "Sketch_6/Face-SketchLine_18f-SketchLine_19f-SketchLine_20f_wire"), model.selection("WIRE", "Sketch_6/Face-SketchLine_17r-SketchLine_18f-SketchLine_20r-SketchLine_19f_wire")], model.selection(), "rib1", "rib1", [model.selection("SOLID", "ExtrusionCut_1_1_2")])
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "Plane_1"), model.selection("VERTEX", "Sketch_3/SketchCircle_10"))
Sketch_7 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_2"))
SketchProjection_12 = Sketch_7.addProjection(model.selection("VERTEX", "ExtrusionFuse_3_1_4/Generated_Vertex&ExtrusionFuse_1_1_2/To_Face"), False)
SketchPoint_7 = SketchProjection_12.createdFeature()
SketchProjection_13 = Sketch_7.addProjection(model.selection("VERTEX", "Sketch_2/SketchPoint_3"), False)
SketchPoint_8 = SketchProjection_13.createdFeature()
model.do()
ExtrusionFuse_5 = model.addExtrusionFuse(Part_1_doc, [model.selection("FACE", "ExtrusionFuse_3_1_5/Modified_Face&ExtrusionFuse_1_1_2/From_Face&ExtrusionFuse_3_1_5/To_Face")], model.selection(), 5, 0, [model.selection("SOLID", "ExtrusionFuse_4_1_3")])
Sketch_8 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_2"))
SketchLine_23 = Sketch_8.addLine(137.4086484957226, -66.64177119510566, 137.4086484957226, -19)
SketchProjection_14 = Sketch_8.addProjection(model.selection("VERTEX", "[ExtrusionFuse_5_1_3/Modified_Face&ExtrusionFuse_1_1_2/From_Face][(ExtrusionFuse_5_1_3/Modified_Face&Sketch_2/SketchLine_10)(ExtrusionFuse_5_1_3/Modified_Face&ExtrusionFuse_1_1_2/From_Face)(ExtrusionFuse_3_1_6/Modified_Face&ExtrusionFuse_1_1_2/To_Face)(ExtrusionFuse_5_1_3/Modified_Face&Sketch_2/SketchLine_11)2][(ExtrusionFuse_5_1_3/Modified_Face&Sketch_2/SketchLine_11)(ExtrusionFuse_5_1_3/Modified_Face&ExtrusionFuse_1_1_2/From_Face)(ExtrusionFuse_3_1_6/Modified_Face&ExtrusionFuse_1_1_2/To_Face)(ExtrusionFuse_5_1_3/Modified_Face&Sketch_2/SketchLine_10)2]"), False)
SketchPoint_9 = SketchProjection_14.createdFeature()
SketchConstraintCoincidence_40 = Sketch_8.setCoincident(SketchLine_23.startPoint(), SketchPoint_9.result())
SketchLine_24 = Sketch_8.addLine(137.4086484957226, -19, 104.0495211712532, -19)
SketchConstraintCoincidence_41 = Sketch_8.setCoincident(SketchLine_23.endPoint(), SketchLine_24.startPoint())
SketchLine_25 = Sketch_8.addLine(104.0495211712532, -19, 137.4086484957226, -66.64177119510566)
SketchConstraintCoincidence_42 = Sketch_8.setCoincident(SketchLine_24.endPoint(), SketchLine_25.startPoint())
SketchConstraintCoincidence_43 = Sketch_8.setCoincident(SketchLine_23.startPoint(), SketchLine_25.endPoint())
SketchConstraintHorizontal_6 = Sketch_8.setHorizontal(SketchLine_24.result())
SketchConstraintVertical_6 = Sketch_8.setVertical(SketchLine_23.result())
SketchProjection_15 = Sketch_8.addProjection(model.selection("EDGE", "[(ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchArc_5_2)(ExtrusionFuse_4_1_5/Modified_Face&Sketch_1/SketchCircle_2_2)(ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchLine_4)(ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchLine_1)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_3_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_3)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_1_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_6_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_8_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_4_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_7_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_3_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_5_2)][ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchLine_1]"), False)
SketchLine_26 = SketchProjection_15.createdFeature()
SketchConstraintCoincidence_44 = Sketch_8.setCoincident(SketchLine_24.startPoint(), SketchLine_26.result())
SketchConstraintAngle_3 = Sketch_8.setAngle(SketchLine_25.result(), SketchLine_24.result(), 55, type = "Direct")
model.do()
Sketch_9 = model.addSketch(Part_1_doc, model.selection("FACE", "ExtrusionFuse_3_1_4/From_Face"))
SketchProjection_16 = Sketch_9.addProjection(model.selection("VERTEX", "Sketch_2/SketchCircle_12"), True)
SketchPoint_10 = SketchProjection_16.createdFeature()
model.do()
Sketch_10 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_2"))
SketchLine_27 = Sketch_10.addLine(156.4086484957226, -101.6417711951257, 192.4086484957226, -101.6417711951257)
SketchLine_27.setAuxiliary(True)
SketchProjection_17 = Sketch_10.addProjection(model.selection("VERTEX", "Sketch_2/SketchCircle_12"), False)
SketchPoint_11 = SketchProjection_17.createdFeature()
SketchConstraintCoincidence_45 = Sketch_10.setCoincident(SketchLine_27.startPoint(), SketchPoint_11.result())
SketchProjection_18 = Sketch_10.addProjection(model.selection("VERTEX", "Sketch_9/SketchPoint_10"), False)
SketchPoint_12 = SketchProjection_18.createdFeature()
SketchConstraintCoincidence_46 = Sketch_10.setCoincident(SketchLine_27.endPoint(), SketchPoint_12.result())
SketchProjection_19 = Sketch_10.addProjection(model.selection("EDGE", "([ExtrusionFuse_3_1_4/Generated_Face&Sketch_3/SketchCircle_11_2][ExtrusionFuse_3_1_4/From_Face])"), False)
SketchLine_28 = SketchProjection_19.createdFeature()
SketchLine_28.setName("SketchLine_29")
SketchLine_28.result().setName("SketchLine_29")
SketchLine_29 = Sketch_10.addLine(191.4029952552692, -71.64177119510568, 156.4086484957226, -71.64177119510568)
SketchLine_29.setName("SketchLine_30")
SketchLine_29.result().setName("SketchLine_30")
SketchProjection_20 = Sketch_10.addProjection(model.selection("EDGE", "Sketch_2/SketchLine_9"), False)
SketchLine_30 = SketchProjection_20.createdFeature()
SketchLine_30.setName("SketchLine_31")
SketchLine_30.result().setName("SketchLine_31")
SketchConstraintCoincidence_47 = Sketch_10.setCoincident(SketchLine_29.endPoint(), SketchLine_30.result())
SketchLine_31 = Sketch_10.addLine(156.4086484957226, -71.64177119510568, 156.4086484957226, -15.63910975456751)
SketchLine_31.setName("SketchLine_32")
SketchLine_31.result().setName("SketchLine_32")
SketchConstraintCoincidence_48 = Sketch_10.setCoincident(SketchLine_29.endPoint(), SketchLine_31.startPoint())
SketchLine_32 = Sketch_10.addLine(156.4086484957226, -15.63910975456751, 191.4029952552692, -71.64177119510568)
SketchLine_32.setName("SketchLine_33")
SketchLine_32.result().setName("SketchLine_33")
SketchConstraintCoincidence_49 = Sketch_10.setCoincident(SketchLine_31.endPoint(), SketchLine_32.startPoint())
SketchConstraintCoincidence_50 = Sketch_10.setCoincident(SketchLine_29.startPoint(), SketchLine_32.endPoint())
SketchConstraintHorizontal_7 = Sketch_10.setHorizontal(SketchLine_29.result())
SketchConstraintVertical_7 = Sketch_10.setVertical(SketchLine_31.result())
SketchConstraintAngle_4 = Sketch_10.setAngle(SketchLine_32.result(), SketchLine_31.result(), 328, type = "Backward")
SketchLine_33 = Sketch_10.addLine(188.2786484957226, -66.64177119510568, 156.4086484957226, -66.64177119510568)
SketchLine_33.setName("SketchLine_34")
SketchLine_33.result().setName("SketchLine_34")
SketchConstraintCoincidence_51 = Sketch_10.setCoincident(SketchLine_33.endPoint(), SketchLine_31.result())
SketchConstraintParallel_3 = Sketch_10.setParallel(SketchLine_33.result(), SketchLine_29.result())
SketchConstraintCoincidence_52 = Sketch_10.setCoincident(SketchLine_33.startPoint(), SketchLine_32.result())
SketchConstraintDistanceVertical_2 = Sketch_10.setVerticalDistance(SketchLine_32.endPoint(), SketchLine_33.startPoint(), 5)
SketchConstraintLength_3 = Sketch_10.setLength(SketchLine_33.result(), 31.87)
SketchConstraintCoincidence_53 = Sketch_10.setCoincident(SketchLine_28.result(), SketchLine_33.startPoint())
model.do()
ExtrusionFuse_6 = model.addExtrusionFuse(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_10")], model.selection(), "rib2", "rib2", [model.selection("SOLID", "ExtrusionFuse_5_1_3"), model.selection("SOLID", "ExtrusionFuse_5_1_2")])
ExtrusionFuse_7 = model.addExtrusionFuse(Part_1_doc, [model.selection("FACE", "Sketch_8/Face-SketchLine_23r-SketchLine_24f-SketchLine_25f")], model.selection(), "rib3", "rib3", [model.selection("SOLID", "ExtrusionFuse_6_1_7")])
Sketch_11 = model.addSketch(Part_1_doc, model.selection("FACE", "(ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchArc_5_2)(ExtrusionFuse_4_1_5/Modified_Face&Sketch_1/SketchCircle_2_2)(ExtrusionFuse_7_1_2/Generated_Face&Sketch_8/SketchLine_24)(ExtrusionFuse_4_1_7/Modified_Face&Extrusion_1_1_1/To_Face)(ExtrusionFuse_4_1_6/Modified_Face&Extrusion_1_1_1/To_Face)(ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchLine_1)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_3_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_3)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_1_2)(ExtrusionFuse_7_1_2/Modified_Face&Sketch_2/SketchLine_12&Extrusion_1_1_1/To_Face)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_6_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_8_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_4_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_7_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_3_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_5_2)"))
SketchProjection_21 = Sketch_11.addProjection(model.selection("VERTEX", "[(ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchArc_5_2)(ExtrusionFuse_4_1_5/Modified_Face&Sketch_1/SketchCircle_2_2)(ExtrusionFuse_7_1_2/Generated_Face&Sketch_8/SketchLine_24)(ExtrusionFuse_4_1_7/Modified_Face&Extrusion_1_1_1/To_Face)(ExtrusionFuse_4_1_6/Modified_Face&Extrusion_1_1_1/To_Face)(ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchLine_1)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_3_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_3)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_1_2)(ExtrusionFuse_7_1_2/Modified_Face&Sketch_2/SketchLine_12&Extrusion_1_1_1/To_Face)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_6_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_8_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_4_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_7_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_3_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_5_2)][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_1_2]__cc"), False)
SketchPoint_13 = SketchProjection_21.createdFeature()
SketchCircle_12 = Sketch_11.addCircle(-28.44724523348785, 137.4086484957226, 13)
SketchCircle_12.setName("SketchCircle_13")
SketchCircle_12.result().setName("SketchCircle_13")
SketchCircle_12.results()[1].setName("SketchCircle_13_2")
SketchConstraintCoincidence_54 = Sketch_11.setCoincident(SketchPoint_13.result(), SketchCircle_12.center())
SketchProjection_22 = Sketch_11.addProjection(model.selection("VERTEX", "[(ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchArc_5_2)(ExtrusionFuse_4_1_5/Modified_Face&Sketch_1/SketchCircle_2_2)(ExtrusionFuse_7_1_2/Generated_Face&Sketch_8/SketchLine_24)(ExtrusionFuse_4_1_7/Modified_Face&Extrusion_1_1_1/To_Face)(ExtrusionFuse_4_1_6/Modified_Face&Extrusion_1_1_1/To_Face)(ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchLine_1)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_3_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_3)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_1_2)(ExtrusionFuse_7_1_2/Modified_Face&Sketch_2/SketchLine_12&Extrusion_1_1_1/To_Face)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_6_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_8_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_4_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_7_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_3_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_5_2)][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_6_2]__cc"), False)
SketchPoint_14 = SketchProjection_22.createdFeature()
SketchCircle_13 = Sketch_11.addCircle(79.55275476651215, 96.4086484957226, 13)
SketchCircle_13.setName("SketchCircle_14")
SketchCircle_13.result().setName("SketchCircle_14")
SketchCircle_13.results()[1].setName("SketchCircle_14_2")
SketchConstraintCoincidence_55 = Sketch_11.setCoincident(SketchPoint_14.result(), SketchCircle_13.center())
SketchProjection_23 = Sketch_11.addProjection(model.selection("VERTEX", "[(ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchArc_5_2)(ExtrusionFuse_4_1_5/Modified_Face&Sketch_1/SketchCircle_2_2)(ExtrusionFuse_7_1_2/Generated_Face&Sketch_8/SketchLine_24)(ExtrusionFuse_4_1_7/Modified_Face&Extrusion_1_1_1/To_Face)(ExtrusionFuse_4_1_6/Modified_Face&Extrusion_1_1_1/To_Face)(ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchLine_1)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_3_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_3)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_1_2)(ExtrusionFuse_7_1_2/Modified_Face&Sketch_2/SketchLine_12&Extrusion_1_1_1/To_Face)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_6_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_8_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_4_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_7_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_3_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_5_2)][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_5_2]__cc"), False)
SketchPoint_15 = SketchProjection_23.createdFeature()
SketchCircle_14 = Sketch_11.addCircle(149.5527547665122, 137.4086484957226, 13)
SketchCircle_14.setName("SketchCircle_15")
SketchCircle_14.result().setName("SketchCircle_15")
SketchCircle_14.results()[1].setName("SketchCircle_15_2")
SketchConstraintCoincidence_56 = Sketch_11.setCoincident(SketchPoint_15.result(), SketchCircle_14.center())
SketchProjection_24 = Sketch_11.addProjection(model.selection("VERTEX", "[(ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchArc_5_2)(ExtrusionFuse_4_1_5/Modified_Face&Sketch_1/SketchCircle_2_2)(ExtrusionFuse_7_1_2/Generated_Face&Sketch_8/SketchLine_24)(ExtrusionFuse_4_1_7/Modified_Face&Extrusion_1_1_1/To_Face)(ExtrusionFuse_4_1_6/Modified_Face&Extrusion_1_1_1/To_Face)(ExtrusionCut_1_1_6/Modified_Face&Sketch_1/SketchLine_1)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_3_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_3)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_1_2)(ExtrusionFuse_7_1_2/Modified_Face&Sketch_2/SketchLine_12&Extrusion_1_1_1/To_Face)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_6_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_8_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_4_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_7_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_3_2)(Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_5_2)][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_3_2]__cc"), False)
SketchPoint_16 = SketchProjection_24.createdFeature()
SketchCircle_15 = Sketch_11.addCircle(212.5527547665122, 96.40864849572259, 13)
SketchCircle_15.setName("SketchCircle_16")
SketchCircle_15.result().setName("SketchCircle_16")
SketchCircle_15.results()[1].setName("SketchCircle_16_2")
SketchConstraintCoincidence_57 = Sketch_11.setCoincident(SketchPoint_16.result(), SketchCircle_15.center())
SketchConstraintEqual_6 = Sketch_11.setEqual(SketchCircle_12.results()[1], SketchCircle_13.results()[1])
SketchConstraintEqual_7 = Sketch_11.setEqual(SketchCircle_13.results()[1], SketchCircle_14.results()[1])
SketchConstraintEqual_8 = Sketch_11.setEqual(SketchCircle_14.results()[1], SketchCircle_15.results()[1])
SketchConstraintRadius_11 = Sketch_11.setRadius(SketchCircle_12.results()[1], 13)
model.do()
ExtrusionCut_2 = model.addExtrusionCut(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_11")], model.selection(), 0, 2, [model.selection("SOLID", "ExtrusionFuse_7_1_2")])
Fillet_1 = model.addFillet(Part_1_doc, [model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_2]")], 19)

model.end()

model.testNbResults(Fillet_1, 1)
model.testNbSubResults(Fillet_1, [15])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.SOLID, [15])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.FACE, [134])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.EDGE, [624])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.VERTEX, [1248])
model.testResultsVolumes(Fillet_1, [867199.641696438])

assert(model.checkPythonDump(model.ModelHighAPI.CHECK_NAMING))
