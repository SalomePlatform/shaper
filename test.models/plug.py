# Copyright (C) 2017-2023  CEA/DEN, EDF R&D
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
from GeomAPI import GeomAPI_Shape

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

Param_Width = model.addParameter(Part_1_doc, "Width", "64")
Param_Depth = model.addParameter(Part_1_doc, "Depth", "33")
Param_D1out = model.addParameter(Part_1_doc, "D1_out", "38")
Param_D2out = model.addParameter(Part_1_doc, "D2_out", "33")
Param_D3out = model.addParameter(Part_1_doc, "D3_out", "30")
Param_D4out = model.addParameter(Part_1_doc, "D4_out", "20")
Param_D1in = model.addParameter(Part_1_doc, "D1_in", "21")
Param_D2in = model.addParameter(Part_1_doc, "D2_in", "5")
Param_ChannelDepth = model.addParameter(Part_1_doc, "Channel_Depth", "1.3")
Param_ChannelWidth = model.addParameter(Part_1_doc, "Channel_Width", "1.5")
Param_Drill1Loc = model.addParameter(Part_1_doc, "Drill1_Loc", "7.5")
Param_Drill1Radius = model.addParameter(Part_1_doc, "Drill1_Radius", "2.25")
Param_Drill1Holder = model.addParameter(Part_1_doc, "Drill1Holder_Width", "8")
Param_Drill2Loc = model.addParameter(Part_1_doc, "Drill2_Loc", "6.5")
Param_Drill2Radius = model.addParameter(Part_1_doc, "Drill2_Radius", "1.4")
Param_Drill2InnerRadius = model.addParameter(Part_1_doc, "Drill2_InnerRadius", "0.5")
Param_Drill2DepthMin = model.addParameter(Part_1_doc, "Drill2_DepthMin", "6.5")
Param_Drill2DepthMax = model.addParameter(Part_1_doc, "Drill2_DepthMax", "7.5")
Param_Chamfer = model.addParameter(Part_1_doc, "Chamfer", "0.5")
Param_Fillet1 = model.addParameter(Part_1_doc, "Fillet1", "5")
Param_Fillet2 = model.addParameter(Part_1_doc, "Fillet2", "0.3")
Param_Fillet3 = model.addParameter(Part_1_doc, "Fillet3", "0.5")
Param_SlotWidth = model.addParameter(Part_1_doc, "Slot_Width", "8")

Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 3, 0.5, 2.5)
SketchLine_2 = Sketch_1.addLine(0.5, 2.5, 9.5, 2.5)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(9.5, 2.5, 9.5, 3.1)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(9.5, 3.1, 24.86574363608673, 3.1)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(24.86574363608673, 3.1, 26, 3.3)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(26, 3.3, 26, 5.5)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(31, 10.5, 64, 10.5)
SketchLine_8 = Sketch_1.addLine(64, 10.5, 64, 15)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_1.addLine(64, 15, 61, 15)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchLine_10 = Sketch_1.addLine(61, 15, 61, 12.75)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchLine_11 = Sketch_1.addLine(61, 12.75, 58, 12.75)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchLine_12 = Sketch_1.addLine(58, 12.75, 58, 14)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchLine_13 = Sketch_1.addLine(58, 14, 57, 15)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchLine_14 = Sketch_1.addLine(57, 15, 54, 15)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchLine_15 = Sketch_1.addLine(54, 15, 52.5, 16.5)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchLine_16 = Sketch_1.addLine(52.5, 16.5, 35, 16.5)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchLine_17 = Sketch_1.addLine(35, 16.5, 34, 15.5)
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_16.endPoint(), SketchLine_17.startPoint())
SketchLine_18 = Sketch_1.addLine(34, 15.5, 32, 15.5)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_17.endPoint(), SketchLine_18.startPoint())
SketchLine_19 = Sketch_1.addLine(32, 15.5, 32, 19)
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
SketchLine_20 = Sketch_1.addLine(32, 19, 22.5, 19)
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_19.endPoint(), SketchLine_20.startPoint())
SketchLine_21 = Sketch_1.addLine(22.5, 19, 22.5, 13)
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchLine_20.endPoint(), SketchLine_21.startPoint())
SketchLine_22 = Sketch_1.addLine(22.5, 13, 22, 12.5)
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchLine_21.endPoint(), SketchLine_22.startPoint())
SketchLine_23 = Sketch_1.addLine(22, 12.5, 22, 10)
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchLine_22.endPoint(), SketchLine_23.startPoint())
SketchLine_24 = Sketch_1.addLine(22, 10, 20, 10)
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchLine_23.endPoint(), SketchLine_24.startPoint())
SketchLine_25 = Sketch_1.addLine(20, 10, 20, 8.7)
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchLine_24.endPoint(), SketchLine_25.startPoint())
SketchLine_26 = Sketch_1.addLine(20, 8.7, 18.5, 8.7)
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchLine_25.endPoint(), SketchLine_26.startPoint())
SketchLine_27 = Sketch_1.addLine(18.5, 8.7, 18.5, 10)
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchLine_26.endPoint(), SketchLine_27.startPoint())
SketchLine_28 = Sketch_1.addLine(18.5, 10, 10.5, 10)
SketchConstraintCoincidence_26 = Sketch_1.setCoincident(SketchLine_27.endPoint(), SketchLine_28.startPoint())
SketchLine_29 = Sketch_1.addLine(10.5, 10, 10.5, 8.7)
SketchConstraintCoincidence_27 = Sketch_1.setCoincident(SketchLine_28.endPoint(), SketchLine_29.startPoint())
SketchLine_30 = Sketch_1.addLine(10.5, 8.7, 9, 8.7)
SketchConstraintCoincidence_28 = Sketch_1.setCoincident(SketchLine_29.endPoint(), SketchLine_30.startPoint())
SketchLine_31 = Sketch_1.addLine(9, 8.7, 9, 10)
SketchConstraintCoincidence_29 = Sketch_1.setCoincident(SketchLine_30.endPoint(), SketchLine_31.startPoint())
SketchLine_32 = Sketch_1.addLine(9, 10, 0.5, 10)
SketchConstraintCoincidence_30 = Sketch_1.setCoincident(SketchLine_31.endPoint(), SketchLine_32.startPoint())
SketchLine_33 = Sketch_1.addLine(0.5, 10, 0, 9.5)
SketchConstraintCoincidence_31 = Sketch_1.setCoincident(SketchLine_32.endPoint(), SketchLine_33.startPoint())
SketchLine_34 = Sketch_1.addLine(0, 9.5, 0, 3)
SketchConstraintCoincidence_32 = Sketch_1.setCoincident(SketchLine_33.endPoint(), SketchLine_34.startPoint())
SketchConstraintCoincidence_33 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_34.endPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintAngle_1 = Sketch_1.setAngleComplementary(SketchLine_4.result(), SketchLine_5.result(), 10)
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_1.endPoint(), "D2_in/2")
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_8.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_9.result())
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_16.result(), SketchLine_7.result())
SketchConstraintAngle_2 = Sketch_1.setAngleComplementary(SketchLine_15.result(), SketchLine_16.result(), 45)
SketchConstraintAngle_3 = Sketch_1.setAngleComplementary(SketchLine_13.result(), SketchLine_14.result(), 45)
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_12.result())
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_10.result())
SketchConstraintHorizontal_5 = Sketch_1.setHorizontal(SketchLine_11.result())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_17.result(), SketchLine_15.result())
SketchConstraintVertical_6 = Sketch_1.setVertical(SketchLine_19.result())
SketchConstraintHorizontal_6 = Sketch_1.setHorizontal(SketchLine_18.result())
SketchConstraintParallel_2 = Sketch_1.setParallel(SketchLine_20.result(), SketchLine_7.result())
SketchConstraintVertical_7 = Sketch_1.setVertical(SketchLine_21.result())
SketchConstraintVertical_8 = Sketch_1.setVertical(SketchLine_23.result())
SketchConstraintHorizontal_7 = Sketch_1.setHorizontal(SketchLine_24.result())
SketchConstraintCollinear_1 = Sketch_1.setCollinear(SketchLine_28.result(), SketchLine_24.result())
SketchConstraintCollinear_2 = Sketch_1.setCollinear(SketchLine_32.result(), SketchLine_28.result())
SketchConstraintVertical_9 = Sketch_1.setVertical(SketchLine_34.result())
SketchConstraintHorizontal_8 = Sketch_1.setHorizontal(SketchLine_30.result())
SketchConstraintCollinear_3 = Sketch_1.setCollinear(SketchLine_26.result(), SketchLine_30.result())
SketchConstraintVertical_10 = Sketch_1.setVertical(SketchLine_31.result())
SketchConstraintVertical_11 = Sketch_1.setVertical(SketchLine_29.result())
SketchConstraintVertical_12 = Sketch_1.setVertical(SketchLine_27.result())
SketchConstraintVertical_13 = Sketch_1.setVertical(SketchLine_25.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_24.result(), "D4_out/2", True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_30.result(), "D4_out/2-Channel_Depth", True)
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_34.endPoint(), SketchLine_1.endPoint(), "Chamfer")
SketchConstraintDistanceVertical_2 = Sketch_1.setVerticalDistance(SketchLine_1.endPoint(), SketchLine_1.startPoint(), "Chamfer")
SketchConstraintDistanceHorizontal_2 = Sketch_1.setHorizontalDistance(SketchLine_34.startPoint(), SketchLine_33.startPoint(), "Chamfer")
SketchConstraintDistanceVertical_3 = Sketch_1.setVerticalDistance(SketchLine_34.startPoint(), SketchLine_33.startPoint(), "Chamfer")
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_35 = SketchProjection_2.createdFeature()
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_3.endPoint(), SketchLine_35.result(), "D2_in/2+0.6", True)
SketchConstraintDistanceHorizontal_3 = Sketch_1.setHorizontalDistance(SketchLine_3.endPoint(), SketchLine_5.endPoint(), "Depth/2")
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchLine_5.endPoint(), SketchLine_35.result(), "D2_in/2+0.8", True)
SketchArc_1 = Sketch_1.addArc(31, 5.5, 31, 10.5, 26, 5.5, False)
SketchConstraintCoincidence_34 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_35 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_6.endPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_7.result())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_6.result())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], "Fillet1")
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchLine_7.endPoint(), SketchLine_35.result(), "D1_in/2", True)
SketchConstraintCollinear_4 = Sketch_1.setCollinear(SketchLine_14.result(), SketchLine_9.result())
SketchConstraintDistanceHorizontal_4 = Sketch_1.setHorizontalDistance(SketchLine_34.startPoint(), SketchLine_23.startPoint(), "Width/32*11")
SketchConstraintDistanceHorizontal_5 = Sketch_1.setHorizontalDistance(SketchLine_23.startPoint(), SketchLine_22.startPoint(), "Chamfer")
SketchConstraintDistanceVertical_4 = Sketch_1.setVerticalDistance(SketchLine_23.startPoint(), SketchLine_22.startPoint(), "Chamfer")
SketchConstraintDistanceHorizontal_6 = Sketch_1.setHorizontalDistance(SketchLine_15.startPoint(), SketchLine_8.endPoint(), 10)
SketchConstraintDistance_6 = Sketch_1.setDistance(SketchLine_8.endPoint(), SketchLine_35.result(), "D3_out/2", True)
SketchConstraintDistance_7 = Sketch_1.setDistance(SketchLine_11.startPoint(), SketchLine_35.result(), "(D3_out+D1_in)/4", True)
SketchConstraintDistanceHorizontal_7 = Sketch_1.setHorizontalDistance(SketchLine_10.startPoint(), SketchLine_8.endPoint(), 3)
SketchConstraintDistanceHorizontal_8 = Sketch_1.setHorizontalDistance(SketchLine_12.endPoint(), SketchLine_10.startPoint(), 3)
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_30.result(), SketchLine_26.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_26.result(), "Channel_Width")
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_28.result(), "Drill1Holder_Width")
SketchConstraintDistanceHorizontal_9 = Sketch_1.setHorizontalDistance(SketchArc_1.center(), SketchLine_7.endPoint(), "Depth")
SketchConstraintDistance_8 = Sketch_1.setDistance(SketchLine_16.startPoint(), SketchLine_35.result(), "D2_out/2", True)
SketchConstraintDistanceHorizontal_10 = Sketch_1.setHorizontalDistance(SketchLine_20.startPoint(), SketchLine_8.endPoint(), "Width/2")
SketchConstraintDistance_9 = Sketch_1.setDistance(SketchLine_17.endPoint(), SketchLine_35.result(), "D2_out/2-1", True)
SketchConstraintDistanceHorizontal_11 = Sketch_1.setHorizontalDistance(SketchLine_20.startPoint(), SketchLine_17.endPoint(), 2)
SketchConstraintCoincidence_36 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_35).startPoint(), SketchLine_34.result())
SketchConstraintDistance_10 = Sketch_1.setDistance(SketchAPI_Line(SketchLine_35).startPoint(), SketchLine_20.result(), "D1_out/2", True)
SketchPoint_2 = Sketch_1.addPoint(14.5, 10)
SketchConstraintCoincidence_37 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_28.result())
SketchConstraintMiddle_1 = Sketch_1.setMiddlePoint(SketchPoint_2.coordinates(), SketchLine_28.result())
SketchConstraintDistanceHorizontal_12 = Sketch_1.setHorizontalDistance(SketchPoint_2.coordinates(), SketchLine_24.startPoint(), "Drill1_Loc")
SketchConstraintDistanceHorizontal_13 = Sketch_1.setHorizontalDistance(SketchLine_34.startPoint(), SketchLine_9.startPoint(), "Width")
SketchConstraintDistanceHorizontal_14 = Sketch_1.setHorizontalDistance(SketchLine_14.startPoint(), SketchLine_12.endPoint(), 1)
SketchConstraintDistance_11 = Sketch_1.setDistance(SketchLine_23.startPoint(), SketchLine_35.result(), "(D4_out+D2_in)/2", True)
model.do()
# check Sketch_1
model.checkSketch(Sketch_1, 0)

Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection("EDGE", "PartSet/OX"), 360, 0)
# check Revolution_1
model.testNbResults(Revolution_1, 1)
model.testNbSubResults(Revolution_1, [0])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.FACE, [35])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.EDGE, [114])
model.testNbSubShapes(Revolution_1, GeomAPI_Shape.VERTEX, [228])
model.testResultsVolumes(Revolution_1, [29055.346994571114919381216168404])

