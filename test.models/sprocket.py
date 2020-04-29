# Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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

Param_m = model.addParameter(Part_1_doc, "m", "1")
Param_Z = model.addParameter(Part_1_doc, "Z", "30")
model.addParameter(Part_1_doc, "d", "m*Z")
model.addParameter(Part_1_doc, "da", "d+2*m")
model.addParameter(Part_1_doc, "df", "d-2.5*m")
model.addParameter(Part_1_doc, "ha", "m")
model.addParameter(Part_1_doc, "hf", "1.25*m")
model.addParameter(Part_1_doc, "h", "2.25*m")
model.addParameter(Part_1_doc, "e", "d/8.")
model.addParameter(Part_1_doc, "angle_L", "360/(d*2)")
model.addParameter(Part_1_doc, "R", "d*(13/16)")
model.addParameter(Part_1_doc, "r_1", "d/4")
model.addParameter(Part_1_doc, "r_2", "d/10")
model.addParameter(Part_1_doc, "r_3", "d/40")
model.addParameter(Part_1_doc, "R_1", "d*(3/20)")
model.addParameter(Part_1_doc, "ep", "1.5")
model.addParameter(Part_1_doc, "angle_1", "15")
model.addParameter(Part_1_doc, "angle_2", "21")
Param_NB = model.addParameter(Part_1_doc, "NB", "6")

Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(0, 0, 30)
SketchCircle_1.setAuxiliary(True)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], "d")
SketchLine_1 = Sketch_1.addLine(-1.83495963955651, 27.43871212577585, -1.570078687288315, 29.95888604263722)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchCircle_1.results()[1])
SketchLine_2 = Sketch_1.addLine(-1.570078687288315, 29.95888604263722, -0.7903152816259503, 31.99023916377664)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(1.834959639556546, 27.43871212577585, 1.570078687288318, 29.95888604263721)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchCircle_1.results()[1])
SketchLine_4 = Sketch_1.addLine(1.570078687288318, 29.95888604263721, 0.7903152816259467, 31.99023916377664)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchCircle_2 = Sketch_1.addCircle(0, 0, 27.5)
SketchCircle_2.setAuxiliary(True)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchCircle_2.center())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchCircle_2.results()[1])
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchCircle_2.results()[1])
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_2.result(), SketchLine_4.result())
SketchArc_1 = Sketch_1.addArc(0, 0, 0.7903152816259467, 31.99023916377664, -0.7903152816259503, 31.99023916377664, False)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchArc_1.center())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchArc_1.endPoint())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_5 = SketchProjection_2.createdFeature()
SketchConstraintAngle_1 = Sketch_1.setAngleComplementary(SketchLine_2.result(), SketchLine_5.result(), "angle_2")
SketchConstraintAngle_2 = Sketch_1.setAngle(SketchLine_3.result(), SketchLine_4.result(), "180+angle_1")
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_3.result(), SketchLine_1.result())
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_1.results()[1], "da")
SketchConstraintRadius_3 = Sketch_1.setRadius(SketchCircle_2.results()[1], "df")
SketchLine_6 = Sketch_1.addLine(1.570078687288318, 29.95888604263721, 0, 0)
SketchLine_6.setAuxiliary(True)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_6.endPoint())
SketchLine_7 = Sketch_1.addLine(-1.570078687288315, 29.95888604263722, 0, 0)
SketchLine_7.setAuxiliary(True)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_7.endPoint())
SketchConstraintAngle_3 = Sketch_1.setAngle(SketchLine_6.result(), SketchLine_5.result(), "angle_L/2")
SketchConstraintAngle_4 = Sketch_1.setAngle(SketchLine_5.result(), SketchLine_7.result(), "angle_L/2")
SketchMultiRotation_1 = Sketch_1.addRotation([SketchLine_3.result()], SketchAPI_Line(SketchLine_5).startPoint(), "360/Z", 2)
[SketchLine_8] = SketchMultiRotation_1.rotated()
SketchLine_8.setAuxiliary(True)
SketchArc_2 = Sketch_1.addArc(0, 0, -1.83495963955651, 27.43871212577585, -3.909967663056207, 27.22061999429577, False)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchArc_2.center())
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_8.result(), SketchArc_2.endPoint())
SketchProjection_3 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_2 = SketchProjection_3.createdFeature()
SketchMultiRotation_2_objects = [SketchArc_2.results()[1], SketchLine_1.result(), SketchLine_2.result(), SketchArc_1.results()[1], SketchLine_4.result(), SketchLine_3.result()]
SketchMultiRotation_2 = Sketch_1.addRotation(SketchMultiRotation_2_objects, SketchAPI_Point(SketchPoint_2).coordinates(), "360/Z", "Z")
[SketchArc_3, SketchArc_4, SketchArc_5, SketchArc_6, SketchArc_7, SketchArc_8, SketchArc_9, SketchArc_10, SketchArc_11, SketchArc_12, SketchArc_13, SketchArc_14, SketchArc_15, SketchArc_16, SketchArc_17, SketchArc_18, SketchArc_19, SketchArc_20, SketchArc_21, SketchArc_22, SketchArc_23, SketchArc_24, SketchArc_25, SketchArc_26, SketchArc_27, SketchArc_28, SketchArc_29, SketchArc_30, SketchArc_31, SketchLine_9, SketchLine_10, SketchLine_11, SketchLine_12, SketchLine_13, SketchLine_14, SketchLine_15, SketchLine_16, SketchLine_17, SketchLine_18, SketchLine_19, SketchLine_20, SketchLine_21, SketchLine_22, SketchLine_23, SketchLine_24, SketchLine_25, SketchLine_26, SketchLine_27, SketchLine_28, SketchLine_29, SketchLine_30, SketchLine_31, SketchLine_32, SketchLine_33, SketchLine_34, SketchLine_35, SketchLine_36, SketchLine_37, SketchLine_38, SketchLine_39, SketchLine_40, SketchLine_41, SketchLine_42, SketchLine_43, SketchLine_44, SketchLine_45, SketchLine_46, SketchLine_47, SketchLine_48, SketchLine_49, SketchLine_50, SketchLine_51, SketchLine_52, SketchLine_53, SketchLine_54, SketchLine_55, SketchLine_56, SketchLine_57, SketchLine_58, SketchLine_59, SketchLine_60, SketchLine_61, SketchLine_62, SketchLine_63, SketchLine_64, SketchLine_65, SketchLine_66, SketchArc_32, SketchArc_33, SketchArc_34, SketchArc_35, SketchArc_36, SketchArc_37, SketchArc_38, SketchArc_39, SketchArc_40, SketchArc_41, SketchArc_42, SketchArc_43, SketchArc_44, SketchArc_45, SketchArc_46, SketchArc_47, SketchArc_48, SketchArc_49, SketchArc_50, SketchArc_51, SketchArc_52, SketchArc_53, SketchArc_54, SketchArc_55, SketchArc_56, SketchArc_57, SketchArc_58, SketchArc_59, SketchArc_60, SketchLine_67, SketchLine_68, SketchLine_69, SketchLine_70, SketchLine_71, SketchLine_72, SketchLine_73, SketchLine_74, SketchLine_75, SketchLine_76, SketchLine_77, SketchLine_78, SketchLine_79, SketchLine_80, SketchLine_81, SketchLine_82, SketchLine_83, SketchLine_84, SketchLine_85, SketchLine_86, SketchLine_87, SketchLine_88, SketchLine_89, SketchLine_90, SketchLine_91, SketchLine_92, SketchLine_93, SketchLine_94, SketchLine_95, SketchLine_96, SketchLine_97, SketchLine_98, SketchLine_99, SketchLine_100, SketchLine_101, SketchLine_102, SketchLine_103, SketchLine_104, SketchLine_105, SketchLine_106, SketchLine_107, SketchLine_108, SketchLine_109, SketchLine_110, SketchLine_111, SketchLine_112, SketchLine_113, SketchLine_114, SketchLine_115, SketchLine_116, SketchLine_117, SketchLine_118, SketchLine_119, SketchLine_120, SketchLine_121, SketchLine_122, SketchLine_123, SketchLine_124] = SketchMultiRotation_2.rotated()
model.do()
model.checkSketch(Sketch_1, 0)

Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), "e", 0)
model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [182])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [1080])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [2160])
model.testResultsVolumes(Extrusion_1, [10398.453966298400700907222926617])

Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/To_Face"))
SketchCircle_3 = Sketch_2.addCircle(0, 0, 4.5)
SketchLine_125 = Sketch_2.addLine(0, 0, -24.375, 0)
SketchLine_125.setAuxiliary(True)
SketchLine_126 = Sketch_2.addLine(0, 0, -12.18750000000149, 21.10936921724762)
SketchLine_126.setAuxiliary(True)
SketchLine_127 = Sketch_2.addLine(-5.267665072275066, 6.123863542435089, -11.74701189249443, 17.34642143491413)
SketchConstraintParallel_1 = Sketch_2.setParallel(SketchLine_127.result(), SketchLine_126.result())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_125.result())
SketchConstraintCoincidence_19 = Sketch_2.setCoincident(SketchLine_125.startPoint(), SketchLine_126.startPoint())
SketchProjection_4 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_3 = SketchProjection_4.createdFeature()
SketchConstraintCoincidence_20 = Sketch_2.setCoincident(SketchLine_125.startPoint(), SketchAPI_Point(SketchPoint_3).coordinates())
SketchCircle_4 = Sketch_2.addCircle(0, 0, 7.5)
SketchCircle_4.setAuxiliary(True)
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchCircle_3.center(), SketchCircle_4.center())
SketchCircle_5 = Sketch_2.addCircle(0, 0, 24.375)
SketchCircle_5.setAuxiliary(True)
SketchConstraintCoincidence_22 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_3).coordinates(), SketchCircle_5.center())
SketchConstraintCoincidence_23 = Sketch_2.setCoincident(SketchLine_126.endPoint(), SketchCircle_5.results()[1])
SketchConstraintCoincidence_24 = Sketch_2.setCoincident(SketchLine_125.endPoint(), SketchCircle_5.results()[1])
SketchConstraintCoincidence_25 = Sketch_2.setCoincident(SketchCircle_3.center(), SketchAPI_Point(SketchPoint_3).coordinates())
SketchArc_61 = Sketch_2.addArc(0, 0, -5.379258295557134, 5.22623958403169, -7.21568539381252, 2.045454545454545, False)
SketchConstraintCoincidence_26 = Sketch_2.setCoincident(SketchCircle_3.center(), SketchArc_61.center())
SketchArc_62 = Sketch_2.addArc(0, 0, -16.35843380263332, 18.07048058367397, -23.82871214537519, 5.131578947350746, False)
SketchConstraintCoincidence_27 = Sketch_2.setCoincident(SketchCircle_3.center(), SketchArc_62.center())
SketchArc_63 = Sketch_2.addArc(-14.34508810384774, 15.8464214349141, -11.74701189249443, 17.34642143491413, -16.35843380263332, 18.07048058367397, False)
SketchConstraintCoincidence_28 = Sketch_2.setCoincident(SketchArc_63.startPoint(), SketchLine_127.endPoint())
SketchConstraintCoincidence_29 = Sketch_2.setCoincident(SketchArc_63.endPoint(), SketchArc_62.startPoint())
SketchConstraintTangent_1 = Sketch_2.setTangent(SketchArc_63.results()[1], SketchLine_127.result())
SketchConstraintTangent_2 = Sketch_2.setTangent(SketchArc_63.results()[1], SketchArc_62.results()[1])
SketchArc_64 = Sketch_2.addArc(-5.917184125113392, 5.74886354243508, -5.379258295557134, 5.22623958403169, -5.267665072275066, 6.123863542435089, False)
SketchConstraintCoincidence_30 = Sketch_2.setCoincident(SketchArc_64.startPoint(), SketchArc_61.startPoint())
SketchConstraintCoincidence_31 = Sketch_2.setCoincident(SketchArc_64.endPoint(), SketchLine_127.startPoint())
SketchConstraintTangent_3 = Sketch_2.setTangent(SketchArc_64.results()[1], SketchArc_61.results()[1])
SketchConstraintTangent_4 = Sketch_2.setTangent(SketchArc_64.results()[1], SketchLine_127.result())
SketchLine_128 = Sketch_2.addLine(-20.8959475736328, 1.5, -7.937253933193772, 1.5)
SketchArc_65 = Sketch_2.addArc(-20.89594757363278, 4.500000000000018, -23.82871214537519, 5.131578947350746, -20.8959475736328, 1.5, False)
SketchConstraintCoincidence_32 = Sketch_2.setCoincident(SketchArc_65.startPoint(), SketchArc_62.endPoint())
SketchConstraintCoincidence_33 = Sketch_2.setCoincident(SketchArc_65.endPoint(), SketchLine_128.startPoint())
SketchConstraintTangent_5 = Sketch_2.setTangent(SketchArc_65.results()[1], SketchLine_128.result())
SketchConstraintTangent_6 = Sketch_2.setTangent(SketchArc_65.results()[1], SketchArc_62.results()[1])
SketchArc_66 = Sketch_2.addArc(-7.937253933193772, 2.25, -7.937253933193772, 1.5, -7.21568539381252, 2.045454545454545, False)
SketchConstraintCoincidence_34 = Sketch_2.setCoincident(SketchArc_66.startPoint(), SketchLine_128.endPoint())
SketchConstraintCoincidence_35 = Sketch_2.setCoincident(SketchArc_66.endPoint(), SketchArc_61.endPoint())
SketchConstraintTangent_7 = Sketch_2.setTangent(SketchArc_66.results()[1], SketchLine_128.result())
SketchConstraintTangent_8 = Sketch_2.setTangent(SketchArc_66.results()[1], SketchArc_61.results()[1])
SketchConstraintParallel_2 = Sketch_2.setParallel(SketchLine_128.result(), SketchLine_125.result())
SketchConstraintRadius_4 = Sketch_2.setRadius(SketchCircle_3.results()[1], "R_1")
SketchConstraintRadius_5 = Sketch_2.setRadius(SketchArc_61.results()[1], "r_1")
SketchConstraintEqual_3 = Sketch_2.setEqual(SketchArc_64.results()[1], SketchArc_66.results()[1])
SketchConstraintEqual_4 = Sketch_2.setEqual(SketchArc_63.results()[1], SketchArc_65.results()[1])
SketchConstraintRadius_6 = Sketch_2.setRadius(SketchArc_64.results()[1], "r_3")
SketchConstraintRadius_7 = Sketch_2.setRadius(SketchArc_63.results()[1], "r_2")
SketchConstraintDistanceVertical_1 = Sketch_2.setVerticalDistance(SketchArc_65.endPoint(), SketchLine_125.endPoint(), "ep")
SketchConstraintRadius_8 = Sketch_2.setRadius(SketchArc_62.results()[1], "R")
SketchConstraintAngle_5 = Sketch_2.setAngle(SketchLine_126.result(), SketchLine_125.result(), "360/NB")
SketchConstraintRadius_9 = Sketch_2.setRadius(SketchCircle_5.results()[1], "R")
SketchConstraintRadius_10 = Sketch_2.setRadius(SketchCircle_4.results()[1], "r_1")
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchLine_127.endPoint(), SketchLine_126.result(), "ep", True)
SketchMultiRotation_3_objects = [SketchArc_61.results()[1], SketchArc_64.results()[1], SketchLine_127.result(), SketchArc_63.results()[1], SketchArc_62.results()[1], SketchArc_65.results()[1], SketchLine_128.result(), SketchArc_66.results()[1]]
SketchMultiRotation_3 = Sketch_2.addRotation(SketchMultiRotation_3_objects, SketchCircle_5.center(), "360/NB", "NB")
[SketchArc_67, SketchArc_68, SketchArc_69, SketchArc_70, SketchArc_71, SketchArc_72, SketchArc_73, SketchArc_74, SketchArc_75, SketchArc_76, SketchLine_129, SketchLine_130, SketchLine_131, SketchLine_132, SketchLine_133, SketchArc_77, SketchArc_78, SketchArc_79, SketchArc_80, SketchArc_81, SketchArc_82, SketchArc_83, SketchArc_84, SketchArc_85, SketchArc_86, SketchArc_87, SketchArc_88, SketchArc_89, SketchArc_90, SketchArc_91, SketchLine_134, SketchLine_135, SketchLine_136, SketchLine_137, SketchLine_138, SketchArc_92, SketchArc_93, SketchArc_94, SketchArc_95, SketchArc_96] = SketchMultiRotation_3.rotated()
model.do()
model.checkSketch(Sketch_2, 0)

Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection(), 0, "e")
model.testNbResults(Extrusion_2, 7)
NB_SUBRES = 0
model.testNbSubResults(Extrusion_2, [NB_SUBRES, NB_SUBRES, NB_SUBRES, NB_SUBRES, NB_SUBRES, NB_SUBRES, NB_SUBRES])
NB_SUBSH = 1
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.SOLID, [NB_SUBSH, NB_SUBSH, NB_SUBSH, NB_SUBSH, NB_SUBSH, NB_SUBSH, NB_SUBSH])
NB_F = [3, 10]
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.FACE, [NB_F[0], NB_F[1], NB_F[1], NB_F[1], NB_F[1], NB_F[1], NB_F[1]])
NB_E = [6, 48]
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.EDGE, [NB_E[0], NB_E[1], NB_E[1], NB_E[1], NB_E[1], NB_E[1], NB_E[1]])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.VERTEX, [2*NB_E[0], 2*NB_E[1], 2*NB_E[1], 2*NB_E[1], 2*NB_E[1], 2*NB_E[1], 2*NB_E[1]])
VOL = [238.564692131974851463382947259, 844.585236664367812409182079136]
model.testResultsVolumes(Extrusion_2, [VOL[0], VOL[1], VOL[1], VOL[1], VOL[1], VOL[1], VOL[1]])

