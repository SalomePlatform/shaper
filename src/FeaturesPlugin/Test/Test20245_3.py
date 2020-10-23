# Copyright (C) 2020  CEA/DEN, EDF R&D
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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(6.188000000000001, 9.028, 6.188000000000001, -3.45)

### Create SketchLine
SketchLine_2 = Sketch_1.addLine(6.188000000000001, -3.45, 0, -3.45)
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(0, -3.45, 0, -6.45)
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())

### Create SketchLine
SketchLine_4 = Sketch_1.addLine(0, -6.45, 7.781999999999999, -6.45)
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())

### Create SketchLine
SketchLine_5 = Sketch_1.addLine(7.781999999999999, -6.45, 7.781999999999999, -3.45)
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())

### Create SketchLine
SketchLine_6 = Sketch_1.addLine(7.781999999999999, -3.45, 6.538000000000001, -3.45)
Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())

### Create SketchLine
SketchLine_7 = Sketch_1.addLine(6.538000000000001, -3.45, 6.538000000000002, -2.4)
Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())

### Create SketchLine
SketchLine_8 = Sketch_1.addLine(6.538000000000002, -2.4, 6.376, -2.25)
Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())

### Create SketchLine
SketchLine_9 = Sketch_1.addLine(6.376, -2.25, 6.375999999999999, 9.028)
Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())

### Create SketchLine
SketchLine_10 = Sketch_1.addLine(6.375999999999999, 9.028, 6.188000000000001, 9.028)
SketchLine_10.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_10.endPoint())
Sketch_1.setHorizontal(SketchLine_10.result())
Sketch_1.setVertical(SketchLine_1.result())
Sketch_1.setVertical(SketchLine_9.result())
Sketch_1.setVertical(SketchLine_3.result())
Sketch_1.setVertical(SketchLine_5.result())
Sketch_1.setHorizontal(SketchLine_2.result())
Sketch_1.setHorizontal(SketchLine_6.result())
Sketch_1.setHorizontal(SketchLine_4.result())
Sketch_1.setCoincident(SketchLine_7.startPoint(), SketchLine_2.result())

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_11 = SketchProjection_1.createdFeature()
Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchLine_11.result())
Sketch_1.setLength(SketchLine_5.result(), 3)
Sketch_1.setDistance(SketchAPI_Line(SketchLine_11).startPoint(), SketchLine_1.result(), 6.188, True)
Sketch_1.setVertical(SketchLine_7.result())

### Create SketchProjection
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_12 = SketchProjection_2.createdFeature()
Sketch_1.setDistance(SketchLine_9.endPoint(), SketchLine_12.result(), 9.028, True)
Sketch_1.setDistance(SketchLine_2.startPoint(), SketchLine_9.result(), 0.188, True)
Sketch_1.setDistance(SketchLine_2.startPoint(), SketchLine_7.result(), 0.35, True)
Sketch_1.setVerticalDistance(SketchLine_7.endPoint(), SketchLine_8.endPoint(), 0.15)
Sketch_1.setDistance(SketchLine_5.endPoint(), SketchLine_12.result(), 3.45, True)
Sketch_1.setDistance(SketchLine_8.endPoint(), SketchLine_12.result(), 2.25, True)

### Create SketchLine
SketchLine_13 = Sketch_1.addLine(7.031999999999999, -4.05, 5.531999999999999, -4.05)

### Create SketchLine
SketchLine_14 = Sketch_1.addLine(5.531999999999999, -4.05, 5.531999999999999, -5.850000000000001)

### Create SketchLine
SketchLine_15 = Sketch_1.addLine(5.531999999999999, -5.850000000000001, 7.031999999999999, -5.850000000000001)

### Create SketchLine
SketchLine_16 = Sketch_1.addLine(7.031999999999999, -5.850000000000001, 7.031999999999999, -4.05)
Sketch_1.setCoincident(SketchLine_16.endPoint(), SketchLine_13.startPoint())
Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
Sketch_1.setHorizontal(SketchLine_13.result())
Sketch_1.setVertical(SketchLine_14.result())
Sketch_1.setHorizontal(SketchLine_15.result())
Sketch_1.setVertical(SketchLine_16.result())
Sketch_1.setLength(SketchLine_14.result(), 1.8)
Sketch_1.setLength(SketchLine_15.result(), 1.5)

### Create SketchLine
SketchLine_17 = Sketch_1.addLine(6.281999999999999, -4.050000000000001, 6.281999999999999, 9.028000000000002)
SketchLine_17.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_17.startPoint(), SketchLine_13.result())
Sketch_1.setCoincident(SketchLine_17.endPoint(), SketchLine_10.result())
Sketch_1.setVertical(SketchLine_17.result())
Sketch_1.setMiddlePoint(SketchLine_17.startPoint(), SketchLine_13.result())
Sketch_1.setDistance(SketchLine_17.startPoint(), SketchLine_5.result(), 1.5, True)
Sketch_1.setMiddlePoint(SketchLine_10.result(), SketchLine_17.endPoint())
Sketch_1.setDistance(SketchLine_14.startPoint(), SketchLine_2.result(), 0.6, True)

### Create SketchLine
SketchLine_18 = Sketch_1.addLine(6.375999999999999, 9.028, 6.534999999999997, 9.186999999999999)
Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_18.startPoint())

### Create SketchLine
SketchLine_19 = Sketch_1.addLine(6.534999999999997, 9.186999999999999, 6.534999999999997, 10.256)
Sketch_1.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())

### Create SketchLine
SketchLine_20 = Sketch_1.addLine(6.534999999999997, 10.256, 6.184999999999998, 10.256)
Sketch_1.setCoincident(SketchLine_19.endPoint(), SketchLine_20.startPoint())

### Create SketchLine
SketchLine_21 = Sketch_1.addLine(6.184999999999998, 10.256, 4.917500000000027, 10.94235984621998)
Sketch_1.setCoincident(SketchLine_20.endPoint(), SketchLine_21.startPoint())
Sketch_1.setHorizontal(SketchLine_20.result())
Sketch_1.setVertical(SketchLine_19.result())

### Create SketchConstraintAngle
Sketch_1.setAngle(SketchLine_10.result(), SketchLine_18.result(), 45, type = "Supplementary")
Sketch_1.setLength(SketchLine_20.result(), 0.35)
Sketch_1.setDistance(SketchLine_19.endPoint(), SketchLine_12.result(), 10.256, True)

### Create SketchArc
SketchArc_1 = Sketch_1.addArc(0, 2.425, 4.917500000000027, 10.94235984621998, 0, 12.26, False)
Sketch_1.setCoincident(SketchLine_11.result(), SketchArc_1.center())
Sketch_1.setCoincident(SketchLine_21.endPoint(), SketchArc_1.startPoint())
Sketch_1.setCoincident(SketchLine_11.result(), SketchArc_1.endPoint())

### Create SketchLine
SketchLine_22 = Sketch_1.addLine(6.188000000000001, 9.028, 6.082, 9.532)
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_22.startPoint())

### Create SketchArc
SketchArc_2 = Sketch_1.addArc(4.79378612024245, 9.263, 6.082, 9.532, 5.676067550792229, 10.23944020672391, False)
Sketch_1.setCoincident(SketchLine_22.endPoint(), SketchArc_2.startPoint())
Sketch_1.setRadius(SketchArc_2.results()[1], 1.316)
Sketch_1.setDistance(SketchArc_2.startPoint(), SketchLine_1.result(), 0.106, True)

### Create SketchLine
SketchLine_23 = Sketch_1.addLine(5.676067550792229, 10.23944020672391, 5.405090045827156, 10.43837553323928)
Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_23.startPoint())

### Create SketchLine
SketchLine_24 = Sketch_1.addLine(5.405090045827156, 10.43837553323928, 5.126644475052085, 10.62934617154252)
Sketch_1.setCoincident(SketchLine_23.endPoint(), SketchLine_24.startPoint())

### Create SketchLine
SketchLine_25 = Sketch_1.addLine(5.126644475052085, 10.62934617154252, 4.83550000000003, 10.80033167999934)
Sketch_1.setCoincident(SketchLine_24.endPoint(), SketchLine_25.startPoint())

### Create SketchArc
SketchArc_3 = Sketch_1.addArc(0, 2.425, 4.83550000000003, 10.80033167999934, 0, 12.096, False)
Sketch_1.setCoincident(SketchLine_11.result(), SketchArc_3.center())
Sketch_1.setCoincident(SketchLine_25.endPoint(), SketchArc_3.startPoint())
Sketch_1.setCoincident(SketchLine_11.result(), SketchArc_3.endPoint())

### Create SketchLine
SketchLine_26 = Sketch_1.addLine(0, 12.096, 0, 12.26)
Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_26.startPoint())
Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_26.endPoint())
Sketch_1.setDistance(SketchArc_1.center(), SketchLine_12.result(), 2.425, True)
Sketch_1.setCoincident(SketchArc_3.center(), SketchArc_1.center())
Sketch_1.setLength(SketchLine_26.result(), 0.164)

### Create SketchLine
SketchLine_27 = Sketch_1.addLine(0, 2.425, 4.917500000000027, 10.94235984621998)
SketchLine_27.setAuxiliary(True)
Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_27.startPoint())
Sketch_1.setCoincident(SketchLine_21.endPoint(), SketchLine_27.endPoint())
Sketch_1.setCoincident(SketchArc_3.startPoint(), SketchLine_27.result())

### Create SketchLine
SketchLine_28 = Sketch_1.addLine(0, 2.425, 5.21991026555713, 10.77860263646605)
SketchLine_28.setAuxiliary(True)
Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_28.startPoint())
Sketch_1.setCoincident(SketchLine_28.endPoint(), SketchLine_21.result())
Sketch_1.setCoincident(SketchLine_28.result(), SketchLine_25.startPoint())

### Create SketchLine
SketchLine_29 = Sketch_1.addLine(0, 2.425, 5.523638941362955, 10.61413149862094)
SketchLine_29.setAuxiliary(True)
Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_29.startPoint())
Sketch_1.setCoincident(SketchLine_29.endPoint(), SketchLine_21.result())
Sketch_1.setCoincident(SketchLine_29.result(), SketchLine_24.startPoint())
Sketch_1.setDistance(SketchLine_25.startPoint(), SketchLine_28.endPoint(), 0.176, True)
Sketch_1.setDistance(SketchLine_24.startPoint(), SketchLine_29.endPoint(), 0.212, True)

### Create SketchConstraintAngle
Sketch_1.setAngle(SketchLine_28.result(), SketchLine_11.result(), 32, type = "Direct")

### Create SketchConstraintAngle
Sketch_1.setAngle(SketchLine_29.result(), SketchLine_11.result(), 34, type = "Direct")

### Create SketchConstraintAngle
Sketch_1.setAngle(SketchLine_27.result(), SketchLine_11.result(), 30, type = "Direct")

### Create SketchLine
SketchLine_30 = Sketch_1.addLine(4.79378612024245, 9.263, 5.676067550792229, 10.23944020672391)
SketchLine_30.setAuxiliary(True)
Sketch_1.setCoincident(SketchArc_2.center(), SketchLine_30.startPoint())
Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_30.endPoint())

### Create SketchLine
SketchLine_31 = Sketch_1.addLine(4.79378612024245, 9.263, 4.79378612024245, 10.72808112087839)
SketchLine_31.setAuxiliary(True)
Sketch_1.setCoincident(SketchArc_2.center(), SketchLine_31.startPoint())
Sketch_1.setVertical(SketchLine_31.result())

### Create SketchConstraintAngle
Sketch_1.setAngle(SketchLine_30.result(), SketchLine_31.result(), 42.1, type = "Direct")
Sketch_1.setDistance(SketchArc_2.center(), SketchLine_12.result(), 9.263, True)
Sketch_1.setCoincident(SketchLine_31.endPoint(), SketchLine_27.result())
Sketch_1.setDistance(SketchLine_22.endPoint(), SketchLine_12.result(), 9.532, True)
Sketch_1.setDistance(SketchLine_18.endPoint(), SketchLine_12.result(), 9.186999999999999, True)
Sketch_1.setRadius(SketchArc_1.results()[1], 9.835000000000001)
model.do()