Plane_4 = model.addPlane(Part_1_doc, model.selection("EDGE", "PartSet/OY"), model.selection("VERTEX", "Sketch_1/SketchPoint_2"), True)

Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchPoint_2"), False)
SketchPoint_3 = SketchProjection_3.createdFeature()
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchArc_1_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_8])"), False)
SketchLine_36 = SketchProjection_4.createdFeature()
SketchCircle_1 = Sketch_2.addCircle(14.5, 0, 2.25)
SketchConstraintCoincidence_38 = Sketch_2.setCoincident(SketchPoint_3.result(), SketchCircle_1.center())
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchCircle_1.results()[1], "Drill1_Radius")
model.do()
# check Sketch_2
model.checkSketch(Sketch_2, 0)

ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_1_2f")], model.selection(), model.selection("FACE", "PartSet/XOZ"), 0, model.selection(), 0, [model.selection("SOLID", "Revolution_1_1")])
# check ExtrusionCut_1
model.testNbResults(ExtrusionCut_1, 1)
model.testNbSubResults(ExtrusionCut_1, [0])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [36])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [132])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [264])
model.testResultsVolumes(ExtrusionCut_1, [28943.115000463178148493170738220])

Fillet_1 = model.addFillet(Part_1_doc, [model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_21][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_20]")], "Fillet1")
# check Fillet_1
model.testNbResults(Fillet_1, 1)
model.testNbSubResults(Fillet_1, [0])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.FACE, [37])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.EDGE, [136])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.VERTEX, [272])
model.testResultsVolumes(Fillet_1, [28340.280766451105591841042041779])