Boolean_1_objects_2 = [model.selection("SOLID", "Extrusion_2_1"), model.selection("SOLID", "Extrusion_2_2"), model.selection("SOLID", "Extrusion_2_3"), model.selection("SOLID", "Extrusion_2_4"), model.selection("SOLID", "Extrusion_2_5"), model.selection("SOLID", "Extrusion_2_6"), model.selection("SOLID", "Extrusion_2_7")]
Boolean_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], Boolean_1_objects_2)
model.do()
model.testNbResults(Boolean_1, 1)
model.testNbSubResults(Boolean_1, [0])
model.testNbSubShapes(Boolean_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Boolean_1, GeomAPI_Shape.FACE, [231])
model.testNbSubShapes(Boolean_1, GeomAPI_Shape.EDGE, [1374])
model.testNbSubShapes(Boolean_1, GeomAPI_Shape.VERTEX, [2748])
model.testResultsVolumes(Boolean_1, [5092.377854112805835029575973749])

# Change parameter 'Z' and check validity of result
Z_REF_DATA = [(20, 171, 1014, 2028,  1667.562983801989503263030201197),
              (22, 183, 1086, 2172,  2160.297076967755856458097696304),
              (25, 201, 1194, 2388,  3068.115476944114561774767935276),
              (40, 291, 1734, 3468, 11471.942563079033789108507335186),
              (34, 255, 1518, 3036,  7238.122819170104776276275515556),
              (30, 231, 1374, 2748,  5092.377854112805835029575973749)]
