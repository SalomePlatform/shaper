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
model.addParameter(partSet, "hh", "20")
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(0, -3.45, 0, -6.45)
SketchLine_2 = Sketch_1.addLine(0, -6.45, 7.781999999999999, -6.45)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(7.781999999999999, -6.45, 7.781999999999999, -3.45)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(7.781999999999999, -3.45, 6.537999941088452, -3.45)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(6.537999941088452, -3.45, 6.537999999999999, -2.4)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(6.537999999999999, -2.4, 6.375999999999999, -2.25)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(6.375999999999999, -2.25, 6.375999999999999, 9.025)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(6.375999999999999, 9.025, 6.537999999999998, 9.186999999999999)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_1.addLine(6.537999999999998, 9.186999999999999, 6.537999999999998, 10.256)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchLine_10 = Sketch_1.addLine(6.537999999999998, 10.256, 6.187999999999998, 10.256)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchLine_11 = Sketch_1.addLine(6.187999999999998, 10.256, 4.9175, 10.94235984621995)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_12 = SketchProjection_1.createdFeature()
SketchArc_1 = Sketch_1.addArc(0, 2.425, 0, 12.26, 4.9175, 10.94235984621995, True)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_12.result(), SketchArc_1.center())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_12.result(), SketchArc_1.startPoint())
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchArc_1.endPoint())
SketchLine_13 = Sketch_1.addLine(0, 12.26, 0, 12.096)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_13.startPoint())
SketchLine_14 = Sketch_1.addLine(0, -3.45, 6.187999958006518, -3.45)
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_14.startPoint())
SketchLine_15 = Sketch_1.addLine(6.187999958006518, -3.45, 6.188, 9.025)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchLine_16 = Sketch_1.addLine(6.188, 9.025, 6.082, 9.532)
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_14.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_10.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_9.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_7.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_15.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_14.result())
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_12.result())
SketchConstraintVertical_6 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_14.endPoint(), SketchLine_5.result(), 0.35, True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_6.endPoint(), SketchLine_15.result(), 0.188, True)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), 3)
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_17 = SketchProjection_2.createdFeature()
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_3.endPoint(), SketchLine_17.result(), 3.45, True)
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchAPI_Line(SketchLine_12).startPoint(), SketchLine_15.result(), 6.188, True)
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchLine_7.endPoint(), SketchLine_17.result(), 9.025, True)
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_12.result())
SketchLine_18 = Sketch_1.addLine(6.188, 9.025, 6.375999999999999, 9.025)
SketchLine_18.setAuxiliary(True)
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchLine_18.startPoint())
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_18.endPoint())
SketchConstraintHorizontal_5 = Sketch_1.setHorizontal(SketchLine_18.result())
SketchConstraintDistance_6 = Sketch_1.setDistance(SketchLine_10.startPoint(), SketchLine_17.result(), 10.256, True)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_10.result(), 0.35)
SketchConstraintAngle_1 = Sketch_1.setAngleComplementary(SketchLine_18.result(), SketchLine_8.result(), 45)
SketchConstraintDistance_7 = Sketch_1.setDistance(SketchLine_8.endPoint(), SketchLine_17.result(), 9.186999999999999, True)
SketchConstraintDistance_8 = Sketch_1.setDistance(SketchLine_16.endPoint(), SketchLine_15.result(), 0.106, True)
SketchConstraintDistance_9 = Sketch_1.setDistance(SketchLine_16.endPoint(), SketchLine_17.result(), 9.532, True)
SketchArc_2 = Sketch_1.addArc(4.793786120242238, 9.263, 6.082, 9.532, 5.676067550792405, 10.23944020672409, False)
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchLine_16.endPoint(), SketchArc_2.startPoint())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_2.results()[1], 1.316)
SketchConstraintDistance_10 = Sketch_1.setDistance(SketchArc_2.center(), SketchLine_17.result(), 9.263, True)
SketchLine_19 = Sketch_1.addLine(4.793786120242238, 9.263, 6.082, 9.532)
SketchLine_19.setAuxiliary(True)
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchArc_2.center(), SketchLine_19.startPoint())
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchLine_16.endPoint(), SketchLine_19.endPoint())
SketchLine_20 = Sketch_1.addLine(4.793786120242238, 9.263, 5.676067550792405, 10.23944020672409)
SketchLine_20.setAuxiliary(True)
SketchConstraintCoincidence_26 = Sketch_1.setCoincident(SketchArc_2.center(), SketchLine_20.startPoint())
SketchConstraintCoincidence_27 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_20.endPoint())
SketchLine_21 = Sketch_1.addLine(4.793786120242238, 9.263, 4.79378612024245, 10.72808112087842)
SketchLine_21.setAuxiliary(True)
SketchConstraintCoincidence_28 = Sketch_1.setCoincident(SketchArc_2.center(), SketchLine_21.startPoint())
SketchConstraintVertical_7 = Sketch_1.setVertical(SketchLine_21.result())
SketchConstraintAngle_2 = Sketch_1.setAngle(SketchLine_20.result(), SketchLine_21.result(), 42.09999999999906)
SketchLine_22 = Sketch_1.addLine(5.676067550792282, 10.23944020672395, 5.404900443923878, 10.43809443685872)
SketchConstraintCoincidence_29 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_22.startPoint())
SketchLine_23 = Sketch_1.addLine(5.404900443923878, 10.43809443685872, 5.12663857042824, 10.62933672216996)
SketchConstraintCoincidence_30 = Sketch_1.setCoincident(SketchLine_22.endPoint(), SketchLine_23.startPoint())
SketchLine_24 = Sketch_1.addLine(5.12663857042824, 10.62933672216996, 4.835500000000001, 10.80033167999931)
SketchConstraintCoincidence_31 = Sketch_1.setCoincident(SketchLine_23.endPoint(), SketchLine_24.startPoint())
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_13.result(), "0.082*2")
SketchArc_3 = Sketch_1.addArc(0, 2.425, 0, 12.096, 4.835500000000001, 10.80033167999931, True)
SketchConstraintCoincidence_32 = Sketch_1.setCoincident(SketchArc_1.center(), SketchArc_3.center())
SketchConstraintCoincidence_33 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchArc_3.startPoint())
SketchConstraintCoincidence_34 = Sketch_1.setCoincident(SketchLine_24.endPoint(), SketchArc_3.endPoint())
SketchConstraintDistance_11 = Sketch_1.setDistance(SketchArc_1.center(), SketchLine_17.result(), 2.425, True)
SketchLine_25 = Sketch_1.addLine(0, 2.425, 4.9175, 10.94235984621995)
SketchLine_25.setName("SketchLine_26")
SketchLine_25.result().setName("SketchLine_26")
SketchLine_25.setAuxiliary(True)
SketchConstraintCoincidence_35 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_25.startPoint())
SketchConstraintCoincidence_35.setName("SketchConstraintCoincidence_37")
SketchConstraintCoincidence_36 = Sketch_1.setCoincident(SketchLine_25.endPoint(), SketchLine_11.result())
SketchConstraintCoincidence_36.setName("SketchConstraintCoincidence_38")
SketchConstraintCoincidence_37 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_25.result())
SketchConstraintCoincidence_37.setName("SketchConstraintCoincidence_39")
SketchLine_26 = Sketch_1.addLine(0, 2.425, 5.220090907696442, 10.77889172431985)
SketchLine_26.setName("SketchLine_27")
SketchLine_26.result().setName("SketchLine_27")
SketchLine_26.setAuxiliary(True)
SketchConstraintCoincidence_38 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_26.startPoint())
SketchConstraintCoincidence_38.setName("SketchConstraintCoincidence_40")
SketchConstraintCoincidence_39 = Sketch_1.setCoincident(SketchLine_26.endPoint(), SketchLine_11.result())
SketchConstraintCoincidence_39.setName("SketchConstraintCoincidence_41")
SketchConstraintCoincidence_40 = Sketch_1.setCoincident(SketchLine_24.startPoint(), SketchLine_26.result())
SketchConstraintCoincidence_40.setName("SketchConstraintCoincidence_42")
SketchLine_27 = Sketch_1.addLine(0, 2.425, 5.524022094217663, 10.61469954608912)
SketchLine_27.setName("SketchLine_28")
SketchLine_27.result().setName("SketchLine_28")
SketchLine_27.setAuxiliary(True)
SketchConstraintCoincidence_41 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_27.startPoint())
SketchConstraintCoincidence_41.setName("SketchConstraintCoincidence_43")
SketchConstraintCoincidence_42 = Sketch_1.setCoincident(SketchLine_27.endPoint(), SketchLine_11.result())
SketchConstraintCoincidence_42.setName("SketchConstraintCoincidence_44")
SketchConstraintCoincidence_43 = Sketch_1.setCoincident(SketchLine_27.result(), SketchLine_23.startPoint())
SketchConstraintCoincidence_43.setName("SketchConstraintCoincidence_45")
SketchConstraintCoincidence_44 = Sketch_1.setCoincident(SketchLine_25.endPoint(), SketchArc_1.endPoint())
SketchConstraintCoincidence_44.setName("SketchConstraintCoincidence_46")
SketchConstraintAngle_3 = Sketch_1.setAngle(SketchLine_25.result(), SketchLine_12.result(), 30.00000000000003)
SketchConstraintAngle_4 = Sketch_1.setAngle(SketchLine_26.result(), SketchLine_12.result(), 32.00000000000001)
SketchConstraintAngle_5 = Sketch_1.setAngle(SketchLine_27.result(), SketchLine_12.result(), 33.99999999999999)
SketchConstraintDistance_12 = Sketch_1.setDistance(SketchLine_23.endPoint(), SketchLine_11.result(), "0.088*2", True)
SketchConstraintDistance_13 = Sketch_1.setDistance(SketchLine_22.endPoint(), SketchLine_11.result(), "0.106*2", True)
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_1.results()[1], 9.835000000000001)
SketchLine_28 = Sketch_1.addLine(7.031999999999999, -4.05, 5.531999999999999, -4.05)
SketchLine_28.setName("SketchLine_29")
SketchLine_28.result().setName("SketchLine_29")
SketchLine_29 = Sketch_1.addLine(5.531999999999999, -4.05, 5.531999999999999, -5.85)
SketchLine_29.setName("SketchLine_30")
SketchLine_29.result().setName("SketchLine_30")
SketchLine_30 = Sketch_1.addLine(5.531999999999999, -5.85, 7.031999999999999, -5.85)
SketchLine_30.setName("SketchLine_31")
SketchLine_30.result().setName("SketchLine_31")
SketchLine_31 = Sketch_1.addLine(7.031999999999999, -5.85, 7.031999999999999, -4.05)
SketchLine_31.setName("SketchLine_32")
SketchLine_31.result().setName("SketchLine_32")
SketchConstraintCoincidence_45 = Sketch_1.setCoincident(SketchLine_31.endPoint(), SketchLine_28.startPoint())
SketchConstraintCoincidence_45.setName("SketchConstraintCoincidence_47")
SketchConstraintCoincidence_46 = Sketch_1.setCoincident(SketchLine_28.endPoint(), SketchLine_29.startPoint())
SketchConstraintCoincidence_46.setName("SketchConstraintCoincidence_48")
SketchConstraintCoincidence_47 = Sketch_1.setCoincident(SketchLine_29.endPoint(), SketchLine_30.startPoint())
SketchConstraintCoincidence_47.setName("SketchConstraintCoincidence_49")
SketchConstraintCoincidence_48 = Sketch_1.setCoincident(SketchLine_30.endPoint(), SketchLine_31.startPoint())
SketchConstraintCoincidence_48.setName("SketchConstraintCoincidence_50")
SketchConstraintHorizontal_6 = Sketch_1.setHorizontal(SketchLine_28.result())
SketchConstraintVertical_8 = Sketch_1.setVertical(SketchLine_29.result())
SketchConstraintHorizontal_7 = Sketch_1.setHorizontal(SketchLine_30.result())
SketchConstraintVertical_9 = Sketch_1.setVertical(SketchLine_31.result())
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_30.result(), 1.5)
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_29.result(), 1.8)
SketchConstraintDistance_14 = Sketch_1.setDistance(SketchLine_29.startPoint(), SketchLine_14.result(), 0.6, True)
SketchLine_32 = Sketch_1.addLine(6.281999999999999, 9.025000000000009, 6.281999999999999, -4.049999999999998)
SketchLine_32.setName("SketchLine_33")
SketchLine_32.result().setName("SketchLine_33")
SketchLine_32.setAuxiliary(True)
SketchConstraintCoincidence_49 = Sketch_1.setCoincident(SketchLine_32.startPoint(), SketchLine_18.result())
SketchConstraintCoincidence_49.setName("SketchConstraintCoincidence_51")
SketchConstraintCoincidence_50 = Sketch_1.setCoincident(SketchLine_32.endPoint(), SketchLine_28.result())
SketchConstraintCoincidence_50.setName("SketchConstraintCoincidence_52")
SketchConstraintVertical_10 = Sketch_1.setVertical(SketchLine_32.result())
SketchConstraintMiddle_1 = Sketch_1.setMiddlePoint(SketchLine_32.endPoint(), SketchLine_28.result())
SketchConstraintMiddle_2 = Sketch_1.setMiddlePoint(SketchLine_32.startPoint(), SketchLine_18.result())
SketchConstraintDistance_15 = Sketch_1.setDistance(SketchLine_32.endPoint(), SketchLine_3.result(), 1.5, True)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchLine_6.endPoint(), SketchLine_5.endPoint(), 0.15)
SketchConstraintDistance_16 = Sketch_1.setDistance(SketchLine_6.endPoint(), SketchLine_17.result(), 2.25, True)
SketchConstraintCoincidence_51 = Sketch_1.setCoincident(SketchLine_21.result(), SketchLine_21.endPoint())
SketchConstraintCoincidence_51.setName("SketchConstraintCoincidence_53")
SketchConstraintCoincidence_52 = Sketch_1.setCoincident(SketchLine_25.result(), SketchLine_21.endPoint())
SketchConstraintCoincidence_52.setName("SketchConstraintCoincidence_54")
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f-SketchLine_5f-SketchLine_6f-SketchLine_7f-SketchLine_8f-SketchLine_9f-SketchLine_10f-SketchLine_11f-SketchArc_1_2f-SketchLine_13f-SketchArc_3_2r-SketchLine_24r-SketchLine_23r-SketchLine_22r-SketchArc_2_2r-SketchLine_16r-SketchLine_15r-SketchLine_14r-SketchLine_32r-SketchLine_31r-SketchLine_30r-SketchLine_29r")], model.selection("EDGE", "PartSet/OZ"), 360, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_33 = Sketch_2.addLine(0, 2.425, 3.282538500974061, 11.74164236673401)
SketchLine_33.setName("SketchLine_34")
SketchLine_33.result().setName("SketchLine_34")
SketchLine_33.setAuxiliary(True)
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_28_StartVertex"), False)
SketchPoint_1 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_53 = Sketch_2.setCoincident(SketchLine_33.startPoint(), SketchPoint_1.result())
SketchConstraintCoincidence_53.setName("SketchConstraintCoincidence_55")
SketchPoint_2 = Sketch_2.addPoint(3.241, 11.6237460014939)
SketchConstraintCoincidence_54 = Sketch_2.setCoincident(SketchPoint_2.coordinates(), SketchLine_33.result())
SketchConstraintCoincidence_54.setName("SketchConstraintCoincidence_56")
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_34 = SketchProjection_4.createdFeature()
SketchLine_34.setName("SketchLine_36")
SketchLine_34.result().setName("SketchLine_36")
SketchConstraintDistance_17 = Sketch_2.setDistance(SketchPoint_2.coordinates(), SketchLine_34.result(), 3.241, True)
SketchProjection_5 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_13"), False)
SketchLine_35 = SketchProjection_5.createdFeature()
SketchLine_35.setName("SketchLine_37")
SketchLine_35.result().setName("SketchLine_37")
SketchProjection_6 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_28"), False)
SketchLine_36 = SketchProjection_6.createdFeature()
SketchLine_36.setName("SketchLine_38")
SketchLine_36.result().setName("SketchLine_38")
SketchArc_4 = Sketch_2.addArc(0, 2.425, 0, 12.178, 5.453808387550194, 10.51060344512932, True)
SketchArc_4.setAuxiliary(True)
SketchConstraintCoincidence_55 = Sketch_2.setCoincident(SketchLine_33.startPoint(), SketchArc_4.center())
SketchConstraintCoincidence_55.setName("SketchConstraintCoincidence_58")
SketchConstraintCoincidence_56 = Sketch_2.setCoincident(SketchLine_35.result(), SketchArc_4.startPoint())
SketchConstraintCoincidence_56.setName("SketchConstraintCoincidence_59")
SketchConstraintCoincidence_57 = Sketch_2.setCoincident(SketchLine_36.result(), SketchArc_4.endPoint())
SketchConstraintCoincidence_57.setName("SketchConstraintCoincidence_60")
SketchConstraintMiddle_3 = Sketch_2.setMiddlePoint(SketchLine_35.result(), SketchArc_4.startPoint())
SketchConstraintCoincidence_58 = Sketch_2.setCoincident(SketchPoint_2.coordinates(), SketchArc_4.results()[1])
SketchConstraintCoincidence_58.setName("SketchConstraintCoincidence_61")
SketchArc_5 = Sketch_2.addArc(0, 2.425, 3.28253850097406, 11.74164236673401, 4.139817701932777, 11.39365616437401, True)
SketchConstraintCoincidence_59 = Sketch_2.setCoincident(SketchLine_33.startPoint(), SketchArc_5.center())
SketchConstraintCoincidence_59.setName("SketchConstraintCoincidence_62")
SketchConstraintCoincidence_60 = Sketch_2.setCoincident(SketchLine_33.result(), SketchArc_5.startPoint())
SketchConstraintCoincidence_60.setName("SketchConstraintCoincidence_63")
SketchLine_37 = Sketch_2.addLine(4.139817701932777, 11.39365616437401, 4.459277785066624, 11.1909606795611)
SketchLine_37.setName("SketchLine_39")
SketchLine_37.result().setName("SketchLine_39")
SketchConstraintCoincidence_61 = Sketch_2.setCoincident(SketchArc_5.endPoint(), SketchLine_37.startPoint())
SketchConstraintCoincidence_61.setName("SketchConstraintCoincidence_64")
SketchProjection_7 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchArc_1_2"), False)
SketchArc_6 = SketchProjection_7.createdFeature()
SketchConstraintCoincidence_62 = Sketch_2.setCoincident(SketchLine_37.endPoint(), SketchArc_6.results()[1])
SketchConstraintCoincidence_62.setName("SketchConstraintCoincidence_65")
SketchLine_38 = Sketch_2.addLine(4.459277785066624, 11.1909606795611, 4.390495384356073, 11.041947909785)
SketchLine_38.setName("SketchLine_40")
SketchLine_38.result().setName("SketchLine_40")
SketchConstraintCoincidence_63 = Sketch_2.setCoincident(SketchLine_37.endPoint(), SketchLine_38.startPoint())
SketchConstraintCoincidence_63.setName("SketchConstraintCoincidence_66")
SketchProjection_8 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchArc_3_2"), False)
SketchArc_7 = SketchProjection_8.createdFeature()
SketchConstraintCoincidence_64 = Sketch_2.setCoincident(SketchLine_38.endPoint(), SketchArc_7.results()[1])
SketchConstraintCoincidence_64.setName("SketchConstraintCoincidence_67")
SketchLine_39 = Sketch_2.addLine(4.390495384356073, 11.041947909785, 4.035044020470619, 11.16667053559354)
SketchLine_39.setName("SketchLine_41")
SketchLine_39.result().setName("SketchLine_41")
SketchConstraintCoincidence_65 = Sketch_2.setCoincident(SketchLine_38.endPoint(), SketchLine_39.startPoint())
SketchConstraintCoincidence_65.setName("SketchConstraintCoincidence_68")
SketchArc_8 = Sketch_2.addArc(0, 2.425, 3.199461499025941, 11.5058496362538, 4.035044020470619, 11.16667053559354, True)
SketchConstraintCoincidence_66 = Sketch_2.setCoincident(SketchLine_33.startPoint(), SketchArc_8.center())
SketchConstraintCoincidence_66.setName("SketchConstraintCoincidence_69")
SketchConstraintCoincidence_67 = Sketch_2.setCoincident(SketchLine_33.result(), SketchArc_8.startPoint())
SketchConstraintCoincidence_67.setName("SketchConstraintCoincidence_70")
SketchConstraintCoincidence_68 = Sketch_2.setCoincident(SketchArc_8.endPoint(), SketchLine_39.endPoint())
SketchConstraintCoincidence_68.setName("SketchConstraintCoincidence_71")
SketchLine_40 = Sketch_2.addLine(0, 2.425, 4.139817701932777, 11.39365616437401)
SketchLine_40.setName("SketchLine_42")
SketchLine_40.result().setName("SketchLine_42")
SketchLine_40.setAuxiliary(True)
SketchConstraintCoincidence_69 = Sketch_2.setCoincident(SketchLine_33.startPoint(), SketchLine_40.startPoint())
SketchConstraintCoincidence_69.setName("SketchConstraintCoincidence_72")
SketchConstraintCoincidence_70 = Sketch_2.setCoincident(SketchArc_5.endPoint(), SketchLine_40.endPoint())
SketchConstraintCoincidence_70.setName("SketchConstraintCoincidence_73")
SketchConstraintCoincidence_71 = Sketch_2.setCoincident(SketchArc_8.endPoint(), SketchLine_40.result())
SketchConstraintCoincidence_71.setName("SketchConstraintCoincidence_74")
SketchConstraintParallel_1 = Sketch_2.setParallel(SketchLine_38.result(), SketchLine_40.result())
SketchLine_41 = Sketch_2.addLine(0, 2.425, 3.9115060144283, 11.49555702253678)
SketchLine_41.setName("SketchLine_43")
SketchLine_41.result().setName("SketchLine_43")
SketchLine_41.setAuxiliary(True)
SketchConstraintCoincidence_72 = Sketch_2.setCoincident(SketchLine_33.startPoint(), SketchLine_41.startPoint())
SketchConstraintCoincidence_72.setName("SketchConstraintCoincidence_75")
SketchConstraintCoincidence_73 = Sketch_2.setCoincident(SketchLine_41.endPoint(), SketchArc_5.results()[1])
SketchConstraintCoincidence_73.setName("SketchConstraintCoincidence_76")
SketchPoint_3 = Sketch_2.addPoint(3.812510620258724, 11.26599240868436)
SketchConstraintCoincidence_74 = Sketch_2.setCoincident(SketchPoint_3.coordinates(), SketchLine_41.result())
SketchConstraintCoincidence_74.setName("SketchConstraintCoincidence_77")
SketchConstraintCoincidence_75 = Sketch_2.setCoincident(SketchPoint_3.coordinates(), SketchArc_8.results()[1])
SketchConstraintCoincidence_75.setName("SketchConstraintCoincidence_78")
SketchLine_42 = Sketch_2.addLine(3.812510620258724, 11.26599240868436, 3.9115060144283, 11.49555702253678)
SketchLine_42.setName("SketchLine_44")
SketchLine_42.result().setName("SketchLine_44")
SketchConstraintCoincidence_76 = Sketch_2.setCoincident(SketchPoint_3.coordinates(), SketchLine_42.startPoint())
SketchConstraintCoincidence_76.setName("SketchConstraintCoincidence_79")
SketchConstraintCoincidence_77 = Sketch_2.setCoincident(SketchLine_41.endPoint(), SketchLine_42.endPoint())
SketchConstraintCoincidence_77.setName("SketchConstraintCoincidence_80")
SketchPoint_4 = Sketch_2.addPoint(3.862008317343517, 11.38077471561057)
SketchConstraintCoincidence_78 = Sketch_2.setCoincident(SketchPoint_4.coordinates(), SketchLine_42.result())
SketchConstraintCoincidence_78.setName("SketchConstraintCoincidence_81")
SketchConstraintMiddle_4 = Sketch_2.setMiddlePoint(SketchPoint_4.coordinates(), SketchLine_42.result())
SketchConstraintCoincidence_79 = Sketch_2.setCoincident(SketchPoint_4.coordinates(), SketchArc_4.results()[1])
SketchConstraintCoincidence_79.setName("SketchConstraintCoincidence_82")
SketchConstraintDistance_18 = Sketch_2.setDistance(SketchLine_42.endPoint(), SketchLine_40.result(), 0.25, True)
SketchConstraintDistance_19 = Sketch_2.setDistance(SketchArc_5.endPoint(), SketchLine_38.result(), 0.375, True)
SketchConstraintDistance_20 = Sketch_2.setDistance(SketchArc_5.startPoint(), SketchLine_42.result(), "1.35/2", True)
SketchConstraintDistance_21 = Sketch_2.setDistance(SketchArc_8.startPoint(), SketchArc_5.startPoint(), 0.25, True)
SketchConstraintCoincidence_80 = Sketch_2.setCoincident(SketchLine_33.endPoint(), SketchArc_5.startPoint())
SketchConstraintCoincidence_80.setName("SketchConstraintCoincidence_83")
SketchLine_43 = Sketch_2.addLine(3.199461499025942, 11.5058496362538, 3.28253850097406, 11.74164236673401)
SketchLine_43.setName("SketchLine_45")
SketchLine_43.result().setName("SketchLine_45")
SketchConstraintCoincidence_81 = Sketch_2.setCoincident(SketchArc_8.startPoint(), SketchLine_43.startPoint())
SketchConstraintCoincidence_81.setName("SketchConstraintCoincidence_84")
SketchConstraintCoincidence_82 = Sketch_2.setCoincident(SketchLine_33.endPoint(), SketchLine_43.endPoint())
SketchConstraintCoincidence_82.setName("SketchConstraintCoincidence_85")
model.do()
Revolution_2 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchArc_5_2f-SketchLine_45r-SketchArc_8_2r-SketchLine_44f")], model.selection("EDGE", "Sketch_2/SketchLine_34"), 360, 0)
Revolution_2.result().setName("Cut_tool")
Revolution_3 = model.addRevolution(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchArc_5_2f-SketchLine_44r-SketchArc_8_2r-SketchLine_41r-SketchLine_40r-SketchLine_39r_wire")], model.selection("EDGE", "Sketch_2/SketchLine_34"), 360, 0)
Revolution_3.result().setName("Fuse_tool")
Symmetry_1 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Cut_tool"), model.selection("SOLID", "Fuse_tool")], model.selection("EDGE", "PartSet/OZ"), True)
Symmetry_1.result().subResult(0).setName("Symmetry_1_1_1")
Symmetry_1.result().subResult(1).setName("Symmetry_1_1_2")
Symmetry_1.results()[1].subResult(0).setName("Symmetry_1_2_1")
Symmetry_1.results()[1].subResult(1).setName("Symmetry_1_2_2")
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Revolution_1_1")], [model.selection("COMPOUND", "Cut_tool")])
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Cut_1_1"), model.selection("COMPOUND", "Fuse_tool")], True)
model.do()
model.testHaveNamingSubshapes(Fuse_1, model, Part_1_doc)
model.end()

assert(model.checkPythonDump())