Fillet_2 = model.addFillet(Part_1_doc, [model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_31"), model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_29"), model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_27"), model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_25")], "Fillet2")
# check Fillet_2
model.testNbResults(Fillet_2, 1)
model.testNbSubResults(Fillet_2, [0])
model.testNbSubShapes(Fillet_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Fillet_2, GeomAPI_Shape.FACE, [45])
model.testNbSubShapes(Fillet_2, GeomAPI_Shape.EDGE, [168])
model.testNbSubShapes(Fillet_2, GeomAPI_Shape.VERTEX, [336])
model.testResultsVolumes(Fillet_2, [28339.715350479829794494435191154])

Fillet_3 = model.addFillet(Part_1_doc, [model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_19")], "Fillet3")
# check Fillet_3
model.testNbResults(Fillet_3, 1)
model.testNbSubResults(Fillet_3, [0])
model.testNbSubShapes(Fillet_3, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Fillet_3, GeomAPI_Shape.FACE, [47])
model.testNbSubShapes(Fillet_3, GeomAPI_Shape.EDGE, [176])
model.testNbSubShapes(Fillet_3, GeomAPI_Shape.VERTEX, [352])
model.testResultsVolumes(Fillet_3, [28338.609830057110229972749948502])

Fillet_4 = model.addFillet(Part_1_doc, [model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_11")], "Fillet3")
# check Fillet_4
model.testNbResults(Fillet_4, 1)
model.testNbSubResults(Fillet_4, [0])
model.testNbSubShapes(Fillet_4, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Fillet_4, GeomAPI_Shape.FACE, [49])
model.testNbSubShapes(Fillet_4, GeomAPI_Shape.EDGE, [184])
model.testNbSubShapes(Fillet_4, GeomAPI_Shape.VERTEX, [368])
model.testResultsVolumes(Fillet_4, [28347.280717884430487174540758133])

Fillet_5 = model.addFillet(Part_1_doc, [model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_9")], "Fillet2")
# check Fillet_5
model.testNbResults(Fillet_5, 1)
model.testNbSubResults(Fillet_5, [0])
model.testNbSubShapes(Fillet_5, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Fillet_5, GeomAPI_Shape.FACE, [51])
model.testNbSubShapes(Fillet_5, GeomAPI_Shape.EDGE, [192])
model.testNbSubShapes(Fillet_5, GeomAPI_Shape.VERTEX, [384])
model.testResultsVolumes(Fillet_5, [28343.655816829603281803429126740])

ExtrusionCut_2 = model.addExtrusionCut(Part_1_doc, [], model.selection(), "D4_out/2", "D4_out/2", [model.selection("SOLID", "Fillet_5_1")])
Sketch_3 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchLine_37 = Sketch_3.addLine(9, -9.914857089637437, 9, -18)
SketchLine_38 = Sketch_3.addLine(9, -18, 0.5, -18)
SketchConstraintCoincidence_39 = Sketch_3.setCoincident(SketchLine_37.endPoint(), SketchLine_38.startPoint())
SketchLine_39 = Sketch_3.addLine(0.5, -18, 0.5, -10)
SketchConstraintCoincidence_40 = Sketch_3.setCoincident(SketchLine_38.endPoint(), SketchLine_39.startPoint())
SketchLine_40 = Sketch_3.addLine(0.5, -10, 7.788407623149843, -8.714857089637437)
SketchConstraintCoincidence_41 = Sketch_3.setCoincident(SketchLine_39.endPoint(), SketchLine_40.startPoint())
SketchLine_41 = Sketch_3.addLine(7.788407623149843, -8.714857089637437, 8, -9.914857089637437)
SketchConstraintCoincidence_42 = Sketch_3.setCoincident(SketchLine_40.endPoint(), SketchLine_41.startPoint())
SketchLine_42 = Sketch_3.addLine(8, -9.914857089637437, 9, -9.914857089637437)
SketchConstraintCoincidence_43 = Sketch_3.setCoincident(SketchLine_41.endPoint(), SketchLine_42.startPoint())
SketchConstraintCoincidence_44 = Sketch_3.setCoincident(SketchLine_42.endPoint(), SketchLine_37.startPoint())
SketchProjection_5 = Sketch_3.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_43 = SketchProjection_5.createdFeature()
SketchProjection_6 = Sketch_3.addProjection(model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_34][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_33])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_33][Fillet_2_1/MF:Fillet&Sketch_1/SketchLine_32])_Fillet_5_1"), False)
SketchLine_44 = SketchProjection_6.createdFeature()
SketchProjection_9 = Sketch_3.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_32"), False)
SketchLine_47 = SketchProjection_9.createdFeature()
SketchConstraintVertical_14 = Sketch_3.setVertical(SketchLine_37.result())
SketchConstraintVertical_15 = Sketch_3.setVertical(SketchLine_39.result())
SketchConstraintHorizontal_9 = Sketch_3.setHorizontal(SketchLine_38.result())
SketchConstraintHorizontal_10 = Sketch_3.setHorizontal(SketchLine_42.result())
SketchConstraintPerpendicular_2 = Sketch_3.setPerpendicular(SketchLine_40.result(), SketchLine_41.result())
SketchConstraintDistanceVertical_5 = Sketch_3.setVerticalDistance(SketchLine_42.startPoint(), SketchLine_40.endPoint(), 1.2)
SketchConstraintMirror_1 = Sketch_3.addMirror(SketchLine_43.result(), [SketchLine_44.result(), SketchLine_47.result()])
[SketchLine_50, SketchLine_51] = SketchConstraintMirror_1.mirrored()
SketchLine_50.setAuxiliary(True)
SketchLine_51.setAuxiliary(True)
SketchConstraintCoincidence_45 = Sketch_3.setCoincident(SketchAPI_Line(SketchLine_51).startPoint(), SketchLine_37.result())
SketchConstraintCoincidence_46 = Sketch_3.setCoincident(SketchAPI_Line(SketchLine_50).startPoint(), SketchLine_39.endPoint())
SketchConstraintAngle_4 = Sketch_3.setAngle(SketchLine_51.result(), SketchLine_40.result(), 10)
SketchConstraintDistanceHorizontal_15 = Sketch_3.setHorizontalDistance(SketchAPI_Line(SketchLine_50).endPoint(), SketchLine_42.startPoint(), "Slot_Width")
SketchConstraintDistanceVertical_6 = Sketch_3.setVerticalDistance(SketchLine_39.startPoint(), SketchLine_39.endPoint(), 8)
ExtrusionCut_2.setNestedSketch(Sketch_3)
model.do()
# check Sketch_3
model.checkSketch(Sketch_3, 0)
# check ExtrusionCut_2
model.testNbResults(ExtrusionCut_2, 1)
model.testNbSubResults(ExtrusionCut_2, [0])
model.testNbSubShapes(ExtrusionCut_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionCut_2, GeomAPI_Shape.FACE, [55])
model.testNbSubShapes(ExtrusionCut_2, GeomAPI_Shape.EDGE, [214])
model.testNbSubShapes(ExtrusionCut_2, GeomAPI_Shape.VERTEX, [428])
model.testResultsVolumes(ExtrusionCut_2, [28317.789547563083033310249447823])

Folder_1 = model.addFolder(Part_1_doc, Fillet_1, Fillet_5)

Sketch_4 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchLine_52 = Sketch_4.addLine(28.5, -19.5, 28.5, 0)
SketchProjection_12 = Sketch_4.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_53 = SketchProjection_12.createdFeature()
SketchConstraintCoincidence_47 = Sketch_4.setCoincident(SketchLine_52.endPoint(), SketchLine_53.result())
SketchLine_54 = Sketch_4.addLine(28.5, 0, 29, 0)
SketchConstraintCoincidence_48 = Sketch_4.setCoincident(SketchLine_52.endPoint(), SketchLine_54.startPoint())
SketchConstraintCoincidence_49 = Sketch_4.setCoincident(SketchLine_54.endPoint(), SketchLine_53.result())
SketchLine_55 = Sketch_4.addLine(29, 0, 29, -11.5)
SketchConstraintCoincidence_50 = Sketch_4.setCoincident(SketchLine_54.endPoint(), SketchLine_55.startPoint())
SketchLine_56 = Sketch_4.addLine(28.5, -19.5, 30.4, -19.5)
SketchConstraintCoincidence_51 = Sketch_4.setCoincident(SketchLine_52.startPoint(), SketchLine_56.startPoint())
SketchLine_57 = Sketch_4.addLine(30.4, -19.5, 29.4, -18.5)
SketchConstraintCoincidence_52 = Sketch_4.setCoincident(SketchLine_56.endPoint(), SketchLine_57.startPoint())
SketchLine_58 = Sketch_4.addLine(29.4, -18.5, 29.4, -12.5)
SketchConstraintCoincidence_53 = Sketch_4.setCoincident(SketchLine_57.endPoint(), SketchLine_58.startPoint())
SketchArc_2 = Sketch_4.addArc(27.95, -12.5, 29.4, -12.5, 29, -11.5, False)
SketchConstraintCoincidence_54 = Sketch_4.setCoincident(SketchLine_58.endPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_55 = Sketch_4.setCoincident(SketchLine_55.endPoint(), SketchArc_2.endPoint())
SketchConstraintVertical_16 = Sketch_4.setVertical(SketchLine_52.result())
SketchConstraintVertical_17 = Sketch_4.setVertical(SketchLine_55.result())
SketchConstraintVertical_18 = Sketch_4.setVertical(SketchLine_58.result())
SketchConstraintHorizontal_11 = Sketch_4.setHorizontal(SketchLine_56.result())
SketchProjection_13 = Sketch_4.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_23_EndVertex"), False)
SketchPoint_4 = SketchProjection_13.createdFeature()
SketchConstraintDistanceHorizontal_17 = Sketch_4.setHorizontalDistance(SketchAPI_Point(SketchPoint_4).coordinates(), SketchLine_52.endPoint(), "Drill2_Loc")
SketchConstraintDistanceHorizontal_18 = Sketch_4.setHorizontalDistance(SketchLine_54.startPoint(), SketchLine_54.endPoint(), "Drill2_InnerRadius")
SketchProjection_14 = Sketch_4.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_20"), False)
SketchLine_59 = SketchProjection_14.createdFeature()
SketchConstraintAngle_5 = Sketch_4.setAngle(SketchLine_57.result(), SketchLine_56.result(), 45)
SketchConstraintMirror_2 = Sketch_4.addMirror(SketchLine_53.result(), [SketchLine_59.result()])
[SketchLine_60] = SketchConstraintMirror_2.mirrored()
SketchLine_60.setAuxiliary(True)
SketchConstraintDistance_12 = Sketch_4.setDistance(SketchArc_2.endPoint(), SketchLine_60.result(), "Drill2_DepthMax", True)
SketchConstraintDistance_13 = Sketch_4.setDistance(SketchLine_58.endPoint(), SketchLine_60.result(), "Drill2_DepthMin", True)
SketchConstraintDistance_14 = Sketch_4.setDistance(SketchLine_57.endPoint(), SketchLine_60.result(), 0.5, True)
SketchConstraintTangent_3 = Sketch_4.setTangent(SketchLine_58.result(), SketchArc_2.results()[1])
SketchPoint_5 = Sketch_4.addPoint(29.9, -19)
SketchConstraintCoincidence_56 = Sketch_4.setCoincident(SketchPoint_5.coordinates(), SketchLine_60.result())
SketchConstraintCoincidence_57 = Sketch_4.setCoincident(SketchPoint_5.coordinates(), SketchLine_57.result())
SketchConstraintDistance_15 = Sketch_4.setDistance(SketchPoint_5.coordinates(), SketchLine_52.result(), "Drill2_Radius", True)
SketchConstraintDistanceVertical_7 = Sketch_4.setVerticalDistance(SketchLine_56.endPoint(), SketchPoint_5.coordinates(), 0.5)
model.do()
# check Sketch_4
model.checkSketch(Sketch_4, 0)

RevolutionCut_1 = model.addRevolutionCut(Part_1_doc, [model.selection("COMPOUND", "Sketch_4")], model.selection("EDGE", "Sketch_4/SketchLine_48"), 360, 0, [model.selection("SOLID", "ExtrusionCut_2_1")])
# check RevolutionCut_1
model.testNbResults(RevolutionCut_1, 1)
model.testNbSubResults(RevolutionCut_1, [0])
model.testNbSubShapes(RevolutionCut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(RevolutionCut_1, GeomAPI_Shape.FACE, [59])
model.testNbSubShapes(RevolutionCut_1, GeomAPI_Shape.EDGE, [242])
model.testNbSubShapes(RevolutionCut_1, GeomAPI_Shape.VERTEX, [484])
model.testResultsVolumes(RevolutionCut_1, [28297.246041844206047244369983673])

RevolutionCut_2 = model.addRevolutionCut(Part_1_doc, [], model.selection("EDGE", "PartSet/OX"), 360, 0, [model.selection("SOLID", "RevolutionCut_1_1")])
Sketch_5 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchLine_61 = Sketch_5.addLine(63.5, 10.5, 64, 10.5)
SketchLine_61.setAuxiliary(True)
SketchProjection_15 = Sketch_5.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_7"), False)
SketchLine_62 = SketchProjection_15.createdFeature()
SketchConstraintCoincidence_58 = Sketch_5.setCoincident(SketchLine_61.startPoint(), SketchLine_62.result())
SketchConstraintCoincidence_59 = Sketch_5.setCoincident(SketchAPI_Line(SketchLine_62).endPoint(), SketchLine_61.endPoint())
SketchLine_63 = Sketch_5.addLine(64.25, 10.25, 64.25, 11.25)
SketchLine_64 = Sketch_5.addLine(64.25, 11.25, 63.25, 10.25)
SketchConstraintCoincidence_60 = Sketch_5.setCoincident(SketchLine_63.endPoint(), SketchLine_64.startPoint())
SketchConstraintLength_3 = Sketch_5.setLength(SketchLine_61.result(), "Chamfer")
SketchConstraintCoincidence_61 = Sketch_5.setCoincident(SketchLine_61.startPoint(), SketchLine_64.result())
SketchLine_65 = Sketch_5.addLine(63.25, 10.25, 64.25, 10.25)
SketchConstraintCoincidence_62 = Sketch_5.setCoincident(SketchLine_64.endPoint(), SketchLine_65.startPoint())
SketchConstraintCoincidence_63 = Sketch_5.setCoincident(SketchLine_63.startPoint(), SketchLine_65.endPoint())
SketchConstraintEqual_2 = Sketch_5.setEqual(SketchLine_65.result(), SketchLine_63.result())
SketchConstraintVertical_19 = Sketch_5.setVertical(SketchLine_63.result())
SketchConstraintHorizontal_12 = Sketch_5.setHorizontal(SketchLine_65.result())
SketchConstraintDistanceVertical_8 = Sketch_5.setVerticalDistance(SketchLine_65.startPoint(), SketchLine_61.startPoint(), "Chamfer/2")
SketchConstraintDistanceVertical_9 = Sketch_5.setVerticalDistance(SketchLine_61.startPoint(), SketchLine_63.endPoint(), "Chamfer*1.5")
RevolutionCut_2.setNestedSketch(Sketch_5)
model.do()
# check Sketch_5
model.checkSketch(Sketch_5, 0)
# check RevolutionCut_2
model.testNbResults(RevolutionCut_2, 1)
model.testNbSubResults(RevolutionCut_2, [0])
model.testNbSubShapes(RevolutionCut_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(RevolutionCut_2, GeomAPI_Shape.FACE, [60])
model.testNbSubShapes(RevolutionCut_2, GeomAPI_Shape.EDGE, [246])
model.testNbSubShapes(RevolutionCut_2, GeomAPI_Shape.VERTEX, [492])
model.testResultsVolumes(RevolutionCut_2, [28288.868184623894194373860955238])

Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), 0, False)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "RevolutionCut_2_1"), model.selection("FACE", "Plane_2")])
# check Partition_1
model.testNbResults(Partition_1, 1)
model.testNbSubResults(Partition_1, [2])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.FACE, [127])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.EDGE, [738])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.VERTEX, [1476])
model.testResultsVolumes(Partition_1, [28288.886256640704232268035411835])