### Create Revolution
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f-SketchLine_5f-SketchLine_6f-SketchLine_7f-SketchLine_8f-SketchLine_9f-SketchLine_18f-SketchLine_19f-SketchLine_20f-SketchLine_21f-SketchArc_1_2f-SketchLine_26r-SketchArc_3_2r-SketchLine_25r-SketchLine_24r-SketchLine_23r-SketchArc_2_2r-SketchLine_22r-SketchLine_16r-SketchLine_15r-SketchLine_14r-SketchLine_13r")], model.selection("EDGE", "PartSet/OZ"), 360, 0)

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))

### Create SketchProjection
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_29_StartVertex"), False)
SketchPoint_1 = SketchProjection_3.createdFeature()

### Create SketchProjection
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_26"), False)
SketchLine_32 = SketchProjection_4.createdFeature()

### Create SketchArc
SketchArc_4 = Sketch_2.addArc(0, 2.425, 0, 12.178, 5.453808387550563, 10.51060344512907, True)
SketchArc_4.setAuxiliary(True)
Sketch_2.setCoincident(SketchPoint_1.result(), SketchArc_4.center())
Sketch_2.setCoincident(SketchLine_32.result(), SketchArc_4.startPoint())

### Create SketchProjection
SketchProjection_5 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_29"), False)
SketchLine_33 = SketchProjection_5.createdFeature()
Sketch_2.setCoincident(SketchArc_4.endPoint(), SketchLine_33.result())
Sketch_2.setMiddlePoint(SketchArc_4.startPoint(), SketchLine_32.result())

### Create SketchLine
SketchLine_34 = Sketch_2.addLine(0, 2.425, 3.282538500974084, 11.74164236673399)
SketchLine_34.setAuxiliary(True)
Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_34.startPoint())

### Create SketchLine
SketchLine_35 = Sketch_2.addLine(3.282538500974084, 11.74164236673399, 3.199461499025965, 11.50584963625379)
Sketch_2.setCoincident(SketchLine_34.endPoint(), SketchLine_35.startPoint())
Sketch_2.setCoincident(SketchLine_35.endPoint(), SketchLine_34.result())
Sketch_2.setLength(SketchLine_35.result(), 0.25)

### Create SketchPoint
SketchPoint_2 = Sketch_2.addPoint(3.241, 11.6237460014939)
Sketch_2.setCoincident(SketchPoint_2.coordinates(), SketchArc_4.results()[1])
Sketch_2.setCoincident(SketchPoint_2.coordinates(), SketchLine_35.result())

### Create SketchProjection
SketchProjection_6 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_36 = SketchProjection_6.createdFeature()
Sketch_2.setDistance(SketchPoint_2.coordinates(), SketchLine_36.result(), 3.241, True)

### Create SketchArc
SketchArc_5 = Sketch_2.addArc(0, 2.425, 3.282538500974084, 11.74164236673399, 4.1398177019328, 11.393656164374, True)
Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchArc_5.center())
Sketch_2.setCoincident(SketchLine_34.endPoint(), SketchArc_5.startPoint())

### Create SketchArc
SketchArc_6 = Sketch_2.addArc(0, 2.425, 3.199461499025965, 11.50584963625379, 4.035044020470642, 11.16667053559353, True)
Sketch_2.setCoincident(SketchAPI_Line(SketchLine_33).startPoint(), SketchArc_6.center())
Sketch_2.setCoincident(SketchLine_35.endPoint(), SketchArc_6.startPoint())

### Create SketchLine
SketchLine_37 = Sketch_2.addLine(0, 2.425, 3.911506014428326, 11.49555702253677)
SketchLine_37.setAuxiliary(True)
Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_37.startPoint())
Sketch_2.setCoincident(SketchLine_37.endPoint(), SketchArc_5.results()[1])

### Create SketchLine
SketchLine_38 = Sketch_2.addLine(0, 2.425, 4.1398177019328, 11.393656164374)
SketchLine_38.setAuxiliary(True)
Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_38.startPoint())
Sketch_2.setCoincident(SketchArc_5.endPoint(), SketchLine_38.endPoint())
Sketch_2.setCoincident(SketchArc_6.endPoint(), SketchLine_38.result())
Sketch_2.setDistance(SketchLine_35.startPoint(), SketchLine_37.result(), 0.675, True)
Sketch_2.setMiddlePoint(SketchPoint_2.coordinates(), SketchLine_35.result())

### Create SketchLine
SketchLine_39 = Sketch_2.addLine(3.911506014428326, 11.49555702253677, 3.81251062025875, 11.26599240868435)
Sketch_2.setCoincident(SketchLine_37.endPoint(), SketchLine_39.startPoint())
Sketch_2.setCoincident(SketchLine_39.endPoint(), SketchArc_6.results()[1])
Sketch_2.setCoincident(SketchLine_39.endPoint(), SketchLine_37.result())

### Create SketchLine
SketchLine_40 = Sketch_2.addLine(4.1398177019328, 11.393656164374, 4.459277785066647, 11.19096067956113)
Sketch_2.setCoincident(SketchArc_5.endPoint(), SketchLine_40.startPoint())

### Create SketchProjection
SketchProjection_7 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchArc_1_2"), False)
SketchArc_7 = SketchProjection_7.createdFeature()
Sketch_2.setCoincident(SketchLine_40.endPoint(), SketchArc_7.results()[1])

### Create SketchLine
SketchLine_41 = Sketch_2.addLine(4.459277785066647, 11.19096067956113, 4.390495384356095, 11.04194790978503)
Sketch_2.setCoincident(SketchLine_40.endPoint(), SketchLine_41.startPoint())

### Create SketchProjection
SketchProjection_8 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchArc_3_2"), False)
SketchArc_8 = SketchProjection_8.createdFeature()
Sketch_2.setCoincident(SketchLine_41.endPoint(), SketchArc_8.results()[1])

### Create SketchLine
SketchLine_42 = Sketch_2.addLine(4.390495384356095, 11.04194790978503, 4.035044020470642, 11.16667053559353)
Sketch_2.setCoincident(SketchLine_41.endPoint(), SketchLine_42.startPoint())
Sketch_2.setCoincident(SketchLine_42.endPoint(), SketchLine_38.result())
Sketch_2.setCoincident(SketchLine_42.endPoint(), SketchArc_6.endPoint())
Sketch_2.setDistance(SketchArc_5.endPoint(), SketchLine_41.result(), 0.375, True)
Sketch_2.setDistance(SketchLine_39.startPoint(), SketchLine_38.result(), 0.25, True)
Sketch_2.setParallel(SketchLine_41.result(), SketchLine_38.result())
model.do()

### Create Revolution
Revolution_2 = model.addRevolution(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchLine_35f-SketchArc_6_2r-SketchLine_39r-SketchArc_5_2f_wire")], model.selection("EDGE", "Sketch_2/SketchLine_35"), 360, 0)

### Create Revolution
Revolution_3 = model.addRevolution(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchArc_5_2f-SketchLine_39f-SketchArc_6_2r-SketchLine_42r-SketchLine_41r-SketchLine_40r_wire")], model.selection("EDGE", "Sketch_2/SketchLine_35"), 360, 0)

### Create Symmetry
Symmetry_1 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Revolution_2_1"), model.selection("SOLID", "Revolution_3_1")], model.selection("FACE", "PartSet/YOZ"), keepOriginal = True)

### Create Cut
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Revolution_1_1")], [model.selection("COMPOUND", "Symmetry_1_1")])

### Create Fuse
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Cut_1_1"), model.selection("COMPOUND", "Symmetry_1_2")], removeEdges = True)

### Create Plane
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), 3.6, False)

### Create Sketch
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))

### Create SketchProjection
SketchProjection_9 = Sketch_3.addProjection(model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_1][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_2]"), True)
SketchCircle_1 = SketchProjection_9.createdFeature()

### Create SketchProjection
SketchProjection_10 = Sketch_3.addProjection(model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_9][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_8]"), True)
SketchCircle_2 = SketchProjection_10.createdFeature()

### Create SketchArc
SketchArc_9 = Sketch_3.addArc(0, 0, 6.094, 0, 5.859569950090201, -1.674, True)
Sketch_3.setCoincident(SketchAPI_Circle(SketchCircle_1).center(), SketchArc_9.center())

### Create SketchProjection
SketchProjection_11 = Sketch_3.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_43 = SketchProjection_11.createdFeature()
Sketch_3.setCoincident(SketchArc_9.startPoint(), SketchLine_43.result())

### Create SketchPoint
SketchPoint_3 = Sketch_3.addPoint(6.188000000000001, 0)
Sketch_3.setCoincident(SketchPoint_3.coordinates(), SketchLine_43.result())
Sketch_3.setCoincident(SketchPoint_3.coordinates(), SketchCircle_1.results()[1])
Sketch_3.setDistance(SketchArc_9.startPoint(), SketchPoint_3.coordinates(), 0.094, True)
Sketch_3.setDistance(SketchArc_9.endPoint(), SketchLine_43.result(), 1.674, True)

### Create SketchLine
SketchLine_44 = Sketch_3.addLine(5.859569950090201, -1.674, 5.870724657144867, -1.956000000000001)
Sketch_3.setCoincident(SketchArc_9.endPoint(), SketchLine_44.startPoint())
Sketch_3.setCoincident(SketchLine_44.endPoint(), SketchCircle_1.results()[1])

### Create SketchArc
SketchArc_10 = Sketch_3.addArc(0, 0, 6.470000000000001, 0, 6.167249305809295, -1.955999999999993, True)
Sketch_3.setCoincident(SketchAPI_Circle(SketchCircle_1).center(), SketchArc_10.center())
Sketch_3.setCoincident(SketchLine_43.result(), SketchArc_10.startPoint())

### Create SketchLine
SketchLine_45 = Sketch_3.addLine(6.167249305809295, -1.955999999999993, 6.068561608816571, -1.956000000000005)
Sketch_3.setCoincident(SketchArc_10.endPoint(), SketchLine_45.startPoint())
Sketch_3.setCoincident(SketchLine_45.endPoint(), SketchCircle_2.results()[1])
Sketch_3.setHorizontal(SketchLine_45.result())
Sketch_3.setCoincident(SketchLine_44.endPoint(), SketchLine_45.result())
Sketch_3.setDistance(SketchArc_10.startPoint(), SketchLine_45.result(), 1.956, True)

### Create SketchPoint
SketchPoint_4 = Sketch_3.addPoint(6.376, 0)
Sketch_3.setCoincident(SketchPoint_4.coordinates(), SketchCircle_2.results()[1])
Sketch_3.setCoincident(SketchPoint_4.coordinates(), SketchLine_43.result())
Sketch_3.setDistance(SketchPoint_4.coordinates(), SketchArc_10.startPoint(), 0.094, True)

### Create SketchConstraintMirror
SketchConstraintMirror_1_objects = [SketchArc_9.results()[1], SketchLine_44.result(), SketchArc_10.results()[1], SketchLine_45.result()]
SketchConstraintMirror_1 = Sketch_3.addMirror(SketchLine_43.result(), SketchConstraintMirror_1_objects)
[SketchArc_11, SketchLine_46, SketchArc_12, SketchLine_47] = SketchConstraintMirror_1.mirrored()
model.do()

### Create Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchProjection_9r-SketchProjection_9r-SketchLine_46r-SketchArc_11_2r-SketchArc_9_2r-SketchLine_44f"), model.selection("WIRE", "Sketch_3/Face-SketchProjection_10r-SketchLine_45r-SketchArc_10_2f-SketchArc_12_2f-SketchLine_47f-SketchProjection_10r_wire")], model.selection(), 0, 4.2)

### Create Sketch
Sketch_4 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))

### Create SketchLine
SketchLine_48 = Sketch_4.addLine(0.75, 2.1, -0.75, 2.1)

### Create SketchLine
SketchLine_49 = Sketch_4.addLine(-0.75, 2.1, -0.75, 0.3)

### Create SketchLine
SketchLine_50 = Sketch_4.addLine(-0.75, 0.3, 0.75, 0.3)

