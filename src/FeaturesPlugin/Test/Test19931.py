# Copyright (C) 2020-2021  CEA/DEN, EDF R&D
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

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(0, 62.99999999999999, 32, 63)
Sketch_1.setHorizontal(SketchLine_1.result())

### Create SketchLine
SketchLine_2 = Sketch_1.addLine(35, 66, 35, 87.99999999999999)
Sketch_1.setVertical(SketchLine_2.result())

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(35, 87.99999999999999, 75.00000000000001, 87.99999999999999)
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
Sketch_1.setHorizontal(SketchLine_3.result())

### Create SketchLine
SketchLine_4 = Sketch_1.addLine(75.00000000000001, 87.99999999999999, 75.00000000000001, 78)
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
Sketch_1.setVertical(SketchLine_4.result())

### Create SketchArc
SketchArc_1 = Sketch_1.addArc(75.00000000000004, 71.99999999999689, 75.00000000000001, 78, 69, 71.99999999999613, False)
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchArc_1.startPoint())

### Create SketchLine
SketchLine_5 = Sketch_1.addLine(69, 71.99999999999613, 69, 52.00000000001324)
Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_5.startPoint())
Sketch_1.setVertical(SketchLine_5.result())

### Create SketchArc
SketchArc_2 = Sketch_1.addArc(57, 52.00000000001324, 69, 52.00000000001324, 57.0000000000007, 40, True)
Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchArc_2.startPoint())
Sketch_1.setTangent(SketchLine_5.result(), SketchArc_2.results()[1])

### Create SketchArc
SketchArc_3 = Sketch_1.addArc(57, 27.99999999998618, 57.0000000000007, 40, 45, 27.99999999998618, False)
Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchArc_3.startPoint())
Sketch_1.setTangent(SketchArc_2.results()[1], SketchArc_3.results()[1])

### Create SketchLine
SketchLine_6 = Sketch_1.addLine(45, 27.99999999998618, 45, 0)
Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_6.startPoint())

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_7 = SketchProjection_1.createdFeature()
Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.result())
Sketch_1.setVertical(SketchLine_6.result())

### Create SketchLine
SketchLine_8 = Sketch_1.addLine(45, 0, 0, 0)
Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_8.startPoint())
Sketch_1.setCoincident(SketchAPI_Line(SketchLine_7).startPoint(), SketchLine_8.endPoint())

### Create SketchProjection
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_9 = SketchProjection_2.createdFeature()
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_9.result())
Sketch_1.setTangent(SketchArc_3.results()[1], SketchLine_6.result())
Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_5.result())
Sketch_1.setLength(SketchLine_8.result(), 45)
Sketch_1.setDistance(SketchLine_1.startPoint(), SketchLine_4.result(), "150/2", True)
Sketch_1.setDistance(SketchLine_3.result(), SketchArc_1.startPoint(), 10, True)
Sketch_1.setRadius(SketchArc_1.results()[1], 6)
Sketch_1.setRadius(SketchArc_2.results()[1], 12)
Sketch_1.setEqual(SketchArc_2.results()[1], SketchArc_3.results()[1])
Sketch_1.setDistance(SketchLine_7.result(), SketchArc_3.startPoint(), 40, True)
Sketch_1.setVerticalDistance(SketchArc_3.startPoint(), SketchArc_1.startPoint(), 38)
Sketch_1.setDistance(SketchLine_9.result(), SketchLine_5.startPoint(), "138/2", True)

### Create SketchLine
SketchLine_10 = Sketch_1.addLine(-75.00000000000001, 77.99999999999693, -50.99999999999996, 78)
SketchLine_10.setName("SketchLine_17")
SketchLine_10.result().setName("SketchLine_17")
Sketch_1.setHorizontal(SketchLine_10.result())

### Create SketchArc
SketchArc_4 = Sketch_1.addArc(-50.99999999999996, 72, -50.99999999999996, 78, -44.99999999999996, 72, True)
SketchArc_4.setName("SketchArc_7")
SketchArc_4.result().setName("SketchArc_7")
SketchArc_4.results()[1].setName("SketchArc_7_2")
Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchArc_4.startPoint())
Sketch_1.setTangent(SketchLine_10.result(), SketchArc_4.results()[1])

### Create SketchArc
SketchArc_5 = Sketch_1.addArc(-28, 49.99999999999999, -28, 52.99999999999999, -25, 49.99999999999999, True)
SketchArc_5.setName("SketchArc_9")
SketchArc_5.result().setName("SketchArc_9")
SketchArc_5.results()[1].setName("SketchArc_9_2")

### Create SketchLine
SketchLine_11 = Sketch_1.addLine(-25, 49.99999999999999, -25, 27.99999999999997)
SketchLine_11.setName("SketchLine_18")
SketchLine_11.result().setName("SketchLine_18")
Sketch_1.setCoincident(SketchArc_5.endPoint(), SketchLine_11.startPoint())
Sketch_1.setVertical(SketchLine_11.result())

### Create SketchCircle
SketchCircle_1 = Sketch_1.addCircle(0, 27.99999999999997, 25)
Sketch_1.setCoincident(SketchLine_9.result(), SketchCircle_1.center())
Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchCircle_1.results()[1])

### Create SketchCircle
SketchCircle_2 = Sketch_1.addCircle(0, 27.99999999999997, 16)
Sketch_1.setCoincident(SketchCircle_1.center(), SketchCircle_2.center())
Sketch_1.setRadius(SketchArc_4.results()[1], 6)
Sketch_1.setRadius(SketchArc_5.results()[1], 3)
Sketch_1.setTangent(SketchArc_5.results()[1], SketchLine_11.result())
Sketch_1.setRadius(SketchCircle_1.results()[1], 25)
Sketch_1.setRadius(SketchCircle_2.results()[1], 16)