Result = model.addRemoveSubShapes(Part_1_doc, model.selection("COMPSOLID", "Partition_1_1"))
Result.setSubShapesToRemove([model.selection("SOLID", "Partition_1_1_2")])
Result.result().setName("Plug")
# check Remove_SubShapes_1
model.testNbResults(Result, 1)
model.testNbSubResults(Result, [0])
model.testNbSubShapes(Result, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Result, GeomAPI_Shape.FACE, [63])
model.testNbSubShapes(Result, GeomAPI_Shape.EDGE, [366])
model.testNbSubShapes(Result, GeomAPI_Shape.VERTEX, [732])
model.testResultsVolumes(Result, [14144.443128843522572424262762070])

model.do()

# change parameters of the model
Param_Width.setValue(100)
model.do()
model.testNbResults(Result, 1)
model.testNbSubResults(Result, [0])
model.testNbSubShapes(Result, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Result, GeomAPI_Shape.FACE, [63])
model.testNbSubShapes(Result, GeomAPI_Shape.EDGE, [364])
model.testNbSubShapes(Result, GeomAPI_Shape.VERTEX, [728])
model.testResultsVolumes(Result, [26613.145838412528974004089832306])

# do small steps in parameter here to avoid "conflicting constraints" message coming from PlaneGCS
Param_Depth.setValue(40)
model.do()
Param_Depth.setValue(50)
model.do()
Param_Depth.setValue(55)
model.do()
model.testNbResults(Result, 1)
model.testNbSubResults(Result, [0])
model.testNbSubShapes(Result, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Result, GeomAPI_Shape.FACE, [63])
model.testNbSubShapes(Result, GeomAPI_Shape.EDGE, [364])
model.testNbSubShapes(Result, GeomAPI_Shape.VERTEX, [728])
model.testResultsVolumes(Result, [22968.704503291784931207075715065])