### Create SketchLine
SketchLine_51 = Sketch_4.addLine(0.75, 0.3, 0.75, 2.1)
Sketch_4.setCoincident(SketchLine_51.endPoint(), SketchLine_48.startPoint())
Sketch_4.setCoincident(SketchLine_48.endPoint(), SketchLine_49.startPoint())
Sketch_4.setCoincident(SketchLine_49.endPoint(), SketchLine_50.startPoint())
Sketch_4.setCoincident(SketchLine_50.endPoint(), SketchLine_51.startPoint())
Sketch_4.setHorizontal(SketchLine_48.result())
Sketch_4.setVertical(SketchLine_49.result())
Sketch_4.setHorizontal(SketchLine_50.result())
Sketch_4.setVertical(SketchLine_51.result())

### Create SketchProjection
SketchProjection_12 = Sketch_4.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_52 = SketchProjection_12.createdFeature()
Sketch_4.setDistance(SketchLine_52.result(), SketchLine_50.endPoint(), 0.3, True)
Sketch_4.setDistance(SketchLine_51.endPoint(), SketchLine_52.result(), 2.1, True)

### Create SketchPoint
SketchPoint_5 = Sketch_4.addPoint(0, 2.1)
Sketch_4.setCoincident(SketchPoint_5.coordinates(), SketchLine_48.result())
Sketch_4.setMiddlePoint(SketchLine_48.result(), SketchPoint_5.coordinates())

### Create SketchProjection
SketchProjection_13 = Sketch_4.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_53 = SketchProjection_13.createdFeature()
Sketch_4.setCoincident(SketchPoint_5.coordinates(), SketchLine_53.result())
Sketch_4.setDistance(SketchPoint_5.coordinates(), SketchLine_51.endPoint(), 0.75, True)
model.do()

### Create Extrusion
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_4/Face-SketchLine_48r-SketchLine_49f-SketchLine_50f-SketchLine_51f")], model.selection(), 10, 0)

### Create Rotation
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_1")], axis = model.selection("EDGE", "PartSet/OZ"), angle = -90)

### Create Rotation
Rotation_2 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Extrusion_2_1")], axis = model.selection("EDGE", "PartSet/OZ"), angle = -90)

### Create Fuse
Fuse_2 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Fuse_1_1"), model.selection("COMPOUND", "Rotation_1_1")], removeEdges = True)

### Create Cut
Cut_2 = model.addCut(Part_1_doc, [model.selection("SOLID", "Fuse_2_1")], [model.selection("SOLID", "Rotation_2_1")])

### Create Group
Group_1_objects = [model.selection("VERTEX", "[Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_5_2][Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_40]"),
                   model.selection("VERTEX", "[Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_5_2][(Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_5_2)(Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_6_2)_Cut_2_1]"),
                   model.selection("VERTEX", "[Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_40][Fuse_1_1/Modified_Face&Sketch_1/SketchArc_1_2]"),
                   model.selection("VERTEX", "[(Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_5_2)(Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_6_2)_Cut_2_1][Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_6_2]"),
                   model.selection("VERTEX", "[Fuse_1_1/Modified_Face&Sketch_1/SketchArc_1_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_21]"),
                   model.selection("VERTEX", "[_new_weak_name_87_Cut_2_1]e[_new_weak_name_78_Cut_2_1]e[_new_weak_name_77_Cut_2_1]e[_new_weak_name_85_Cut_2_1]e"),
                   model.selection("VERTEX", "[_new_weak_name_78_Cut_2_1]e[_new_weak_name_64_Cut_2_1]e[_new_weak_name_77_Cut_2_1]e"),
                   model.selection("VERTEX", "[Fuse_1_1/Modified_Face&Sketch_1/SketchArc_1_2][new_weak_name_2]"),
                   model.selection("VERTEX", "[Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_6_2][Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_42]"),
                   model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_21][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_20]"),
                   model.selection("VERTEX", "[Fuse_1_1/Modified_Face&Sketch_2/SketchLine_40][Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_5_2]"),
                   model.selection("VERTEX", "[Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_42][Fuse_1_1/Modified_Face&Sketch_1/SketchArc_3_2]"),
                   model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_20][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_19]"),
                   model.selection("VERTEX", "[Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_5_2][(Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_5_2)(Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_6_2)_Cut_2_1]"),
                   model.selection("VERTEX", "[Fuse_1_1/Modified_Face&Sketch_1/SketchArc_3_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_25]"),
                   model.selection("VERTEX", "[_new_weak_name_86_Cut_2_1]e[_new_weak_name_75_Cut_2_1]e[_new_weak_name_74_Cut_2_1]e[_new_weak_name_84_Cut_2_1]e"),
                   model.selection("VERTEX", "[_new_weak_name_75_Cut_2_1]e[_new_weak_name_63_Cut_2_1]e[_new_weak_name_74_Cut_2_1]e"),
                   model.selection("VERTEX", "[Fuse_1_1/Modified_Face&Sketch_1/SketchArc_3_2][new_weak_name_2]"),
                   model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_19][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_18]"),
                   model.selection("VERTEX", "[(Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_5_2)(Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_6_2)_Cut_2_1][Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_6_2]"),
                   model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_25][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_24]"),
                   model.selection("VERTEX", "[Fuse_1_1/Modified_Face&Sketch_2/SketchLine_42][Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_6_2]"),
                   model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_18][Fuse_2_1/Modified_Face&Sketch_1/SketchLine_9]"),
                   model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_24][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_23]"),
                   model.selection("VERTEX", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_9][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_8]"),
                   model.selection("VERTEX", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_9][Fuse_2_1/Modified_Face&Extrusion_1_2/From_Face][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_47]"),
                   model.selection("VERTEX", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_9][Fuse_2_1/Modified_Face&Extrusion_1_2/From_Face][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_45]"),
                   model.selection("VERTEX", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_9][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_45][Fuse_2_1/Modified_Face&Extrusion_1_2/To_Face]"),
                   model.selection("VERTEX", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_9][Fuse_2_1/Modified_Face&Extrusion_1_2/To_Face][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_47]"),
                   model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_23][Revolution_1_1/Generated_Face&Sketch_1/SketchArc_2_2]"),
                   model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_8][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7]"),
                   model.selection("VERTEX", "[Fuse_2_1/Modified_Face&Extrusion_1_2/From_Face][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_45][Cut_2_1/Modified_Face&Sketch_3/SketchArc_12_2&Sketch_3/SketchArc_10_2]"),
                   model.selection("VERTEX", "[Fuse_2_1/Modified_Face&Extrusion_1_2/From_Face][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_47][Cut_2_1/Modified_Face&Sketch_3/SketchArc_12_2&Sketch_3/SketchArc_10_2]"),
                   model.selection("VERTEX", "[Fuse_2_1/Modified_Face&Sketch_3/SketchLine_45][Fuse_2_1/Modified_Face&Extrusion_1_2/To_Face][Cut_2_1/Modified_Face&Sketch_3/SketchArc_12_2&Sketch_3/SketchArc_10_2]"),
                   model.selection("VERTEX", "[Fuse_2_1/Modified_Face&Extrusion_1_2/To_Face][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_47][Cut_2_1/Modified_Face&Sketch_3/SketchArc_12_2&Sketch_3/SketchArc_10_2]"),
                   model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchArc_2_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_22]"),
                   model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_6]"),
                   model.selection("VERTEX", "Cut_2_1/Generated_Vertex&Sketch_3/SketchArc_12_2&Sketch_3/SketchArc_10_2&new_weak_name_4"),
                   model.selection("VERTEX", "Cut_2_1/Generated_Vertex&Sketch_3/SketchArc_12_2&Sketch_3/SketchArc_10_2&new_weak_name_3"),
                   model.selection("VERTEX", "Cut_2_1/Generated_Vertex&Sketch_3/SketchArc_12_2&Sketch_3/SketchArc_10_2&new_weak_name_1"),
                   model.selection("VERTEX", "Cut_2_1/Generated_Vertex&Sketch_3/SketchArc_12_2&Sketch_3/SketchArc_10_2&new_weak_name_2"),
                   model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_22][Fuse_2_1/Modified_Face&Sketch_1/SketchLine_1]"),
                   model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_6][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5]"),
                   model.selection("VERTEX", "Cut_2_1/Generated_Vertex&Sketch_3/SketchArc_9_2&Sketch_3/SketchArc_11_2&new_weak_name_4"),
                   model.selection("VERTEX", "Cut_2_1/Generated_Vertex&Sketch_3/SketchArc_9_2&Sketch_3/SketchArc_11_2&new_weak_name_3"),
                   model.selection("VERTEX", "Cut_2_1/Generated_Vertex&Sketch_3/SketchArc_9_2&Sketch_3/SketchArc_11_2&new_weak_name_1"),
                   model.selection("VERTEX", "Cut_2_1/Generated_Vertex&Sketch_3/SketchArc_9_2&Sketch_3/SketchArc_11_2&new_weak_name_2"),
                   model.selection("VERTEX", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_1][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_2]"),
                   model.selection("VERTEX", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_1][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_44][Fuse_2_1/Modified_Face&Extrusion_1_1/From_Face]"),
                   model.selection("VERTEX", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_1][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_44][Fuse_2_1/Modified_Face&Extrusion_1_1/To_Face]"),
                   model.selection("VERTEX", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_1][Fuse_2_1/Modified_Face&Extrusion_1_1/From_Face][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_46]"),
                   model.selection("VERTEX", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_1][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_46][Fuse_2_1/Modified_Face&Extrusion_1_1/To_Face]"),
                   model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_4]"),
                   model.selection("VERTEX", "[Cut_2_1/Modified_Face&Sketch_3/SketchArc_9_2&Sketch_3/SketchArc_11_2][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_44][Fuse_2_1/Modified_Face&Extrusion_1_1/From_Face]"),
                   model.selection("VERTEX", "[Cut_2_1/Modified_Face&Sketch_3/SketchArc_9_2&Sketch_3/SketchArc_11_2][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_44][Fuse_2_1/Modified_Face&Extrusion_1_1/To_Face]"),
                   model.selection("VERTEX", "[Cut_2_1/Modified_Face&Sketch_3/SketchArc_9_2&Sketch_3/SketchArc_11_2][Fuse_2_1/Modified_Face&Extrusion_1_1/From_Face][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_46]"),
                   model.selection("VERTEX", "[Cut_2_1/Modified_Face&Sketch_3/SketchArc_9_2&Sketch_3/SketchArc_11_2][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_46][Fuse_2_1/Modified_Face&Extrusion_1_1/To_Face]"),
                   model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_16][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_15]"),
                   model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_16][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_13]"),
                   model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_15][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_14]"),
                   model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_13][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_14]")]
Group_1 = model.addGroup(Part_1_doc, "VERTEX", Group_1_objects)