for z in Z_REF_DATA:
    Param_Z.setValue(z[0])
    model.do()
    Part_1_doc.setCurrentFeature(Boolean_1.feature(), False)
    model.do()

    model.testNbResults(Boolean_1, 1)
    model.testNbSubResults(Boolean_1, [0])
    model.testNbSubShapes(Boolean_1, GeomAPI_Shape.SOLID, [1])
    model.testNbSubShapes(Boolean_1, GeomAPI_Shape.FACE, [z[1]])
    model.testNbSubShapes(Boolean_1, GeomAPI_Shape.EDGE, [z[2]])
    model.testNbSubShapes(Boolean_1, GeomAPI_Shape.VERTEX, [z[3]])
    model.testResultsVolumes(Boolean_1, [z[4]])

# Change parameter 'm' and check validity of result
M_REF_DATA = [(1.5, 14094.989002197671652538701891899),
              (1.2, 8007.778784522399291745387017727),
              (1.0, 5092.377854113199646235443651676)]
for m in M_REF_DATA:
    Param_m.setValue(m[0])
    model.do()

    model.testNbResults(Boolean_1, 1)
    model.testNbSubResults(Boolean_1, [0])
    model.testNbSubShapes(Boolean_1, GeomAPI_Shape.SOLID, [1])
    model.testNbSubShapes(Boolean_1, GeomAPI_Shape.FACE, [231])
    model.testNbSubShapes(Boolean_1, GeomAPI_Shape.EDGE, [1374])
    model.testNbSubShapes(Boolean_1, GeomAPI_Shape.VERTEX, [2748])
    model.testResultsVolumes(Boolean_1, [m[1]])