### Create SketchLine
SketchLine_12 = Sketch_1.addLine(-44.99999999999996, 72, -44.99999999999998, 64.99999999999999)
SketchLine_12.setName("SketchLine_19")
SketchLine_12.result().setName("SketchLine_19")
Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchLine_12.startPoint())
Sketch_1.setVertical(SketchLine_12.result())

### Create SketchLine
SketchLine_13 = Sketch_1.addLine(-28, 52.99999999999999, -32.99999999999998, 52.99999999999999)
SketchLine_13.setName("SketchLine_20")
SketchLine_13.result().setName("SketchLine_20")
Sketch_1.setCoincident(SketchArc_5.startPoint(), SketchLine_13.startPoint())
Sketch_1.setHorizontal(SketchLine_13.result())

### Create SketchArc
SketchArc_6 = Sketch_1.addArc(-32.99999999999998, 64.99999999999999, -44.99999999999998, 64.99999999999999, -32.99999999999998, 52.99999999999999, False)
SketchArc_6.setName("SketchArc_10")
SketchArc_6.result().setName("SketchArc_10")
SketchArc_6.results()[1].setName("SketchArc_10_2")
Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchArc_6.startPoint())
Sketch_1.setTangent(SketchLine_12.result(), SketchArc_6.results()[1])
Sketch_1.setCoincident(SketchLine_13.result(), SketchArc_6.endPoint())
Sketch_1.setTangent(SketchArc_4.results()[1], SketchLine_12.result())
Sketch_1.setTangent(SketchArc_6.results()[1], SketchLine_13.result())
Sketch_1.setTangent(SketchLine_13.result(), SketchArc_5.results()[1])
Sketch_1.setRadius(SketchArc_6.results()[1], 12)
Sketch_1.setCoincident(SketchArc_6.endPoint(), SketchLine_13.endPoint())

### Create SketchLine
SketchLine_14 = Sketch_1.addLine(-25, 27.99999999999997, 0, 27.99999999999997)
SketchLine_14.setName("SketchLine_21")
SketchLine_14.result().setName("SketchLine_21")
SketchLine_14.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_14.startPoint())
Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_14.endPoint())
Sketch_1.setHorizontal(SketchLine_14.result())

### Create SketchArc
SketchArc_7 = Sketch_1.addArc(32, 66, 32, 63, 35, 66, False)
SketchArc_7.setName("SketchArc_11")
SketchArc_7.result().setName("SketchArc_11")
SketchArc_7.results()[1].setName("SketchArc_11_2")

### Create SketchConstraintMirror
SketchConstraintMirror_1_objects = [SketchLine_4.result(), SketchLine_5.result(), SketchLine_1.result(), SketchLine_3.result(), SketchLine_2.result(), SketchLine_6.result(), SketchLine_8.result(), SketchArc_1.results()[1], SketchArc_3.results()[1], SketchArc_2.results()[1], SketchArc_7.results()[1]]
SketchConstraintMirror_1 = Sketch_1.addMirror(SketchLine_9.result(), SketchConstraintMirror_1_objects)
[SketchLine_15, SketchLine_16, SketchLine_17, SketchLine_18, SketchLine_19, SketchLine_20, SketchLine_21, SketchArc_8, SketchArc_9, SketchArc_10, SketchArc_11] = SketchConstraintMirror_1.mirrored()
SketchArc_11.setName("SketchArc_12")
SketchArc_11.result().setName("SketchArc_12")
SketchArc_11.results()[1].setName("SketchArc_12_2")
SketchArc_10.setName("SketchArc_6")
SketchArc_10.result().setName("SketchArc_6")
SketchArc_10.results()[1].setName("SketchArc_6_2")
SketchArc_9.setName("SketchArc_5")
SketchArc_9.result().setName("SketchArc_5")
SketchArc_9.results()[1].setName("SketchArc_5_2")
SketchArc_8.setName("SketchArc_4")
SketchArc_8.result().setName("SketchArc_4")
SketchArc_8.results()[1].setName("SketchArc_4_2")
SketchLine_21.setName("SketchLine_16")
SketchLine_21.result().setName("SketchLine_16")
SketchLine_20.setName("SketchLine_15")
SketchLine_20.result().setName("SketchLine_15")
SketchLine_19.setName("SketchLine_14")
SketchLine_19.result().setName("SketchLine_14")
SketchLine_18.setName("SketchLine_13")
SketchLine_18.result().setName("SketchLine_13")
SketchLine_17.setName("SketchLine_12")
SketchLine_17.result().setName("SketchLine_12")
SketchLine_16.setName("SketchLine_11")
SketchLine_16.result().setName("SketchLine_11")
SketchLine_15.setName("SketchLine_10")
SketchLine_15.result().setName("SketchLine_10")
Sketch_1.setCoincident(SketchAPI_Line(SketchLine_15).endPoint(), SketchLine_10.startPoint())
Sketch_1.setDistance(SketchLine_12.result(), SketchAPI_Line(SketchLine_17).startPoint(), 45, True)
Sketch_1.setDistance(SketchLine_18.result(), SketchLine_14.endPoint(), 60, True)
Sketch_1.setDistance(SketchLine_17.result(), SketchLine_13.startPoint(), 10, True)

### Create SketchPoint
SketchPoint_1 = Sketch_1.addPoint(35, 62.99999999999999)
SketchPoint_1.setAuxiliary(True)
Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_1.result())
Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_2.result())
Sketch_1.setDistance(SketchLine_1.startPoint(), SketchPoint_1.coordinates(), 35, False)
Sketch_1.setDistance(SketchPoint_1.coordinates(), SketchLine_2.endPoint(), 25, False)
Sketch_1.setCoincident(SketchArc_7.startPoint(), SketchLine_1.endPoint())
Sketch_1.setCoincident(SketchArc_7.endPoint(), SketchLine_2.startPoint())
Sketch_1.setTangent(SketchArc_7.results()[1], SketchLine_1.result())
Sketch_1.setTangent(SketchArc_7.results()[1], SketchLine_2.result())
Sketch_1.setRadius(SketchArc_7.results()[1], 3)

