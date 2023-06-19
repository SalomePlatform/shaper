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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 0, 0, -99)
SketchLine_1.setAuxiliary(True)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchPoint_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, -44, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_2.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchLine_3 = Sketch_1.addLine(-44, 0, -44, -57.48528137423859)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), 44)
SketchArc_1 = Sketch_1.addArc(-31, -57.48528137423859, -44, -57.48528137423859, -40.19238815542511, -66.6776695296637, False)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchArc_1.startPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_3.result(), SketchArc_1.results()[1])
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 13)
SketchLine_4 = Sketch_1.addLine(-40.19238815542511, -66.6776695296637, -13.4350288425444, -93.43502884254441)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_4.startPoint())
SketchArc_2 = Sketch_1.addArc(0, -80, -13.4350288425444, -93.43502884254441, 0, -99, False)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_1.result(), SketchArc_2.center())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_1.result(), SketchArc_2.endPoint())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_4.result())
SketchConstraintTangent_3 = Sketch_1.setTangent(SketchLine_4.result(), SketchArc_2.results()[1])
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchArc_2.endPoint())
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_2.results()[1], 19)
SketchLine_5 = Sketch_1.addLine(0, -80, -13.4350288425444, -93.43502884254441)
SketchLine_5.setName("SketchLine_6")
SketchLine_5.result().setName("SketchLine_6")
SketchLine_5.setAuxiliary(True)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchArc_2.center(), SketchLine_5.startPoint())
SketchConstraintCoincidence_10.setName("SketchConstraintCoincidence_12")
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.endPoint())
SketchConstraintCoincidence_11.setName("SketchConstraintCoincidence_13")
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_5.result(), SketchLine_1.result(), 45, type = "Supplementary")
SketchConstraintMirror_1_objects = [SketchLine_2.result(), SketchLine_3.result(), SketchArc_1.results()[1], SketchLine_4.result(), SketchArc_2.results()[1]]
SketchConstraintMirror_1 = Sketch_1.addMirror(SketchLine_1.result(), SketchConstraintMirror_1_objects)
[SketchLine_6, SketchLine_7, SketchArc_3, SketchLine_8, SketchArc_4] = SketchConstraintMirror_1.mirrored()
SketchLine_8.setName("SketchLine_9")
SketchLine_8.result().setName("SketchLine_9")
SketchLine_7.setName("SketchLine_8")
SketchLine_7.result().setName("SketchLine_8")
SketchLine_6.setName("SketchLine_7")
SketchLine_6.result().setName("SketchLine_7")
SketchCircle_1 = Sketch_1.addCircle(-25, -44, 5)
SketchCircle_1.setName("SketchCircle_2")
SketchCircle_1.result().setName("SketchCircle_2")
SketchCircle_1.results()[1].setName("SketchCircle_2_2")
SketchCircle_2 = Sketch_1.addCircle(25, -44, 5)
SketchCircle_2.setName("SketchCircle_3")
SketchCircle_2.result().setName("SketchCircle_3")
SketchCircle_2.results()[1].setName("SketchCircle_3_2")
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchCircle_1.results()[1], SketchCircle_2.results()[1])
SketchConstraintRadius_3 = Sketch_1.setRadius(SketchCircle_1.results()[1], 5)
SketchConstraintRadius_3.setName("SketchConstraintRadius_7")
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchCircle_1.center(), SketchCircle_2.center(), 50, True)
SketchConstraintDistance_1.setName("SketchConstraintDistance_4")
SketchLine_9 = Sketch_1.addLine(-25, -44, 25, -44)
SketchLine_9.setName("SketchLine_21")
SketchLine_9.result().setName("SketchLine_21")
SketchLine_9.setAuxiliary(True)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_9.startPoint())
SketchConstraintCoincidence_12.setName("SketchConstraintCoincidence_28")
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchCircle_2.center(), SketchLine_9.endPoint())
SketchConstraintCoincidence_13.setName("SketchConstraintCoincidence_29")
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_9.result())
SketchConstraintHorizontal_2.setName("SketchConstraintHorizontal_4")
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchAPI_Arc(SketchArc_4).center(), SketchLine_9.result(), 36, True)
SketchConstraintDistance_2.setName("SketchConstraintDistance_5")
SketchPoint_2 = Sketch_1.addPoint(2.896598636358403e-31, -44)
SketchPoint_2.setName("SketchPoint_4")
SketchPoint_2.result().setName("SketchPoint_4")
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_9.result())
SketchConstraintCoincidence_14.setName("SketchConstraintCoincidence_30")
SketchConstraintMiddle_1 = Sketch_1.setMiddlePoint(SketchLine_9.result(), SketchPoint_2.coordinates())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_1.result())
SketchConstraintCoincidence_15.setName("SketchConstraintCoincidence_31")
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_6.result(), SketchLine_5.startPoint(), 80, True)
SketchConstraintDistance_3.setName("SketchConstraintDistance_8")
model.do()
Sketch_1.changeFacesOrder([[SketchLine_2.result(), SketchLine_3.result(), SketchArc_1.results()[1], SketchLine_4.result(), SketchArc_2.results()[1], SketchArc_4.results()[1], SketchLine_8.result(), SketchArc_3.results()[1], SketchLine_7.result(), SketchLine_6.result(), SketchCircle_1.results()[1], SketchCircle_2.results()[1]],
                           [SketchCircle_2.results()[1]],
                           [SketchCircle_1.results()[1]]
                          ])
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_2f-SketchLine_3f-SketchArc_1_2f-SketchLine_4f-SketchArc_2_2f-SketchArc_4_2f-SketchLine_9r-SketchArc_3_2f-SketchLine_8r-SketchLine_7r-SketchCircle_2_2r-SketchCircle_3_2r")], model.selection(), 10, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/To_Face"))
SketchLine_10 = Sketch_2.addLine(0, 0, 0, -16)
SketchLine_10.setName("SketchLine_10")
SketchLine_10.result().setName("SketchLine_10")
SketchLine_10.setAuxiliary(True)
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_7][Extrusion_1_1/To_Face]"), False)
SketchPoint_3 = SketchProjection_2.createdFeature()
SketchPoint_3.setName("SketchPoint_2")
SketchPoint_3.result().setName("SketchPoint_2")
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchLine_10.startPoint(), SketchPoint_3.result())
SketchConstraintCoincidence_16.setName("SketchConstraintCoincidence_14")
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_10.result())
SketchLine_11 = Sketch_2.addLine(-44, 0, -44, -13)
SketchLine_11.setName("SketchLine_11")
SketchLine_11.result().setName("SketchLine_11")
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/To_Face]"), False)
SketchPoint_4 = SketchProjection_3.createdFeature()
SketchPoint_4.setName("SketchPoint_3")
SketchPoint_4.result().setName("SketchPoint_3")
SketchConstraintCoincidence_17 = Sketch_2.setCoincident(SketchLine_11.startPoint(), SketchPoint_4.result())
SketchConstraintCoincidence_17.setName("SketchConstraintCoincidence_15")
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_11.result())
SketchLine_12 = Sketch_2.addLine(-44, -13, -21.99999999999997, -13)
SketchLine_12.setName("SketchLine_12")
SketchLine_12.result().setName("SketchLine_12")
SketchConstraintCoincidence_18 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_18.setName("SketchConstraintCoincidence_16")
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_12.result())
SketchConstraintHorizontal_3.setName("SketchConstraintHorizontal_2")
SketchArc_5 = Sketch_2.addArc(-22, -16.00000000000039, -21.99999999999997, -13, -19, -16, True)
SketchConstraintCoincidence_19 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchArc_5.startPoint())
SketchConstraintCoincidence_19.setName("SketchConstraintCoincidence_17")
SketchConstraintTangent_4 = Sketch_2.setTangent(SketchLine_12.result(), SketchArc_5.results()[1])
SketchLine_13 = Sketch_2.addLine(-19, -16, 0, -16)
SketchLine_13.setName("SketchLine_13")
SketchLine_13.result().setName("SketchLine_13")
SketchConstraintCoincidence_20 = Sketch_2.setCoincident(SketchArc_5.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_20.setName("SketchConstraintCoincidence_18")
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_10.result())
SketchConstraintCoincidence_21.setName("SketchConstraintCoincidence_19")
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_13.result())
SketchConstraintHorizontal_4.setName("SketchConstraintHorizontal_3")
SketchConstraintRadius_4 = Sketch_2.setRadius(SketchArc_5.results()[1], 3)
SketchConstraintRadius_4.setName("SketchConstraintRadius_3")
SketchConstraintLength_2 = Sketch_2.setLength(SketchLine_13.result(), "38/2")
SketchConstraintDistance_4 = Sketch_2.setDistance(SketchLine_13.result(), SketchLine_12.endPoint(), 3, True)
SketchConstraintDistance_4.setName("SketchConstraintDistance_2")
SketchConstraintLength_3 = Sketch_2.setLength(SketchLine_11.result(), 13)
SketchConstraintCoincidence_22 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_10.endPoint())
SketchConstraintCoincidence_22.setName("SketchConstraintCoincidence_20")
SketchLine_14 = Sketch_2.addLine(-44, 0, -25, 0)
SketchLine_14.setName("SketchLine_14")
SketchLine_14.result().setName("SketchLine_14")
SketchConstraintCoincidence_23 = Sketch_2.setCoincident(SketchLine_11.startPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_23.setName("SketchConstraintCoincidence_21")
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/To_Face]"), False)
SketchLine_15 = SketchProjection_4.createdFeature()
SketchLine_15.setName("SketchLine_15")
SketchLine_15.result().setName("SketchLine_15")
SketchConstraintCoincidence_24 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchLine_15.result())
SketchConstraintCoincidence_24.setName("SketchConstraintCoincidence_22")
SketchProjection_5 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_16 = SketchProjection_5.createdFeature()
SketchLine_16.setName("SketchLine_16")
SketchLine_16.result().setName("SketchLine_16")
SketchArc_6 = Sketch_2.addArc(-25, 6, -25, 0, -19, 6, False)
SketchConstraintCoincidence_25 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchArc_6.startPoint())
SketchConstraintCoincidence_25.setName("SketchConstraintCoincidence_23")
SketchConstraintTangent_5 = Sketch_2.setTangent(SketchLine_14.result(), SketchArc_6.results()[1])
SketchArc_7 = Sketch_2.addArc(0, 5.999999999999997, -19, 6, 0, 25, True)
SketchConstraintCoincidence_26 = Sketch_2.setCoincident(SketchArc_6.endPoint(), SketchArc_7.startPoint())
SketchConstraintCoincidence_26.setName("SketchConstraintCoincidence_24")
SketchConstraintTangent_6 = Sketch_2.setTangent(SketchArc_6.results()[1], SketchArc_7.results()[1])
SketchConstraintCoincidence_27 = Sketch_2.setCoincident(SketchArc_7.endPoint(), SketchLine_16.result())
SketchConstraintCoincidence_27.setName("SketchConstraintCoincidence_25")
SketchConstraintCoincidence_28 = Sketch_2.setCoincident(SketchArc_7.center(), SketchLine_16.result())
SketchConstraintCoincidence_28.setName("SketchConstraintCoincidence_26")
SketchConstraintRadius_5 = Sketch_2.setRadius(SketchArc_7.results()[1], 19)
SketchConstraintRadius_5.setName("SketchConstraintRadius_4")
SketchConstraintRadius_6 = Sketch_2.setRadius(SketchArc_6.results()[1], 6)
SketchConstraintRadius_6.setName("SketchConstraintRadius_5")
SketchConstraintDistance_5 = Sketch_2.setDistance(SketchArc_7.center(), SketchLine_12.result(), 19, True)
SketchConstraintDistance_5.setName("SketchConstraintDistance_3")
SketchConstraintMirror_2_objects = [SketchArc_5.results()[1], SketchLine_12.result(), SketchLine_11.result(), SketchArc_6.results()[1], SketchArc_7.results()[1], SketchLine_13.result(), SketchLine_14.result()]
SketchConstraintMirror_2 = Sketch_2.addMirror(SketchLine_10.result(), SketchConstraintMirror_2_objects)
[SketchArc_8, SketchLine_17, SketchLine_18, SketchArc_9, SketchArc_10, SketchLine_19, SketchLine_20] = SketchConstraintMirror_2.mirrored()
SketchLine_20.setName("SketchLine_20")
SketchLine_20.result().setName("SketchLine_20")
SketchLine_19.setName("SketchLine_19")
SketchLine_19.result().setName("SketchLine_19")
SketchLine_18.setName("SketchLine_18")
SketchLine_18.result().setName("SketchLine_18")
SketchLine_17.setName("SketchLine_17")
SketchLine_17.result().setName("SketchLine_17")
SketchCircle_3 = Sketch_2.addCircle(0, 5.999999999999997, 9.5)
SketchCircle_3.setName("SketchCircle_1")
SketchCircle_3.result().setName("SketchCircle_1")
SketchCircle_3.results()[1].setName("SketchCircle_1_2")
SketchConstraintCoincidence_29 = Sketch_2.setCoincident(SketchArc_7.center(), SketchCircle_3.center())
SketchConstraintCoincidence_29.setName("SketchConstraintCoincidence_27")
SketchConstraintRadius_7 = Sketch_2.setRadius(SketchCircle_3.results()[1], "19/2")
SketchConstraintRadius_7.setName("SketchConstraintRadius_6")
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_11f-SketchLine_12f-SketchArc_5_2r-SketchLine_13f-SketchLine_19r-SketchArc_8_2r-SketchLine_17r-SketchLine_18r-SketchLine_20f-SketchArc_9_2r-SketchArc_10_2f-SketchArc_7_2f-SketchArc_6_2r-SketchLine_14r-SketchCircle_1_2r")], model.selection(), 40, 10)
Sketch_3 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))
SketchLine_21 = Sketch_3.addLine(-16, 50, -20, 50)
SketchLine_21.setName("SketchLine_22")
SketchLine_21.result().setName("SketchLine_22")
SketchProjection_6 = Sketch_3.addProjection(model.selection("VERTEX", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_13][Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_19][Extrusion_2_1/To_Face]"), False)
SketchPoint_5 = SketchProjection_6.createdFeature()
SketchConstraintCoincidence_30 = Sketch_3.setCoincident(SketchLine_21.startPoint(), SketchPoint_5.result())
SketchConstraintCoincidence_30.setName("SketchConstraintCoincidence_32")
SketchConstraintHorizontal_5 = Sketch_3.setHorizontal(SketchLine_21.result())
SketchLine_22 = Sketch_3.addLine(-20, 50, -80, 10)
SketchLine_22.setName("SketchLine_23")
SketchLine_22.result().setName("SketchLine_23")
SketchConstraintCoincidence_31 = Sketch_3.setCoincident(SketchLine_21.endPoint(), SketchLine_22.startPoint())
SketchConstraintCoincidence_31.setName("SketchConstraintCoincidence_33")
SketchProjection_7 = Sketch_3.addProjection(model.selection("EDGE", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchArc_5_2][Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_13]"), True)
SketchLine_23 = SketchProjection_7.createdFeature()
SketchLine_23.setName("SketchLine_24")
SketchLine_23.result().setName("SketchLine_24")
SketchLine_24 = Sketch_3.addLine(-16, 9.999999999999996, -80, 10)
SketchLine_24.setName("SketchLine_25")
SketchLine_24.result().setName("SketchLine_25")
SketchConstraintCoincidence_32 = Sketch_3.setCoincident(SketchLine_24.startPoint(), SketchLine_23.result())
SketchConstraintCoincidence_32.setName("SketchConstraintCoincidence_34")
SketchConstraintHorizontal_6 = Sketch_3.setHorizontal(SketchLine_24.result())
SketchConstraintCoincidence_33 = Sketch_3.setCoincident(SketchLine_22.endPoint(), SketchLine_24.endPoint())
SketchConstraintCoincidence_33.setName("SketchConstraintCoincidence_35")
SketchProjection_8 = Sketch_3.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_25 = SketchProjection_8.createdFeature()
SketchLine_25.setName("SketchLine_26")
SketchLine_25.result().setName("SketchLine_26")
SketchConstraintDistance_6 = Sketch_3.setDistance(SketchLine_22.startPoint(), SketchLine_25.result(), 20, True)
SketchConstraintDistance_6.setName("SketchConstraintDistance_6")
SketchConstraintDistance_7 = Sketch_3.setDistance(SketchAPI_Point(SketchPoint_5).coordinates(), SketchLine_24.startPoint(), 40, True)
SketchConstraintDistance_7.setName("SketchConstraintDistance_7")
SketchConstraintLength_4 = Sketch_3.setLength(SketchLine_24.result(), "67-3")
model.do()
Extrusion_3 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchLine_22r-SketchLine_23f-SketchLine_25r-SketchProjection_7f")], model.selection(), 5, 5)
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_1_2f")], model.selection(), [model.selection("SOLID", "Extrusion_1_1")])
Fuse_1_objects_1 = [model.selection("SOLID", "Extrusion_2_1"), model.selection("SOLID", "ExtrusionCut_1_1"), model.selection("SOLID", "Extrusion_3_1")]
Fuse_1 = model.addFuse(Part_1_doc, Fuse_1_objects_1, removeEdges = True, keepSubResults = True)
model.testHaveNamingSubshapes(Fuse_1, model, Part_1_doc)
model.end()

from GeomAPI import GeomAPI_Shape
model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [0])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.FACE, [27])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.EDGE, [150])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [300])
model.testResultsVolumes(Fuse_1, [162690.939657109178369864821434021])

from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()

# Check that the features are not in error
for i in range(Part_1_doc.size("Features")):
  feature = objectToFeature(Part_1_doc.object("Features", i))
  name = feature.name()
  error = feature.error()
  assert(error == ''), "The feature {0} is in error: {1}".format(name, error)
  assert(aFactory.validate(feature)), "The feature {0} is in error: {1}".format(name, error)

assert(Part_1_doc.size("Groups") == 3)

assert(model.checkPythonDump())