# change parameter 'NB' and check validity of result
NB_REF_DATA = [(4, 215, 1278, 2556, 4669.256782562274565862026065588),
               (8, 247, 1470, 2940, 5515.498925714943652565125375986),
               (7, 239, 1422, 2844, 5303.938389925016963388770818710),
               (6, 231, 1374, 2748, 5092.377854112805835029575973749)]
for n in NB_REF_DATA:
    Param_NB.setValue(n[0])
    model.do()
    Part_1_doc.setCurrentFeature(Boolean_1.feature(), False)
    model.do()
    Boolean_1_objects_2 = []
    for res in Extrusion_2.results():
        Boolean_1_objects_2.append(model.selection("SOLID", res.name()))
    Boolean_1.setToolObjects(Boolean_1_objects_2)
    model.do()

    model.testNbResults(Boolean_1, 1)
    model.testNbSubResults(Boolean_1, [0])
    model.testNbSubShapes(Boolean_1, GeomAPI_Shape.SOLID, [1])
    model.testNbSubShapes(Boolean_1, GeomAPI_Shape.FACE, [n[1]])
    model.testNbSubShapes(Boolean_1, GeomAPI_Shape.EDGE, [n[2]])
    model.testNbSubShapes(Boolean_1, GeomAPI_Shape.VERTEX, [n[3]])
    model.testResultsVolumes(Boolean_1, [n[4]])

model.end()

assert(model.checkPythonDump())