### Create SketchConstraintMirror
SketchConstraintMirror_2_objects = [SketchLine_10.result(), SketchArc_4.results()[1], SketchLine_12.result(), SketchArc_6.results()[1], SketchLine_13.result(), SketchArc_5.results()[1], SketchLine_11.result()]
SketchConstraintMirror_2 = Sketch_1.addMirror(SketchLine_9.result(), SketchConstraintMirror_2_objects)
[SketchLine_22, SketchArc_12, SketchLine_23, SketchArc_13, SketchLine_24, SketchArc_14, SketchLine_25] = SketchConstraintMirror_2.mirrored()
SketchArc_14.setName("SketchArc_15")
SketchArc_14.result().setName("SketchArc_15")
SketchArc_14.results()[1].setName("SketchArc_15_2")
SketchArc_13.setName("SketchArc_14")
SketchArc_13.result().setName("SketchArc_14")
SketchArc_13.results()[1].setName("SketchArc_14_2")
SketchArc_12.setName("SketchArc_13")
SketchArc_12.result().setName("SketchArc_13")
SketchArc_12.results()[1].setName("SketchArc_13_2")
model.do()

### Create Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_22f-SketchArc_13_2f-SketchLine_23f-SketchArc_14_2r-SketchLine_24r-SketchArc_15_2f-SketchLine_25f-SketchCircle_1_2r-SketchLine_18r-SketchArc_9_2f-SketchLine_20f-SketchArc_10_2r-SketchLine_19r-SketchArc_7_2f-SketchLine_17r-SketchArc_4_2r-SketchLine_11f-SketchArc_6_2f-SketchArc_5_2r-SketchLine_15f-SketchLine_16f-SketchLine_8r-SketchLine_6r-SketchArc_3_2r-SketchArc_2_2f-SketchLine_5r-SketchArc_1_2r")], model.selection(), 10, 0)

### Create Extrusion
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_12f-SketchArc_12_2r-SketchLine_14f-SketchLine_13f-SketchLine_10f-SketchLine_17f-SketchArc_7_2r-SketchLine_19f-SketchArc_10_2f-SketchLine_20r-SketchArc_9_2r-SketchLine_18f-SketchCircle_1_2r-SketchLine_25r-SketchArc_15_2r-SketchLine_24f-SketchArc_14_2f-SketchLine_23r-SketchArc_13_2r-SketchLine_22r-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchArc_11_2r-SketchLine_1r")], model.selection(), 35, 0)

### Create Extrusion
Extrusion_3 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f-SketchCircle_1_2f-SketchCircle_2_2r")], model.selection(), 45, 0)

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_2_1/Generated_Face&Sketch_1/SketchLine_22"))

### Create SketchCircle
SketchCircle_3 = Sketch_2.addCircle(63, 23, 5)
Sketch_2.setRadius(SketchCircle_3.results()[1], 5)

### Create SketchProjection
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_26 = SketchProjection_3.createdFeature()
Sketch_2.setDistance(SketchCircle_3.center(), SketchLine_26.result(), "126/2", True)

### Create SketchProjection
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_27 = SketchProjection_4.createdFeature()
Sketch_2.setDistance(SketchCircle_3.center(), SketchLine_27.result(), 23, True)

### Create SketchConstraintMirror
SketchConstraintMirror_3 = Sketch_2.addMirror(SketchLine_26.result(), [SketchCircle_3.results()[1]])
[SketchCircle_4] = SketchConstraintMirror_3.mirrored()
model.do()

### Create ExtrusionCut
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_2")], model.selection(), 0, 10, [model.selection("SOLID", "Extrusion_2_1")])

### Create Sketch
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6"))

### Create SketchLine
SketchLine_28 = Sketch_3.addLine(0, 0, 9, -55.99999999999999)

### Create SketchCircle
SketchCircle_5 = Sketch_3.addCircle(28, -56, 19)
Sketch_3.setCoincident(SketchLine_28.endPoint(), SketchCircle_5.results()[1])

### Create SketchCircle
SketchCircle_6 = Sketch_3.addCircle(28, -56, 10)
Sketch_3.setCoincident(SketchCircle_5.center(), SketchCircle_6.center())

### Create SketchLine
SketchLine_29 = Sketch_3.addLine(47, -55.99999999999999, 56.26584765647708, -18.93660937409168)
Sketch_3.setCoincident(SketchLine_29.startPoint(), SketchCircle_5.results()[1])
Sketch_3.setRadius(SketchCircle_5.results()[1], "38/2")
Sketch_3.setRadius(SketchCircle_6.results()[1], 10)

### Create SketchProjection
SketchProjection_5 = Sketch_3.addProjection(model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_8][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/From_Face]"), False)
SketchPoint_2 = SketchProjection_5.createdFeature()
Sketch_3.setCoincident(SketchLine_28.startPoint(), SketchAPI_Point(SketchPoint_2).coordinates())

### Create SketchProjection
SketchProjection_6 = Sketch_3.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/From_Face]"), False)
SketchLine_30 = SketchProjection_6.createdFeature()
Sketch_3.setDistance(SketchCircle_5.center(), SketchLine_30.result(), 56, True)