### Create Group
Group_2_objects = [model.selection("EDGE", "[Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_5_2][Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_40]"),
                   model.selection("EDGE", "[Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_5_2][(Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_5_2)(Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_6_2)_Cut_2_1]"),
                   model.selection("EDGE", "([Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_5_2][Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_40])([Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_40][Fuse_1_1/Modified_Face&Sketch_1/SketchArc_1_2])2_Cut_2_1"),
                   model.selection("EDGE", "[Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_40][Fuse_1_1/Modified_Face&Sketch_1/SketchArc_1_2]"),
                   model.selection("EDGE", "([Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_5_2][Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_40])([Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_40][Fuse_1_1/Modified_Face&Sketch_1/SketchArc_1_2])_Cut_2_1"),
                   model.selection("EDGE", "([Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_5_2][Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_40])2([Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_6_2][Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_42])2_Cut_2_1"),
                   model.selection("EDGE", "[(Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_5_2)(Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_6_2)_Cut_2_1][Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_6_2]"),
                   model.selection("EDGE", "[Fuse_1_1/Modified_Face&Sketch_1/SketchArc_1_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_21]"),
                   model.selection("EDGE", "([Fuse_1_1/Modified_Face&Sketch_1/SketchArc_1_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_21])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_21][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_20])2([Fuse_1_1/Modified_Face&Sketch_2/SketchLine_40][Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_5_2])2_Cut_2_1"),
                   model.selection("EDGE", "[Fuse_1_1/Modified_Face&Sketch_1/SketchArc_1_2][Fuse_1_1/Modified_Face&Sketch_2/SketchLine_40][new_weak_name_2]"),
                   model.selection("EDGE", "([Fuse_1_1/Modified_Face&Sketch_1/SketchArc_1_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_21])3([Fuse_1_1/Modified_Face&Sketch_2/SketchLine_40][Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_5_2])3_Cut_2_1"),
                   model.selection("EDGE", "[Fuse_1_1/Modified_Face&Sketch_1/SketchArc_1_2][Fuse_1_1/Modified_Face&Sketch_2/SketchLine_40][new_weak_name_1]"),
                   model.selection("EDGE", "[Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_6_2][Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_42]"),
                   model.selection("EDGE", "([Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_6_2][Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_42])([Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_42][Fuse_1_1/Modified_Face&Sketch_1/SketchArc_3_2])2_Cut_2_1"),
                   model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_21][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_20]"),
                   model.selection("EDGE", "([Fuse_1_1/Modified_Face&Sketch_1/SketchArc_1_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_21])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_21][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_20])_Cut_2_1"),
                   model.selection("EDGE", "([Fuse_1_1/Modified_Face&Sketch_2/SketchLine_40][Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_5_2])([Fuse_1_1/Modified_Face&Sketch_1/SketchArc_1_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_21])2_Cut_2_1"),
                   model.selection("EDGE", "[Fuse_1_1/Modified_Face&Sketch_2/SketchLine_40][Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_5_2]"),
                   model.selection("EDGE", "[Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_42][Fuse_1_1/Modified_Face&Sketch_1/SketchArc_3_2]"),
                   model.selection("EDGE", "([Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_6_2][Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_42])([Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_42][Fuse_1_1/Modified_Face&Sketch_1/SketchArc_3_2])_Cut_2_1"),
                   model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_20][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_19]"),
                   model.selection("EDGE", "[Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_5_2][(Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_5_2)(Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_6_2)_Cut_2_1]"),
                   model.selection("EDGE", "([Fuse_1_1/Modified_Face&Sketch_2/SketchLine_40][Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_5_2])([Fuse_1_1/Modified_Face&Sketch_1/SketchArc_1_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_21])3([Fuse_1_1/Modified_Face&Sketch_2/SketchLine_42][Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_6_2])3_Cut_2_1"),
                   model.selection("EDGE", "[Fuse_1_1/Modified_Face&Sketch_1/SketchArc_3_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_25]"),
                   model.selection("EDGE", "([Fuse_1_1/Modified_Face&Sketch_1/SketchArc_3_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_25])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_25][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_24])2([Fuse_1_1/Modified_Face&Sketch_2/SketchLine_42][Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_6_2])2_Cut_2_1"),
                   model.selection("EDGE", "[Fuse_1_1/Modified_Face&Sketch_1/SketchArc_3_2][Fuse_1_1/Modified_Face&Sketch_2/SketchLine_42][new_weak_name_2]"),
                   model.selection("EDGE", "([Fuse_1_1/Modified_Face&Sketch_1/SketchArc_3_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_25])3([Fuse_1_1/Modified_Face&Sketch_2/SketchLine_42][Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_6_2])3_Cut_2_1"),
                   model.selection("EDGE", "[Fuse_1_1/Modified_Face&Sketch_1/SketchArc_3_2][Fuse_1_1/Modified_Face&Sketch_2/SketchLine_42][new_weak_name_1]"),
                   model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_19][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_18]"),
                   model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_20][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_19])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_19][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_18])_Cut_2_1"),
                   model.selection("EDGE", "([Fuse_1_1/Modified_Face&Sketch_2/SketchLine_40][Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_5_2])2([Fuse_1_1/Modified_Face&Sketch_2/SketchLine_42][Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_6_2])2_Cut_2_1"),
                   model.selection("EDGE", "[(Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_5_2)(Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_6_2)_Cut_2_1][Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_6_2]"),
                   model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_25][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_24]"),
                   model.selection("EDGE", "([Fuse_1_1/Modified_Face&Sketch_1/SketchArc_3_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_25])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_25][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_24])_Cut_2_1"),
                   model.selection("EDGE", "([Fuse_1_1/Modified_Face&Sketch_2/SketchLine_42][Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_6_2])([Fuse_1_1/Modified_Face&Sketch_1/SketchArc_3_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_25])2_Cut_2_1"),
                   model.selection("EDGE", "[Fuse_1_1/Modified_Face&Sketch_2/SketchLine_42][Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_6_2]"),
                   model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_18][Fuse_2_1/Modified_Face&Sketch_1/SketchLine_9]"),
                   model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_19][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_18])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_18][Fuse_2_1/Modified_Face&Sketch_1/SketchLine_9])_Cut_2_1"),
                   model.selection("EDGE", "([Fuse_1_1/Modified_Face&Sketch_2/SketchLine_42][Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_6_2])([Fuse_1_1/Modified_Face&Sketch_2/SketchLine_40][Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_5_2])3([Fuse_1_1/Modified_Face&Sketch_1/SketchArc_3_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_25])3_Cut_2_1"),
                   model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_24][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_23]"),
                   model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_25][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_24])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_24][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_23])_Cut_2_1"),
                   model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_18][Fuse_2_1/Modified_Face&Sketch_1/SketchLine_9])([Fuse_2_1/Modified_Face&Sketch_1/SketchLine_9][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_8])_Cut_2_1"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_9][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_8]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_9][Fuse_2_1/Modified_Face&Extrusion_1_2/From_Face]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_9][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_45]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_9][Fuse_2_1/Modified_Face&Extrusion_1_2/To_Face]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_9][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_47]"),
                   model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_23][Revolution_1_1/Generated_Face&Sketch_1/SketchArc_2_2]"),
                   model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_24][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_23])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_23][Revolution_1_1/Generated_Face&Sketch_1/SketchArc_2_2])_Cut_2_1"),
                   model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_8][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7]"),
                   model.selection("EDGE", "([Fuse_2_1/Modified_Face&Sketch_1/SketchLine_9][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_8])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_8][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7])_Cut_2_1"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Extrusion_1_2/From_Face][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_45]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Extrusion_1_2/From_Face][Cut_2_1/Modified_Face&Sketch_3/SketchArc_12_2&Sketch_3/SketchArc_10_2]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Extrusion_1_2/From_Face][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_47]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Sketch_3/SketchLine_45][Cut_2_1/Modified_Face&Sketch_3/SketchArc_12_2&Sketch_3/SketchArc_10_2]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Sketch_3/SketchLine_45][Fuse_2_1/Modified_Face&Extrusion_1_2/To_Face]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Extrusion_1_2/To_Face][Cut_2_1/Modified_Face&Sketch_3/SketchArc_12_2&Sketch_3/SketchArc_10_2]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Extrusion_1_2/To_Face][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_47]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Sketch_3/SketchLine_47][Cut_2_1/Modified_Face&Sketch_3/SketchArc_12_2&Sketch_3/SketchArc_10_2]"),
                   model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchArc_2_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_22]"),
                   model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_23][Revolution_1_1/Generated_Face&Sketch_1/SketchArc_2_2])([Revolution_1_1/Generated_Face&Sketch_1/SketchArc_2_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_22])_Cut_2_1"),
                   model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_6]"),
                   model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_8][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_6])_Cut_2_1"),
                   model.selection("EDGE", "Cut_2_1/Generated_Edge&Sketch_4/SketchLine_51&Sketch_3/SketchArc_12_2&Sketch_3/SketchArc_10_2"),
                   model.selection("EDGE", "Cut_2_1/Generated_Edge&Sketch_4/SketchLine_50&Sketch_3/SketchArc_12_2&Sketch_3/SketchArc_10_2"),
                   model.selection("EDGE", "Cut_2_1/Generated_Edge&Sketch_4/SketchLine_49&Sketch_3/SketchArc_12_2&Sketch_3/SketchArc_10_2"),
                   model.selection("EDGE", "Cut_2_1/Generated_Edge&Sketch_4/SketchLine_48&Sketch_3/SketchArc_12_2&Sketch_3/SketchArc_10_2"),
                   model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_22][Fuse_2_1/Modified_Face&Sketch_1/SketchLine_1]"),
                   model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchArc_2_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_22])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_22][Fuse_2_1/Modified_Face&Sketch_1/SketchLine_1])_Cut_2_1"),
                   model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_6][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5]"),
                   model.selection("EDGE", "Cut_2_1/Generated_Edge&Sketch_4/SketchLine_51&Sketch_3/SketchArc_9_2&Sketch_3/SketchArc_11_2"),
                   model.selection("EDGE", "[Cut_2_1/Modified_Face&Sketch_4/SketchLine_51][Cut_2_1/Modified_Face&Sketch_4/SketchLine_48]"),
                   model.selection("EDGE", "[Cut_2_1/Modified_Face&Sketch_4/SketchLine_51][Cut_2_1/Modified_Face&Sketch_4/SketchLine_50]"),
                   model.selection("EDGE", "Cut_2_1/Generated_Edge&Sketch_4/SketchLine_50&Sketch_3/SketchArc_9_2&Sketch_3/SketchArc_11_2"),
                   model.selection("EDGE", "[Cut_2_1/Modified_Face&Sketch_4/SketchLine_50][Cut_2_1/Modified_Face&Sketch_4/SketchLine_49]"),
                   model.selection("EDGE", "Cut_2_1/Generated_Edge&Sketch_4/SketchLine_49&Sketch_3/SketchArc_9_2&Sketch_3/SketchArc_11_2"),
                   model.selection("EDGE", "[Cut_2_1/Modified_Face&Sketch_4/SketchLine_49][Cut_2_1/Modified_Face&Sketch_4/SketchLine_48]"),
                   model.selection("EDGE", "Cut_2_1/Generated_Edge&Sketch_4/SketchLine_48&Sketch_3/SketchArc_9_2&Sketch_3/SketchArc_11_2"),
                   model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_22][Fuse_2_1/Modified_Face&Sketch_1/SketchLine_1])([Fuse_2_1/Modified_Face&Sketch_1/SketchLine_1][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_2])_Cut_2_1"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_1][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_2]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_1][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_44]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_1][Fuse_2_1/Modified_Face&Extrusion_1_1/From_Face]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_1][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_46]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Sketch_1/SketchLine_1][Fuse_2_1/Modified_Face&Extrusion_1_1/To_Face]"),
                   model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_4]"),
                   model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_6][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_4])_Cut_2_1"),
                   model.selection("EDGE", "[Cut_2_1/Modified_Face&Sketch_3/SketchArc_9_2&Sketch_3/SketchArc_11_2][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_44]"),
                   model.selection("EDGE", "[Cut_2_1/Modified_Face&Sketch_3/SketchArc_9_2&Sketch_3/SketchArc_11_2][Fuse_2_1/Modified_Face&Extrusion_1_1/From_Face]"),
                   model.selection("EDGE", "[Cut_2_1/Modified_Face&Sketch_3/SketchArc_9_2&Sketch_3/SketchArc_11_2][Fuse_2_1/Modified_Face&Extrusion_1_1/To_Face]"),
                   model.selection("EDGE", "[Cut_2_1/Modified_Face&Sketch_3/SketchArc_9_2&Sketch_3/SketchArc_11_2][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_46]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Sketch_3/SketchLine_44][Fuse_2_1/Modified_Face&Extrusion_1_1/From_Face]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Sketch_3/SketchLine_44][Fuse_2_1/Modified_Face&Extrusion_1_1/To_Face]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Extrusion_1_1/From_Face][Fuse_2_1/Modified_Face&Sketch_3/SketchLine_46]"),
                   model.selection("EDGE", "[Fuse_2_1/Modified_Face&Sketch_3/SketchLine_46][Fuse_2_1/Modified_Face&Extrusion_1_1/To_Face]"),
                   model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_16][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_15]"),
                   model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_16][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_13]"),
                   model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_16][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_15])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_16][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_13])_Cut_2_1"),
                   model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_15][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_14]"),
                   model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_13][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_14]"),
                   model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_15][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_14])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_13][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_14])_Cut_2_1")]
Group_2 = model.addGroup(Part_1_doc, "EDGE", Group_2_objects)

