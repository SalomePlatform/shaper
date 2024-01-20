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
Part_1.setName("Part_4")
Part_1.result().setName("Part_4")
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(6.187999999999998, 10.256, 4.9175, 10.94235984621995)
SketchLine_1.setName("SketchLine_11")
SketchLine_1.result().setName("SketchLine_11")
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
SketchLine_2.setName("SketchLine_12")
SketchLine_2.result().setName("SketchLine_12")
SketchArc_1 = Sketch_1.addArc(0, 2.425, 0, 12.26, 4.9175, 10.94235984621995, True)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_2.result(), SketchArc_1.center())
SketchConstraintCoincidence_1.setName("SketchConstraintCoincidence_11")
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.result(), SketchArc_1.startPoint())
SketchConstraintCoincidence_2.setName("SketchConstraintCoincidence_12")
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchArc_1.endPoint())
SketchConstraintCoincidence_3.setName("SketchConstraintCoincidence_13")
SketchLine_3 = Sketch_1.addLine(0, 12.26, 0, 12.096)
SketchLine_3.setName("SketchLine_13")
SketchLine_3.result().setName("SketchLine_13")
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4.setName("SketchConstraintCoincidence_14")
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_4 = SketchProjection_2.createdFeature()
SketchLine_4.setName("SketchLine_17")
SketchLine_4.result().setName("SketchLine_17")
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_2.result())
SketchConstraintCoincidence_5.setName("SketchConstraintCoincidence_20")
SketchLine_5 = Sketch_1.addLine(5.404900443923878, 10.43809443685872, 5.12663857042824, 10.62933672216996)
SketchLine_5.setName("SketchLine_23")
SketchLine_5.result().setName("SketchLine_23")
SketchLine_6 = Sketch_1.addLine(5.12663857042824, 10.62933672216996, 4.835500000000001, 10.80033167999931)
SketchLine_6.setName("SketchLine_24")
SketchLine_6.result().setName("SketchLine_24")
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_6.setName("SketchConstraintCoincidence_31")
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), "0.082*2")
SketchConstraintLength_1.setName("SketchConstraintLength_3")
SketchArc_2 = Sketch_1.addArc(0, 2.425, 0, 12.096, 4.835500000000001, 10.80033167999931, True)
SketchArc_2.setName("SketchArc_3")
SketchArc_2.result().setName("SketchArc_3")
SketchArc_2.results()[1].setName("SketchArc_3_2")
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchArc_1.center(), SketchArc_2.center())
SketchConstraintCoincidence_7.setName("SketchConstraintCoincidence_32")
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_8.setName("SketchConstraintCoincidence_33")
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchArc_2.endPoint())
SketchConstraintCoincidence_9.setName("SketchConstraintCoincidence_34")
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchArc_1.center(), SketchLine_4.result(), 2.425, True)
SketchConstraintDistance_1.setName("SketchConstraintDistance_11")
SketchLine_7 = Sketch_1.addLine(0, 2.425, 5.524022094217663, 10.61469954608912)
SketchLine_7.setName("SketchLine_28")
SketchLine_7.result().setName("SketchLine_28")
SketchLine_7.setAuxiliary(True)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_7.startPoint())
SketchConstraintCoincidence_10.setName("SketchConstraintCoincidence_43")
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_1.result())
SketchConstraintCoincidence_11.setName("SketchConstraintCoincidence_44")
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_7.result(), SketchLine_5.startPoint())
SketchConstraintCoincidence_12.setName("SketchConstraintCoincidence_45")
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_7.result(), SketchLine_2.result(), 34)
SketchConstraintAngle_1.setName("SketchConstraintAngle_5")
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_5.endPoint(), SketchLine_1.result(), "0.088*2", True)
SketchConstraintDistance_2.setName("SketchConstraintDistance_12")
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 9.835000000000001)
SketchConstraintRadius_1.setName("SketchConstraintRadius_2")
SketchLine_8 = Sketch_1.addLine(5.404900443923878, 10.43809443685872, 6.187999999999998, 10.256)
SketchLine_8.setName("SketchLine_19")
SketchLine_8.result().setName("SketchLine_19")
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_13.setName("SketchConstraintCoincidence_46")
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_8.endPoint())
SketchConstraintCoincidence_14.setName("SketchConstraintCoincidence_47")
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_11r-SketchArc_1_2f-SketchLine_13f-SketchArc_3_2r-SketchLine_24r-SketchLine_23r-SketchLine_19f")], model.selection("EDGE", "PartSet/OZ"), 360, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_9 = Sketch_2.addLine(0, 2.425, 3.282538500974061, 11.74164236673401)
SketchLine_9.setName("SketchLine_34")
SketchLine_9.result().setName("SketchLine_34")
SketchLine_9.setAuxiliary(True)
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_28_StartVertex"), False)
SketchPoint_1 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchLine_9.startPoint(), SketchPoint_1.result())
SketchConstraintCoincidence_15.setName("SketchConstraintCoincidence_55")
SketchPoint_2 = Sketch_2.addPoint(3.241, 11.6237460014939)
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchPoint_2.coordinates(), SketchLine_9.result())
SketchConstraintCoincidence_16.setName("SketchConstraintCoincidence_56")
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_10 = SketchProjection_4.createdFeature()
SketchLine_10.setName("SketchLine_36")
SketchLine_10.result().setName("SketchLine_36")
SketchConstraintDistance_3 = Sketch_2.setDistance(SketchPoint_2.coordinates(), SketchLine_10.result(), 3.241, True)
SketchConstraintDistance_3.setName("SketchConstraintDistance_17")
SketchProjection_5 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_13"), False)
SketchLine_11 = SketchProjection_5.createdFeature()
SketchLine_11.setName("SketchLine_37")
SketchLine_11.result().setName("SketchLine_37")
SketchProjection_6 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_28"), False)
SketchLine_12 = SketchProjection_6.createdFeature()
SketchLine_12.setName("SketchLine_38")
SketchLine_12.result().setName("SketchLine_38")
SketchArc_3 = Sketch_2.addArc(0, 2.425, 0, 12.178, 5.453808387550194, 10.51060344512932, True)
SketchArc_3.setName("SketchArc_4")
SketchArc_3.result().setName("SketchArc_4")
SketchArc_3.results()[1].setName("SketchArc_4_2")
SketchArc_3.setAuxiliary(True)
SketchConstraintCoincidence_17 = Sketch_2.setCoincident(SketchLine_9.startPoint(), SketchArc_3.center())
SketchConstraintCoincidence_17.setName("SketchConstraintCoincidence_58")
SketchConstraintCoincidence_18 = Sketch_2.setCoincident(SketchLine_11.result(), SketchArc_3.startPoint())
SketchConstraintCoincidence_18.setName("SketchConstraintCoincidence_59")
SketchConstraintCoincidence_19 = Sketch_2.setCoincident(SketchLine_12.result(), SketchArc_3.endPoint())
SketchConstraintCoincidence_19.setName("SketchConstraintCoincidence_60")
SketchConstraintMiddle_1 = Sketch_2.setMiddlePoint(SketchLine_11.result(), SketchArc_3.startPoint())
SketchConstraintMiddle_1.setName("SketchConstraintMiddle_3")
SketchConstraintCoincidence_20 = Sketch_2.setCoincident(SketchPoint_2.coordinates(), SketchArc_3.results()[1])
SketchConstraintCoincidence_20.setName("SketchConstraintCoincidence_61")
SketchArc_4 = Sketch_2.addArc(0, 2.425, 3.28253850097406, 11.74164236673401, 4.139817701932777, 11.39365616437401, True)
SketchArc_4.setName("SketchArc_5")
SketchArc_4.result().setName("SketchArc_5")
SketchArc_4.results()[1].setName("SketchArc_5_2")
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchLine_9.startPoint(), SketchArc_4.center())
SketchConstraintCoincidence_21.setName("SketchConstraintCoincidence_62")
SketchConstraintCoincidence_22 = Sketch_2.setCoincident(SketchLine_9.result(), SketchArc_4.startPoint())
SketchConstraintCoincidence_22.setName("SketchConstraintCoincidence_63")
SketchLine_13 = Sketch_2.addLine(4.139817701932777, 11.39365616437401, 4.459277785066624, 11.1909606795611)
SketchLine_13.setName("SketchLine_39")
SketchLine_13.result().setName("SketchLine_39")
SketchConstraintCoincidence_23 = Sketch_2.setCoincident(SketchArc_4.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_23.setName("SketchConstraintCoincidence_64")
SketchProjection_7 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchArc_1_2"), False)
SketchArc_5 = SketchProjection_7.createdFeature()
SketchArc_5.setName("SketchArc_6")
SketchArc_5.result().setName("SketchArc_6")
SketchArc_5.results()[1].setName("SketchArc_6_2")
SketchConstraintCoincidence_24 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchArc_5.results()[1])
SketchConstraintCoincidence_24.setName("SketchConstraintCoincidence_65")
SketchLine_14 = Sketch_2.addLine(4.459277785066624, 11.1909606795611, 4.390495384356073, 11.041947909785)
SketchLine_14.setName("SketchLine_40")
SketchLine_14.result().setName("SketchLine_40")
SketchConstraintCoincidence_25 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_25.setName("SketchConstraintCoincidence_66")
SketchProjection_8 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchArc_3_2"), False)
SketchArc_6 = SketchProjection_8.createdFeature()
SketchArc_6.setName("SketchArc_7")
SketchArc_6.result().setName("SketchArc_7")
SketchArc_6.results()[1].setName("SketchArc_7_2")
SketchConstraintCoincidence_26 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchArc_6.results()[1])
SketchConstraintCoincidence_26.setName("SketchConstraintCoincidence_67")
SketchLine_15 = Sketch_2.addLine(4.390495384356073, 11.041947909785, 4.035044020470619, 11.16667053559354)
SketchLine_15.setName("SketchLine_41")
SketchLine_15.result().setName("SketchLine_41")
SketchConstraintCoincidence_27 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_27.setName("SketchConstraintCoincidence_68")
SketchArc_7 = Sketch_2.addArc(0, 2.425, 3.199461499025941, 11.5058496362538, 4.035044020470619, 11.16667053559354, True)
SketchArc_7.setName("SketchArc_8")
SketchArc_7.result().setName("SketchArc_8")
SketchArc_7.results()[1].setName("SketchArc_8_2")
SketchConstraintCoincidence_28 = Sketch_2.setCoincident(SketchLine_9.startPoint(), SketchArc_7.center())
SketchConstraintCoincidence_28.setName("SketchConstraintCoincidence_69")
SketchConstraintCoincidence_29 = Sketch_2.setCoincident(SketchLine_9.result(), SketchArc_7.startPoint())
SketchConstraintCoincidence_29.setName("SketchConstraintCoincidence_70")
SketchConstraintCoincidence_30 = Sketch_2.setCoincident(SketchArc_7.endPoint(), SketchLine_15.endPoint())
SketchConstraintCoincidence_30.setName("SketchConstraintCoincidence_71")
SketchLine_16 = Sketch_2.addLine(0, 2.425, 4.139817701932777, 11.39365616437401)
SketchLine_16.setName("SketchLine_42")
SketchLine_16.result().setName("SketchLine_42")
SketchLine_16.setAuxiliary(True)
SketchConstraintCoincidence_31 = Sketch_2.setCoincident(SketchLine_9.startPoint(), SketchLine_16.startPoint())
SketchConstraintCoincidence_31.setName("SketchConstraintCoincidence_72")
SketchConstraintCoincidence_32 = Sketch_2.setCoincident(SketchArc_4.endPoint(), SketchLine_16.endPoint())
SketchConstraintCoincidence_32.setName("SketchConstraintCoincidence_73")
SketchConstraintCoincidence_33 = Sketch_2.setCoincident(SketchArc_7.endPoint(), SketchLine_16.result())
SketchConstraintCoincidence_33.setName("SketchConstraintCoincidence_74")
SketchConstraintParallel_1 = Sketch_2.setParallel(SketchLine_14.result(), SketchLine_16.result())
SketchLine_17 = Sketch_2.addLine(0, 2.425, 3.9115060144283, 11.49555702253678)
SketchLine_17.setName("SketchLine_43")
SketchLine_17.result().setName("SketchLine_43")
SketchLine_17.setAuxiliary(True)
SketchConstraintCoincidence_34 = Sketch_2.setCoincident(SketchLine_9.startPoint(), SketchLine_17.startPoint())
SketchConstraintCoincidence_34.setName("SketchConstraintCoincidence_75")
SketchConstraintCoincidence_35 = Sketch_2.setCoincident(SketchLine_17.endPoint(), SketchArc_4.results()[1])
SketchConstraintCoincidence_35.setName("SketchConstraintCoincidence_76")
SketchPoint_3 = Sketch_2.addPoint(3.812510620258724, 11.26599240868436)
SketchConstraintCoincidence_36 = Sketch_2.setCoincident(SketchPoint_3.coordinates(), SketchLine_17.result())
SketchConstraintCoincidence_36.setName("SketchConstraintCoincidence_77")
SketchConstraintCoincidence_37 = Sketch_2.setCoincident(SketchPoint_3.coordinates(), SketchArc_7.results()[1])
SketchConstraintCoincidence_37.setName("SketchConstraintCoincidence_78")
SketchLine_18 = Sketch_2.addLine(3.812510620258724, 11.26599240868436, 3.9115060144283, 11.49555702253678)
SketchLine_18.setName("SketchLine_44")
SketchLine_18.result().setName("SketchLine_44")
SketchConstraintCoincidence_38 = Sketch_2.setCoincident(SketchPoint_3.coordinates(), SketchLine_18.startPoint())
SketchConstraintCoincidence_38.setName("SketchConstraintCoincidence_79")
SketchConstraintCoincidence_39 = Sketch_2.setCoincident(SketchLine_17.endPoint(), SketchLine_18.endPoint())
SketchConstraintCoincidence_39.setName("SketchConstraintCoincidence_80")
SketchPoint_4 = Sketch_2.addPoint(3.862008317343517, 11.38077471561057)
SketchConstraintCoincidence_40 = Sketch_2.setCoincident(SketchPoint_4.coordinates(), SketchLine_18.result())
SketchConstraintCoincidence_40.setName("SketchConstraintCoincidence_81")
SketchConstraintMiddle_2 = Sketch_2.setMiddlePoint(SketchPoint_4.coordinates(), SketchLine_18.result())
SketchConstraintMiddle_2.setName("SketchConstraintMiddle_4")
SketchConstraintCoincidence_41 = Sketch_2.setCoincident(SketchPoint_4.coordinates(), SketchArc_3.results()[1])
SketchConstraintCoincidence_41.setName("SketchConstraintCoincidence_82")
SketchConstraintDistance_4 = Sketch_2.setDistance(SketchLine_18.endPoint(), SketchLine_16.result(), 0.25, True)
SketchConstraintDistance_4.setName("SketchConstraintDistance_18")
SketchConstraintDistance_5 = Sketch_2.setDistance(SketchArc_4.endPoint(), SketchLine_14.result(), 0.375, True)
SketchConstraintDistance_5.setName("SketchConstraintDistance_19")
SketchConstraintDistance_6 = Sketch_2.setDistance(SketchArc_4.startPoint(), SketchLine_18.result(), "1.35/2", True)
SketchConstraintDistance_6.setName("SketchConstraintDistance_20")
SketchConstraintDistance_7 = Sketch_2.setDistance(SketchArc_7.startPoint(), SketchArc_4.startPoint(), 0.25, True)
SketchConstraintDistance_7.setName("SketchConstraintDistance_21")
SketchConstraintCoincidence_42 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchArc_4.startPoint())
SketchConstraintCoincidence_42.setName("SketchConstraintCoincidence_83")
SketchLine_19 = Sketch_2.addLine(3.199461499025942, 11.5058496362538, 3.28253850097406, 11.74164236673401)
SketchLine_19.setName("SketchLine_45")
SketchLine_19.result().setName("SketchLine_45")
SketchConstraintCoincidence_43 = Sketch_2.setCoincident(SketchArc_7.startPoint(), SketchLine_19.startPoint())
SketchConstraintCoincidence_43.setName("SketchConstraintCoincidence_84")
SketchConstraintCoincidence_44 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_19.endPoint())
SketchConstraintCoincidence_44.setName("SketchConstraintCoincidence_85")
model.do()
Revolution_2 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchArc_5_2f-SketchLine_45r-SketchArc_8_2r-SketchLine_44f")], model.selection("EDGE", "Sketch_2/SketchLine_34"), 360, 0)
Revolution_2.result().setName("Cut_tool")
Revolution_3 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchArc_5_2f-SketchLine_44r-SketchArc_8_2r-SketchLine_41r-SketchLine_40r-SketchLine_39r")], model.selection("EDGE", "Sketch_2/SketchLine_34"), 360, 0)
Revolution_3.result().setName("Fuse_tool")
Symmetry_1 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Cut_tool"), model.selection("SOLID", "Fuse_tool")], model.selection("EDGE", "PartSet/OZ"), True)
Symmetry_1.result().subResult(0).setName("Symmetry_1_1_1")
Symmetry_1.result().subResult(1).setName("Symmetry_1_1_2")
Symmetry_1.results()[1].subResult(0).setName("Symmetry_1_2_1")
Symmetry_1.results()[1].subResult(1).setName("Symmetry_1_2_2")
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Revolution_1_1")], [model.selection("COMPOUND", "Cut_tool")])
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Cut_1_1"), model.selection("COMPOUND", "Fuse_tool")])
FusionFaces_1 = model.addFusionFaces(Part_1_doc, model.selection("SOLID", "Fuse_1_1"))
model.do()

# check the faces are fused
model.checkResult(FusionFaces_1, model, 1, [0], [1], [16], [76], [152])

model.end()