### Create SketchLine
SketchLine_31 = Sketch_3.addLine(9, -55.99999999999999, 47, -55.99999999999999)
SketchLine_31.setAuxiliary(True)
Sketch_3.setCoincident(SketchLine_31.startPoint(), SketchCircle_5.results()[1])
Sketch_3.setCoincident(SketchLine_31.endPoint(), SketchCircle_5.results()[1])
Sketch_3.setHorizontal(SketchLine_31.result())
Sketch_3.setCoincident(SketchLine_31.result(), SketchCircle_5.center())
Sketch_3.setCoincident(SketchLine_28.endPoint(), SketchLine_31.startPoint())
Sketch_3.setCoincident(SketchLine_29.startPoint(), SketchLine_31.endPoint())

### Create SketchProjection
SketchProjection_7 = Sketch_3.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_8][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6]"), False)
SketchLine_32 = SketchProjection_7.createdFeature()
Sketch_3.setDistance(SketchCircle_6.center(), SketchLine_32.result(), 28, True)

### Create SketchLine
SketchLine_33 = Sketch_3.addLine(56.26584765647708, -18.93660937409168, 61, 0)
SketchLine_33.setAuxiliary(True)
Sketch_3.setCoincident(SketchLine_29.endPoint(), SketchLine_33.startPoint())
Sketch_3.setCoincident(SketchLine_33.endPoint(), SketchLine_30.result())
Sketch_3.setCollinear(SketchLine_33.result(), SketchLine_29.result())
Sketch_3.setHorizontalDistance(SketchCircle_5.center(), SketchLine_33.endPoint(), 33)

### Create SketchProjection
SketchProjection_8 = Sketch_3.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_23][Extrusion_1_1/From_Face]"), False)
SketchLine_34 = SketchProjection_8.createdFeature()

### Create SketchArc
SketchArc_15 = Sketch_3.addArc(80.51941016011038, -25, 56.26584765647708, -18.93660937409168, 80.519344153399, 0, True)
SketchArc_15.setName("SketchArc_16")
SketchArc_15.result().setName("SketchArc_16")
SketchArc_15.results()[1].setName("SketchArc_16_2")
Sketch_3.setCoincident(SketchLine_29.endPoint(), SketchArc_15.startPoint())
Sketch_3.setTangent(SketchLine_29.result(), SketchArc_15.results()[1])
Sketch_3.setCoincident(SketchLine_34.result(), SketchArc_15.endPoint())
Sketch_3.setTangent(SketchLine_34.result(), SketchArc_15.results()[1])
Sketch_3.setRadius(SketchArc_15.results()[1], 25)

### Create SketchLine
SketchLine_35 = Sketch_3.addLine(0, 0, 80.519344153399, 0)
Sketch_3.setCoincident(SketchLine_28.startPoint(), SketchLine_35.startPoint())
Sketch_3.setCoincident(SketchArc_15.endPoint(), SketchLine_35.endPoint())
model.do()
Sketch_3.changeFacesOrder([[SketchCircle_6.results()[1]],
                           [SketchLine_28.result(), SketchCircle_5.results()[1], SketchCircle_5.results()[1], SketchLine_29.result(), SketchArc_15.results()[1], SketchLine_35.result()],
                           [SketchCircle_5.results()[1], SketchCircle_5.results()[1], SketchCircle_5.results()[1], SketchCircle_6.results()[1]]
                          ])
model.do()

### Create Extrusion
Extrusion_4 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchLine_28r-SketchCircle_5_2r-SketchCircle_5_2r-SketchLine_29f-SketchArc_16_2r-SketchLine_35r")], model.selection(), 0, 10)

### Create Extrusion
Extrusion_5 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchCircle_5_2f-SketchCircle_5_2f-SketchCircle_5_2f-SketchCircle_6_2r")], model.selection(), 3, 13)