### Create Group
Group_3_objects = [model.selection("FACE", "Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_5_2"),
                   model.selection("FACE", "Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_40"),
                   model.selection("FACE", "(Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_5_2)(Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_6_2)_Cut_2_1"),
                   model.selection("FACE", "Fuse_1_1/Modified_Face&Sketch_1/SketchArc_1_2"),
                   model.selection("FACE", "Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchArc_6_2"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_21"),
                   model.selection("FACE", "Fuse_1_1/Modified_Face&Sketch_2/SketchLine_40"),
                   model.selection("FACE", "Symmetry_1_2_2/MF:Symmetried&Sketch_2/SketchLine_42"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_20"),
                   model.selection("FACE", "Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_5_2"),
                   model.selection("FACE", "Fuse_1_1/Modified_Face&Sketch_1/SketchArc_3_2"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_19"),
                   model.selection("FACE", "(Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_5_2)(Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_6_2)_Cut_2_1"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_25"),
                   model.selection("FACE", "Fuse_1_1/Modified_Face&Sketch_2/SketchLine_42"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_18"),
                   model.selection("FACE", "Symmetry_1_2_1/MF:Symmetried&Sketch_2/SketchArc_6_2"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_24"),
                   model.selection("FACE", "Fuse_2_1/Modified_Face&Sketch_1/SketchLine_9"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_23"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_8"),
                   model.selection("FACE", "Fuse_2_1/Modified_Face&Extrusion_1_2/From_Face"),
                   model.selection("FACE", "Fuse_2_1/Modified_Face&Sketch_3/SketchLine_45"),
                   model.selection("FACE", "Fuse_2_1/Modified_Face&Extrusion_1_2/To_Face"),
                   model.selection("FACE", "Fuse_2_1/Modified_Face&Sketch_3/SketchLine_47"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchArc_2_2"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7"),
                   model.selection("FACE", "Cut_2_1/Modified_Face&Sketch_3/SketchArc_12_2&Sketch_3/SketchArc_10_2"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_22"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_6"),
                   model.selection("FACE", "Cut_2_1/Modified_Face&Sketch_4/SketchLine_51"),
                   model.selection("FACE", "Cut_2_1/Modified_Face&Sketch_4/SketchLine_50"),
                   model.selection("FACE", "Cut_2_1/Modified_Face&Sketch_4/SketchLine_49"),
                   model.selection("FACE", "Cut_2_1/Modified_Face&Sketch_4/SketchLine_48"),
                   model.selection("FACE", "Fuse_2_1/Modified_Face&Sketch_1/SketchLine_1"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5"),
                   model.selection("FACE", "Cut_2_1/Modified_Face&Sketch_3/SketchArc_9_2&Sketch_3/SketchArc_11_2"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_2"),
                   model.selection("FACE", "Fuse_2_1/Modified_Face&Sketch_3/SketchLine_44"),
                   model.selection("FACE", "Fuse_2_1/Modified_Face&Extrusion_1_1/From_Face"),
                   model.selection("FACE", "Fuse_2_1/Modified_Face&Sketch_3/SketchLine_46"),
                   model.selection("FACE", "Fuse_2_1/Modified_Face&Extrusion_1_1/To_Face"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_4"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_16"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_15"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_13"),
                   model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_14")]
Group_3 = model.addGroup(Part_1_doc, "FACE", Group_3_objects)

### Create Partition
Partition_1_objects = [model.selection("SOLID", "Cut_2_1"),
                       model.selection("FACE", "PartSet/XOZ"),
                       model.selection("FACE", "PartSet/YOZ")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects, keepSubResults = True)

### Create Revolution
Revolution_4 = model.addRevolution(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchLine_35")], model.selection("EDGE", "PartSet/OZ"), 360, 0)

### Create Split
Split_1 = model.addSplit(Part_1_doc, [model.selection("COMPOUND", "all-in-Partition_1")], [model.selection("FACE", "Revolution_4_1")], keepSubResults = True)

### Create Plane
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOZ"), model.selection("EDGE", "Sketch_2/SketchLine_34"), 45)

### Create Sketch
Sketch_5 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_2"))

### Create SketchLine
SketchLine_54 = Sketch_5.addLine(5.509213645269787, 10.82082332930156, 5.382690082387538, 10.60520378465038)

### Create SketchLine
SketchLine_55 = Sketch_5.addLine(5.382690082387538, 10.60520378465038, 5.5912917558727, 10.4792224745154)
Sketch_5.setCoincident(SketchLine_54.endPoint(), SketchLine_55.startPoint())

### Create SketchLine
SketchLine_56 = Sketch_5.addLine(5.5912917558727, 10.4792224745154, 5.928669438910004, 10.31165844357506)
Sketch_5.setCoincident(SketchLine_55.endPoint(), SketchLine_56.startPoint())

### Create SketchIntersectionPoint
SketchIntersectionPoint_1 = Sketch_5.addIntersectionPoint(model.selection("EDGE", "[Split_1_1_7/Modified_Face&Sketch_2/SketchLine_39][Split_1_1_7/Modified_Face&Sketch_2/SketchArc_5_2]"), True)
[SketchPoint_6] = SketchIntersectionPoint_1.intersectionPoints()

### Create SketchIntersectionPoint
SketchIntersectionPoint_2 = Sketch_5.addIntersectionPoint(model.selection("EDGE", "[Split_1_1_7/Modified_Face&Sketch_2/SketchArc_5_2][Split_1_1_7/Modified_Face&Sketch_2/SketchLine_40]"), True)
[SketchPoint_7] = SketchIntersectionPoint_2.intersectionPoints()

### Create SketchIntersectionPoint
SketchIntersectionPoint_3 = Sketch_5.addIntersectionPoint(model.selection("EDGE", "[Split_1_1_7/Modified_Face&Sketch_1/SketchArc_1_2][Split_1_1_7/Modified_Face&Sketch_2/SketchLine_40]"), True)
[SketchPoint_8] = SketchIntersectionPoint_3.intersectionPoints()

### Create SketchIntersectionPoint
SketchIntersectionPoint_4 = Sketch_5.addIntersectionPoint(model.selection("EDGE", "[Split_1_1_7/Modified_Face&Sketch_2/SketchArc_6_2][Split_1_1_7/Modified_Face&Sketch_2/SketchLine_39]"), True)
[SketchPoint_9] = SketchIntersectionPoint_4.intersectionPoints()
Sketch_5.setCoincident(SketchLine_54.startPoint(), SketchAPI_Point(SketchPoint_6).coordinates())
Sketch_5.setCoincident(SketchLine_55.startPoint(), SketchAPI_Point(SketchPoint_9).coordinates())

### Create SketchIntersectionPoint
SketchIntersectionPoint_5 = Sketch_5.addIntersectionPoint(model.selection("EDGE", "[Split_1_1_7/Modified_Face&Sketch_2/SketchLine_42][Split_1_1_7/Modified_Face&Sketch_2/SketchArc_6_2]"), True)
[SketchPoint_10] = SketchIntersectionPoint_5.intersectionPoints()

### Create SketchIntersectionPoint
SketchIntersectionPoint_6 = Sketch_5.addIntersectionPoint(model.selection("EDGE", "[Split_1_1_7/Modified_Face&Sketch_1/SketchArc_3_2][Split_1_1_7/Modified_Face&Sketch_2/SketchLine_42]"), True)
[SketchPoint_11] = SketchIntersectionPoint_6.intersectionPoints()
Sketch_5.setCoincident(SketchLine_55.endPoint(), SketchAPI_Point(SketchPoint_10).coordinates())
Sketch_5.setCoincident(SketchLine_56.endPoint(), SketchAPI_Point(SketchPoint_11).coordinates())

### Create SketchLine
SketchLine_57 = Sketch_5.addLine(5.723231855768999, 10.69157079694662, 5.5912917558727, 10.4792224745154)
SketchLine_57.setName("SketchLine_72")
SketchLine_57.result().setName("SketchLine_72")
Sketch_5.setCoincident(SketchLine_55.endPoint(), SketchLine_57.endPoint())

### Create SketchLine
SketchLine_58 = Sketch_5.addLine(5.928669438910005, 10.31165844357506, 6.034221518827074, 10.48153710152002)
SketchLine_58.setName("SketchLine_109")
SketchLine_58.result().setName("SketchLine_109")
Sketch_5.setCoincident(SketchLine_56.endPoint(), SketchLine_58.startPoint())

### Create SketchLine
SketchLine_59 = Sketch_5.addLine(6.034221518827074, 10.48153710152002, 5.723231855768999, 10.69157079694662)
SketchLine_59.setName("SketchLine_113")
SketchLine_59.result().setName("SketchLine_113")
Sketch_5.setCoincident(SketchLine_58.endPoint(), SketchLine_59.startPoint())
Sketch_5.setCoincident(SketchLine_58.result(), SketchAPI_Point(SketchPoint_8).coordinates())
Sketch_5.setLength(SketchLine_58.result(), 0.2)

### Create SketchLine
SketchLine_60 = Sketch_5.addLine(5.723231855768999, 10.69157079694662, 5.514274587785066, 10.82944811108761)
SketchLine_60.setName("SketchLine_114")
SketchLine_60.result().setName("SketchLine_114")
Sketch_5.setCoincident(SketchAPI_Point(SketchPoint_7).coordinates(), SketchLine_60.startPoint())

### Create SketchLine
SketchLine_61 = Sketch_5.addLine(5.514274587785066, 10.82944811108761, 5.509213645269788, 10.82082332930156)
SketchLine_61.setName("SketchLine_115")
SketchLine_61.result().setName("SketchLine_115")
Sketch_5.setCoincident(SketchLine_60.endPoint(), SketchLine_61.startPoint())
Sketch_5.setCoincident(SketchLine_54.startPoint(), SketchLine_61.endPoint())
Sketch_5.setCollinear(SketchLine_61.result(), SketchLine_54.result())
Sketch_5.setLength(SketchLine_61.result(), 0.01)
Sketch_5.setCoincident(SketchLine_60.startPoint(), SketchLine_59.endPoint())
Sketch_5.setCoincident(SketchLine_60.startPoint(), SketchLine_57.startPoint())
model.do()

### Create Face
Face_1 = model.addFace(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_5")])

### Create Revolution
Revolution_5 = model.addRevolution(Part_1_doc, [model.selection("EDGE", "Face_1_2/Modified_Edge&Sketch_5/SketchLine_109")], model.selection("EDGE", "PartSet/OZ"), model.selection("FACE", "PartSet/YOZ"), 0, model.selection(), 0)

### Create Symmetry
Symmetry_2 = model.addSymmetry(Part_1_doc, [model.selection("COMPOUND", "all-in-Face_1"), model.selection("COMPOUND", "all-in-Revolution_5")], model.selection("FACE", "PartSet/YOZ"), keepOriginal = True, keepSubResults = True)

### Create Symmetry
Symmetry_3 = model.addSymmetry(Part_1_doc, [model.selection("COMPOUND", "all-in-Symmetry_2")], model.selection("FACE", "PartSet/XOZ"), keepOriginal = True, keepSubResults = True)

### Create Point
Point_2 = model.addPoint(Part_1_doc, model.selection("EDGE", "[Split_1_1_7/Modified_Face&Sketch_1/SketchArc_1_2][Partition_1_1_4/Modified_Face&Sketch_1/SketchLine_21]"), 1.25, False, False)

### Create Plane
Plane_6 = model.addPlane(Part_1_doc, model.selection("EDGE", "Symmetry_3_1_1_1_1_2/ME:Symmetried&Sketch_5/SketchLine_109"), model.selection("VERTEX", "Point_1"), False)

### Create Sketch
Sketch_6 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_3"))

### Create SketchIntersectionPoint
SketchIntersectionPoint_7 = Sketch_6.addIntersectionPoint(model.selection("EDGE", "[Partition_1_1_4/Modified_Face&Sketch_1/SketchLine_25][Split_1_1_7/Modified_Face&Sketch_1/SketchArc_3_2]"), True)
[SketchPoint_12] = SketchIntersectionPoint_7.intersectionPoints()

### Create SketchIntersectionPoint
SketchIntersectionPoint_8 = Sketch_6.addIntersectionPoint(model.selection("EDGE", "[Split_1_1_7/Modified_Face&Sketch_1/SketchArc_1_2][Split_1_1_7/Modified_Face&Sketch_2/SketchLine_40]"), True)
SketchIntersectionPoint_8.setName("SketchIntersectionPoint_9")
SketchIntersectionPoint_8.result().setName("SketchIntersectionPoint_9")
[SketchPoint_13] = SketchIntersectionPoint_8.intersectionPoints()
SketchPoint_13.setName("SketchPoint_14")
SketchPoint_13.result().setName("SketchPoint_14")

### Create SketchIntersectionPoint
SketchIntersectionPoint_9 = Sketch_6.addIntersectionPoint(model.selection("EDGE", "[Split_1_1_7/Modified_Face&Sketch_1/SketchArc_3_2][Split_1_1_7/Modified_Face&Sketch_2/SketchLine_42]"), True)
SketchIntersectionPoint_9.setName("SketchIntersectionPoint_10")
SketchIntersectionPoint_9.result().setName("SketchIntersectionPoint_10")
[SketchPoint_14] = SketchIntersectionPoint_9.intersectionPoints()
SketchPoint_14.setName("SketchPoint_15")
SketchPoint_14.result().setName("SketchPoint_15")

### Create SketchProjection
SketchProjection_14 = Sketch_6.addProjection(model.selection("VERTEX", "Point_1"), False)
SketchPoint_15 = SketchProjection_14.createdFeature()
SketchPoint_15.setName("SketchPoint_16")
SketchPoint_15.result().setName("SketchPoint_16")

### Create SketchLine
SketchLine_62 = Sketch_6.addLine(5.246389275102901, -10.59990828317982, 5.359721474154084, -10.7814541886472)
SketchLine_62.setName("SketchLine_60")
SketchLine_62.result().setName("SketchLine_60")
Sketch_6.setCoincident(SketchLine_62.result(), SketchAPI_Point(SketchPoint_15).coordinates())
Sketch_6.setCoincident(SketchLine_62.result(), SketchAPI_Point(SketchPoint_12).coordinates())
Sketch_6.setDistance(SketchAPI_Point(SketchPoint_15).coordinates(), SketchLine_62.endPoint(), 0.05, True)
Sketch_6.setCoincident(SketchLine_62.startPoint(), SketchAPI_Point(SketchPoint_12).coordinates())
model.do()

### Create Filling
Filling_1 = model.addFilling(Part_1_doc, [model.selection("EDGE", "Sketch_6/SketchLine_60"), model.selection("EDGE", "Symmetry_3_1_1_1_1_2/ME:Symmetried&Sketch_5/SketchLine_109")])

### Create Revolution
Revolution_6 = model.addRevolution(Part_1_doc, [model.selection("EDGE", "Sketch_6/SketchLine_60")], model.selection("EDGE", "PartSet/OZ"), 360, 0)

### Create Point
Point_3 = model.addPoint(Part_1_doc, model.selection("EDGE", "[Partition_1_1_4/Modified_Face&Sketch_1/SketchLine_21][Partition_1_1_4/Modified_Face&Sketch_1/SketchLine_20]"), 1.5, False, False)

### Create Plane
Plane_7 = model.addPlane(Part_1_doc, model.selection("EDGE", "Sketch_6/SketchLine_60"), model.selection("VERTEX", "Point_2"), False)

### Create Sketch
Sketch_7 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_4"))

### Create SketchIntersectionPoint
SketchIntersectionPoint_10 = Sketch_7.addIntersectionPoint(model.selection("EDGE", "[Partition_1_1_4/Modified_Face&Sketch_1/SketchLine_20][Partition_1_1_4/Modified_Face&Sketch_1/SketchLine_19]"), True)
SketchIntersectionPoint_10.setName("SketchIntersectionPoint_11")
SketchIntersectionPoint_10.result().setName("SketchIntersectionPoint_11")
[SketchPoint_16] = SketchIntersectionPoint_10.intersectionPoints()
SketchPoint_16.setName("SketchPoint_17")
SketchPoint_16.result().setName("SketchPoint_17")

### Create SketchIntersectionPoint
SketchIntersectionPoint_11 = Sketch_7.addIntersectionPoint(model.selection("EDGE", "[Partition_1_1_4/Modified_Face&Sketch_1/SketchLine_19][Partition_1_1_4/Modified_Face&Sketch_1/SketchLine_18]"), True)
SketchIntersectionPoint_11.setName("SketchIntersectionPoint_12")
SketchIntersectionPoint_11.result().setName("SketchIntersectionPoint_12")
[SketchPoint_17] = SketchIntersectionPoint_11.intersectionPoints()
SketchPoint_17.setName("SketchPoint_18")
SketchPoint_17.result().setName("SketchPoint_18")

### Create SketchIntersectionPoint
SketchIntersectionPoint_12 = Sketch_7.addIntersectionPoint(model.selection("EDGE", "[Partition_1_1_4/Modified_Face&Sketch_1/SketchLine_18][Partition_1_1_4/Modified_Face&Sketch_1/SketchLine_9]"), True)
SketchIntersectionPoint_12.setName("SketchIntersectionPoint_13")
SketchIntersectionPoint_12.result().setName("SketchIntersectionPoint_13")
[SketchPoint_18] = SketchIntersectionPoint_12.intersectionPoints()
SketchPoint_18.setName("SketchPoint_19")
SketchPoint_18.result().setName("SketchPoint_19")

### Create SketchLine
SketchLine_63 = Sketch_7.addLine(6.348502432487296, -9.04671367200557, 6.153596006787127, -10.27430623369788)
SketchLine_63.setName("SketchLine_65")
SketchLine_63.result().setName("SketchLine_65")
Sketch_7.setCoincident(SketchAPI_Point(SketchPoint_18).coordinates(), SketchLine_63.startPoint())

### Create SketchProjection
SketchProjection_15 = Sketch_7.addProjection(model.selection("VERTEX", "Point_2"), False)
SketchProjection_15.setName("SketchProjection_16")
SketchProjection_15.result().setName("SketchProjection_16")
SketchPoint_19 = SketchProjection_15.createdFeature()
SketchPoint_19.setName("SketchPoint_20")
SketchPoint_19.result().setName("SketchPoint_20")
Sketch_7.setCoincident(SketchLine_63.endPoint(), SketchPoint_19.result())

### Create SketchIntersectionPoint
SketchIntersectionPoint_13 = Sketch_7.addIntersectionPoint(model.selection("EDGE", "[Partition_1_1_4/Modified_Face&Sketch_1/SketchArc_2_2][Partition_1_1_4/Modified_Face&Sketch_1/SketchLine_23]"), True)
SketchIntersectionPoint_13.setName("SketchIntersectionPoint_14")
SketchIntersectionPoint_13.result().setName("SketchIntersectionPoint_14")
[SketchPoint_20] = SketchIntersectionPoint_13.intersectionPoints()
SketchPoint_20.setName("SketchPoint_21")
SketchPoint_20.result().setName("SketchPoint_21")

### Create SketchIntersectionPoint
SketchIntersectionPoint_14 = Sketch_7.addIntersectionPoint(model.selection("EDGE", "[Partition_1_1_4/Modified_Face&Sketch_1/SketchLine_22][Partition_1_1_4/Modified_Face&Sketch_1/SketchArc_2_2]"), True)
SketchIntersectionPoint_14.setName("SketchIntersectionPoint_15")
SketchIntersectionPoint_14.result().setName("SketchIntersectionPoint_15")
[SketchPoint_21] = SketchIntersectionPoint_14.intersectionPoints()
SketchPoint_21.setName("SketchPoint_22")
SketchPoint_21.result().setName("SketchPoint_22")

### Create SketchLine
SketchLine_64 = Sketch_7.addLine(5.644308024489205, -10.25641345263865, 6.15359600678713, -10.27430623369788)
SketchLine_64.setName("SketchLine_67")
SketchLine_64.result().setName("SketchLine_67")
SketchLine_64.setAuxiliary(True)
Sketch_7.setCoincident(SketchAPI_Point(SketchPoint_20).coordinates(), SketchLine_64.startPoint())
Sketch_7.setCoincident(SketchLine_63.endPoint(), SketchLine_64.endPoint())

### Create SketchLine
SketchLine_65 = Sketch_7.addLine(6.15359600678713, -10.27430623369788, 6.052762716433288, -9.549982350617446)
SketchLine_65.setName("SketchLine_68")
SketchLine_65.result().setName("SketchLine_68")
SketchLine_65.setAuxiliary(True)
Sketch_7.setCoincident(SketchLine_63.endPoint(), SketchLine_65.startPoint())
Sketch_7.setCoincident(SketchAPI_Point(SketchPoint_21).coordinates(), SketchLine_65.endPoint())

### Create SketchLine
SketchLine_66 = Sketch_7.addLine(5.644308024489205, -10.25641345263865, 6.052762716433288, -9.549982350617446)
SketchLine_66.setName("SketchLine_69")
SketchLine_66.result().setName("SketchLine_69")
SketchLine_66.setAuxiliary(True)
Sketch_7.setCoincident(SketchAPI_Point(SketchPoint_20).coordinates(), SketchLine_66.startPoint())
Sketch_7.setCoincident(SketchAPI_Point(SketchPoint_21).coordinates(), SketchLine_66.endPoint())

### Create SketchLine
SketchLine_67 = Sketch_7.addLine(6.15359600678713, -10.27430623369788, 5.812047228248534, -9.966304921873915)
SketchLine_67.setName("SketchLine_70")
SketchLine_67.result().setName("SketchLine_70")
Sketch_7.setCoincident(SketchLine_63.endPoint(), SketchLine_67.startPoint())
Sketch_7.setCoincident(SketchLine_67.endPoint(), SketchLine_66.result())

### Create SketchConstraintAngle
Sketch_7.setAngle(SketchLine_67.result(), SketchLine_64.result(), "80.0626341144/2", type = "Direct")

### Create SketchIntersectionPoint
SketchIntersectionPoint_15 = Sketch_7.addIntersectionPoint(model.selection("EDGE", "[Partition_1_1_4/Modified_Face&Sketch_1/SketchLine_1][Partition_1_1_4/Modified_Face&Sketch_1/SketchLine_22]"), True)
SketchIntersectionPoint_15.setName("SketchIntersectionPoint_16")
SketchIntersectionPoint_15.result().setName("SketchIntersectionPoint_16")
[SketchPoint_22] = SketchIntersectionPoint_15.intersectionPoints()
SketchPoint_22.setName("SketchPoint_23")
SketchPoint_22.result().setName("SketchPoint_23")

### Create SketchLine
SketchLine_68 = Sketch_7.addLine(6.348502432487296, -9.04671367200557, 6.160385566110681, -9.046221812193464)
SketchLine_68.setName("SketchLine_71")
SketchLine_68.result().setName("SketchLine_71")
Sketch_7.setCoincident(SketchAPI_Point(SketchPoint_18).coordinates(), SketchLine_68.startPoint())
Sketch_7.setCoincident(SketchAPI_Point(SketchPoint_22).coordinates(), SketchLine_68.endPoint())

### Create SketchLine
SketchLine_69 = Sketch_7.addLine(6.348502432487297, -9.046713672005572, 6.603839274138267, -9.303368522147153)
SketchLine_69.setName("SketchLine_83")
SketchLine_69.result().setName("SketchLine_83")
Sketch_7.setCoincident(SketchAPI_Point(SketchPoint_18).coordinates(), SketchLine_69.startPoint())

### Create SketchLine
SketchLine_70 = Sketch_7.addLine(6.603839274138267, -9.303368522147153, 6.603839274138267, -10.27548346237037)
SketchLine_70.setName("SketchLine_85")
SketchLine_70.result().setName("SketchLine_85")
Sketch_7.setCoincident(SketchLine_69.endPoint(), SketchLine_70.startPoint())
Sketch_7.setVertical(SketchLine_70.result())

### Create SketchLine
SketchLine_71 = Sketch_7.addLine(6.603839274138267, -10.27548346237037, 6.153596006787128, -10.27430623369788)
SketchLine_71.setName("SketchLine_86")
SketchLine_71.result().setName("SketchLine_86")
Sketch_7.setCoincident(SketchLine_70.endPoint(), SketchLine_71.startPoint())
Sketch_7.setCoincident(SketchLine_63.endPoint(), SketchLine_71.endPoint())
Sketch_7.setCoincident(SketchAPI_Point(SketchPoint_16).coordinates(), SketchLine_71.result())
Sketch_7.setCoincident(SketchAPI_Point(SketchPoint_17).coordinates(), SketchLine_69.result())
Sketch_7.setDistance(SketchAPI_Point(SketchPoint_16).coordinates(), SketchLine_70.result(), 0.1, True)
model.do()

### Create Filling
Filling_2 = model.addFilling(Part_1_doc, [model.selection("EDGE", "Sketch_7/SketchLine_70"), model.selection("EDGE", "Revolution_6_1/From_Edge")])

### Create Face
Face_2 = model.addFace(Part_1_doc, [model.selection("COMPOUND", "Sketch_7")])

### Create Filling
Filling_3 = model.addFilling(Part_1_doc, [model.selection("EDGE", "Sketch_7/SketchLine_71"), model.selection("EDGE", "Sketch_7/SketchLine_70")])

### Create Symmetry
Symmetry_4_objects = [model.selection("FACE", "Filling_1_1"),
                      model.selection("FACE", "Filling_2_1"),
                      model.selection("FACE", "Filling_3_1"),
                      model.selection("FACE", "Face_2_1")]
Symmetry_4 = model.addSymmetry(Part_1_doc, Symmetry_4_objects, model.selection("FACE", "PartSet/YOZ"), keepOriginal = True, keepSubResults = True)

### Create Symmetry
Symmetry_5 = model.addSymmetry(Part_1_doc, [model.selection("COMPOUND", "all-in-Symmetry_4")], model.selection("FACE", "PartSet/XOZ"), keepOriginal = True, keepSubResults = True)

### Create Revolution
Revolution_7_objects = [model.selection("EDGE", "Sketch_7/SketchLine_70"),
                        model.selection("EDGE", "Sketch_7/SketchLine_71"),
                        model.selection("EDGE", "Symmetry_5_1_1_3_1/ME:Symmetried&Filling_3_1/Edge_0_1")]
Revolution_7 = model.addRevolution(Part_1_doc, Revolution_7_objects, model.selection("EDGE", "PartSet/OZ"), 360, 0)

### Create Split
Split_2_objects_2 = [model.selection("COMPOUND", "all-in-Revolution_7"),
                     model.selection("COMPOUND", "all-in-Symmetry_5"),
                     model.selection("COMPOUND", "all-in-Revolution_6"),
                     model.selection("COMPOUND", "all-in-Symmetry_3")]
Split_2 = model.addSplit(Part_1_doc, [model.selection("COMPOUND", "all-in-Split_1")], Split_2_objects_2, keepSubResults = True)

### Create Revolution
Revolution_8 = model.addRevolution(Part_1_doc, [model.selection("EDGE", "Split_2_1_43/Modified_Edge&Sketch_5/SketchLine_72"), model.selection("EDGE", "Split_2_1_43/Modified_Edge&Revolution_5_1/From_Edge&Sketch_5/SketchLine_109&Filling_1_1/Edge_0_4")], model.selection("EDGE", "Sketch_2/SketchLine_34"), 360, 0)

### Create Symmetry
Symmetry_6 = model.addSymmetry(Part_1_doc, [model.selection("COMPOUND", "all-in-Revolution_8")], model.selection("FACE", "PartSet/YOZ"), keepOriginal = True, keepSubResults = True)

### Create Split
Split_3 = model.addSplit(Part_1_doc, [model.selection("COMPOUND", "all-in-Split_2")], [model.selection("COMPOUND", "all-in-Symmetry_6")], keepSubResults = True)

### Create Sketch
Sketch_8 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))

### Create SketchLine
SketchLine_72 = Sketch_8.addLine(5.126644475052088, 10.62934617154252, 5.215654385578071, 10.78090722433818)
SketchLine_72.setName("SketchLine_74")
SketchLine_72.result().setName("SketchLine_74")

### Create SketchLine
SketchLine_73 = Sketch_8.addLine(5.405090045827155, 10.43837553323928, 5.525091876190383, 10.61334472455802)
SketchLine_73.setName("SketchLine_76")
SketchLine_73.result().setName("SketchLine_76")

### Create SketchLine
SketchLine_74 = Sketch_8.addLine(6.082, 9.532, 6.535000000000001, 9.532)
SketchLine_74.setName("SketchLine_78")
SketchLine_74.result().setName("SketchLine_78")
Sketch_8.setHorizontal(SketchLine_74.result())

### Create SketchProjection
SketchProjection_16 = Sketch_8.addProjection(model.selection("EDGE", "Partition_1_1_4/Generated_Edge&PartSet/YOZ/YOZ&Sketch_1/SketchLine_25"), False)
SketchProjection_16.setName("SketchProjection_33")
SketchProjection_16.result().setName("SketchProjection_33")
SketchLine_75 = SketchProjection_16.createdFeature()
SketchLine_75.setName("SketchLine_116")
SketchLine_75.result().setName("SketchLine_116")

### Create SketchProjection
SketchProjection_17 = Sketch_8.addProjection(model.selection("EDGE", "Partition_1_1_4/Generated_Edge&PartSet/YOZ/YOZ&Sketch_1/SketchLine_24"), False)
SketchProjection_17.setName("SketchProjection_38")
SketchProjection_17.result().setName("SketchProjection_38")
SketchLine_76 = SketchProjection_17.createdFeature()
SketchLine_76.setName("SketchLine_117")
SketchLine_76.result().setName("SketchLine_117")

### Create SketchProjection
SketchProjection_18 = Sketch_8.addProjection(model.selection("EDGE", "Partition_1_1_4/Generated_Edge&PartSet/YOZ/YOZ&Sketch_1/SketchLine_23"), False)
SketchProjection_18.setName("SketchProjection_39")
SketchProjection_18.result().setName("SketchProjection_39")
SketchLine_77 = SketchProjection_18.createdFeature()
SketchLine_77.setName("SketchLine_118")
SketchLine_77.result().setName("SketchLine_118")

### Create SketchProjection
SketchProjection_19 = Sketch_8.addProjection(model.selection("EDGE", "Split_2_1_17/Modified_Edge&PartSet/YOZ/YOZ&Sketch_1/SketchArc_2_2"), False)
SketchProjection_19.setName("SketchProjection_40")
SketchProjection_19.result().setName("SketchProjection_40")
SketchBSpline_1 = SketchProjection_19.createdFeature()

### Create SketchProjection
SketchProjection_20 = Sketch_8.addProjection(model.selection("EDGE", "Partition_1_1_4/Generated_Edge&PartSet/YOZ/YOZ&Sketch_1/SketchLine_19"), False)
SketchProjection_20.setName("SketchProjection_41")
SketchProjection_20.result().setName("SketchProjection_41")
SketchLine_78 = SketchProjection_20.createdFeature()
SketchLine_78.setName("SketchLine_119")
SketchLine_78.result().setName("SketchLine_119")
Sketch_8.setCoincident(SketchLine_74.endPoint(), SketchLine_78.result())
Sketch_8.setCoincident(SketchLine_74.startPoint(), SketchAPI_BSpline(SketchBSpline_1).startPoint())

### Create SketchLine
SketchLine_79 = Sketch_8.addLine(5.676067550792231, 10.23944020672391, 5.829536340268943, 10.44848598250872)
SketchLine_79.setName("SketchLine_120")
SketchLine_79.result().setName("SketchLine_120")
Sketch_8.setCoincident(SketchAPI_Line(SketchLine_77).endPoint(), SketchLine_79.startPoint())

### Create SketchProjection
SketchProjection_21 = Sketch_8.addProjection(model.selection("EDGE", "Partition_1_1_4/Generated_Edge&PartSet/YOZ/YOZ&Sketch_1/SketchLine_21"), False)
SketchProjection_21.setName("SketchProjection_42")
SketchProjection_21.result().setName("SketchProjection_42")
SketchLine_80 = SketchProjection_21.createdFeature()
SketchLine_80.setName("SketchLine_121")
SketchLine_80.result().setName("SketchLine_121")
Sketch_8.setCoincident(SketchLine_79.endPoint(), SketchLine_80.result())
Sketch_8.setPerpendicular(SketchLine_79.result(), SketchLine_77.result())
Sketch_8.setPerpendicular(SketchLine_73.result(), SketchLine_76.result())
Sketch_8.setPerpendicular(SketchLine_72.result(), SketchLine_75.result())
Sketch_8.setCoincident(SketchLine_73.startPoint(), SketchAPI_Line(SketchLine_76).endPoint())
Sketch_8.setCoincident(SketchLine_72.startPoint(), SketchAPI_Line(SketchLine_75).endPoint())
Sketch_8.setCoincident(SketchLine_72.endPoint(), SketchLine_80.result())
Sketch_8.setCoincident(SketchLine_73.endPoint(), SketchLine_80.result())
model.do()

### Create Revolution
Revolution_9 = model.addRevolution(Part_1_doc, [model.selection("COMPOUND", "Sketch_8")], model.selection("EDGE", "PartSet/OZ"), 360, 0, "Edges")

### Create Split
Split_4 = model.addSplit(Part_1_doc, [model.selection("COMPOUND", "all-in-Split_3")], [model.selection("COMPOUND", "all-in-Revolution_9")], keepSubResults = True)

### Create Sketch
Sketch_9 = model.addSketch(Part_1_doc, model.selection("FACE", "Partition_1_1_2/Modified_Face&Sketch_1/SketchLine_4"))

### Create SketchLine
SketchLine_81 = Sketch_9.addLine(5.502704971193711, -5.502704971193713, 3.192087844797337, -3.192087844797337)
SketchLine_81.setName("SketchLine_87")
SketchLine_81.result().setName("SketchLine_87")

### Create SketchProjection
SketchProjection_22 = Sketch_9.addProjection(model.selection("EDGE", "[Partition_1_1_4/Modified_Face&Sketch_1/SketchLine_5][Partition_1_1_4/Modified_Face&Sketch_1/SketchLine_4]"), False)
SketchProjection_22.setName("SketchProjection_26")
SketchProjection_22.result().setName("SketchProjection_26")
SketchArc_13 = SketchProjection_22.createdFeature()
Sketch_9.setCoincident(SketchLine_81.startPoint(), SketchArc_13.results()[1])

### Create SketchLine
SketchLine_82 = Sketch_9.addLine(3.192087844797337, -3.192087844797337, 3.891, 0)
SketchLine_82.setName("SketchLine_88")
SketchLine_82.result().setName("SketchLine_88")
Sketch_9.setCoincident(SketchLine_81.endPoint(), SketchLine_82.startPoint())

### Create SketchProjection
SketchProjection_23 = Sketch_9.addProjection(model.selection("EDGE", "Partition_1_1_4/Generated_Edge&PartSet/XOZ/XOZ&Sketch_1/SketchLine_4"), False)
SketchProjection_23.setName("SketchProjection_27")
SketchProjection_23.result().setName("SketchProjection_27")
SketchLine_83 = SketchProjection_23.createdFeature()
SketchLine_83.setName("SketchLine_89")
SketchLine_83.result().setName("SketchLine_89")
Sketch_9.setCoincident(SketchLine_82.endPoint(), SketchLine_83.result())

### Create SketchLine
SketchLine_84 = Sketch_9.addLine(3.192087844797337, -3.192087844797337, 0, -3.891)
SketchLine_84.setName("SketchLine_90")
SketchLine_84.result().setName("SketchLine_90")
Sketch_9.setCoincident(SketchLine_81.endPoint(), SketchLine_84.startPoint())

### Create SketchProjection
SketchProjection_24 = Sketch_9.addProjection(model.selection("EDGE", "Partition_1_1_4/Generated_Edge&PartSet/YOZ/YOZ&Sketch_1/SketchLine_4"), False)
SketchProjection_24.setName("SketchProjection_28")
SketchProjection_24.result().setName("SketchProjection_28")
SketchLine_85 = SketchProjection_24.createdFeature()
SketchLine_85.setName("SketchLine_91")
SketchLine_85.result().setName("SketchLine_91")
Sketch_9.setCoincident(SketchLine_84.endPoint(), SketchLine_85.result())
Sketch_9.setEqual(SketchLine_84.result(), SketchLine_81.result())
Sketch_9.setEqual(SketchLine_81.result(), SketchLine_82.result())
Sketch_9.setMiddlePoint(SketchLine_84.endPoint(), SketchLine_85.result())
Sketch_9.setMiddlePoint(SketchLine_82.endPoint(), SketchLine_83.result())
Sketch_9.setMiddlePoint(SketchLine_81.startPoint(), SketchArc_13.results()[1])

### Create SketchMultiRotation
SketchMultiRotation_1_objects = [SketchLine_81.result(), SketchLine_84.result(), SketchLine_82.result()]
SketchMultiRotation_1 = Sketch_9.addRotation(SketchMultiRotation_1_objects, SketchAPI_Line(SketchLine_83).endPoint(), 360, 4, True)
[SketchLine_86, SketchLine_87, SketchLine_88, SketchLine_89, SketchLine_90, SketchLine_91, SketchLine_92, SketchLine_93, SketchLine_94] = SketchMultiRotation_1.rotated()
SketchLine_94.setName("SketchLine_100")
SketchLine_94.result().setName("SketchLine_100")
SketchLine_93.setName("SketchLine_99")
SketchLine_93.result().setName("SketchLine_99")
SketchLine_92.setName("SketchLine_98")
SketchLine_92.result().setName("SketchLine_98")
SketchLine_91.setName("SketchLine_97")
SketchLine_91.result().setName("SketchLine_97")
SketchLine_90.setName("SketchLine_96")
SketchLine_90.result().setName("SketchLine_96")
SketchLine_89.setName("SketchLine_95")
SketchLine_89.result().setName("SketchLine_95")
SketchLine_88.setName("SketchLine_94")
SketchLine_88.result().setName("SketchLine_94")
SketchLine_87.setName("SketchLine_93")
SketchLine_87.result().setName("SketchLine_93")
SketchLine_86.setName("SketchLine_92")
SketchLine_86.result().setName("SketchLine_92")
model.do()

### Create Extrusion
Extrusion_3 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_9")], model.selection(), model.selection("FACE", "Partition_1_1_4/Modified_Face&Sketch_1/SketchLine_2"), 0, model.selection(), 0, "Edges")

### Create Plane
Plane_8 = model.addPlane(Part_1_doc, model.selection("EDGE", "PartSet/OZ"), model.selection("VERTEX", "Split_2_1_10/Modified_Vertex&Sketch_7/SketchLine_83_StartVertex&Sketch_7/SketchLine_65_StartVertex"), False)

### Create Split
Split_5 = model.addSplit(Part_1_doc, [model.selection("COMPOUND", "all-in-Split_4")], [model.selection("COMPOUND", "all-in-Extrusion_3")], keepSubResults = True)

### Create Sketch
Sketch_10 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_5"))

### Create SketchProjection
SketchProjection_25 = Sketch_10.addProjection(model.selection("VERTEX", "Split_2_1_10/Modified_Vertex&Sketch_7/SketchLine_83_StartVertex&Sketch_7/SketchLine_65_StartVertex"), False)
SketchProjection_25.setName("SketchProjection_35")
SketchProjection_25.result().setName("SketchProjection_35")
SketchPoint_23 = SketchProjection_25.createdFeature()
SketchPoint_23.setName("SketchPoint_27")
SketchPoint_23.result().setName("SketchPoint_27")

### Create SketchLine
SketchLine_95 = Sketch_10.addLine(-7.781999999999998, -3.45, -3.801997755365569, -3.45)
SketchLine_95.setName("SketchLine_107")
SketchLine_95.result().setName("SketchLine_107")

### Create SketchLine
SketchLine_96 = Sketch_10.addLine(-6.188000000000001, -3.45, -6.538000000000001, -3.45)
SketchLine_96.setName("SketchLine_108")
SketchLine_96.result().setName("SketchLine_108")

### Create SketchIntersectionPoint
SketchIntersectionPoint_16 = Sketch_10.addIntersectionPoint(model.selection("EDGE", "[Split_5_1_21/Modified_Face&Sketch_1/SketchLine_5&new_weak_name_1][(Split_5_1_21/Modified_Face&PartSet/XOZ/XOZ)(Split_5_1_21/Modified_Face&Sketch_1/SketchLine_7)(Split_5_1_21/Modified_Face&Extrusion_3_1_5/Extrusion_3_1_5)]"), True)
SketchIntersectionPoint_16.setName("SketchIntersectionPoint_17")
SketchIntersectionPoint_16.result().setName("SketchIntersectionPoint_17")
[SketchPoint_24] = SketchIntersectionPoint_16.intersectionPoints()
SketchPoint_24.setName("SketchPoint_25")
SketchPoint_24.result().setName("SketchPoint_25")

### Create SketchIntersectionPoint
SketchIntersectionPoint_17 = Sketch_10.addIntersectionPoint(model.selection("EDGE", "Extrusion_3_1_11/To_Edge"), True)
SketchIntersectionPoint_17.setName("SketchIntersectionPoint_18")
SketchIntersectionPoint_17.result().setName("SketchIntersectionPoint_18")
[SketchPoint_25] = SketchIntersectionPoint_17.intersectionPoints()
SketchPoint_25.setName("SketchPoint_26")
SketchPoint_25.result().setName("SketchPoint_26")
Sketch_10.setCoincident(SketchLine_95.startPoint(), SketchAPI_Point(SketchPoint_24).coordinates())
Sketch_10.setCoincident(SketchLine_95.endPoint(), SketchAPI_Point(SketchPoint_25).coordinates())

### Create SketchIntersectionPoint
SketchIntersectionPoint_18 = Sketch_10.addIntersectionPoint(model.selection("EDGE", "[(Split_5_1_21/Modified_Face&PartSet/XOZ/XOZ)(Split_5_1_21/Modified_Face&Sketch_1/SketchLine_7)(Split_5_1_21/Modified_Face&Extrusion_3_1_5/Extrusion_3_1_5)][Split_5_1_21/Modified_Face&Sketch_1/SketchLine_7]"), True)
SketchIntersectionPoint_18.setName("SketchIntersectionPoint_19")
SketchIntersectionPoint_18.result().setName("SketchIntersectionPoint_19")
[SketchPoint_26] = SketchIntersectionPoint_18.intersectionPoints()
SketchPoint_26.setName("SketchPoint_28")
SketchPoint_26.result().setName("SketchPoint_28")

### Create SketchIntersectionPoint
SketchIntersectionPoint_19 = Sketch_10.addIntersectionPoint(model.selection("EDGE", "[Split_5_1_21/Modified_Face&Sketch_1/SketchLine_1][(Split_5_1_21/Modified_Face&PartSet/XOZ/XOZ)(Split_5_1_21/Modified_Face&Sketch_1/SketchLine_1)(Split_5_1_21/Modified_Face&Extrusion_3_1_11/Extrusion_3_1_11)(Split_5_1_21/Modified_Face&Extrusion_3_1_5/Extrusion_3_1_5)]"), True)
SketchIntersectionPoint_19.setName("SketchIntersectionPoint_20")
SketchIntersectionPoint_19.result().setName("SketchIntersectionPoint_20")
[SketchPoint_27] = SketchIntersectionPoint_19.intersectionPoints()
SketchPoint_27.setName("SketchPoint_29")
SketchPoint_27.result().setName("SketchPoint_29")
Sketch_10.setCoincident(SketchAPI_Point(SketchPoint_26).coordinates(), SketchLine_96.endPoint())
Sketch_10.setCoincident(SketchAPI_Point(SketchPoint_27).coordinates(), SketchLine_96.startPoint())
model.do()
Sketch_10.result().setName("Sketch_10_Copy")

### Create Filling
Filling_4 = model.addFilling(Part_1_doc, [model.selection("EDGE", "Sketch_10_Copy/SketchLine_108"), model.selection("EDGE", "Split_2_1_9/Modified_Edge&Filling_3_1/Edge_0_2")])

### Create Extrusion
Extrusion_4 = model.addExtrusion(Part_1_doc, [model.selection("EDGE", "Sketch_10_Copy/SketchLine_107")], model.selection("EDGE", "PartSet/OZ"), model.selection("FACE", "(Split_5_1_21/Modified_Face&PartSet/XOZ/XOZ)(Split_5_1_21/Modified_Face&Extrusion_3_1_11/Extrusion_3_1_11)(Split_5_1_21/Modified_Face&Extrusion_3_1_5/Extrusion_3_1_5)(Split_5_1_21/Modified_Face&Sketch_1/SketchLine_7)2(Partition_1_1_1/Modified_Face&Sketch_1/SketchLine_8)2(Split_2_1_8/Modified_Face&Sketch_1/SketchLine_9)2(Split_5_1_21/Modified_Face&Sketch_1/SketchLine_1)2(Split_5_1_21/Modified_Face&PartSet/YOZ/YOZ)2(Split_5_1_21/Modified_Face&Extrusion_3_1_8/Extrusion_3_1_8)2"), 0, model.selection(), 0)

### Create Sketch
Sketch_11 = model.addSketch(Part_1_doc, model.selection("FACE", "Sketch_9/Face-SketchLine_88f-SketchLine_95r-SketchLine_98f-SketchLine_96r-SketchLine_99f-SketchLine_97r-SketchLine_100f-SketchLine_90r"))

### Create SketchLine
SketchLine_97 = Sketch_11.addLine(-3.693983753048041, 0.8998171836933224, 0, 1.098155695798001)
SketchLine_97.setName("SketchLine_111")
SketchLine_97.result().setName("SketchLine_111")

### Create SketchProjection
SketchProjection_26 = Sketch_11.addProjection(model.selection("VERTEX", "[Extrusion_4_1/To_Edge]e[Extrusion_4_1/Generated_Edge&Sketch_10_Copy/SketchLine_107_EndVertex]e"), False)
SketchProjection_26.setName("SketchProjection_34")
SketchProjection_26.result().setName("SketchProjection_34")
SketchPoint_28 = SketchProjection_26.createdFeature()
SketchPoint_28.setName("SketchPoint_30")
SketchPoint_28.result().setName("SketchPoint_30")
Sketch_11.setCoincident(SketchLine_97.startPoint(), SketchPoint_28.result())

### Create SketchProjection
SketchProjection_27 = Sketch_11.addProjection(model.selection("EDGE", "Split_5_1_20/Modified_Edge&PartSet/YOZ/YOZ&Sketch_1/SketchLine_4"), False)
SketchProjection_27.setName("SketchProjection_29")
SketchProjection_27.result().setName("SketchProjection_29")
SketchLine_98 = SketchProjection_27.createdFeature()
SketchLine_98.setName("SketchLine_101")
SketchLine_98.result().setName("SketchLine_101")
Sketch_11.setCoincident(SketchLine_97.endPoint(), SketchLine_98.result())
Sketch_11.setDistance(SketchAPI_Line(SketchLine_98).startPoint(), SketchLine_97.result(), "0.921*3.891/3.268", True)
model.do()

### Create Extrusion
Extrusion_5 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_11")], model.selection("EDGE", "PartSet/OZ"), model.selection("FACE", "Split_5_1_20/Modified_Face&Sketch_1/SketchLine_2"), 0, model.selection(), 0, "Edges")

### Create Symmetry
Symmetry_7_objects = [model.selection("FACE", "Extrusion_5_1"),
                      model.selection("FACE", "Extrusion_4_1"),
                      model.selection("FACE", "Filling_4_1")]
Symmetry_7 = model.addSymmetry(Part_1_doc, Symmetry_7_objects, model.selection("FACE", "PartSet/YOZ"), keepOriginal = True, keepSubResults = True)

### Create Symmetry
Symmetry_8 = model.addSymmetry(Part_1_doc, [model.selection("COMPOUND", "all-in-Symmetry_7")], model.selection("FACE", "PartSet/XOZ"), keepOriginal = True, keepSubResults = True)

### Create Split
Split_6 = model.addSplit(Part_1_doc, [model.selection("COMPOUND", "all-in-Split_5")], [model.selection("COMPOUND", "all-in-Symmetry_8")], keepSubResults = True)

model.end()

from GeomAPI import *

model.testNbResults(Split_6, 1)
model.testNbSubResults(Split_6, [116])
model.testNbSubShapes(Split_6, GeomAPI_Shape.SOLID, [116])
model.testNbSubShapes(Split_6, GeomAPI_Shape.FACE, [856])
model.testNbSubShapes(Split_6, GeomAPI_Shape.EDGE, [3636])
model.testNbSubShapes(Split_6, GeomAPI_Shape.VERTEX, [7272])
model.testResultsVolumes(Split_6, [612.72746616262998])

assert(model.checkPythonDump())
