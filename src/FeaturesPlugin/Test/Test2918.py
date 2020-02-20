# Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(6.188, 9.028, 6.188, -3.45)
SketchLine_2 = Sketch_1.addLine(6.188, -3.45, 0, -3.45)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(0, -3.45, 0, -6.45)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(0, -6.45, 7.782, -6.45)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(7.782, -6.45, 7.782, -3.45)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(7.782, -3.45, 6.538, -3.45)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(6.538, -3.45, 6.538, -2.4)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(6.538, -2.4, 6.376, -2.25)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_1.addLine(6.376, -2.25, 6.376, 9.028)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchLine_10 = Sketch_1.addLine(6.376, 9.028, 6.188, 9.028)
SketchLine_10.setAuxiliary(True)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_10.endPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_10.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_9.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_7.startPoint(), SketchLine_2.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_11 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchLine_11.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_5.result(), 3)
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchAPI_Line(SketchLine_11).startPoint(), SketchLine_1.result(), 6.188, True)
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_7.result())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_12 = SketchProjection_2.createdFeature()
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_9.endPoint(), SketchLine_12.result(), 9.028, True)
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_2.startPoint(), SketchLine_9.result(), 0.188, True)
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchLine_2.startPoint(), SketchLine_7.result(), 0.35, True)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchLine_7.endPoint(), SketchLine_8.endPoint(), 0.15)
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchLine_5.endPoint(), SketchLine_12.result(), 3.45, True)
SketchConstraintDistance_6 = Sketch_1.setDistance(SketchLine_8.endPoint(), SketchLine_12.result(), 2.25, True)
SketchLine_13 = Sketch_1.addLine(7.032, -4.05, 5.532, -4.05)
SketchLine_14 = Sketch_1.addLine(5.532, -4.05, 5.532, -5.85)
SketchLine_15 = Sketch_1.addLine(5.532, -5.85, 7.032, -5.85)
SketchLine_16 = Sketch_1.addLine(7.032, -5.85, 7.032, -4.05)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_16.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchConstraintHorizontal_5 = Sketch_1.setHorizontal(SketchLine_13.result())
SketchConstraintVertical_6 = Sketch_1.setVertical(SketchLine_14.result())
SketchConstraintHorizontal_6 = Sketch_1.setHorizontal(SketchLine_15.result())
SketchConstraintVertical_7 = Sketch_1.setVertical(SketchLine_16.result())
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_14.result(), 1.8)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_15.result(), 1.5)
SketchLine_17 = Sketch_1.addLine(6.282, -4.05, 6.282, 9.028)
SketchLine_17.setAuxiliary(True)
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_17.startPoint(), SketchLine_13.result())
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_17.endPoint(), SketchLine_10.result())
SketchConstraintVertical_8 = Sketch_1.setVertical(SketchLine_17.result())
SketchConstraintMiddle_1 = Sketch_1.setMiddlePoint(SketchLine_17.startPoint(), SketchLine_13.result())
SketchConstraintDistance_7 = Sketch_1.setDistance(SketchLine_17.startPoint(), SketchLine_5.result(), 1.5, True)
SketchConstraintMiddle_2 = Sketch_1.setMiddlePoint(SketchLine_10.result(), SketchLine_17.endPoint())
SketchConstraintDistance_8 = Sketch_1.setDistance(SketchLine_14.startPoint(), SketchLine_2.result(), 0.6, True)
SketchLine_18 = Sketch_1.addLine(6.376, 9.028, 6.535, 9.187)
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_18.startPoint())
SketchLine_19 = Sketch_1.addLine(6.535, 9.187, 6.535, 10.256)
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
SketchLine_20 = Sketch_1.addLine(6.535, 10.256, 6.185, 10.256)
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchLine_19.endPoint(), SketchLine_20.startPoint())
SketchLine_21 = Sketch_1.addLine(6.185, 10.256, 4.9175, 10.94235984621998)
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchLine_20.endPoint(), SketchLine_21.startPoint())
SketchConstraintHorizontal_7 = Sketch_1.setHorizontal(SketchLine_20.result())
SketchConstraintVertical_9 = Sketch_1.setVertical(SketchLine_19.result())
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_10.result(), SketchLine_18.result(), 45, type = "Supplementary")
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_20.result(), 0.35)
SketchConstraintDistance_9 = Sketch_1.setDistance(SketchLine_19.endPoint(), SketchLine_12.result(), 10.256, True)
SketchArc_1 = Sketch_1.addArc(0, 2.425, 4.9175, 10.94235984621998, 0, 12.26, False)
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchLine_11.result(), SketchArc_1.center())
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchLine_21.endPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchLine_11.result(), SketchArc_1.endPoint())
SketchLine_22 = Sketch_1.addLine(6.188, 9.028, 6.082, 9.532)
SketchConstraintCoincidence_26 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_22.startPoint())
SketchArc_2 = Sketch_1.addArc(4.79378612024245, 9.263, 6.082, 9.532, 5.676067550792229, 10.23944020672391, False)
SketchConstraintCoincidence_27 = Sketch_1.setCoincident(SketchLine_22.endPoint(), SketchArc_2.startPoint())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_2.results()[1], 1.316)
SketchConstraintDistance_10 = Sketch_1.setDistance(SketchArc_2.startPoint(), SketchLine_1.result(), 0.106, True)
SketchLine_23 = Sketch_1.addLine(5.676067550792229, 10.23944020672391, 5.405090045827156, 10.43837553323928)
SketchConstraintCoincidence_28 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_23.startPoint())
SketchLine_24 = Sketch_1.addLine(5.405090045827156, 10.43837553323928, 5.126644475052085, 10.62934617154252)
SketchConstraintCoincidence_29 = Sketch_1.setCoincident(SketchLine_23.endPoint(), SketchLine_24.startPoint())
SketchLine_25 = Sketch_1.addLine(5.126644475052085, 10.62934617154252, 4.8355, 10.80033167999934)
SketchConstraintCoincidence_30 = Sketch_1.setCoincident(SketchLine_24.endPoint(), SketchLine_25.startPoint())
SketchArc_3 = Sketch_1.addArc(0, 2.425, 4.8355, 10.80033167999934, 0, 12.096, False)
SketchConstraintCoincidence_31 = Sketch_1.setCoincident(SketchLine_11.result(), SketchArc_3.center())
SketchConstraintCoincidence_32 = Sketch_1.setCoincident(SketchLine_25.endPoint(), SketchArc_3.startPoint())
SketchConstraintCoincidence_33 = Sketch_1.setCoincident(SketchLine_11.result(), SketchArc_3.endPoint())
SketchLine_26 = Sketch_1.addLine(0, 12.096, 0, 12.26)
SketchConstraintCoincidence_34 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_26.startPoint())
SketchConstraintCoincidence_35 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_26.endPoint())
SketchConstraintDistance_11 = Sketch_1.setDistance(SketchArc_1.center(), SketchLine_12.result(), 2.425, True)
SketchConstraintCoincidence_36 = Sketch_1.setCoincident(SketchArc_3.center(), SketchArc_1.center())
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_26.result(), 0.164)
SketchLine_27 = Sketch_1.addLine(0, 2.425, 4.9175, 10.94235984621998)
SketchLine_27.setAuxiliary(True)
SketchConstraintCoincidence_37 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_27.startPoint())
SketchConstraintCoincidence_38 = Sketch_1.setCoincident(SketchLine_21.endPoint(), SketchLine_27.endPoint())
SketchConstraintCoincidence_39 = Sketch_1.setCoincident(SketchArc_3.startPoint(), SketchLine_27.result())
SketchLine_28 = Sketch_1.addLine(0, 2.425, 5.21991026555713, 10.77860263646605)
SketchLine_28.setAuxiliary(True)
SketchConstraintCoincidence_40 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_28.startPoint())
SketchConstraintCoincidence_41 = Sketch_1.setCoincident(SketchLine_28.endPoint(), SketchLine_21.result())
SketchConstraintCoincidence_42 = Sketch_1.setCoincident(SketchLine_28.result(), SketchLine_25.startPoint())
SketchLine_29 = Sketch_1.addLine(0, 2.425, 5.523638941362955, 10.61413149862094)
SketchLine_29.setAuxiliary(True)
SketchConstraintCoincidence_43 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_29.startPoint())
SketchConstraintCoincidence_44 = Sketch_1.setCoincident(SketchLine_29.endPoint(), SketchLine_21.result())
SketchConstraintCoincidence_45 = Sketch_1.setCoincident(SketchLine_29.result(), SketchLine_24.startPoint())
SketchConstraintDistance_12 = Sketch_1.setDistance(SketchLine_25.startPoint(), SketchLine_28.endPoint(), 0.176, True)
SketchConstraintDistance_13 = Sketch_1.setDistance(SketchLine_24.startPoint(), SketchLine_29.endPoint(), 0.212, True)
SketchConstraintAngle_2 = Sketch_1.setAngle(SketchLine_28.result(), SketchLine_11.result(), 32, type = "Direct")
SketchConstraintAngle_3 = Sketch_1.setAngle(SketchLine_29.result(), SketchLine_11.result(), 34, type = "Direct")
SketchConstraintAngle_4 = Sketch_1.setAngle(SketchLine_27.result(), SketchLine_11.result(), 30, type = "Direct")
SketchLine_30 = Sketch_1.addLine(4.79378612024245, 9.263, 5.676067550792229, 10.23944020672391)
SketchLine_30.setAuxiliary(True)
SketchConstraintCoincidence_46 = Sketch_1.setCoincident(SketchArc_2.center(), SketchLine_30.startPoint())
SketchConstraintCoincidence_47 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_30.endPoint())
SketchLine_31 = Sketch_1.addLine(4.79378612024245, 9.263, 4.79378612024245, 10.72808112087839)
SketchLine_31.setAuxiliary(True)
SketchConstraintCoincidence_48 = Sketch_1.setCoincident(SketchArc_2.center(), SketchLine_31.startPoint())
SketchConstraintVertical_10 = Sketch_1.setVertical(SketchLine_31.result())
SketchConstraintAngle_5 = Sketch_1.setAngle(SketchLine_30.result(), SketchLine_31.result(), 42.1, type = "Direct")
SketchConstraintDistance_14 = Sketch_1.setDistance(SketchArc_2.center(), SketchLine_12.result(), 9.263, True)
SketchConstraintCoincidence_49 = Sketch_1.setCoincident(SketchLine_31.endPoint(), SketchLine_27.result())
SketchConstraintDistance_15 = Sketch_1.setDistance(SketchLine_22.endPoint(), SketchLine_12.result(), 9.532, True)
SketchConstraintDistance_16 = Sketch_1.setDistance(SketchLine_18.endPoint(), SketchLine_12.result(), 9.187, True)
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_1.results()[1], 9.835)
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", (3.891, 0, 2.905000000000017))], model.selection("EDGE", "PartSet/OZ"), 360, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX_5", (0, 0, 2.425)), False)
SketchPoint_1 = SketchProjection_3.createdFeature()
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", (0, 0, 12.178)), False)
SketchLine_32 = SketchProjection_4.createdFeature()
SketchArc_4 = Sketch_2.addArc(0, 2.425, 0, 12.178, 5.453808387550563, 10.51060344512907, True)
SketchArc_4.setAuxiliary(True)
SketchConstraintCoincidence_50 = Sketch_2.setCoincident(SketchPoint_1.result(), SketchArc_4.center())
SketchConstraintCoincidence_51 = Sketch_2.setCoincident(SketchLine_32.result(), SketchArc_4.startPoint())
SketchProjection_5 = Sketch_2.addProjection(model.selection("EDGE", (2.761819470681477, 0, 6.519565749310471)), False)
SketchLine_33 = SketchProjection_5.createdFeature()
SketchConstraintCoincidence_52 = Sketch_2.setCoincident(SketchArc_4.endPoint(), SketchLine_33.result())
SketchConstraintMiddle_3 = Sketch_2.setMiddlePoint(SketchArc_4.startPoint(), SketchLine_32.result())
SketchLine_34 = Sketch_2.addLine(0, 2.425, 3.282538500974084, 11.74164236673399)
SketchLine_34.setAuxiliary(True)
SketchConstraintCoincidence_53 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_34.startPoint())
SketchLine_35 = Sketch_2.addLine(3.282538500974084, 11.74164236673399, 3.199461499025965, 11.50584963625379)
SketchConstraintCoincidence_54 = Sketch_2.setCoincident(SketchLine_34.endPoint(), SketchLine_35.startPoint())
SketchConstraintCoincidence_55 = Sketch_2.setCoincident(SketchLine_35.endPoint(), SketchLine_34.result())
SketchConstraintLength_6 = Sketch_2.setLength(SketchLine_35.result(), 0.25)
SketchPoint_2 = Sketch_2.addPoint(3.241, 11.6237460014939)
SketchConstraintCoincidence_56 = Sketch_2.setCoincident(SketchPoint_2.coordinates(), SketchArc_4.results()[1])
SketchConstraintCoincidence_57 = Sketch_2.setCoincident(SketchPoint_2.coordinates(), SketchLine_35.result())
SketchProjection_6 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_36 = SketchProjection_6.createdFeature()
SketchConstraintDistance_17 = Sketch_2.setDistance(SketchPoint_2.coordinates(), SketchLine_36.result(), 3.241, True)
SketchArc_5 = Sketch_2.addArc(0, 2.425, 3.282538500974084, 11.74164236673399, 4.1398177019328, 11.393656164374, True)
SketchConstraintCoincidence_58 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchArc_5.center())
SketchConstraintCoincidence_59 = Sketch_2.setCoincident(SketchLine_34.endPoint(), SketchArc_5.startPoint())
SketchArc_6 = Sketch_2.addArc(0, 2.425, 3.199461499025965, 11.50584963625379, 4.035044020470642, 11.16667053559353, True)
SketchConstraintCoincidence_60 = Sketch_2.setCoincident(SketchAPI_Line(SketchLine_33).startPoint(), SketchArc_6.center())
SketchConstraintCoincidence_61 = Sketch_2.setCoincident(SketchLine_35.endPoint(), SketchArc_6.startPoint())
SketchLine_37 = Sketch_2.addLine(0, 2.425, 3.911506014428326, 11.49555702253677)
SketchLine_37.setAuxiliary(True)
SketchConstraintCoincidence_62 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_37.startPoint())
SketchConstraintCoincidence_63 = Sketch_2.setCoincident(SketchLine_37.endPoint(), SketchArc_5.results()[1])
SketchLine_38 = Sketch_2.addLine(0, 2.425, 4.1398177019328, 11.393656164374)
SketchLine_38.setAuxiliary(True)
SketchConstraintCoincidence_64 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_38.startPoint())
SketchConstraintCoincidence_65 = Sketch_2.setCoincident(SketchArc_5.endPoint(), SketchLine_38.endPoint())
SketchConstraintCoincidence_66 = Sketch_2.setCoincident(SketchArc_6.endPoint(), SketchLine_38.result())
SketchConstraintDistance_18 = Sketch_2.setDistance(SketchLine_35.startPoint(), SketchLine_37.result(), 0.675, True)
SketchConstraintMiddle_4 = Sketch_2.setMiddlePoint(SketchPoint_2.coordinates(), SketchLine_35.result())
SketchLine_39 = Sketch_2.addLine(3.911506014428326, 11.49555702253677, 3.81251062025875, 11.26599240868435)
SketchConstraintCoincidence_67 = Sketch_2.setCoincident(SketchLine_37.endPoint(), SketchLine_39.startPoint())
SketchConstraintCoincidence_68 = Sketch_2.setCoincident(SketchLine_39.endPoint(), SketchArc_6.results()[1])
SketchConstraintCoincidence_69 = Sketch_2.setCoincident(SketchLine_39.endPoint(), SketchLine_37.result())
SketchLine_40 = Sketch_2.addLine(4.1398177019328, 11.393656164374, 4.459277785066647, 11.19096067956113)
SketchConstraintCoincidence_70 = Sketch_2.setCoincident(SketchArc_5.endPoint(), SketchLine_40.startPoint())
SketchProjection_7 = Sketch_2.addProjection(model.selection("EDGE_3", (2.545485308583308, 0, 11.92488050155302)), False)
SketchArc_7 = SketchProjection_7.createdFeature()
SketchConstraintCoincidence_71 = Sketch_2.setCoincident(SketchLine_40.endPoint(), SketchArc_7.results()[1])
SketchLine_41 = Sketch_2.addLine(4.459277785066647, 11.19096067956113, 4.390495384356095, 11.04194790978503)
SketchConstraintCoincidence_72 = Sketch_2.setCoincident(SketchLine_40.endPoint(), SketchLine_41.startPoint())
SketchProjection_8 = Sketch_2.addProjection(model.selection("EDGE_3", (2.503038985186497, 0, 11.76646866604162)), False)
SketchArc_8 = SketchProjection_8.createdFeature()
SketchConstraintCoincidence_73 = Sketch_2.setCoincident(SketchLine_41.endPoint(), SketchArc_8.results()[1])
SketchLine_42 = Sketch_2.addLine(4.390495384356095, 11.04194790978503, 4.035044020470642, 11.16667053559353)
SketchConstraintCoincidence_74 = Sketch_2.setCoincident(SketchLine_41.endPoint(), SketchLine_42.startPoint())
SketchConstraintCoincidence_75 = Sketch_2.setCoincident(SketchLine_42.endPoint(), SketchLine_38.result())
SketchConstraintCoincidence_76 = Sketch_2.setCoincident(SketchLine_42.endPoint(), SketchArc_6.endPoint())
SketchConstraintDistance_19 = Sketch_2.setDistance(SketchArc_5.endPoint(), SketchLine_41.result(), 0.375, True)
SketchConstraintDistance_20 = Sketch_2.setDistance(SketchLine_39.startPoint(), SketchLine_38.result(), 0.25, True)
SketchConstraintParallel_1 = Sketch_2.setParallel(SketchLine_41.result(), SketchLine_38.result())
model.do()
Revolution_2 = model.addRevolution(Part_1_doc, [model.selection("WIRE", (3.862008317343538, 0, 11.38077471561056))], model.selection("EDGE", (3.241000000000025, 0, 11.62374600149389)), 360, 0)
Revolution_3 = model.addRevolution(Part_1_doc, [model.selection("WIRE", (4.212769702413368, 0, 11.10430922268928))], model.selection("EDGE", (3.241000000000025, 0, 11.62374600149389)), 360, 0)
Symmetry_1 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", (3.237001631529592, 7.752869270149964e-17, 11.61239765963003)), model.selection("SOLID", (3.223059230627338, 1.806177914907969e-16, 11.57282573597984))], model.selection("FACE", "PartSet/YOZ"), True)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", (9.171993615882458e-05, 1.467829010982793e-16, 1.298864381774082))], [model.selection("COMPOUND", (0, 0, 11.60508262087049))])
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", (9.190387067667382e-05, -2.335384201710612e-09, 1.278348504308934)), model.selection("COMPOUND", (0, 0, 11.54608275997197))], removeEdges = True)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), 3.6, False)
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", (0, 0, 3.6)))
SketchProjection_9 = Sketch_3.addProjection(model.selection("EDGE", (-6.188, 7.578114393123822e-16, -3.45)), True)
SketchCircle_1 = SketchProjection_9.createdFeature()
SketchProjection_10 = Sketch_3.addProjection(model.selection("EDGE", (-6.376, 7.808347991363525e-16, -2.25)), True)
SketchCircle_2 = SketchProjection_10.createdFeature()
SketchArc_9 = Sketch_3.addArc(0, 0, 6.094, 0, 5.859569950090201, -1.674, True)
SketchConstraintCoincidence_77 = Sketch_3.setCoincident(SketchAPI_Circle(SketchCircle_1).center(), SketchArc_9.center())
SketchProjection_11 = Sketch_3.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_43 = SketchProjection_11.createdFeature()
SketchConstraintCoincidence_78 = Sketch_3.setCoincident(SketchArc_9.startPoint(), SketchLine_43.result())
SketchPoint_3 = Sketch_3.addPoint(6.188, 0)
SketchConstraintCoincidence_79 = Sketch_3.setCoincident(SketchPoint_3.coordinates(), SketchLine_43.result())
SketchConstraintCoincidence_80 = Sketch_3.setCoincident(SketchPoint_3.coordinates(), SketchCircle_1.results()[1])
SketchConstraintDistance_21 = Sketch_3.setDistance(SketchArc_9.startPoint(), SketchPoint_3.coordinates(), 0.094, True)
SketchConstraintDistance_22 = Sketch_3.setDistance(SketchArc_9.endPoint(), SketchLine_43.result(), 1.674, True)
SketchLine_44 = Sketch_3.addLine(5.859569950090201, -1.674, 5.870724657144867, -1.956)
SketchConstraintCoincidence_81 = Sketch_3.setCoincident(SketchArc_9.endPoint(), SketchLine_44.startPoint())
SketchConstraintCoincidence_82 = Sketch_3.setCoincident(SketchLine_44.endPoint(), SketchCircle_1.results()[1])
SketchArc_10 = Sketch_3.addArc(0, 0, 6.47, 0, 6.167249305809295, -1.956, True)
SketchConstraintCoincidence_83 = Sketch_3.setCoincident(SketchAPI_Circle(SketchCircle_1).center(), SketchArc_10.center())
SketchConstraintCoincidence_84 = Sketch_3.setCoincident(SketchLine_43.result(), SketchArc_10.startPoint())
SketchLine_45 = Sketch_3.addLine(6.167249305809295, -1.956, 6.068561608816571, -1.956)
SketchConstraintCoincidence_85 = Sketch_3.setCoincident(SketchArc_10.endPoint(), SketchLine_45.startPoint())
SketchConstraintCoincidence_86 = Sketch_3.setCoincident(SketchLine_45.endPoint(), SketchCircle_2.results()[1])
SketchConstraintHorizontal_8 = Sketch_3.setHorizontal(SketchLine_45.result())
SketchConstraintCoincidence_87 = Sketch_3.setCoincident(SketchLine_44.endPoint(), SketchLine_45.result())
SketchConstraintDistance_23 = Sketch_3.setDistance(SketchArc_10.startPoint(), SketchLine_45.result(), 1.956, True)
SketchPoint_4 = Sketch_3.addPoint(6.376, 0)
SketchConstraintCoincidence_88 = Sketch_3.setCoincident(SketchPoint_4.coordinates(), SketchCircle_2.results()[1])
SketchConstraintCoincidence_89 = Sketch_3.setCoincident(SketchPoint_4.coordinates(), SketchLine_43.result())
SketchConstraintDistance_24 = Sketch_3.setDistance(SketchPoint_4.coordinates(), SketchArc_10.startPoint(), 0.094, True)
SketchConstraintMirror_1_objects = [SketchArc_9.results()[1], SketchLine_44.result(), SketchArc_10.results()[1], SketchLine_45.result()]
SketchConstraintMirror_1 = Sketch_3.addMirror(SketchLine_43.result(), SketchConstraintMirror_1_objects)
[SketchArc_11, SketchLine_46, SketchArc_12, SketchLine_47] = SketchConstraintMirror_1.mirrored()
model.do()
Sketch_3.changeFacesOrder([[SketchProjection_9.result(), SketchProjection_9.result(), SketchLine_46.result(), SketchArc_11.results()[1], SketchArc_9.results()[1], SketchLine_44.result()],
                           [SketchProjection_10.result(), SketchProjection_10.result(), SketchProjection_10.result(), SketchProjection_9.result(), SketchProjection_9.result(), SketchProjection_9.result()],
                           [SketchProjection_9.result(), SketchLine_44.result(), SketchArc_9.results()[1], SketchArc_11.results()[1], SketchLine_46.result()],
                           [SketchProjection_10.result(), SketchLine_45.result(), SketchArc_10.results()[1], SketchArc_12.results()[1], SketchLine_47.result(), SketchProjection_10.result()]
                          ])
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", (6.023784975045098, 0, 3.6)), model.selection("WIRE", (6.393864363926704, 0.9896456414839456, 3.6))], model.selection(), 0, 4.2)
Sketch_4 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))
SketchLine_48 = Sketch_4.addLine(0.75, 2.1, -0.75, 2.1)
SketchLine_49 = Sketch_4.addLine(-0.75, 2.1, -0.75, 0.3)
SketchLine_50 = Sketch_4.addLine(-0.75, 0.3, 0.75, 0.3)
SketchLine_51 = Sketch_4.addLine(0.75, 0.3, 0.75, 2.1)
SketchConstraintCoincidence_90 = Sketch_4.setCoincident(SketchLine_51.endPoint(), SketchLine_48.startPoint())
SketchConstraintCoincidence_91 = Sketch_4.setCoincident(SketchLine_48.endPoint(), SketchLine_49.startPoint())
SketchConstraintCoincidence_92 = Sketch_4.setCoincident(SketchLine_49.endPoint(), SketchLine_50.startPoint())
SketchConstraintCoincidence_93 = Sketch_4.setCoincident(SketchLine_50.endPoint(), SketchLine_51.startPoint())
SketchConstraintHorizontal_9 = Sketch_4.setHorizontal(SketchLine_48.result())
SketchConstraintVertical_11 = Sketch_4.setVertical(SketchLine_49.result())
SketchConstraintHorizontal_10 = Sketch_4.setHorizontal(SketchLine_50.result())
SketchConstraintVertical_12 = Sketch_4.setVertical(SketchLine_51.result())
SketchProjection_12 = Sketch_4.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_52 = SketchProjection_12.createdFeature()
SketchConstraintDistance_25 = Sketch_4.setDistance(SketchLine_52.result(), SketchLine_50.endPoint(), 0.3, True)
SketchConstraintDistance_26 = Sketch_4.setDistance(SketchLine_51.endPoint(), SketchLine_52.result(), 2.1, True)
SketchPoint_5 = Sketch_4.addPoint(0, 2.1)
SketchConstraintCoincidence_94 = Sketch_4.setCoincident(SketchPoint_5.coordinates(), SketchLine_48.result())
SketchConstraintMiddle_5 = Sketch_4.setMiddlePoint(SketchLine_48.result(), SketchPoint_5.coordinates())
SketchProjection_13 = Sketch_4.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_53 = SketchProjection_13.createdFeature()
SketchConstraintCoincidence_95 = Sketch_4.setCoincident(SketchPoint_5.coordinates(), SketchLine_53.result())
SketchConstraintDistance_27 = Sketch_4.setDistance(SketchPoint_5.coordinates(), SketchLine_51.endPoint(), 0.75, True)
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", (0, 0, 1.2))], model.selection(), 10, 0)
Rotation_1_objects = [model.selection("SOLID", (5, 1.166200656118862e-18, 1.2)), model.selection("SOLID", (6.038396788450424, -2.054057850558325e-15, 1.5)), model.selection("SOLID", (6.316150725419533, -7.758519898162708e-14, 1.5))]
Rotation_1 = model.addRotation(Part_1_doc, Rotation_1_objects, model.selection("EDGE", "PartSet/OZ"), -90)
Fuse_2_objects_1 = [model.selection("SOLID", (9.187239971515918e-05, -6.832080613557312e-09, 1.282177819724956)), model.selection("SOLID", (-1.349638054860434e-15, -6.038396788450424, 1.5)), model.selection("SOLID", (-7.680624658573511e-14, -6.316150725419533, 1.5))]
Fuse_2 = model.addFuse(Part_1_doc, Fuse_2_objects_1)
Cut_2 = model.addCut(Part_1_doc, [model.selection("SOLID", (9.177890659903279e-05, -0.006341145656800184, 1.28239948002437))], [model.selection("SOLID", (6.393306363619619e-16, -4.999999999999998, 1.2))])
FusionFaces_1 = model.addFusionFaces(Part_1_doc, model.selection("SOLID", (9.190526314899106e-05, 0.002297421001363739, 1.282512923455053)))

model.end()

model.testNbResults(FusionFaces_1, 1)
model.testNbSubResults(FusionFaces_1, [0])
model.testNbSubShapes(FusionFaces_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(FusionFaces_1, GeomAPI_Shape.FACE, [47])
model.testNbSubShapes(FusionFaces_1, GeomAPI_Shape.EDGE, [202])
model.testNbSubShapes(FusionFaces_1, GeomAPI_Shape.VERTEX, [404])
model.testResultsVolumes(FusionFaces_1, [612.7268292882]

assert(model.checkPythonDump())