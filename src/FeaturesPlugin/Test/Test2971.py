# Copyright (C) 2019-2021  CEA/DEN, EDF R&D
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

toto=model.addParameter(Part_1_doc, "r_fond_ext", "1950")
model.addParameter(Part_1_doc, "r_fond_int", "1724")
model.addParameter(Part_1_doc, "r_fillet_38", "38")
model.addParameter(Part_1_doc, "r_fillet_40", "40")
model.addParameter(Part_1_doc, "ep_fond", "170")
model.addParameter(Part_1_doc, "ep_support", "254")
model.addParameter(Part_1_doc, "dist_1", "89.6")
model.addParameter(Part_1_doc, "ep_homo", "603.4")
model.addParameter(Part_1_doc, "ep_virole", "98.6")
model.addParameter(Part_1_doc, "h_virole", "1720")
model.addParameter(Part_1_doc, "r_ext_virole", "1897")
model.addParameter(Part_1_doc, "dist_cer_1", "3")
model.addParameter(Part_1_doc, "dist_cer_2", "55")
model.addParameter(Part_1_doc, "r_homo", "1653")
model.addParameter(Part_1_doc, "half_pp", "30")
model.addParameter(Part_1_doc, "pitch", "27.43")
model.addParameter(Part_1_doc, "rue_eau", "150")
model.addParameter(Part_1_doc, "dist_rue", "rue_eau/2 - pitch/2")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(1897, 693, 1897, 2413)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), "h_virole")
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchLine_2 = Sketch_1.addLine(1897, 2413, 1798.4, 2413)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_2.result(), "ep_virole")
SketchLine_3 = Sketch_1.addLine(1798.4, 2413, 1798.4, 641.4)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_1.endPoint(), "r_ext_virole")
SketchLine_4 = Sketch_1.addLine(1760.4, 603.4, 1653, 603.4)
SketchLine_5 = Sketch_1.addLine(1653, 603.4, 1653, 0)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(1653, 0, 1683.450919985492, 0)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_5.result(), "ep_homo")
SketchArc_1 = Sketch_1.addArc(1760.4, 641.4, 1760.4, 603.4, 1798.4, 641.4, False)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_3.endPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_3.result())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_4.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchLine_5.startPoint(), SketchLine_1.startPoint(), "dist_1 ")
SketchLine_7 = Sketch_1.addLine(1897, 693, 1897, -214)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(1937, -254, 2151, -254)
SketchLine_9 = Sketch_1.addLine(2151, -254, 2151, -508)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchLine_10 = Sketch_1.addLine(2151, -508, 1892.243906054397, -508)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_10.result(), SketchLine_8.result())
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_9.result(), "ep_support")
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_8.result())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_11 = SketchProjection_2.createdFeature()
SketchArc_2 = Sketch_1.addArc(0, 3, 1723.437877704862, -41.02137767220898, 0, -1721, True)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_11.result(), SketchArc_2.center())
SketchLine_12 = Sketch_1.addLine(0, -1721, 0, -1891)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_12.startPoint())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_12.result())
SketchConstraintDistanceVertical_2 = Sketch_1.setVerticalDistance(SketchArc_2.center(), SketchAPI_Line(SketchLine_11).startPoint(), "dist_cer_1")
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_12.result(), "ep_fond")
SketchArc_3 = Sketch_1.addArc(1683.450919985492, -40, 1723.437877704862, -41.02137767220898, 1683.450919985492, 0, False)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchArc_3.startPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_6.endPoint())
SketchConstraintTangent_3 = Sketch_1.setTangent(SketchArc_3.results()[1], SketchLine_6.result())
SketchConstraintTangent_4 = Sketch_1.setTangent(SketchArc_3.results()[1], SketchArc_2.results()[1])
SketchArc_4 = Sketch_1.addArc(1937, -214, 1897, -214, 1937, -254, False)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchArc_4.startPoint(), SketchLine_7.endPoint())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchLine_8.startPoint())
SketchConstraintTangent_5 = Sketch_1.setTangent(SketchArc_4.results()[1], SketchLine_7.result())
SketchConstraintTangent_6 = Sketch_1.setTangent(SketchArc_4.results()[1], SketchLine_8.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchArc_5 = Sketch_1.addArc(0, 55, 0, -1891, 1854.132246315134, -535.8549848942598, False)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_11.result(), SketchArc_5.center())
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchArc_5.startPoint())
SketchConstraintDistanceVertical_3 = Sketch_1.setVerticalDistance(SketchArc_5.center(), SketchAPI_Line(SketchLine_11).startPoint(), "dist_cer_2")
SketchArc_6 = Sketch_1.addArc(1892.243906054397, -548, 1892.243906054397, -508, 1854.132246315134, -535.8549848942598, False)
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchArc_6.startPoint(), SketchLine_10.endPoint())
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchArc_6.endPoint(), SketchArc_5.endPoint())
SketchConstraintTangent_7 = Sketch_1.setTangent(SketchArc_6.results()[1], SketchLine_10.result())
SketchConstraintTangent_8 = Sketch_1.setTangent(SketchArc_6.results()[1], SketchArc_5.results()[1])
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_6.results()[1], "r_fillet_40")
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_4.results()[1], "r_fillet_40")
SketchConstraintRadius_3 = Sketch_1.setRadius(SketchArc_3.results()[1], "r_fillet_40")
SketchConstraintRadius_4 = Sketch_1.setRadius(SketchArc_1.results()[1], "r_fillet_38")
SketchConstraintRadius_5 = Sketch_1.setRadius(SketchArc_2.results()[1], "r_fond_int")
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_7.result())
SketchConstraintVertical_6 = Sketch_1.setVertical(SketchLine_9.result())
SketchConstraintDistanceHorizontal_2 = Sketch_1.setHorizontalDistance(SketchLine_12.startPoint(), SketchLine_7.startPoint(), "r_ext_virole")
SketchConstraintDistanceHorizontal_3 = Sketch_1.setHorizontalDistance(SketchAPI_Line(SketchLine_11).startPoint(), SketchLine_6.startPoint(), "r_homo")
SketchConstraintDistanceVertical_4 = Sketch_1.setVerticalDistance(SketchAPI_Line(SketchLine_11).startPoint(), SketchLine_5.startPoint(), "ep_homo")
SketchConstraintDistanceHorizontal_4 = Sketch_1.setHorizontalDistance(SketchArc_4.startPoint(), SketchLine_9.startPoint(), "ep_support")
SketchConstraintDistanceVertical_5 = Sketch_1.setVerticalDistance(SketchAPI_Line(SketchLine_11).startPoint(), SketchLine_9.startPoint(), "ep_support")
SketchLine_13 = Sketch_1.addLine(1723.437877704863, -41.02137767220899, 1897, -41.02137767220221)
SketchLine_13.setName("SketchLine_23")
SketchLine_13.result().setName("SketchLine_23")
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_7.result())
SketchConstraintHorizontal_5 = Sketch_1.setHorizontal(SketchLine_13.result())
SketchLine_14 = Sketch_1.addLine(1897, 693, 1798.4, 693)
SketchLine_14.setName("SketchLine_24")
SketchLine_14.result().setName("SketchLine_24")
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_3.result())
SketchConstraintHorizontal_6 = Sketch_1.setHorizontal(SketchLine_14.result())
model.do()
Sketch_1.setName("GVProfile")
Sketch_1.result().setName("GVProfile")
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("COMPOUND", "all-in-GVProfile")], model.selection("EDGE", "PartSet/OZ"), 180, 0)
Revolution_1.setName("GVBody")
Revolution_1.result().setName("Revolution_1_1")
Revolution_1.result().subResult(0).setName("GVBody_1_1")
Revolution_1.result().subResult(1).setName("GVBody_1_2")
Revolution_1.result().subResult(2).setName("GVBody_1_3")
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_2 = SketchProjection_3.createdFeature()
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_15 = SketchProjection_4.createdFeature()
SketchLine_15.setName("SketchLine_14")
SketchLine_15.result().setName("SketchLine_14")
SketchLine_16 = Sketch_2.addLine(30, -40, 30, -1806)
SketchLine_16.setName("SketchLine_15")
SketchLine_16.result().setName("SketchLine_15")
SketchLine_17 = Sketch_2.addLine(30, -1806, 0, -1806)
SketchLine_17.setName("SketchLine_16")
SketchLine_17.result().setName("SketchLine_16")
SketchConstraintCoincidence_24 = Sketch_2.setCoincident(SketchLine_16.endPoint(), SketchLine_17.startPoint())
SketchLine_18 = Sketch_2.addLine(0, 0, 70, 0)
SketchLine_18.setName("SketchLine_18")
SketchLine_18.result().setName("SketchLine_18")
SketchConstraintCoincidence_25 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_18.startPoint())
SketchArc_7 = Sketch_2.addArc(70, -40, 70, 0, 30, -40, False)
SketchConstraintCoincidence_26 = Sketch_2.setCoincident(SketchLine_18.endPoint(), SketchArc_7.startPoint())
SketchConstraintTangent_9 = Sketch_2.setTangent(SketchLine_18.result(), SketchArc_7.results()[1])
SketchConstraintCoincidence_27 = Sketch_2.setCoincident(SketchLine_16.result(), SketchArc_7.endPoint())
SketchConstraintRadius_6 = Sketch_2.setRadius(SketchArc_7.results()[1], "r_fillet_40")
SketchConstraintHorizontal_7 = Sketch_2.setHorizontal(SketchLine_18.result())
SketchConstraintVertical_7 = Sketch_2.setVertical(SketchLine_16.result())
SketchConstraintCoincidence_28 = Sketch_2.setCoincident(SketchLine_16.startPoint(), SketchArc_7.endPoint())
SketchConstraintDistanceHorizontal_5 = Sketch_2.setHorizontalDistance(SketchLine_18.startPoint(), SketchLine_16.startPoint(), "half_pp")
SketchConstraintHorizontal_8 = Sketch_2.setHorizontal(SketchLine_17.result())
SketchConstraintTangent_10 = Sketch_2.setTangent(SketchArc_7.results()[1], SketchLine_16.result())
SketchProjection_5 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchProjection_5.setName("SketchProjection_6")
SketchProjection_5.result().setName("SketchProjection_6")
SketchLine_19 = SketchProjection_5.createdFeature()
SketchLine_19.setName("SketchLine_19")
SketchLine_19.result().setName("SketchLine_19")
SketchProjection_6 = Sketch_2.addProjection(model.selection("EDGE", "[GVBody_1_3/From_Face][GVBody_1_3/To_Face]"), False)
SketchProjection_6.setName("SketchProjection_7")
SketchProjection_6.result().setName("SketchProjection_7")
SketchLine_20 = SketchProjection_6.createdFeature()
SketchLine_20.setName("SketchLine_25")
SketchLine_20.result().setName("SketchLine_25")
SketchConstraintMiddle_1 = Sketch_2.setMiddlePoint(SketchLine_20.result(), SketchLine_17.endPoint())
SketchProjection_7 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchProjection_7.setName("SketchProjection_8")
SketchProjection_7.result().setName("SketchProjection_8")
SketchLine_21 = SketchProjection_7.createdFeature()
SketchLine_21.setName("SketchLine_21")
SketchLine_21.result().setName("SketchLine_21")
SketchConstraintMirror_1_objects = [SketchLine_18.result(), SketchArc_7.results()[1], SketchLine_16.result(), SketchLine_17.result()]
SketchConstraintMirror_1 = Sketch_2.addMirror(SketchLine_21.result(), SketchConstraintMirror_1_objects)
[SketchLine_22, SketchArc_8, SketchLine_23, SketchLine_24] = SketchConstraintMirror_1.mirrored()
SketchLine_24.setName("SketchLine_27")
SketchLine_24.result().setName("SketchLine_27")
SketchLine_23.setName("SketchLine_26")
SketchLine_23.result().setName("SketchLine_26")
SketchLine_22.setName("SketchLine_22")
SketchLine_22.result().setName("SketchLine_22")
model.do()
Sketch_2.setName("PPProfile")
Sketch_2.result().setName("PPProfile")
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-PPProfile")], model.selection(), 0, 2300)
Cut_1 = model.addCut(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_1")], [model.selection("COMPSOLID", "Revolution_1_1")])
Recover_1 = model.addRecover(Part_1_doc, Cut_1, [Revolution_1.result()])
Remove_SubShapes_1 = model.addRemoveSubShapes(Part_1_doc, model.selection("COMPOUND", "Cut_1_1"))
Remove_SubShapes_1.setSubShapesToRemove([model.selection("SOLID", "Cut_1_1_2")])
Sketch_3 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchLine_25 = Sketch_3.addLine(0, 0, 0, 1653)
SketchLine_25.setName("SketchLine_28")
SketchLine_25.result().setName("SketchLine_28")
SketchProjection_8 = Sketch_3.addProjection(model.selection("VERTEX", "[Recover_1_1_2/Modified_Face&GVProfile/SketchLine_6][Recover_1_1_2/Modified_Face&GVProfile/SketchArc_3_2]__cc"), False)
SketchProjection_8.setName("SketchProjection_9")
SketchProjection_8.result().setName("SketchProjection_9")
SketchPoint_3 = SketchProjection_8.createdFeature()
SketchConstraintCoincidence_29 = Sketch_3.setCoincident(SketchLine_25.startPoint(), SketchPoint_3.result())
SketchProjection_9 = Sketch_3.addProjection(model.selection("VERTEX", "[(Remove_SubShapes_1_1/Modified_Face&PPProfile/SketchArc_7_2)(Remove_SubShapes_1_1/Modified_Face&Extrusion_1_1/To_Face)(Remove_SubShapes_1_1/Modified_Face&GVProfile/SketchLine_6&PPProfile/SketchLine_18)][Remove_SubShapes_1_1/Modified_Face&GVProfile/SketchLine_6&PPProfile/SketchLine_18][Remove_SubShapes_1_1/Modified_Face&GVProfile/SketchLine_6&PPProfile/SketchLine_22][(Remove_SubShapes_1_1/Modified_Face&Extrusion_1_1/To_Face)(Remove_SubShapes_1_1/Modified_Face&GVProfile/SketchLine_6&PPProfile/SketchLine_22)(Remove_SubShapes_1_1/Modified_Face&PPProfile/SketchArc_8_2)]"), False)
SketchProjection_9.setName("SketchProjection_10")
SketchProjection_9.result().setName("SketchProjection_10")
SketchPoint_4 = SketchProjection_9.createdFeature()
SketchConstraintCoincidence_30 = Sketch_3.setCoincident(SketchLine_25.endPoint(), SketchPoint_4.result())
SketchLine_26 = Sketch_3.addLine(0, 0, -1653, 0)
SketchLine_26.setName("SketchLine_29")
SketchLine_26.result().setName("SketchLine_29")
SketchConstraintCoincidence_31 = Sketch_3.setCoincident(SketchLine_25.startPoint(), SketchLine_26.startPoint())
SketchProjection_10 = Sketch_3.addProjection(model.selection("VERTEX", "Recover_1_1_2/Modified_Vertex&weak_name_8"), False)
SketchProjection_10.setName("SketchProjection_11")
SketchProjection_10.result().setName("SketchProjection_11")
SketchPoint_5 = SketchProjection_10.createdFeature()
SketchConstraintCoincidence_32 = Sketch_3.setCoincident(SketchLine_26.endPoint(), SketchPoint_5.result())
SketchArc_9 = Sketch_3.addArc(0, 0, -1653, 0, 0, 1653, True)
SketchConstraintCoincidence_33 = Sketch_3.setCoincident(SketchLine_25.startPoint(), SketchArc_9.center())
SketchConstraintCoincidence_34 = Sketch_3.setCoincident(SketchLine_26.endPoint(), SketchArc_9.startPoint())
SketchConstraintCoincidence_35 = Sketch_3.setCoincident(SketchLine_25.result(), SketchArc_9.endPoint())
SketchLine_27 = Sketch_3.addLine(-75, 0, -75, 1651.297671529879)
SketchLine_27.setName("SketchLine_30")
SketchLine_27.result().setName("SketchLine_30")
SketchConstraintCoincidence_36 = Sketch_3.setCoincident(SketchLine_27.startPoint(), SketchLine_26.result())
SketchConstraintCoincidence_37 = Sketch_3.setCoincident(SketchLine_27.endPoint(), SketchArc_9.results()[1])
SketchConstraintVertical_8 = Sketch_3.setVertical(SketchLine_27.result())
SketchConstraintDistanceHorizontal_6 = Sketch_3.setHorizontalDistance(SketchLine_27.startPoint(), SketchArc_9.center(), "rue_eau/2")
SketchLine_28 = Sketch_3.addLine(-75, 0, -1205.745800334843, 1130.745800334845)
SketchLine_28.setName("SketchLine_31")
SketchLine_28.result().setName("SketchLine_31")
SketchLine_28.setAuxiliary(True)
SketchConstraintCoincidence_38 = Sketch_3.setCoincident(SketchLine_27.startPoint(), SketchLine_28.startPoint())
SketchConstraintCoincidence_39 = Sketch_3.setCoincident(SketchLine_28.endPoint(), SketchArc_9.results()[1])
SketchConstraintAngle_1 = Sketch_3.setAngle(SketchLine_28.result(), SketchLine_26.result(), 45)
SketchLine_29 = Sketch_3.addLine(-743.85, 0, -743.85, 825.6488357649396)
SketchLine_29.setName("SketchLine_32")
SketchLine_29.result().setName("SketchLine_32")
SketchConstraintCoincidence_40 = Sketch_3.setCoincident(SketchLine_29.startPoint(), SketchLine_26.result())
SketchLine_30 = Sketch_3.addLine(-743.85, 825.6488357649396, -75, 825.6488357649395)
SketchLine_30.setName("SketchLine_33")
SketchLine_30.result().setName("SketchLine_33")
SketchConstraintCoincidence_41 = Sketch_3.setCoincident(SketchLine_29.endPoint(), SketchLine_30.startPoint())
SketchConstraintCoincidence_42 = Sketch_3.setCoincident(SketchLine_30.endPoint(), SketchLine_27.result())
SketchLine_31 = Sketch_3.addLine(-743.85, 825.6488357649396, -1127.232311682702, 1209.031147447615)
SketchLine_31.setName("SketchLine_34")
SketchLine_31.result().setName("SketchLine_34")
SketchConstraintCoincidence_43 = Sketch_3.setCoincident(SketchLine_29.endPoint(), SketchLine_31.startPoint())
SketchConstraintCoincidence_44 = Sketch_3.setCoincident(SketchLine_31.endPoint(), SketchArc_9.results()[1])
SketchConstraintParallel_2 = Sketch_3.setParallel(SketchLine_31.result(), SketchLine_28.result())
SketchConstraintMiddle_2 = Sketch_3.setMiddlePoint(SketchLine_30.endPoint(), SketchLine_27.result())
SketchConstraintHorizontal_9 = Sketch_3.setHorizontal(SketchLine_30.result())
SketchConstraintVertical_9 = Sketch_3.setVertical(SketchLine_29.result())
SketchConstraintDistanceHorizontal_7 = Sketch_3.setHorizontalDistance(SketchLine_25.startPoint(), SketchLine_29.startPoint(), "r_homo*0.5*0.9")
SketchProjection_11 = Sketch_3.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchProjection_11.setName("SketchProjection_12")
SketchProjection_11.result().setName("SketchProjection_12")
SketchLine_32 = SketchProjection_11.createdFeature()
SketchLine_32.setName("SketchLine_35")
SketchLine_32.result().setName("SketchLine_35")
SketchConstraintMirror_2_objects = [SketchLine_26.result(), SketchLine_29.result(), SketchArc_9.results()[1], SketchLine_31.result(), SketchLine_30.result(), SketchLine_27.result()]
SketchConstraintMirror_2 = Sketch_3.addMirror(SketchLine_32.result(), SketchConstraintMirror_2_objects)
[SketchLine_33, SketchLine_34, SketchArc_10, SketchLine_35, SketchLine_36, SketchLine_37] = SketchConstraintMirror_2.mirrored()
SketchLine_37.setName("SketchLine_40")
SketchLine_37.result().setName("SketchLine_40")
SketchLine_36.setName("SketchLine_39")
SketchLine_36.result().setName("SketchLine_39")
SketchLine_35.setName("SketchLine_38")
SketchLine_35.result().setName("SketchLine_38")
SketchLine_34.setName("SketchLine_37")
SketchLine_34.result().setName("SketchLine_37")
SketchLine_33.setName("SketchLine_36")
SketchLine_33.result().setName("SketchLine_36")
model.do()
Sketch_3.setName("PHomoProfile")
Sketch_3.result().setName("PHomoProfile")
Sketch_3.changeFacesOrder([[SketchLine_25.result(), SketchArc_9.results()[1], SketchLine_27.result(), SketchLine_27.result(), SketchLine_26.result()],
                           [SketchLine_29.result(), SketchLine_31.result(), SketchArc_9.results()[1], SketchLine_26.result()],
                           [SketchLine_27.result(), SketchLine_30.result(), SketchLine_29.result(), SketchLine_26.result()],
                           [SketchArc_9.results()[1], SketchLine_31.result(), SketchLine_30.result(), SketchLine_27.result()],
                           [SketchLine_33.result(), SketchLine_37.result(), SketchLine_37.result(), SketchArc_10.results()[1], SketchLine_25.result()],
                           [SketchLine_33.result(), SketchArc_10.results()[1], SketchLine_35.result(), SketchLine_34.result()],
                           [SketchLine_33.result(), SketchLine_34.result(), SketchLine_36.result(), SketchLine_37.result()],
                           [SketchArc_10.results()[1], SketchLine_37.result(), SketchLine_36.result(), SketchLine_35.result()]
                          ])
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-PHomoProfile")], model.selection(), "ep_homo", 0)
Partition_1_objects = [model.selection("SOLID", "Remove_SubShapes_1_1"), model.selection("COMPSOLID", "Recover_1_1"), model.selection("COMPSOLID", "Extrusion_2_1")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
Group_1 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Partition_1_1_2")])
Group_1.setName("virole")
Group_1.result().setName("virole")
Group_2 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Partition_1_1_4")])
Group_2.setName("bol")
Group_2.result().setName("bol")
Group_3 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Partition_1_1_1")])
Group_3.setName("pp")
Group_3.result().setName("pp")
Group_4_objects = [model.selection("SOLID", "Partition_1_1_6"),
                   model.selection("SOLID", "Partition_1_1_8"),
                   model.selection("SOLID", "Partition_1_1_7"),
                   model.selection("SOLID", "Partition_1_1_12"),
                   model.selection("SOLID", "Partition_1_1_11"),
                   model.selection("SOLID", "Partition_1_1_10")]
Group_4 = model.addGroup(Part_1_doc, Group_4_objects)
Group_4.setName("pt")
Group_4.result().setName("pt")
Group_5 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Partition_1_1_3")])
Group_5.setName("pt_virole")
Group_5.result().setName("pt_virole")
Group_6 = model.addGroup(Part_1_doc, [model.selection("SOLID", "Partition_1_1_9"), model.selection("SOLID", "Partition_1_1_5")])
Group_6.setName("axe_eau")
Group_6.result().setName("axe_eau")
Group_7_objects = [model.selection("FACE", "Partition_1_1_3/Modified_Face&GVProfile/SketchLine_4"),
                   model.selection("FACE", "Recover_1_1_2/Modified_Face&GVProfile/SketchArc_1_2"),
                   model.selection("FACE", "Recover_1_1_2/Modified_Face&GVProfile/SketchLine_3"),
                   model.selection("FACE", "Recover_1_1_1/Modified_Face&GVProfile/SketchLine_3"),
                   model.selection("FACE", "Extrusion_2_1_1/To_Face"),
                   model.selection("FACE", "Extrusion_2_1_5/To_Face")]
Group_7 = model.addGroup(Part_1_doc, Group_7_objects)
Group_7.setName("faces_sec")
Group_7.result().setName("faces_sec")
Group_8_objects = [model.selection("FACE", "(Partition_1_1_4/Modified_Face&GVProfile/SketchLine_23)(Partition_1_1_4/Modified_Face&GVBody_1_3/To_Face)(Recover_1_1_3/Modified_Face&GVProfile/SketchArc_5_2)2(Recover_1_1_3/Modified_Face&GVProfile/SketchArc_6_2)2(Recover_1_1_3/Modified_Face&GVProfile/SketchLine_10)2(Recover_1_1_3/Modified_Face&GVProfile/SketchLine_9)2(Recover_1_1_3/Modified_Face&GVProfile/SketchLine_8)2(Recover_1_1_3/Modified_Face&GVProfile/SketchArc_4_2)2(Recover_1_1_3/Modified_Face&GVProfile/SketchLine_7)2(Partition_1_1_4/Modified_Face&GVBody_1_3/From_Face)2"),
                   model.selection("FACE", "(Partition_1_1_3/Modified_Face&GVProfile/SketchLine_23)(Partition_1_1_3/Modified_Face&GVBody_1_2/To_Face)(Recover_1_1_2/Modified_Face&GVProfile/SketchLine_3)2(Recover_1_1_2/Modified_Face&GVProfile/SketchArc_1_2)2(Partition_1_1_3/Modified_Face&GVProfile/SketchLine_4)2(Partition_1_1_3/Modified_Face&GVProfile/SketchLine_6&PPProfile/SketchLine_18)2(Partition_1_1_3/Modified_Face&GVProfile/SketchLine_6&PPProfile/SketchLine_22)2(Recover_1_1_2/Modified_Face&GVProfile/SketchLine_7)2(Recover_1_1_2/Modified_Face&GVProfile/SketchLine_24)2(Partition_1_1_3/Modified_Face&GVBody_1_2/From_Face)2"),
                   model.selection("FACE", "(Partition_1_1_3/Modified_Face&GVProfile/SketchLine_6&PPProfile/SketchLine_22)(Partition_1_1_3/Modified_Face&GVBody_1_2/To_Face)"),
                   model.selection("FACE", "Remove_SubShapes_1_1/Modified_Face&PPProfile/SketchLine_26"),
                   model.selection("FACE", "Remove_SubShapes_1_1/Modified_Face&PPProfile/SketchArc_8_2"),
                   model.selection("FACE", "(Partition_1_1_5/Modified_Face&PHomoProfile/SketchArc_9_2&GVProfile/SketchLine_5)(Extrusion_2_1_4/Generated_Face&PHomoProfile/SketchLine_30)(Extrusion_2_1_3/Generated_Face&PHomoProfile/SketchLine_30)(Partition_1_1_5/Modified_Face&PHomoProfile/SketchLine_29)(Partition_1_1_5/Modified_Face&Extrusion_2_1_1/From_Face&PPProfile/SketchLine_22)"),
                   model.selection("FACE", "Remove_SubShapes_1_1/Modified_Face&PPProfile/SketchLine_15"),
                   model.selection("FACE", "Remove_SubShapes_1_1/Modified_Face&PPProfile/SketchArc_7_2"),
                   model.selection("FACE", "(Partition_1_1_9/Modified_Face&PHomoProfile/SketchLine_36)(Extrusion_2_1_7/Generated_Face&PHomoProfile/SketchLine_40)(Extrusion_2_1_8/Generated_Face&PHomoProfile/SketchLine_40)(Partition_1_1_9/Modified_Face&PHomoProfile/SketchArc_10_2&GVProfile/SketchLine_5)(Partition_1_1_9/Modified_Face&Extrusion_2_1_5/From_Face&PPProfile/SketchLine_18)"),
                   model.selection("FACE", "(Partition_1_1_4/Modified_Face&GVProfile/SketchLine_23)(Partition_1_1_4/Modified_Face&GVBody_1_3/From_Face)(Recover_1_1_3/Modified_Face&GVProfile/SketchArc_5_2)2(Recover_1_1_3/Modified_Face&GVProfile/SketchArc_6_2)2(Recover_1_1_3/Modified_Face&GVProfile/SketchLine_10)2(Recover_1_1_3/Modified_Face&GVProfile/SketchLine_9)2(Recover_1_1_3/Modified_Face&GVProfile/SketchLine_8)2(Recover_1_1_3/Modified_Face&GVProfile/SketchArc_4_2)2(Recover_1_1_3/Modified_Face&GVProfile/SketchLine_7)2(Partition_1_1_4/Modified_Face&GVBody_1_3/To_Face)2"),
                   model.selection("FACE", "(Partition_1_1_3/Modified_Face&GVProfile/SketchLine_23)(Partition_1_1_3/Modified_Face&GVBody_1_2/From_Face)(Recover_1_1_2/Modified_Face&GVProfile/SketchLine_3)2(Recover_1_1_2/Modified_Face&GVProfile/SketchArc_1_2)2(Partition_1_1_3/Modified_Face&GVProfile/SketchLine_4)2(Partition_1_1_3/Modified_Face&GVProfile/SketchLine_6&PPProfile/SketchLine_18)2(Partition_1_1_3/Modified_Face&GVProfile/SketchLine_6&PPProfile/SketchLine_22)2(Recover_1_1_2/Modified_Face&GVProfile/SketchLine_7)2(Recover_1_1_2/Modified_Face&GVProfile/SketchLine_24)2(Partition_1_1_3/Modified_Face&GVBody_1_2/To_Face)2"),
                   model.selection("FACE", "(Partition_1_1_3/Modified_Face&GVProfile/SketchLine_6&PPProfile/SketchLine_18)(Partition_1_1_3/Modified_Face&GVBody_1_2/From_Face)")]
Group_8 = model.addGroup(Part_1_doc, Group_8_objects)
Group_8.setName("faces_pr")
Group_8.result().setName("faces_pr")
Group_9_objects = [model.selection("FACE", "Partition_1_1_6/Modified_Face&PHomoProfile/SketchLine_29"),
                   model.selection("FACE", "Extrusion_2_1_3/Generated_Face&PHomoProfile/SketchLine_29"),
                   model.selection("FACE", "Extrusion_2_1_7/Generated_Face&PHomoProfile/SketchLine_36"),
                   model.selection("FACE", "Partition_1_1_10/Modified_Face&PHomoProfile/SketchLine_36")]
Group_9 = model.addGroup(Part_1_doc, Group_9_objects)
Group_9.setName("pt_symz")
Group_9.result().setName("pt_symz")
Group_10_objects = [model.selection("FACE", "Recover_1_1_1/Modified_Face&GVBody_1_1/To_Face"),
                    model.selection("FACE", "Recover_1_1_1/Modified_Face&GVBody_1_1/From_Face"),
                    model.selection("FACE", "Partition_1_1_3/Modified_Face&GVBody_1_2/From_Face"),
                    model.selection("FACE", "Partition_1_1_10/Modified_Face&PHomoProfile/SketchLine_36"),
                    model.selection("FACE", "Extrusion_2_1_7/Generated_Face&PHomoProfile/SketchLine_36"),
                    model.selection("FACE", "Extrusion_2_1_3/Generated_Face&PHomoProfile/SketchLine_29"),
                    model.selection("FACE", "Partition_1_1_6/Modified_Face&PHomoProfile/SketchLine_29"),
                    model.selection("FACE", "Partition_1_1_3/Modified_Face&GVBody_1_2/To_Face"),
                    model.selection("FACE", "Partition_1_1_5/Modified_Face&PHomoProfile/SketchLine_29"),
                    model.selection("FACE", "Partition_1_1_9/Modified_Face&PHomoProfile/SketchLine_36"),
                    model.selection("FACE", "Partition_1_1_1/Modified_Face&Extrusion_1_1/To_Face"),
                    model.selection("FACE", "Partition_1_1_4/Modified_Face&GVBody_1_3/From_Face"),
                    model.selection("FACE", "Partition_1_1_4/Modified_Face&GVBody_1_3/To_Face")]
Group_10 = model.addGroup(Part_1_doc, Group_10_objects)
Group_10.setName("symz")
Group_10.result().setName("symz")
Group_11 = model.addGroup(Part_1_doc, [model.selection("FACE", "Recover_1_1_3/Modified_Face&GVProfile/SketchLine_10")])
Group_11.setName("appui")
Group_11.result().setName("appui")
Group_12 = model.addGroup(Part_1_doc, [model.selection("FACE", "Recover_1_1_1/Modified_Face&GVProfile/SketchLine_2")])
Group_12.setName("sup")
Group_12.result().setName("sup")
Group_13_objects = [model.selection("FACE", "Partition_1_1_6/Modified_Face&Extrusion_2_1_2/To_Face"),
                    model.selection("FACE", "Extrusion_2_1_4/To_Face"),
                    model.selection("FACE", "Extrusion_2_1_3/To_Face"),
                    model.selection("FACE", "Extrusion_2_1_8/To_Face"),
                    model.selection("FACE", "Extrusion_2_1_7/To_Face"),
                    model.selection("FACE", "Partition_1_1_10/Modified_Face&Extrusion_2_1_6/To_Face")]
Group_13 = model.addGroup(Part_1_doc, Group_13_objects)
Group_13.setName("pt_sec")
Group_13.result().setName("pt_sec")
Group_14_objects = [model.selection("FACE", "Partition_1_1_6/Modified_Face&Extrusion_2_1_2/From_Face"),
                    model.selection("FACE", "Extrusion_2_1_3/From_Face"),
                    model.selection("FACE", "Extrusion_2_1_4/From_Face"),
                    model.selection("FACE", "Extrusion_2_1_7/From_Face"),
                    model.selection("FACE", "Extrusion_2_1_8/From_Face"),
                    model.selection("FACE", "Partition_1_1_10/Modified_Face&Extrusion_2_1_6/From_Face")]
Group_14 = model.addGroup(Part_1_doc, Group_14_objects)
Group_14.setName("pt_pr")
Group_14.result().setName("pt_pr")
Group_15 = model.addGroup(Part_1_doc, [model.selection("VERTEX", "Recover_1_1_3/Modified_Vertex&weak_name_9")])
Group_15.setName("NO_DX")
Group_15.result().setName("NO_DX")
model.end()

from ModelAPI import *

aFactory = ModelAPI_Session.get().validators()

# Check groups
for i in range(Part_1_doc.size("Groups")):
  GroupFeature = Part_1_doc.feature(objectToResult(Part_1_doc.object("Groups", i)))
  assert(aFactory.validate(GroupFeature))

model.begin()
toto.setValue(2000)
model.end()

# Check groups again
for i in range(Part_1_doc.size("Groups")):
  GroupFeature = Part_1_doc.feature(objectToResult(Part_1_doc.object("Groups", i)))
  assert(aFactory.validate(GroupFeature))