### Create Symmetry
Symmetry_1 = model.addSymmetry(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_4"), model.selection("COMPOUND", "all-in-Extrusion_5")], model.selection("FACE", "PartSet/YOZ"), keepOriginal = True, keepSubResults = True)

### Create Fuse
Fuse_1_objects_1 = [model.selection("COMPOUND", "all-in-Extrusion_1"),
                    model.selection("COMPOUND", "all-in-Extrusion_3"),
                    model.selection("COMPOUND", "all-in-ExtrusionCut_1"),
                    model.selection("COMPOUND", "all-in-Symmetry_1")]
Fuse_1 = model.addFuse(Part_1_doc, Fuse_1_objects_1, removeEdges = True, keepSubResults = True)

model.do()


### Create Part
Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()

### Create Sketch
Sketch_4 = model.addSketch(Part_2_doc, model.defaultPlane("XOY"))

### Create SketchLine
SketchLine_36 = Sketch_4.addLine(0, 62.99999999999999, 32, 63)
Sketch_4.setHorizontal(SketchLine_36.result())

### Create SketchLine
SketchLine_37 = Sketch_4.addLine(35, 66, 35, 87.99999999999999)
Sketch_4.setVertical(SketchLine_37.result())

### Create SketchLine
SketchLine_38 = Sketch_4.addLine(35, 87.99999999999999, 75.00000000000001, 87.99999999999999)
Sketch_4.setCoincident(SketchLine_37.endPoint(), SketchLine_38.startPoint())
Sketch_4.setHorizontal(SketchLine_38.result())

### Create SketchLine
SketchLine_39 = Sketch_4.addLine(75.00000000000001, 87.99999999999999, 75.00000000000001, 78)
Sketch_4.setCoincident(SketchLine_38.endPoint(), SketchLine_39.startPoint())
Sketch_4.setVertical(SketchLine_39.result())

### Create SketchArc
SketchArc_16 = Sketch_4.addArc(75.00000000000004, 71.99999999999689, 75.00000000000001, 78, 69, 71.99999999999613, False)
Sketch_4.setCoincident(SketchLine_39.endPoint(), SketchArc_16.startPoint())

### Create SketchLine
SketchLine_40 = Sketch_4.addLine(69, 71.99999999999613, 69, 52.00000000001324)
Sketch_4.setCoincident(SketchArc_16.endPoint(), SketchLine_40.startPoint())
Sketch_4.setVertical(SketchLine_40.result())

### Create SketchArc
SketchArc_17 = Sketch_4.addArc(57, 52.00000000001324, 69, 52.00000000001324, 57.0000000000007, 40, True)
Sketch_4.setCoincident(SketchLine_40.endPoint(), SketchArc_17.startPoint())
Sketch_4.setTangent(SketchLine_40.result(), SketchArc_17.results()[1])

### Create SketchArc
SketchArc_18 = Sketch_4.addArc(57, 27.99999999998618, 57.0000000000007, 40, 45, 27.99999999998618, False)
Sketch_4.setCoincident(SketchArc_17.endPoint(), SketchArc_18.startPoint())
Sketch_4.setTangent(SketchArc_17.results()[1], SketchArc_18.results()[1])

### Create SketchLine
SketchLine_41 = Sketch_4.addLine(45, 27.99999999998618, 45, 0)
Sketch_4.setCoincident(SketchArc_18.endPoint(), SketchLine_41.startPoint())

### Create SketchProjection
SketchProjection_9 = Sketch_4.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_42 = SketchProjection_9.createdFeature()
Sketch_4.setCoincident(SketchLine_41.endPoint(), SketchLine_42.result())
Sketch_4.setVertical(SketchLine_41.result())

### Create SketchLine
SketchLine_43 = Sketch_4.addLine(45, 0, 0, 0)
Sketch_4.setCoincident(SketchLine_41.endPoint(), SketchLine_43.startPoint())
Sketch_4.setCoincident(SketchAPI_Line(SketchLine_42).startPoint(), SketchLine_43.endPoint())

### Create SketchProjection
SketchProjection_10 = Sketch_4.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_44 = SketchProjection_10.createdFeature()
Sketch_4.setCoincident(SketchLine_36.startPoint(), SketchLine_44.result())
Sketch_4.setTangent(SketchArc_18.results()[1], SketchLine_41.result())
Sketch_4.setTangent(SketchArc_16.results()[1], SketchLine_40.result())
Sketch_4.setLength(SketchLine_43.result(), 45)
Sketch_4.setDistance(SketchLine_36.startPoint(), SketchLine_39.result(), "150/2", True)
Sketch_4.setDistance(SketchLine_38.result(), SketchArc_16.startPoint(), 10, True)
Sketch_4.setRadius(SketchArc_16.results()[1], 6)
Sketch_4.setRadius(SketchArc_17.results()[1], 12)
Sketch_4.setEqual(SketchArc_17.results()[1], SketchArc_18.results()[1])
Sketch_4.setDistance(SketchLine_42.result(), SketchArc_18.startPoint(), 40, True)
Sketch_4.setVerticalDistance(SketchArc_18.startPoint(), SketchArc_16.startPoint(), 38)
Sketch_4.setDistance(SketchLine_44.result(), SketchLine_40.startPoint(), "138/2", True)

### Create SketchLine
SketchLine_45 = Sketch_4.addLine(-75.00000000000001, 77.99999999999693, -50.99999999999996, 78)
SketchLine_45.setName("SketchLine_17")
SketchLine_45.result().setName("SketchLine_17")
Sketch_4.setHorizontal(SketchLine_45.result())

### Create SketchArc
SketchArc_19 = Sketch_4.addArc(-50.99999999999996, 72, -50.99999999999996, 78, -44.99999999999996, 72, True)
SketchArc_19.setName("SketchArc_7")
SketchArc_19.result().setName("SketchArc_7")
SketchArc_19.results()[1].setName("SketchArc_7_2")
Sketch_4.setCoincident(SketchLine_45.endPoint(), SketchArc_19.startPoint())
Sketch_4.setTangent(SketchLine_45.result(), SketchArc_19.results()[1])

### Create SketchArc
SketchArc_20 = Sketch_4.addArc(-28, 49.99999999999999, -28, 52.99999999999999, -25, 49.99999999999999, True)
SketchArc_20.setName("SketchArc_9")
SketchArc_20.result().setName("SketchArc_9")
SketchArc_20.results()[1].setName("SketchArc_9_2")

### Create SketchLine
SketchLine_46 = Sketch_4.addLine(-25, 49.99999999999999, -25, 27.99999999999997)
SketchLine_46.setName("SketchLine_18")
SketchLine_46.result().setName("SketchLine_18")
Sketch_4.setCoincident(SketchArc_20.endPoint(), SketchLine_46.startPoint())
Sketch_4.setVertical(SketchLine_46.result())

### Create SketchCircle
SketchCircle_7 = Sketch_4.addCircle(0, 27.99999999999997, 25)
Sketch_4.setCoincident(SketchLine_44.result(), SketchCircle_7.center())
Sketch_4.setCoincident(SketchLine_46.endPoint(), SketchCircle_7.results()[1])

### Create SketchCircle
SketchCircle_8 = Sketch_4.addCircle(0, 27.99999999999997, 16)
Sketch_4.setCoincident(SketchCircle_7.center(), SketchCircle_8.center())
Sketch_4.setRadius(SketchArc_19.results()[1], 6)
Sketch_4.setRadius(SketchArc_20.results()[1], 3)
Sketch_4.setTangent(SketchArc_20.results()[1], SketchLine_46.result())
Sketch_4.setRadius(SketchCircle_7.results()[1], 25)
Sketch_4.setRadius(SketchCircle_8.results()[1], 16)

### Create SketchLine
SketchLine_47 = Sketch_4.addLine(-44.99999999999996, 72, -44.99999999999998, 64.99999999999999)
SketchLine_47.setName("SketchLine_19")
SketchLine_47.result().setName("SketchLine_19")
Sketch_4.setCoincident(SketchArc_19.endPoint(), SketchLine_47.startPoint())
Sketch_4.setVertical(SketchLine_47.result())

### Create SketchLine
SketchLine_48 = Sketch_4.addLine(-28, 52.99999999999999, -32.99999999999998, 52.99999999999999)
SketchLine_48.setName("SketchLine_20")
SketchLine_48.result().setName("SketchLine_20")
Sketch_4.setCoincident(SketchArc_20.startPoint(), SketchLine_48.startPoint())
Sketch_4.setHorizontal(SketchLine_48.result())

### Create SketchArc
SketchArc_21 = Sketch_4.addArc(-32.99999999999998, 64.99999999999999, -44.99999999999998, 64.99999999999999, -32.99999999999998, 52.99999999999999, False)
SketchArc_21.setName("SketchArc_10")
SketchArc_21.result().setName("SketchArc_10")
SketchArc_21.results()[1].setName("SketchArc_10_2")
Sketch_4.setCoincident(SketchLine_47.endPoint(), SketchArc_21.startPoint())
Sketch_4.setTangent(SketchLine_47.result(), SketchArc_21.results()[1])
Sketch_4.setCoincident(SketchLine_48.result(), SketchArc_21.endPoint())
Sketch_4.setTangent(SketchArc_19.results()[1], SketchLine_47.result())
Sketch_4.setTangent(SketchArc_21.results()[1], SketchLine_48.result())
Sketch_4.setTangent(SketchLine_48.result(), SketchArc_20.results()[1])
Sketch_4.setRadius(SketchArc_21.results()[1], 12)
Sketch_4.setCoincident(SketchArc_21.endPoint(), SketchLine_48.endPoint())

### Create SketchLine
SketchLine_49 = Sketch_4.addLine(-25, 27.99999999999997, 0, 27.99999999999997)
SketchLine_49.setName("SketchLine_21")
SketchLine_49.result().setName("SketchLine_21")
SketchLine_49.setAuxiliary(True)
Sketch_4.setCoincident(SketchLine_46.endPoint(), SketchLine_49.startPoint())
Sketch_4.setCoincident(SketchCircle_7.center(), SketchLine_49.endPoint())
Sketch_4.setHorizontal(SketchLine_49.result())

### Create SketchArc
SketchArc_22 = Sketch_4.addArc(32, 66, 32, 63, 35, 66, False)
SketchArc_22.setName("SketchArc_11")
SketchArc_22.result().setName("SketchArc_11")
SketchArc_22.results()[1].setName("SketchArc_11_2")

### Create SketchConstraintMirror
SketchConstraintMirror_4_objects = [SketchLine_39.result(), SketchLine_40.result(), SketchLine_36.result(), SketchLine_38.result(), SketchLine_37.result(), SketchLine_41.result(), SketchLine_43.result(), SketchArc_16.results()[1], SketchArc_18.results()[1], SketchArc_17.results()[1], SketchArc_22.results()[1]]
SketchConstraintMirror_4 = Sketch_4.addMirror(SketchLine_44.result(), SketchConstraintMirror_4_objects)
[SketchLine_50, SketchLine_51, SketchLine_52, SketchLine_53, SketchLine_54, SketchLine_55, SketchLine_56, SketchArc_23, SketchArc_24, SketchArc_25, SketchArc_26] = SketchConstraintMirror_4.mirrored()
SketchArc_26.setName("SketchArc_12")
SketchArc_26.result().setName("SketchArc_12")
SketchArc_26.results()[1].setName("SketchArc_12_2")
SketchArc_25.setName("SketchArc_6")
SketchArc_25.result().setName("SketchArc_6")
SketchArc_25.results()[1].setName("SketchArc_6_2")
SketchArc_24.setName("SketchArc_5")
SketchArc_24.result().setName("SketchArc_5")
SketchArc_24.results()[1].setName("SketchArc_5_2")
SketchArc_23.setName("SketchArc_4")
SketchArc_23.result().setName("SketchArc_4")
SketchArc_23.results()[1].setName("SketchArc_4_2")
SketchLine_56.setName("SketchLine_16")
SketchLine_56.result().setName("SketchLine_16")
SketchLine_55.setName("SketchLine_15")
SketchLine_55.result().setName("SketchLine_15")
SketchLine_54.setName("SketchLine_14")
SketchLine_54.result().setName("SketchLine_14")
SketchLine_53.setName("SketchLine_13")
SketchLine_53.result().setName("SketchLine_13")
SketchLine_52.setName("SketchLine_12")
SketchLine_52.result().setName("SketchLine_12")
SketchLine_51.setName("SketchLine_11")
SketchLine_51.result().setName("SketchLine_11")
SketchLine_50.setName("SketchLine_10")
SketchLine_50.result().setName("SketchLine_10")
Sketch_4.setCoincident(SketchAPI_Line(SketchLine_50).endPoint(), SketchLine_45.startPoint())
Sketch_4.setDistance(SketchLine_47.result(), SketchAPI_Line(SketchLine_52).startPoint(), 45, True)
Sketch_4.setDistance(SketchLine_53.result(), SketchLine_49.endPoint(), 60, True)
Sketch_4.setDistance(SketchLine_52.result(), SketchLine_48.startPoint(), 10, True)

### Create SketchPoint
SketchPoint_3 = Sketch_4.addPoint(35, 62.99999999999999)
SketchPoint_3.setAuxiliary(True)
Sketch_4.setCoincident(SketchPoint_3.coordinates(), SketchLine_36.result())
Sketch_4.setCoincident(SketchPoint_3.coordinates(), SketchLine_37.result())
Sketch_4.setDistance(SketchLine_36.startPoint(), SketchPoint_3.coordinates(), 35, False)
Sketch_4.setDistance(SketchPoint_3.coordinates(), SketchLine_37.endPoint(), 25, False)
Sketch_4.setCoincident(SketchArc_22.startPoint(), SketchLine_36.endPoint())
Sketch_4.setCoincident(SketchArc_22.endPoint(), SketchLine_37.startPoint())
Sketch_4.setTangent(SketchArc_22.results()[1], SketchLine_36.result())
Sketch_4.setTangent(SketchArc_22.results()[1], SketchLine_37.result())
Sketch_4.setRadius(SketchArc_22.results()[1], 3)

### Create SketchConstraintMirror
SketchConstraintMirror_5_objects = [SketchLine_45.result(), SketchArc_19.results()[1], SketchLine_47.result(), SketchArc_21.results()[1], SketchLine_48.result(), SketchArc_20.results()[1], SketchLine_46.result()]
SketchConstraintMirror_5 = Sketch_4.addMirror(SketchLine_44.result(), SketchConstraintMirror_5_objects)
[SketchLine_57, SketchArc_27, SketchLine_58, SketchArc_28, SketchLine_59, SketchArc_29, SketchLine_60] = SketchConstraintMirror_5.mirrored()
SketchArc_29.setName("SketchArc_15")
SketchArc_29.result().setName("SketchArc_15")
SketchArc_29.results()[1].setName("SketchArc_15_2")
SketchArc_28.setName("SketchArc_14")
SketchArc_28.result().setName("SketchArc_14")
SketchArc_28.results()[1].setName("SketchArc_14_2")
SketchArc_27.setName("SketchArc_13")
SketchArc_27.result().setName("SketchArc_13")
SketchArc_27.results()[1].setName("SketchArc_13_2")
model.do()

### Create Extrusion
Extrusion_6 = model.addExtrusion(Part_2_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_22f-SketchArc_13_2f-SketchLine_23f-SketchArc_14_2r-SketchLine_24r-SketchArc_15_2f-SketchLine_25f-SketchCircle_1_2r-SketchLine_18r-SketchArc_9_2f-SketchLine_20f-SketchArc_10_2r-SketchLine_19r-SketchArc_7_2f-SketchLine_17r-SketchArc_4_2r-SketchLine_11f-SketchArc_6_2f-SketchArc_5_2r-SketchLine_15f-SketchLine_16f-SketchLine_8r-SketchLine_6r-SketchArc_3_2r-SketchArc_2_2f-SketchLine_5r-SketchArc_1_2r")], model.selection(), 10, 0)

### Create Extrusion
Extrusion_7 = model.addExtrusion(Part_2_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_12f-SketchArc_12_2r-SketchLine_14f-SketchLine_13f-SketchLine_10f-SketchLine_17f-SketchArc_7_2r-SketchLine_19f-SketchArc_10_2f-SketchLine_20r-SketchArc_9_2r-SketchLine_18f-SketchCircle_1_2r-SketchLine_25r-SketchArc_15_2r-SketchLine_24f-SketchArc_14_2f-SketchLine_23r-SketchArc_13_2r-SketchLine_22r-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchArc_11_2r-SketchLine_1r")], model.selection(), 35, 0)

### Create Extrusion
Extrusion_8 = model.addExtrusion(Part_2_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f-SketchCircle_1_2f-SketchCircle_2_2r")], model.selection(), 45, 0)

### Create Sketch
Sketch_5 = model.addSketch(Part_2_doc, model.selection("FACE", "Extrusion_2_1/Generated_Face&Sketch_1/SketchLine_22"))

### Create SketchCircle
SketchCircle_9 = Sketch_5.addCircle(63, 23, 5)
Sketch_5.setRadius(SketchCircle_9.results()[1], 5)

### Create SketchProjection
SketchProjection_11 = Sketch_5.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_61 = SketchProjection_11.createdFeature()
Sketch_5.setDistance(SketchCircle_9.center(), SketchLine_61.result(), "126/2", True)

### Create SketchProjection
SketchProjection_12 = Sketch_5.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_62 = SketchProjection_12.createdFeature()
Sketch_5.setDistance(SketchCircle_9.center(), SketchLine_62.result(), 23, True)

### Create SketchConstraintMirror
SketchConstraintMirror_6 = Sketch_5.addMirror(SketchLine_61.result(), [SketchCircle_9.results()[1]])
[SketchCircle_10] = SketchConstraintMirror_6.mirrored()
model.do()

### Create ExtrusionCut
ExtrusionCut_2 = model.addExtrusionCut(Part_2_doc, [model.selection("COMPOUND", "all-in-Sketch_2")], model.selection(), 0, 10, [model.selection("SOLID", "Extrusion_2_1")])

### Create Sketch
Sketch_6 = model.addSketch(Part_2_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6"))

### Create SketchLine
SketchLine_63 = Sketch_6.addLine(0, 0, 9, -55.99999999999999)

### Create SketchCircle
SketchCircle_11 = Sketch_6.addCircle(28, -56, 19)
Sketch_6.setCoincident(SketchLine_63.endPoint(), SketchCircle_11.results()[1])

### Create SketchCircle
SketchCircle_12 = Sketch_6.addCircle(28, -56, 10)
Sketch_6.setCoincident(SketchCircle_11.center(), SketchCircle_12.center())

### Create SketchLine
SketchLine_64 = Sketch_6.addLine(47, -55.99999999999999, 56.26584765647708, -18.93660937409168)
Sketch_6.setCoincident(SketchLine_64.startPoint(), SketchCircle_11.results()[1])
Sketch_6.setRadius(SketchCircle_11.results()[1], "38/2")
Sketch_6.setRadius(SketchCircle_12.results()[1], 10)

### Create SketchProjection
SketchProjection_13 = Sketch_6.addProjection(model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_8][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/From_Face]"), False)
SketchPoint_4 = SketchProjection_13.createdFeature()
Sketch_6.setCoincident(SketchLine_63.startPoint(), SketchAPI_Point(SketchPoint_4).coordinates())

### Create SketchProjection
SketchProjection_14 = Sketch_6.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/From_Face]"), False)
SketchLine_65 = SketchProjection_14.createdFeature()
Sketch_6.setDistance(SketchCircle_11.center(), SketchLine_65.result(), 56, True)

### Create SketchLine
SketchLine_66 = Sketch_6.addLine(9, -55.99999999999999, 47, -55.99999999999999)
SketchLine_66.setAuxiliary(True)
Sketch_6.setCoincident(SketchLine_66.startPoint(), SketchCircle_11.results()[1])
Sketch_6.setCoincident(SketchLine_66.endPoint(), SketchCircle_11.results()[1])
Sketch_6.setHorizontal(SketchLine_66.result())
Sketch_6.setCoincident(SketchLine_66.result(), SketchCircle_11.center())
Sketch_6.setCoincident(SketchLine_63.endPoint(), SketchLine_66.startPoint())
Sketch_6.setCoincident(SketchLine_64.startPoint(), SketchLine_66.endPoint())

### Create SketchProjection
SketchProjection_15 = Sketch_6.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_8][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6]"), False)
SketchLine_67 = SketchProjection_15.createdFeature()
Sketch_6.setDistance(SketchCircle_12.center(), SketchLine_67.result(), 28, True)

### Create SketchLine
SketchLine_68 = Sketch_6.addLine(56.26584765647708, -18.93660937409168, 61, 0)
SketchLine_68.setAuxiliary(True)
Sketch_6.setCoincident(SketchLine_64.endPoint(), SketchLine_68.startPoint())
Sketch_6.setCoincident(SketchLine_68.endPoint(), SketchLine_65.result())
Sketch_6.setCollinear(SketchLine_68.result(), SketchLine_64.result())
Sketch_6.setHorizontalDistance(SketchCircle_11.center(), SketchLine_68.endPoint(), 33)

### Create SketchProjection
SketchProjection_16 = Sketch_6.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_23][Extrusion_1_1/From_Face]"), False)
SketchLine_69 = SketchProjection_16.createdFeature()

### Create SketchArc
SketchArc_30 = Sketch_6.addArc(80.51941016011038, -25, 56.26584765647708, -18.93660937409168, 80.519344153399, 0, True)
SketchArc_30.setName("SketchArc_16")
SketchArc_30.result().setName("SketchArc_16")
SketchArc_30.results()[1].setName("SketchArc_16_2")
Sketch_6.setCoincident(SketchLine_64.endPoint(), SketchArc_30.startPoint())
Sketch_6.setTangent(SketchLine_64.result(), SketchArc_30.results()[1])
Sketch_6.setCoincident(SketchLine_69.result(), SketchArc_30.endPoint())
Sketch_6.setTangent(SketchLine_69.result(), SketchArc_30.results()[1])
Sketch_6.setRadius(SketchArc_30.results()[1], 25)

### Create SketchLine
SketchLine_70 = Sketch_6.addLine(0, 0, 80.519344153399, 0)
Sketch_6.setCoincident(SketchLine_63.startPoint(), SketchLine_70.startPoint())
Sketch_6.setCoincident(SketchArc_30.endPoint(), SketchLine_70.endPoint())
model.do()
Sketch_6.changeFacesOrder([[SketchCircle_12.results()[1]],
                           [SketchLine_63.result(), SketchCircle_11.results()[1], SketchCircle_11.results()[1], SketchLine_64.result(), SketchArc_30.results()[1], SketchLine_70.result()],
                           [SketchCircle_11.results()[1], SketchCircle_11.results()[1], SketchCircle_11.results()[1], SketchCircle_12.results()[1]]
                          ])
model.do()

### Create Extrusion
Extrusion_9 = model.addExtrusion(Part_2_doc, [model.selection("FACE", "Sketch_3/Face-SketchLine_28r-SketchCircle_5_2r-SketchCircle_5_2r-SketchLine_29f-SketchArc_16_2r-SketchLine_35r")], model.selection(), 0, 10)

### Create Extrusion
Extrusion_10 = model.addExtrusion(Part_2_doc, [model.selection("FACE", "Sketch_3/Face-SketchCircle_5_2f-SketchCircle_5_2f-SketchCircle_5_2f-SketchCircle_6_2r")], model.selection(), 3, 13)

### Create Symmetry
Symmetry_2 = model.addSymmetry(Part_2_doc, [model.selection("COMPOUND", "all-in-Extrusion_4"), model.selection("COMPOUND", "all-in-Extrusion_5")], model.selection("FACE", "PartSet/YOZ"), keepOriginal = True, keepSubResults = True)

### Create Fuse
Fuse_2_objects_1 = [model.selection("SOLID", "Extrusion_1_1"),
                    model.selection("SOLID", "Extrusion_3_1"),
                    model.selection("SOLID", "ExtrusionCut_1_1"),
                    model.selection("COMPOUND", "Symmetry_1_1"),
                    model.selection("COMPOUND", "Symmetry_1_2")]
Fuse_2 = model.addFuse(Part_2_doc, Fuse_2_objects_1, removeEdges = True, keepSubResults = True)

model.do()

model.end()


model.checkResult(Fuse_1, model, 1, [0], [1], [58], [326], [652])
model.testResultsVolumes(Fuse_1, [244644.913443274])
model.checkResult(Fuse_2, model, 1, [0], [1], [58], [326], [652])
model.testResultsVolumes(Fuse_2, [244644.913443274])

assert(model.checkPythonDump())
