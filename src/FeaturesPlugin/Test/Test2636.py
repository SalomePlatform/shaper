## Copyright (C) 2019-2023  CEA, EDF
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
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
##
## See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
##

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
model.addParameter(partSet, "epsilon", "0.1")
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchLine_1.setName("SketchLine_2")
SketchLine_1.result().setName("SketchLine_2")
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchLine_2.setName("SketchLine_3")
SketchLine_2.result().setName("SketchLine_3")
SketchLine_3 = Sketch_1.addLine(0, 0, 0, -21)
SketchLine_3.setName("SketchLine_4")
SketchLine_3.result().setName("SketchLine_4")
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_1).startPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(0, -21, 54, -21)
SketchLine_4.setName("SketchLine_5")
SketchLine_4.result().setName("SketchLine_5")
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(54, -21, 57, -18)
SketchLine_5.setName("SketchLine_6")
SketchLine_5.result().setName("SketchLine_6")
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), "42/2")
SketchConstraintAngle_1 = Sketch_1.setAngleComplementary(SketchLine_4.result(), SketchLine_5.result(), 45)
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_5.endPoint(), SketchLine_3.endPoint(), 57)
SketchLine_6 = Sketch_1.addLine(57, -18, 159.1, -18)
SketchLine_6.setName("SketchLine_7")
SketchLine_6.result().setName("SketchLine_7")
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_5.endPoint(), SketchLine_1.result(), "36/2", True)
SketchLine_7 = Sketch_1.addLine(162.1, -21, 162.1, -31.5)
SketchLine_7.setName("SketchLine_8")
SketchLine_7.result().setName("SketchLine_8")
SketchLine_8 = Sketch_1.addLine(162.1, -31.5, 204.1, -31.5)
SketchLine_8.setName("SketchLine_9")
SketchLine_8.result().setName("SketchLine_9")
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_1.addLine(204.1, -31.5, 204.1, -21)
SketchLine_9.setName("SketchLine_10")
SketchLine_9.result().setName("SketchLine_10")
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchLine_10 = Sketch_1.addLine(204.1, -21, 181.1, -21)
SketchLine_10.setName("SketchLine_11")
SketchLine_10.result().setName("SketchLine_11")
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchLine_11 = Sketch_1.addLine(181.1, -21, 168.9756443470179, 0)
SketchLine_11.setName("SketchLine_12")
SketchLine_11.result().setName("SketchLine_12")
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchLine_12 = Sketch_1.addLine(168.9756443470179, 0, 0, 0)
SketchLine_12.setName("SketchLine_13")
SketchLine_12.result().setName("SketchLine_13")
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_1).startPoint(), SketchLine_12.endPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_12.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_10.result())
SketchConstraintHorizontal_5 = Sketch_1.setHorizontal(SketchLine_8.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_7.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_9.result())
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchLine_12.startPoint(), SketchLine_8.endPoint(), "63/2")
SketchConstraintDistanceVertical_2 = Sketch_1.setVerticalDistance(SketchLine_12.startPoint(), SketchLine_10.startPoint(), "42/2")
SketchConstraintAngle_2 = Sketch_1.setAngleComplementary(SketchLine_12.result(), SketchLine_11.result(), 60)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_10.result(), 23)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_8.result(), 42)
SketchLine_13 = Sketch_1.addLine(54, -21, 54, 0)
SketchLine_13.setName("SketchLine_14")
SketchLine_13.result().setName("SketchLine_14")
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_1.result())
SketchLine_14 = Sketch_1.addLine(57, -18, 57, 0)
SketchLine_14.setName("SketchLine_15")
SketchLine_14.result().setName("SketchLine_15")
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_1.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_14.result())
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_13.result())
SketchLine_15 = Sketch_1.addLine(181.1, -21, 181.1, -31.5)
SketchLine_15.setName("SketchLine_18")
SketchLine_15.result().setName("SketchLine_18")
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchLine_8.result())
SketchConstraintVertical_6 = Sketch_1.setVertical(SketchLine_15.result())
SketchArc_1 = Sketch_1.addArc(159.1, -21, 162.1, -21, 159.1, -18, False)
SketchPoint_1 = Sketch_1.addPoint(162.1, -18)
SketchPoint_1.setName("SketchPoint_2")
SketchPoint_1.result().setName("SketchPoint_2")
SketchPoint_1.setAuxiliary(True)
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_6.result())
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_7.result())
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_6.startPoint(), SketchPoint_1.coordinates(), "105+epsilon", False)
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_6.endPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_6.result())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_7.result())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 3)
SketchLine_16 = Sketch_1.addLine(162.1, -21, 181.1, -21)
SketchLine_16.setName("SketchLine_36")
SketchLine_16.result().setName("SketchLine_36")
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchLine_16.endPoint(), SketchLine_11.result())
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchLine_16.startPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchLine_16.endPoint(), SketchLine_15.startPoint())
SketchLine_17 = Sketch_1.addLine(159.1, -18, 159.1, 0)
SketchLine_17.setName("SketchLine_35")
SketchLine_17.result().setName("SketchLine_35")
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_17.startPoint())
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchLine_17.endPoint(), SketchLine_12.result())
SketchConstraintVertical_7 = Sketch_1.setVertical(SketchLine_17.result())
SketchLine_18 = Sketch_1.addLine(159.1, -18, 175.3264973081038, -11)
SketchLine_18.setName("SketchLine_37")
SketchLine_18.result().setName("SketchLine_37")
SketchConstraintCoincidence_26 = Sketch_1.setCoincident(SketchLine_18.endPoint(), SketchLine_11.result())
SketchConstraintCoincidence_27 = Sketch_1.setCoincident(SketchLine_18.startPoint(), SketchLine_6.endPoint())
SketchLine_19 = Sketch_1.addLine(168.9756443470179, 0, 168.9756443470179, -5)
SketchLine_19.setName("SketchLine_38")
SketchLine_19.result().setName("SketchLine_38")
SketchLine_19.setAuxiliary(True)
SketchConstraintCoincidence_28 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_19.startPoint())
SketchConstraintVertical_8 = Sketch_1.setVertical(SketchLine_19.result())
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_19.result(), 5)
SketchConstraintDistanceVertical_3 = Sketch_1.setVerticalDistance(SketchLine_18.endPoint(), SketchLine_12.startPoint(), 11)
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection("EDGE", "Sketch_1/SketchLine_13"), 360, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Revolution_1_1_1/Generated_Face&Sketch_1/SketchLine_4"))
SketchPoint_2 = Sketch_2.addPoint(0, 0)
SketchPoint_2.setName("SketchPoint_3")
SketchPoint_2.result().setName("SketchPoint_3")
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "[Revolution_1_1_1/Generated_Face&Sketch_1/SketchLine_4][Revolution_1_1_1/Generated_Face&Sketch_1/SketchLine_5]__cc"), False)
SketchProjection_3.setName("SketchProjection_4")
SketchProjection_3.result().setName("SketchProjection_4")
SketchPoint_3 = SketchProjection_3.createdFeature()
SketchPoint_3.setName("SketchPoint_4")
SketchPoint_3.result().setName("SketchPoint_4")
SketchConstraintCoincidence_29 = Sketch_2.setCoincident(SketchPoint_2.coordinates(), SketchPoint_3.result())
model.do()
Sketch_2.setName("Sketch_3")
Sketch_2.result().setName("Sketch_3")
model.do()
Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Sketch_3 = model.addSketch(Part_2_doc, model.defaultPlane("XOY"))
SketchProjection_4 = Sketch_3.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_20 = SketchProjection_4.createdFeature()
SketchLine_20.setName("SketchLine_2")
SketchLine_20.result().setName("SketchLine_2")
SketchProjection_5 = Sketch_3.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_21 = SketchProjection_5.createdFeature()
SketchLine_21.setName("SketchLine_3")
SketchLine_21.result().setName("SketchLine_3")
SketchLine_22 = Sketch_3.addLine(0, 0, 0, -21)
SketchLine_22.setName("SketchLine_4")
SketchLine_22.result().setName("SketchLine_4")
SketchConstraintCoincidence_30 = Sketch_3.setCoincident(SketchAPI_Line(SketchLine_20).startPoint(), SketchLine_22.startPoint())
SketchLine_23 = Sketch_3.addLine(0, -21, 54, -21)
SketchLine_23.setName("SketchLine_5")
SketchLine_23.result().setName("SketchLine_5")
SketchConstraintCoincidence_31 = Sketch_3.setCoincident(SketchLine_22.endPoint(), SketchLine_23.startPoint())
SketchLine_24 = Sketch_3.addLine(54, -21, 57, -18)
SketchLine_24.setName("SketchLine_6")
SketchLine_24.result().setName("SketchLine_6")
SketchConstraintCoincidence_32 = Sketch_3.setCoincident(SketchLine_23.endPoint(), SketchLine_24.startPoint())
SketchConstraintVertical_9 = Sketch_3.setVertical(SketchLine_22.result())
SketchConstraintHorizontal_6 = Sketch_3.setHorizontal(SketchLine_23.result())
SketchConstraintLength_5 = Sketch_3.setLength(SketchLine_22.result(), "42/2")
SketchConstraintAngle_3 = Sketch_3.setAngleComplementary(SketchLine_23.result(), SketchLine_24.result(), 45)
SketchConstraintDistanceHorizontal_2 = Sketch_3.setHorizontalDistance(SketchLine_24.endPoint(), SketchLine_22.endPoint(), 57)
SketchLine_25 = Sketch_3.addLine(57, -18, 159.1, -18)
SketchLine_25.setName("SketchLine_7")
SketchLine_25.result().setName("SketchLine_7")
SketchConstraintCoincidence_33 = Sketch_3.setCoincident(SketchLine_24.endPoint(), SketchLine_25.startPoint())
SketchConstraintHorizontal_7 = Sketch_3.setHorizontal(SketchLine_25.result())
SketchConstraintDistance_3 = Sketch_3.setDistance(SketchLine_24.endPoint(), SketchLine_20.result(), "36/2", True)
SketchLine_26 = Sketch_3.addLine(162.1, -21, 162.1, -31.5)
SketchLine_26.setName("SketchLine_8")
SketchLine_26.result().setName("SketchLine_8")
SketchLine_27 = Sketch_3.addLine(162.1, -31.5, 204.1, -31.5)
SketchLine_27.setName("SketchLine_9")
SketchLine_27.result().setName("SketchLine_9")
SketchConstraintCoincidence_34 = Sketch_3.setCoincident(SketchLine_26.endPoint(), SketchLine_27.startPoint())
SketchLine_28 = Sketch_3.addLine(204.1, -31.5, 204.1, -21)
SketchLine_28.setName("SketchLine_10")
SketchLine_28.result().setName("SketchLine_10")
SketchConstraintCoincidence_35 = Sketch_3.setCoincident(SketchLine_27.endPoint(), SketchLine_28.startPoint())
SketchLine_29 = Sketch_3.addLine(204.1, -21, 181.1, -21)
SketchLine_29.setName("SketchLine_11")
SketchLine_29.result().setName("SketchLine_11")
SketchConstraintCoincidence_36 = Sketch_3.setCoincident(SketchLine_28.endPoint(), SketchLine_29.startPoint())
SketchLine_30 = Sketch_3.addLine(181.1, -21, 168.9756443470179, 0)
SketchLine_30.setName("SketchLine_12")
SketchLine_30.result().setName("SketchLine_12")
SketchConstraintCoincidence_37 = Sketch_3.setCoincident(SketchLine_29.endPoint(), SketchLine_30.startPoint())
SketchLine_31 = Sketch_3.addLine(168.9756443470179, 0, 0, 0)
SketchLine_31.setName("SketchLine_13")
SketchLine_31.result().setName("SketchLine_13")
SketchConstraintCoincidence_38 = Sketch_3.setCoincident(SketchLine_30.endPoint(), SketchLine_31.startPoint())
SketchConstraintCoincidence_39 = Sketch_3.setCoincident(SketchAPI_Line(SketchLine_20).startPoint(), SketchLine_31.endPoint())
SketchConstraintHorizontal_8 = Sketch_3.setHorizontal(SketchLine_31.result())
SketchConstraintHorizontal_9 = Sketch_3.setHorizontal(SketchLine_29.result())
SketchConstraintHorizontal_10 = Sketch_3.setHorizontal(SketchLine_27.result())
SketchConstraintVertical_10 = Sketch_3.setVertical(SketchLine_26.result())
SketchConstraintVertical_11 = Sketch_3.setVertical(SketchLine_28.result())
SketchConstraintDistanceVertical_4 = Sketch_3.setVerticalDistance(SketchLine_31.startPoint(), SketchLine_27.endPoint(), "63/2")
SketchConstraintDistanceVertical_5 = Sketch_3.setVerticalDistance(SketchLine_31.startPoint(), SketchLine_29.startPoint(), "42/2")
SketchConstraintAngle_4 = Sketch_3.setAngleComplementary(SketchLine_31.result(), SketchLine_30.result(), 60)
SketchConstraintLength_6 = Sketch_3.setLength(SketchLine_29.result(), 23)
SketchConstraintLength_7 = Sketch_3.setLength(SketchLine_27.result(), 42)
SketchLine_32 = Sketch_3.addLine(54, -21, 54, 0)
SketchLine_32.setName("SketchLine_14")
SketchLine_32.result().setName("SketchLine_14")
SketchConstraintCoincidence_40 = Sketch_3.setCoincident(SketchLine_23.endPoint(), SketchLine_32.startPoint())
SketchConstraintCoincidence_41 = Sketch_3.setCoincident(SketchLine_32.endPoint(), SketchLine_20.result())
SketchLine_33 = Sketch_3.addLine(57, -18, 57, 0)
SketchLine_33.setName("SketchLine_15")
SketchLine_33.result().setName("SketchLine_15")
SketchConstraintCoincidence_42 = Sketch_3.setCoincident(SketchLine_24.endPoint(), SketchLine_33.startPoint())
SketchConstraintCoincidence_43 = Sketch_3.setCoincident(SketchLine_33.endPoint(), SketchLine_20.result())
SketchConstraintVertical_12 = Sketch_3.setVertical(SketchLine_33.result())
SketchConstraintVertical_13 = Sketch_3.setVertical(SketchLine_32.result())
SketchLine_34 = Sketch_3.addLine(181.1, -21, 181.1, -31.5)
SketchLine_34.setName("SketchLine_18")
SketchLine_34.result().setName("SketchLine_18")
SketchConstraintCoincidence_44 = Sketch_3.setCoincident(SketchLine_29.endPoint(), SketchLine_34.startPoint())
SketchConstraintCoincidence_45 = Sketch_3.setCoincident(SketchLine_34.endPoint(), SketchLine_27.result())
SketchConstraintVertical_14 = Sketch_3.setVertical(SketchLine_34.result())
SketchArc_2 = Sketch_3.addArc(159.1, -21, 162.1, -21, 159.1, -18, False)
SketchPoint_4 = Sketch_3.addPoint(162.1, -18)
SketchPoint_4.setName("SketchPoint_2")
SketchPoint_4.result().setName("SketchPoint_2")
SketchPoint_4.setAuxiliary(True)
SketchConstraintCoincidence_46 = Sketch_3.setCoincident(SketchPoint_4.coordinates(), SketchLine_25.result())
SketchConstraintCoincidence_47 = Sketch_3.setCoincident(SketchPoint_4.coordinates(), SketchLine_26.result())
SketchConstraintDistance_4 = Sketch_3.setDistance(SketchLine_25.startPoint(), SketchPoint_4.coordinates(), "105+epsilon", False)
SketchConstraintCoincidence_48 = Sketch_3.setCoincident(SketchArc_2.startPoint(), SketchLine_26.startPoint())
SketchConstraintCoincidence_49 = Sketch_3.setCoincident(SketchArc_2.endPoint(), SketchLine_25.endPoint())
SketchConstraintTangent_3 = Sketch_3.setTangent(SketchArc_2.results()[1], SketchLine_25.result())
SketchConstraintTangent_4 = Sketch_3.setTangent(SketchArc_2.results()[1], SketchLine_26.result())
SketchConstraintRadius_2 = Sketch_3.setRadius(SketchArc_2.results()[1], 3)
SketchLine_35 = Sketch_3.addLine(162.1, -21, 181.1, -21)
SketchLine_35.setName("SketchLine_36")
SketchLine_35.result().setName("SketchLine_36")
SketchConstraintCoincidence_50 = Sketch_3.setCoincident(SketchLine_35.endPoint(), SketchLine_30.result())
SketchConstraintCoincidence_51 = Sketch_3.setCoincident(SketchLine_35.startPoint(), SketchLine_26.startPoint())
SketchConstraintCoincidence_52 = Sketch_3.setCoincident(SketchLine_35.endPoint(), SketchLine_34.startPoint())
SketchLine_36 = Sketch_3.addLine(159.1, -18, 159.1, 0)
SketchLine_36.setName("SketchLine_35")
SketchLine_36.result().setName("SketchLine_35")
SketchConstraintCoincidence_53 = Sketch_3.setCoincident(SketchLine_25.endPoint(), SketchLine_36.startPoint())
SketchConstraintCoincidence_54 = Sketch_3.setCoincident(SketchLine_36.endPoint(), SketchLine_31.result())
SketchConstraintVertical_15 = Sketch_3.setVertical(SketchLine_36.result())
SketchLine_37 = Sketch_3.addLine(159.1, -18, 175.3264973081038, -11)
SketchLine_37.setName("SketchLine_37")
SketchLine_37.result().setName("SketchLine_37")
SketchConstraintCoincidence_55 = Sketch_3.setCoincident(SketchLine_37.endPoint(), SketchLine_30.result())
SketchConstraintCoincidence_56 = Sketch_3.setCoincident(SketchLine_37.startPoint(), SketchLine_25.endPoint())
SketchLine_38 = Sketch_3.addLine(168.9756443470179, 0, 168.9756443470179, -5)
SketchLine_38.setName("SketchLine_38")
SketchLine_38.result().setName("SketchLine_38")
SketchLine_38.setAuxiliary(True)
SketchConstraintCoincidence_57 = Sketch_3.setCoincident(SketchLine_30.endPoint(), SketchLine_38.startPoint())
SketchConstraintVertical_16 = Sketch_3.setVertical(SketchLine_38.result())
SketchConstraintLength_8 = Sketch_3.setLength(SketchLine_38.result(), 5)
SketchConstraintDistanceVertical_6 = Sketch_3.setVerticalDistance(SketchLine_37.endPoint(), SketchLine_31.startPoint(), 11)
model.do()
Revolution_2 = model.addRevolution(Part_2_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection("EDGE", "Sketch_1/SketchLine_13"), 360, 0)
Sketch_4 = model.addSketch(Part_2_doc, model.selection("FACE", "Revolution_1_1_1/Generated_Face&Sketch_1/SketchLine_4"))
SketchLine_39 = Sketch_4.addLine(50, 0, -50, 0)
SketchLine_39.setName("SketchLine_19")
SketchLine_39.result().setName("SketchLine_19")
SketchConstraintHorizontal_11 = Sketch_4.setHorizontal(SketchLine_39.result())
SketchProjection_6 = Sketch_4.addProjection(model.selection("VERTEX", "[Revolution_1_1_1/Generated_Face&Sketch_1/SketchLine_4][Revolution_1_1_1/Generated_Face&Sketch_1/SketchLine_5]__cc"), False)
SketchPoint_5 = SketchProjection_6.createdFeature()
SketchPoint_5.setName("SketchPoint_1")
SketchPoint_5.result().setName("SketchPoint_1")
SketchConstraintLength_9 = Sketch_4.setLength(SketchLine_39.result(), 100)
SketchLine_40 = Sketch_4.addLine(0, -50, 0, 50)
SketchLine_40.setName("SketchLine_20")
SketchLine_40.result().setName("SketchLine_20")
SketchProjection_7 = Sketch_4.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_41 = SketchProjection_7.createdFeature()
SketchLine_41.setName("SketchLine_21")
SketchLine_41.result().setName("SketchLine_21")
SketchConstraintCoincidence_58 = Sketch_4.setCoincident(SketchLine_40.startPoint(), SketchLine_41.result())
SketchConstraintVertical_17 = Sketch_4.setVertical(SketchLine_40.result())
SketchConstraintLength_10 = Sketch_4.setLength(SketchLine_40.result(), 100)
SketchCircle_1 = Sketch_4.addCircle(0, 0, 7)
SketchCircle_1.setAuxiliary(True)
SketchConstraintCoincidence_59 = Sketch_4.setCoincident(SketchAPI_Point(SketchPoint_5).coordinates(), SketchCircle_1.center())
SketchConstraintRadius_3 = Sketch_4.setRadius(SketchCircle_1.results()[1], 7)
SketchLine_42 = Sketch_4.addLine(0, 0, -4.949747468305834, -4.949747468305832)
SketchLine_42.setName("SketchLine_22")
SketchLine_42.result().setName("SketchLine_22")
SketchLine_42.setAuxiliary(True)
SketchConstraintCoincidence_60 = Sketch_4.setCoincident(SketchAPI_Point(SketchPoint_5).coordinates(), SketchLine_42.startPoint())
SketchConstraintCoincidence_61 = Sketch_4.setCoincident(SketchLine_42.endPoint(), SketchCircle_1.results()[1])
SketchConstraintAngle_5 = Sketch_4.setAngleComplementary(SketchLine_42.result(), SketchLine_39.result(), 45)
SketchLine_43 = Sketch_4.addLine(0, -7, -4.949747468305834, -4.949747468305832)
SketchLine_43.setName("SketchLine_23")
SketchLine_43.result().setName("SketchLine_23")
SketchConstraintCoincidence_62 = Sketch_4.setCoincident(SketchLine_43.startPoint(), SketchLine_41.result())
SketchConstraintCoincidence_63 = Sketch_4.setCoincident(SketchLine_42.endPoint(), SketchLine_43.endPoint())
SketchLine_44 = Sketch_4.addLine(-4.949747468305834, -4.949747468305832, -7, 0)
SketchLine_44.setName("SketchLine_24")
SketchLine_44.result().setName("SketchLine_24")
SketchConstraintCoincidence_64 = Sketch_4.setCoincident(SketchLine_42.endPoint(), SketchLine_44.startPoint())
SketchConstraintCoincidence_65 = Sketch_4.setCoincident(SketchLine_44.endPoint(), SketchLine_39.result())
SketchLine_45 = Sketch_4.addLine(-4.949747468305834, -4.949747468305832, -33.23401871576774, -33.23401871576773)
SketchLine_45.setName("SketchLine_25")
SketchLine_45.result().setName("SketchLine_25")
SketchConstraintCoincidence_66 = Sketch_4.setCoincident(SketchLine_42.endPoint(), SketchLine_45.startPoint())
SketchConstraintCollinear_1 = Sketch_4.setCollinear(SketchLine_45.result(), SketchLine_42.result())
SketchConstraintCoincidence_67 = Sketch_4.setCoincident(SketchLine_43.startPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_68 = Sketch_4.setCoincident(SketchLine_44.endPoint(), SketchCircle_1.results()[1])
SketchConstraintLength_11 = Sketch_4.setLength(SketchLine_45.result(), 40)
SketchMultiRotation_1_objects = [SketchLine_43.result(), SketchLine_45.result(), SketchLine_44.result()]
SketchMultiRotation_1 = Sketch_4.addRotation(SketchMultiRotation_1_objects, SketchAPI_Line(SketchLine_41).startPoint(), 90, 4)
[SketchLine_46, SketchLine_47, SketchLine_48, SketchLine_49, SketchLine_50, SketchLine_51, SketchLine_52, SketchLine_53, SketchLine_54] = SketchMultiRotation_1.rotated()
SketchLine_54.setName("SketchLine_34")
SketchLine_54.result().setName("SketchLine_34")
SketchLine_53.setName("SketchLine_33")
SketchLine_53.result().setName("SketchLine_33")
SketchLine_52.setName("SketchLine_28")
SketchLine_52.result().setName("SketchLine_28")
SketchLine_51.setName("SketchLine_32")
SketchLine_51.result().setName("SketchLine_32")
SketchLine_50.setName("SketchLine_31")
SketchLine_50.result().setName("SketchLine_31")
SketchLine_49.setName("SketchLine_27")
SketchLine_49.result().setName("SketchLine_27")
SketchLine_48.setName("SketchLine_30")
SketchLine_48.result().setName("SketchLine_30")
SketchLine_47.setName("SketchLine_29")
SketchLine_47.result().setName("SketchLine_29")
SketchLine_46.setName("SketchLine_26")
SketchLine_46.result().setName("SketchLine_26")
model.do()
Edge_1_objects = [model.selection("EDGE", "Sketch_2/SketchLine_27"), model.selection("EDGE", "Sketch_2/SketchLine_26"), model.selection("EDGE", "Sketch_2/SketchLine_28"), model.selection("EDGE", "Sketch_2/SketchLine_19"), model.selection("EDGE", "Sketch_2/SketchLine_25"), model.selection("EDGE", "Sketch_2/SketchLine_23"), model.selection("EDGE", "Sketch_2/SketchLine_24"), model.selection("EDGE", "Sketch_2/SketchLine_30"), model.selection("EDGE", "Sketch_2/SketchLine_34"), model.selection("EDGE", "Sketch_2/SketchLine_29"), model.selection("EDGE", "Sketch_2/SketchLine_33"), model.selection("EDGE", "Sketch_2/SketchLine_31"), model.selection("EDGE", "Sketch_2/SketchLine_20"), model.selection("EDGE", "Sketch_2/SketchLine_32")]
Edge_1 = model.addEdge(Part_2_doc, Edge_1_objects)
Extrusion_1_objects = [model.selection("EDGE", "Edge_1_1"), model.selection("EDGE", "Edge_1_2"), model.selection("EDGE", "Edge_1_3"), model.selection("EDGE", "Edge_1_4"), model.selection("EDGE", "Edge_1_5"), model.selection("EDGE", "Edge_1_6"), model.selection("EDGE", "Edge_1_7"), model.selection("EDGE", "Edge_1_8"), model.selection("EDGE", "Edge_1_9"), model.selection("EDGE", "Edge_1_10"), model.selection("EDGE", "Edge_1_11"), model.selection("EDGE", "Edge_1_12"), model.selection("EDGE", "Edge_1_13"), model.selection("EDGE", "Edge_1_14")]
Extrusion_1 = model.addExtrusion(Part_2_doc, Extrusion_1_objects, model.selection("EDGE", "PartSet/OX"), 300, 0)
Split_1_objects_2 = [model.selection("FACE", "Extrusion_1_1"), model.selection("FACE", "Extrusion_1_2"), model.selection("FACE", "Extrusion_1_3"), model.selection("FACE", "Extrusion_1_4"), model.selection("FACE", "Extrusion_1_5"), model.selection("FACE", "Extrusion_1_6"), model.selection("FACE", "Extrusion_1_7"), model.selection("FACE", "Extrusion_1_8"), model.selection("FACE", "Extrusion_1_9"), model.selection("FACE", "Extrusion_1_10"), model.selection("FACE", "Extrusion_1_11"), model.selection("FACE", "Extrusion_1_12"), model.selection("FACE", "Extrusion_1_13"), model.selection("FACE", "Extrusion_1_14")]
Split_1 = model.addSplit(Part_2_doc, [model.selection("COMPSOLID", "Revolution_1_1")], Split_1_objects_2)
Group_1_objects = [model.selection("SOLID", "Split_1_1_25"), model.selection("SOLID", "Split_1_1_28"), model.selection("SOLID", "Split_1_1_26"), model.selection("SOLID", "Split_1_1_27"), model.selection("SOLID", "Split_1_1_31"), model.selection("SOLID", "Split_1_1_35"), model.selection("SOLID", "Split_1_1_36"), model.selection("SOLID", "Split_1_1_32"), model.selection("SOLID", "Split_1_1_34"), model.selection("SOLID", "Split_1_1_30"), model.selection("SOLID", "Split_1_1_33"), model.selection("SOLID", "Split_1_1_29")]
Group_1 = model.addGroup(Part_2_doc, Group_1_objects)
Group_1.setName("CORPS")
Group_1.result().setName("CORPS")
Group_2_objects = [model.selection("SOLID", "Split_1_1_41"), model.selection("SOLID", "Split_1_1_49"), model.selection("SOLID", "Split_1_1_69"), model.selection("SOLID", "Split_1_1_71"), model.selection("SOLID", "Split_1_1_51"), model.selection("SOLID", "Split_1_1_43"), model.selection("SOLID", "Split_1_1_44"), model.selection("SOLID", "Split_1_1_67"), model.selection("SOLID", "Split_1_1_39"), model.selection("SOLID", "Split_1_1_47"), model.selection("SOLID", "Split_1_1_45"), model.selection("SOLID", "Split_1_1_37"), model.selection("SOLID", "Split_1_1_65"), model.selection("SOLID", "Split_1_1_66"), model.selection("SOLID", "Split_1_1_38"), model.selection("SOLID", "Split_1_1_46"), model.selection("SOLID", "Split_1_1_48"), model.selection("SOLID", "Split_1_1_40"), model.selection("SOLID", "Split_1_1_68"), model.selection("SOLID", "Split_1_1_42"), model.selection("SOLID", "Split_1_1_70"), model.selection("SOLID", "Split_1_1_50"), model.selection("SOLID", "Split_1_1_52"), model.selection("SOLID", "Split_1_1_72"), model.selection("SOLID", "Split_1_1_63"), model.selection("SOLID", "Split_1_1_64"), model.selection("SOLID", "Split_1_1_61"), model.selection("SOLID", "Split_1_1_62"), model.selection("SOLID", "Split_1_1_58"), model.selection("SOLID", "Split_1_1_57"), model.selection("SOLID", "Split_1_1_54"), model.selection("SOLID", "Split_1_1_53"), model.selection("SOLID", "Split_1_1_55"), model.selection("SOLID", "Split_1_1_56"), model.selection("SOLID", "Split_1_1_60"), model.selection("SOLID", "Split_1_1_59")]
Group_2 = model.addGroup(Part_2_doc, Group_2_objects)
Group_2.setName("TETE")
Group_2.result().setName("TETE")
Group_3_objects = [model.selection("SOLID", "Split_1_1_2"), model.selection("SOLID", "Split_1_1_1"), model.selection("SOLID", "Split_1_1_5"), model.selection("SOLID", "Split_1_1_9"), model.selection("SOLID", "Split_1_1_13"), model.selection("SOLID", "Split_1_1_14"), model.selection("SOLID", "Split_1_1_15"), model.selection("SOLID", "Split_1_1_19"), model.selection("SOLID", "Split_1_1_11"), model.selection("SOLID", "Split_1_1_12"), model.selection("SOLID", "Split_1_1_24"), model.selection("SOLID", "Split_1_1_23"), model.selection("SOLID", "Split_1_1_16"), model.selection("SOLID", "Split_1_1_8"), model.selection("SOLID", "Split_1_1_20"), model.selection("SOLID", "Split_1_1_4"), model.selection("SOLID", "Split_1_1_3"), model.selection("SOLID", "Split_1_1_7"), model.selection("SOLID", "Split_1_1_10"), model.selection("SOLID", "Split_1_1_6")]
Group_3 = model.addGroup(Part_2_doc, Group_3_objects)
Group_3.setName("PIED")
Group_3.result().setName("PIED")
Group_4 = model.addGroup(Part_2_doc, [model.selection("EDGE", "Split_1_1_36/Generated_Edge&Sketch_2/SketchLine_31&Sketch_1/SketchLine_7")])
Group_4.setName("corps_hauteur")
Group_4.result().setName("corps_hauteur")
Group_5 = model.addGroup(Part_2_doc, [model.selection("EDGE", "Split_1_1_11/Generated_Edge&Sketch_2/SketchLine_31&Sketch_1/SketchLine_5")])
Group_5.setName("pied_hauteur")
Group_5.result().setName("pied_hauteur")
Group_6 = model.addGroup(Part_2_doc, [model.selection("EDGE", "Split_1_1_24/Generated_Edge&Sketch_2/SketchLine_31&Sketch_1/SketchLine_6")])
Group_6.setName("chanfrein")
Group_6.result().setName("chanfrein")
Group_7 = model.addGroup(Part_2_doc, [model.selection("EDGE", "Split_1_1_72/Generated_Edge&Sketch_2/SketchLine_31&Sketch_1/SketchArc_1_2")])
Group_7.setName("filet")
Group_7.result().setName("filet")
Group_8 = model.addGroup(Part_2_doc, [model.selection("EDGE", "Split_1_1_42/Generated_Edge&Sketch_2/SketchLine_31&Sketch_1/SketchLine_9")])
Group_8.setName("tete_bas")
Group_8.result().setName("tete_bas")
Group_9_objects = [model.selection("FACE", "Split_1_1_12/Modified_Face&Sketch_1/SketchLine_5"), model.selection("FACE", "Split_1_1_11/Modified_Face&Sketch_1/SketchLine_5"), model.selection("FACE", "Split_1_1_9/Modified_Face&Sketch_1/SketchLine_5"), model.selection("FACE", "Split_1_1_8/Modified_Face&Sketch_1/SketchLine_5"), model.selection("FACE", "Split_1_1_4/Modified_Face&Sketch_1/SketchLine_5"), model.selection("FACE", "Split_1_1_1/Modified_Face&Sketch_1/SketchLine_5"), model.selection("FACE", "Split_1_1_2/Modified_Face&Sketch_1/SketchLine_5"), model.selection("FACE", "Split_1_1_5/Modified_Face&Sketch_1/SketchLine_5")]
Group_9 = model.addGroup(Part_2_doc, Group_9_objects)
Group_9.setName("ADH_PIEDS")
Group_9.result().setName("ADH_PIEDS")
Group_10_objects = [model.selection("FACE", "Split_1_1_13/Modified_Face&Sketch_1/SketchLine_6"), model.selection("FACE", "Split_1_1_14/Modified_Face&Sketch_1/SketchLine_6"), model.selection("FACE", "Split_1_1_25/Modified_Face&Sketch_1/SketchLine_7"), model.selection("FACE", "Split_1_1_26/Modified_Face&Sketch_1/SketchLine_7"), model.selection("FACE", "Split_1_1_15/Modified_Face&Sketch_1/SketchLine_6"), model.selection("FACE", "Split_1_1_27/Modified_Face&Sketch_1/SketchLine_7"), model.selection("FACE", "Split_1_1_19/Modified_Face&Sketch_1/SketchLine_6"), model.selection("FACE", "Split_1_1_31/Modified_Face&Sketch_1/SketchLine_7"), model.selection("FACE", "Split_1_1_23/Modified_Face&Sketch_1/SketchLine_6"), model.selection("FACE", "Split_1_1_35/Modified_Face&Sketch_1/SketchLine_7"), model.selection("FACE", "Split_1_1_24/Modified_Face&Sketch_1/SketchLine_6"), model.selection("FACE", "Split_1_1_36/Modified_Face&Sketch_1/SketchLine_7"), model.selection("FACE", "Split_1_1_32/Modified_Face&Sketch_1/SketchLine_7"), model.selection("FACE", "Split_1_1_20/Modified_Face&Sketch_1/SketchLine_6"), model.selection("FACE", "Split_1_1_16/Modified_Face&Sketch_1/SketchLine_6"), model.selection("FACE", "Split_1_1_28/Modified_Face&Sketch_1/SketchLine_7"), model.selection("FACE", "Split_1_1_70/Modified_Face&Sketch_1/SketchArc_1_2"), model.selection("FACE", "Split_1_1_42/Modified_Face&Sketch_1/SketchLine_8"), model.selection("FACE", "Split_1_1_68/Modified_Face&Sketch_1/SketchArc_1_2"), model.selection("FACE", "Split_1_1_40/Modified_Face&Sketch_1/SketchLine_8"), model.selection("FACE", "Split_1_1_66/Modified_Face&Sketch_1/SketchArc_1_2"), model.selection("FACE", "Split_1_1_38/Modified_Face&Sketch_1/SketchLine_8"), model.selection("FACE", "Split_1_1_37/Modified_Face&Sketch_1/SketchLine_8"), model.selection("FACE", "Split_1_1_65/Modified_Face&Sketch_1/SketchArc_1_2"), model.selection("FACE", "Split_1_1_39/Modified_Face&Sketch_1/SketchLine_8"), model.selection("FACE", "Split_1_1_67/Modified_Face&Sketch_1/SketchArc_1_2"), model.selection("FACE", "Split_1_1_69/Modified_Face&Sketch_1/SketchArc_1_2"), model.selection("FACE", "Split_1_1_41/Modified_Face&Sketch_1/SketchLine_8"), model.selection("FACE", "Split_1_1_43/Modified_Face&Sketch_1/SketchLine_8"), model.selection("FACE", "Split_1_1_71/Modified_Face&Sketch_1/SketchArc_1_2"), model.selection("FACE", "Split_1_1_44/Modified_Face&Sketch_1/SketchLine_8"), model.selection("FACE", "Split_1_1_72/Modified_Face&Sketch_1/SketchArc_1_2"), model.selection("FACE", "Split_1_1_52/Modified_Face&Sketch_1/SketchLine_9"), model.selection("FACE", "Split_1_1_44/Modified_Face&Sketch_1/SketchLine_9"), model.selection("FACE", "Split_1_1_43/Modified_Face&Sketch_1/SketchLine_9"), model.selection("FACE", "Split_1_1_51/Modified_Face&Sketch_1/SketchLine_9"), model.selection("FACE", "Split_1_1_50/Modified_Face&Sketch_1/SketchLine_9"), model.selection("FACE", "Split_1_1_42/Modified_Face&Sketch_1/SketchLine_9"), model.selection("FACE", "Split_1_1_40/Modified_Face&Sketch_1/SketchLine_9"), model.selection("FACE", "Split_1_1_48/Modified_Face&Sketch_1/SketchLine_9"), model.selection("FACE", "Split_1_1_46/Modified_Face&Sketch_1/SketchLine_9"), model.selection("FACE", "Split_1_1_38/Modified_Face&Sketch_1/SketchLine_9"), model.selection("FACE", "Split_1_1_37/Modified_Face&Sketch_1/SketchLine_9"), model.selection("FACE", "Split_1_1_45/Modified_Face&Sketch_1/SketchLine_9"), model.selection("FACE", "Split_1_1_39/Modified_Face&Sketch_1/SketchLine_9"), model.selection("FACE", "Split_1_1_47/Modified_Face&Sketch_1/SketchLine_9"), model.selection("FACE", "Split_1_1_49/Modified_Face&Sketch_1/SketchLine_9"), model.selection("FACE", "Split_1_1_41/Modified_Face&Sketch_1/SketchLine_9")]
Group_10 = model.addGroup(Part_2_doc, Group_10_objects)
Group_10.setName("CONTACT_VIS")
Group_10.result().setName("CONTACT_VIS")
Translation_1 = model.addTranslation(Part_2_doc, [model.selection("COMPSOLID", "Split_1_1")], 0, 100, 0)
AngularCopy_1 = model.addMultiRotation(Part_2_doc, [model.selection("COMPSOLID", "Translation_1_1")], model.selection("EDGE", "PartSet/OX"), 4)
model.do()

# move groups to the end
Part_2_doc.moveFeature(Group_1.feature(), AngularCopy_1.feature())
Part_2_doc.moveFeature(Group_2.feature(), Group_1.feature())
Part_2_doc.moveFeature(Group_3.feature(), Group_2.feature())
Part_2_doc.moveFeature(Group_4.feature(), Group_3.feature())
Part_2_doc.moveFeature(Group_5.feature(), Group_4.feature())
Part_2_doc.moveFeature(Group_6.feature(), Group_5.feature())
Part_2_doc.moveFeature(Group_7.feature(), Group_6.feature())
Part_2_doc.moveFeature(Group_8.feature(), Group_7.feature())
Part_2_doc.moveFeature(Group_9.feature(), Group_8.feature())
Part_2_doc.moveFeature(Group_10.feature(), Group_9.feature())
model.end()

from ModelAPI import *

aFactory = ModelAPI_Session.get().validators()

# Check groups
a = 0
num_in_groups = [48, 144, 80, 4, 4, 4, 4, 4, 32, 192]
for i in range(Part_2_doc.size("Groups")):
  GroupFeature = Part_2_doc.feature(objectToResult(Part_2_doc.object("Groups", i)))
  assert(aFactory.validate(GroupFeature))
  assert(GroupFeature.selectionList("group_list").size() == num_in_groups[a])
  a = a + 1

assert(model.checkPythonDump())