Param_Drill1Loc.setValue(15)
model.do()
model.testNbResults(Result, 1)
model.testNbSubResults(Result, [0])
model.testNbSubShapes(Result, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Result, GeomAPI_Shape.FACE, [63])
model.testNbSubShapes(Result, GeomAPI_Shape.EDGE, [364])
model.testNbSubShapes(Result, GeomAPI_Shape.VERTEX, [728])
model.testResultsVolumes(Result, [22969.259323538735770853236317635])

Param_Drill1Holder.setValue(16)
model.do()
model.testNbSubResults(Result, [0])
model.testNbSubShapes(Result, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Result, GeomAPI_Shape.FACE, [63])
model.testNbSubShapes(Result, GeomAPI_Shape.EDGE, [364])
model.testNbSubShapes(Result, GeomAPI_Shape.VERTEX, [728])
model.testResultsVolumes(Result, [22969.553506208951148437336087227])

Param_Drill2Loc.setValue(7.5)
model.do()
model.testNbResults(Result, 1)
model.testNbSubResults(Result, [0])
model.testNbSubShapes(Result, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Result, GeomAPI_Shape.FACE, [63])
model.testNbSubShapes(Result, GeomAPI_Shape.EDGE, [364])
model.testNbSubShapes(Result, GeomAPI_Shape.VERTEX, [728])
model.testResultsVolumes(Result, [22969.986144881491782143712043762])

Param_Drill2Radius.setValue(1.9)
model.do()
model.testNbResults(Result, 1)
model.testNbSubResults(Result, [0])
model.testNbSubShapes(Result, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Result, GeomAPI_Shape.FACE, [63])
model.testNbSubShapes(Result, GeomAPI_Shape.EDGE, [364])
model.testNbSubShapes(Result, GeomAPI_Shape.VERTEX, [728])
model.testResultsVolumes(Result, [22956.808010807682876475155353546])

Param_Fillet1.setValue(3)
model.do()
model.testNbResults(Result, 1)
model.testNbSubResults(Result, [0])
model.testNbSubShapes(Result, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Result, GeomAPI_Shape.FACE, [63])
model.testNbSubShapes(Result, GeomAPI_Shape.EDGE, [364])
model.testNbSubShapes(Result, GeomAPI_Shape.VERTEX, [728])
model.testResultsVolumes(Result, [23373.503259112552768783643841743])

model.end()

assert(model.checkPythonDump())
