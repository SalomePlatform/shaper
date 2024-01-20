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
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(1.31, 2.37, 1.31, 0)
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchLine_2 = Sketch_1.addLine(1.29, 2.37, 1.29, 0)
SketchLine_3 = Sketch_1.addLine(1.29, 2.37, 1.31, 2.37)
SketchLine_3.setAuxiliary(True)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchArc_1 = Sketch_1.addArc(2.429999974807509, 2.369762445228637, 1.32, 2.475237825900849, 2.499428733897898, 3.48259875277915, True)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 1.115)
SketchLine_4 = Sketch_1.addLine(1.31, 2.37, 1.32, 2.475237825900849)
SketchLine_4.setName("SketchLine_5")
SketchLine_4.result().setName("SketchLine_5")
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_4.endPoint())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_2.result())
SketchPoint_1 = Sketch_1.addPoint(1.29, 0)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_2.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_5 = SketchProjection_1.createdFeature()
SketchLine_5.setName("SketchLine_6")
SketchLine_5.result().setName("SketchLine_6")
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_5.result())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_6 = SketchProjection_2.createdFeature()
SketchLine_6.setName("SketchLine_7")
SketchLine_6.result().setName("SketchLine_7")
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchPoint_1.coordinates(), SketchLine_6.result(), 1.29, True)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), 0.02)
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_1.startPoint(), SketchLine_4.endPoint(), 0.01)
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_4.result())
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_1.startPoint(), SketchArc_1.center(), 1.12, True)
SketchLine_7 = Sketch_1.addLine(2.499428733897898, 3.48259875277915, 2.429999974807509, 2.369762445228637)
SketchLine_7.setName("SketchLine_8")
SketchLine_7.result().setName("SketchLine_8")
SketchLine_7.setAuxiliary(True)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_7.endPoint())
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_7.result(), SketchLine_3.result(), 93.56999999998719)
SketchArc_2 = Sketch_1.addArc(2.349985665002739, 1.087256027998574, 2.499428733897898, 3.48259875277915, 3.77688682869805, 3.017010693505298, True)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchArc_2.startPoint())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchArc_2.results()[1])
SketchArc_3 = Sketch_1.addArc(1.546759218272587, 0.0009649642070809712, 3.77688682869805, 3.017010693505298, 4.166514953924199, 2.685528408329715, True)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchArc_3.startPoint())
SketchConstraintTangent_3 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchArc_3.results()[1])
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_3.results()[1], 3.751)
SketchConstraintRadius_3 = Sketch_1.setRadius(SketchArc_2.results()[1], 2.4)
SketchLine_8 = Sketch_1.addLine(2.349985665002739, 1.087256027998574, 3.77688682869805, 3.017010693505298)
SketchLine_8.setName("SketchLine_9")
SketchLine_8.result().setName("SketchLine_9")
SketchLine_8.setAuxiliary(True)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_8.endPoint())
SketchConstraintAngle_2 = Sketch_1.setAngle(SketchLine_8.result(), SketchLine_7.result(), 32.91000000001281)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_8.startPoint(), SketchArc_2.center())
SketchLine_9 = Sketch_1.addLine(1.546759218272496, 0.000964964207000808, 4.166514953924199, 2.685528408329715)
SketchLine_9.setName("SketchLine_10")
SketchLine_9.result().setName("SketchLine_10")
SketchLine_9.setAuxiliary(True)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_9.endPoint())
SketchConstraintAngle_3 = Sketch_1.setAngle(SketchLine_9.result(), SketchLine_8.result(), 7.820000000000003)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_9.startPoint(), SketchArc_3.center())
SketchLine_10 = Sketch_1.addLine(4.166514953924199, 2.685528408329715, 4.411514953924289, 2.446442922550623)
SketchLine_10.setName("SketchLine_11")
SketchLine_10.result().setName("SketchLine_11")
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_10.startPoint())
SketchArc_4 = Sketch_1.addArc(3.489606777155361, 1.501728514028915, 4.411514953924289, 2.446442922550623, 4.534275383417573, 2.308616054576027, True)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchArc_4.startPoint())
SketchConstraintTangent_4 = Sketch_1.setTangent(SketchLine_10.result(), SketchArc_4.results()[1])
SketchConstraintDistanceHorizontal_2 = Sketch_1.setHorizontalDistance(SketchArc_3.endPoint(), SketchArc_4.startPoint(), 0.245)
SketchConstraintTangent_5 = Sketch_1.setTangent(SketchArc_3.results()[1], SketchLine_10.result())
SketchLine_11 = Sketch_1.addLine(3.489606777155361, 1.501728514028915, 4.411514953924289, 2.446442922550623)
SketchLine_11.setName("SketchLine_12")
SketchLine_11.result().setName("SketchLine_12")
SketchLine_11.setAuxiliary(True)
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchArc_4.center(), SketchLine_11.startPoint())
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_11.endPoint())
SketchLine_12 = Sketch_1.addLine(3.489606777155361, 1.501728514028915, 4.534275383417573, 2.308616054576027)
SketchLine_12.setName("SketchLine_13")
SketchLine_12.result().setName("SketchLine_13")
SketchLine_12.setAuxiliary(True)
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchArc_4.center(), SketchLine_12.startPoint())
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchLine_12.endPoint())
SketchConstraintAngle_4 = Sketch_1.setAngleBackward(SketchLine_11.result(), SketchLine_12.result(), 8.018000000993595)
SketchConstraintRadius_4 = Sketch_1.setRadius(SketchArc_4.results()[1], 1.32)
SketchArc_5 = Sketch_1.addArc(1.545335047953034, 0, 4.534275383417573, 2.308616054576027, 3.791202568545627, -3.03637143583555, True)
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchArc_5.startPoint())
SketchConstraintTangent_6 = Sketch_1.setTangent(SketchArc_4.results()[1], SketchArc_5.results()[1])
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchArc_5.center(), SketchLine_5.result())
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchPoint_1.coordinates(), SketchLine_3.result(), "4.74/2", True)
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_5.result())
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_5.result())
SketchArc_6 = Sketch_1.addArc(2.43, 2.369999999999995, 1.29, 2.37, 2.429999974807022, 3.510000000000007, True)
SketchArc_6.setName("SketchArc_11")
SketchArc_6.result().setName("SketchArc_11")
SketchArc_6.results()[1].setName("SketchArc_11_2")
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchArc_6.startPoint())
SketchConstraintTangent_7 = Sketch_1.setTangent(SketchLine_2.result(), SketchArc_6.results()[1])
SketchArc_7 = Sketch_1.addArc(2.430000026076943, 1.189999999999999, 2.429999974807022, 3.510000000000007, 3.806722385037566, 3.05736058283826, True)
SketchArc_7.setName("SketchArc_12")
SketchArc_7.result().setName("SketchArc_12")
SketchArc_7.results()[1].setName("SketchArc_12_2")
SketchConstraintCoincidence_26 = Sketch_1.setCoincident(SketchArc_6.endPoint(), SketchArc_7.startPoint())
SketchConstraintTangent_8 = Sketch_1.setTangent(SketchArc_6.results()[1], SketchArc_7.results()[1])
SketchConstraintRadius_5 = Sketch_1.setRadius(SketchArc_6.results()[1], 1.14)
SketchLine_13 = Sketch_1.addLine(2.429999974807509, 2.369762445228637, 2.429999974807022, 3.510000000000007)
SketchLine_13.setName("SketchLine_17")
SketchLine_13.result().setName("SketchLine_17")
SketchLine_13.setAuxiliary(True)
SketchConstraintCoincidence_27 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_13.startPoint())
SketchConstraintCoincidence_28 = Sketch_1.setCoincident(SketchArc_6.endPoint(), SketchLine_13.endPoint())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_13.result())
SketchConstraintRadius_6 = Sketch_1.setRadius(SketchArc_7.results()[1], 2.32)
SketchConstraintCoincidence_29 = Sketch_1.setCoincident(SketchArc_7.endPoint(), SketchLine_8.result())
SketchArc_8 = Sketch_1.addArc(1.552665663222913, 0, 3.806722385037566, 3.05736058283826, 5.35111606287941, 0, True)
SketchArc_8.setName("SketchArc_13")
SketchArc_8.result().setName("SketchArc_13")
SketchArc_8.results()[1].setName("SketchArc_13_2")
SketchConstraintMirror_1_objects = [SketchLine_2.result(), SketchLine_1.result(), SketchLine_4.result(), SketchArc_6.results()[1], SketchArc_7.results()[1], SketchArc_8.results()[1], SketchLine_13.result(), SketchLine_8.result()]
SketchConstraintMirror_1 = Sketch_1.addMirror(SketchLine_5.result(), SketchConstraintMirror_1_objects)
[SketchLine_14, SketchLine_15, SketchLine_16, SketchArc_9, SketchArc_10, SketchArc_11, SketchLine_17, SketchLine_18] = SketchConstraintMirror_1.mirrored()
SketchLine_18.setName("SketchLine_19")
SketchLine_18.result().setName("SketchLine_19")
SketchLine_17.setName("SketchLine_18")
SketchLine_17.result().setName("SketchLine_18")
SketchArc_11.setName("SketchArc_14")
SketchArc_11.result().setName("SketchArc_14")
SketchArc_11.results()[1].setName("SketchArc_14_2")
SketchArc_10.setName("SketchArc_10")
SketchArc_10.result().setName("SketchArc_10")
SketchArc_10.results()[1].setName("SketchArc_10_2")
SketchArc_9.setName("SketchArc_9")
SketchArc_9.result().setName("SketchArc_9")
SketchArc_9.results()[1].setName("SketchArc_9_2")
SketchLine_16.setName("SketchLine_16")
SketchLine_16.result().setName("SketchLine_16")
SketchLine_15.setName("SketchLine_15")
SketchLine_15.result().setName("SketchLine_15")
SketchLine_14.setName("SketchLine_14")
SketchLine_14.result().setName("SketchLine_14")
SketchConstraintCoincidence_30 = Sketch_1.setCoincident(SketchArc_7.endPoint(), SketchArc_8.startPoint())
SketchConstraintTangent_9 = Sketch_1.setTangent(SketchArc_7.results()[1], SketchArc_8.results()[1])
SketchConstraintCoincidence_31 = Sketch_1.setCoincident(SketchLine_5.result(), SketchArc_8.endPoint())
SketchConstraintCoincidence_32 = Sketch_1.setCoincident(SketchArc_8.center(), SketchLine_5.result())
SketchArc_12 = Sketch_1.addArc(2.429999974807509, -2.369762445228637, 1.32, -2.475237825900849, 2.429999974807509, -3.484762445228914, False)
SketchArc_12.setName("SketchArc_15")
SketchArc_12.result().setName("SketchArc_15")
SketchArc_12.results()[1].setName("SketchArc_15_2")
SketchConstraintCoincidence_33 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_17).startPoint(), SketchArc_12.center())
SketchConstraintCoincidence_34 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_16).endPoint(), SketchArc_12.startPoint())
SketchConstraintCoincidence_35 = Sketch_1.setCoincident(SketchLine_17.result(), SketchArc_12.endPoint())
SketchConstraintCoincidence_36 = Sketch_1.setCoincident(SketchArc_5.endPoint(), SketchLine_18.result())
SketchArc_13 = Sketch_1.addArc(2.430961195591607, -1.197350205596287, 3.791202568545627, -3.03637143583555, 2.429999974807033, -3.484762445228914, True)
SketchArc_13.setName("SketchArc_16")
SketchArc_13.result().setName("SketchArc_16")
SketchArc_13.results()[1].setName("SketchArc_16_2")
SketchConstraintCoincidence_37 = Sketch_1.setCoincident(SketchArc_5.endPoint(), SketchArc_13.startPoint())
SketchConstraintTangent_10 = Sketch_1.setTangent(SketchArc_5.results()[1], SketchArc_13.results()[1])
SketchConstraintCoincidence_38 = Sketch_1.setCoincident(SketchArc_13.endPoint(), SketchArc_12.endPoint())
SketchLine_19 = Sketch_1.addLine(1.125, 0, 1.125, 2.370000000000003)
SketchLine_19.setName("SketchLine_20")
SketchLine_19.result().setName("SketchLine_20")
SketchConstraintCoincidence_39 = Sketch_1.setCoincident(SketchLine_19.startPoint(), SketchLine_5.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_19.result())
SketchConstraintCoincidence_40 = Sketch_1.setCoincident(SketchLine_19.endPoint(), SketchLine_3.result())
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchLine_19.endPoint(), SketchLine_2.result(), 0.165, True)
SketchArc_14 = Sketch_1.addArc(2.43, 2.369999999999994, 1.125, 2.370000000000003, 2.429999974807022, 3.674999999999994, True)
SketchArc_14.setName("SketchArc_17")
SketchArc_14.result().setName("SketchArc_17")
SketchArc_14.results()[1].setName("SketchArc_17_2")
SketchConstraintCoincidence_41 = Sketch_1.setCoincident(SketchLine_19.endPoint(), SketchArc_14.startPoint())
SketchArc_15 = Sketch_1.addArc(2.43000002278185, 1.189896770657599, 2.429999974807022, 3.674999999999994, 3.904833906055851, 3.190047534283911, True)
SketchArc_15.setName("SketchArc_18")
SketchArc_15.result().setName("SketchArc_18")
SketchArc_15.results()[1].setName("SketchArc_18_2")
SketchConstraintCoincidence_42 = Sketch_1.setCoincident(SketchArc_14.endPoint(), SketchArc_15.startPoint())
SketchConstraintTangent_11 = Sketch_1.setTangent(SketchArc_14.results()[1], SketchArc_15.results()[1])
SketchArc_16 = Sketch_1.addArc(1.552616124088898, 0, 3.904833906055851, 3.190047534283911, 5.51611606287941, 0, True)
SketchArc_16.setName("SketchArc_19")
SketchArc_16.result().setName("SketchArc_19")
SketchArc_16.results()[1].setName("SketchArc_19_2")
SketchConstraintCoincidence_43 = Sketch_1.setCoincident(SketchArc_15.endPoint(), SketchArc_16.startPoint())
SketchConstraintTangent_12 = Sketch_1.setTangent(SketchArc_15.results()[1], SketchArc_16.results()[1])
SketchConstraintCoincidence_44 = Sketch_1.setCoincident(SketchLine_5.result(), SketchArc_16.endPoint())
SketchConstraintCoincidence_45 = Sketch_1.setCoincident(SketchArc_14.endPoint(), SketchLine_13.result())
SketchConstraintCoincidence_46 = Sketch_1.setCoincident(SketchArc_14.center(), SketchArc_6.center())
SketchConstraintCoincidence_47 = Sketch_1.setCoincident(SketchArc_16.center(), SketchLine_5.result())
SketchConstraintCoincidence_48 = Sketch_1.setCoincident(SketchArc_16.startPoint(), SketchLine_8.result())
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchAPI_Arc(SketchArc_11).endPoint(), SketchArc_16.endPoint(), 0.165, True)
SketchLine_20 = Sketch_1.addLine(1.05, 0, 1.05, 2.199861545438666)
SketchLine_20.setName("SketchLine_22")
SketchLine_20.result().setName("SketchLine_22")
SketchConstraintCoincidence_49 = Sketch_1.setCoincident(SketchLine_20.startPoint(), SketchLine_5.result())
SketchLine_21 = Sketch_1.addLine(1.05, 2.199861545438666, 1.08, 2.370000000000011)
SketchLine_21.setName("SketchLine_23")
SketchLine_21.result().setName("SketchLine_23")
SketchConstraintCoincidence_50 = Sketch_1.setCoincident(SketchLine_20.endPoint(), SketchLine_21.startPoint())
SketchConstraintCoincidence_51 = Sketch_1.setCoincident(SketchLine_21.endPoint(), SketchLine_3.result())
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_20.result())
SketchConstraintDistance_6 = Sketch_1.setDistance(SketchLine_21.endPoint(), SketchLine_20.result(), 0.03, True)
SketchConstraintAngle_5 = Sketch_1.setAngle(SketchLine_20.result(), SketchLine_21.result(), 169.9999999984344)
SketchConstraintDistance_7 = Sketch_1.setDistance(SketchLine_21.endPoint(), SketchLine_19.result(), 0.045, True)
SketchArc_17 = Sketch_1.addArc(2.429999974807022, 2.369762445224826, 1.08, 2.370000000000011, 2.429999974807022, 3.719762440932702, True)
SketchArc_17.setName("SketchArc_23")
SketchArc_17.result().setName("SketchArc_23")
SketchArc_17.results()[1].setName("SketchArc_23_2")
SketchConstraintCoincidence_52 = Sketch_1.setCoincident(SketchLine_21.endPoint(), SketchArc_17.startPoint())
SketchConstraintCoincidence_53 = Sketch_1.setCoincident(SketchArc_17.center(), SketchLine_13.startPoint())
SketchConstraintCoincidence_54 = Sketch_1.setCoincident(SketchArc_17.endPoint(), SketchLine_13.result())
SketchArc_18 = Sketch_1.addArc(2.43000002278185, 1.189896770657599, 2.429999974807022, 3.719762440932702, 3.93144708721947, 3.226039452870244, True)
SketchArc_18.setName("SketchArc_24")
SketchArc_18.result().setName("SketchArc_24")
SketchArc_18.results()[1].setName("SketchArc_24_2")
SketchConstraintCoincidence_55 = Sketch_1.setCoincident(SketchArc_15.center(), SketchArc_18.center())
SketchConstraintCoincidence_56 = Sketch_1.setCoincident(SketchArc_17.endPoint(), SketchArc_18.startPoint())
SketchConstraintCoincidence_57 = Sketch_1.setCoincident(SketchArc_18.endPoint(), SketchLine_8.result())
SketchArc_19 = Sketch_1.addArc(1.552616124088887, 0, 3.93144708721947, 3.226039452870244, 5.560878503552593, 0, True)
SketchArc_19.setName("SketchArc_25")
SketchArc_19.result().setName("SketchArc_25")
SketchArc_19.results()[1].setName("SketchArc_25_2")
SketchConstraintMirror_2_objects = [SketchLine_19.result(), SketchArc_14.results()[1], SketchArc_15.results()[1], SketchArc_16.results()[1], SketchLine_20.result(), SketchArc_19.results()[1], SketchArc_17.results()[1], SketchLine_21.result(), SketchArc_18.results()[1]]
SketchConstraintMirror_2 = Sketch_1.addMirror(SketchLine_5.result(), SketchConstraintMirror_2_objects)
[SketchLine_22, SketchArc_20, SketchArc_21, SketchArc_22, SketchLine_23, SketchArc_23, SketchArc_24, SketchLine_24, SketchArc_25] = SketchConstraintMirror_2.mirrored()
SketchArc_25.setName("SketchArc_28")
SketchArc_25.result().setName("SketchArc_28")
SketchArc_25.results()[1].setName("SketchArc_28_2")
SketchLine_24.setName("SketchLine_25")
SketchLine_24.result().setName("SketchLine_25")
SketchArc_24.setName("SketchArc_27")
SketchArc_24.result().setName("SketchArc_27")
SketchArc_24.results()[1].setName("SketchArc_27_2")
SketchArc_23.setName("SketchArc_26")
SketchArc_23.result().setName("SketchArc_26")
SketchArc_23.results()[1].setName("SketchArc_26_2")
SketchLine_23.setName("SketchLine_24")
SketchLine_23.result().setName("SketchLine_24")
SketchArc_22.setName("SketchArc_22")
SketchArc_22.result().setName("SketchArc_22")
SketchArc_22.results()[1].setName("SketchArc_22_2")
SketchArc_21.setName("SketchArc_21")
SketchArc_21.result().setName("SketchArc_21")
SketchArc_21.results()[1].setName("SketchArc_21_2")
SketchArc_20.setName("SketchArc_20")
SketchArc_20.result().setName("SketchArc_20")
SketchArc_20.results()[1].setName("SketchArc_20_2")
SketchLine_22.setName("SketchLine_21")
SketchLine_22.result().setName("SketchLine_21")
SketchConstraintCoincidence_58 = Sketch_1.setCoincident(SketchArc_18.endPoint(), SketchArc_19.startPoint())
SketchConstraintCoincidence_59 = Sketch_1.setCoincident(SketchArc_19.endPoint(), SketchLine_5.result())
SketchConstraintCoincidence_60 = Sketch_1.setCoincident(SketchArc_19.center(), SketchLine_5.result())
SketchConstraintCoincidence_61 = Sketch_1.setCoincident(SketchArc_19.center(), SketchArc_16.center())
SketchLine_25 = Sketch_1.addLine(2.429999974807022, -3.509999999999994, 2.429999974807022, -3.674999999999994)
SketchLine_25.setName("SketchLine_26")
SketchLine_25.result().setName("SketchLine_26")
SketchConstraintCoincidence_62 = Sketch_1.setCoincident(SketchAPI_Arc(SketchArc_9).endPoint(), SketchLine_25.startPoint())
SketchConstraintCoincidence_63 = Sketch_1.setCoincident(SketchAPI_Arc(SketchArc_20).endPoint(), SketchLine_25.endPoint())
SketchLine_26 = Sketch_1.addLine(2.429999974807022, 3.510000000000007, 2.429999974807022, 3.674999999999994)
SketchLine_26.setName("SketchLine_27")
SketchLine_26.result().setName("SketchLine_27")
SketchConstraintCoincidence_64 = Sketch_1.setCoincident(SketchArc_6.endPoint(), SketchLine_26.startPoint())
SketchConstraintCoincidence_65 = Sketch_1.setCoincident(SketchArc_14.endPoint(), SketchLine_26.endPoint())
SketchLine_27 = Sketch_1.addLine(2.429999974807022, 3.719762440932702, 2.429999974807022, 3.674999999999994)
SketchLine_27.setName("SketchLine_28")
SketchLine_27.result().setName("SketchLine_28")
SketchConstraintCoincidence_66 = Sketch_1.setCoincident(SketchArc_17.endPoint(), SketchLine_27.startPoint())
SketchConstraintCoincidence_67 = Sketch_1.setCoincident(SketchArc_14.endPoint(), SketchLine_27.endPoint())
SketchLine_28 = Sketch_1.addLine(2.429999974807022, 3.510000000000032, 2.429999974807022, 3.484762445224835)
SketchLine_28.setName("SketchLine_29")
SketchLine_28.result().setName("SketchLine_29")
SketchConstraintCoincidence_68 = Sketch_1.setCoincident(SketchArc_6.endPoint(), SketchLine_28.startPoint())
SketchConstraintCoincidence_69 = Sketch_1.setCoincident(SketchLine_28.endPoint(), SketchArc_1.results()[1])
SketchConstraintCoincidence_70 = Sketch_1.setCoincident(SketchLine_28.endPoint(), SketchLine_13.result())
SketchLine_29 = Sketch_1.addLine(2.429999974807022, -3.509999999999994, 2.429999974807033, -3.484762445228914)
SketchLine_29.setName("SketchLine_30")
SketchLine_29.result().setName("SketchLine_30")
SketchConstraintCoincidence_71 = Sketch_1.setCoincident(SketchAPI_Arc(SketchArc_9).endPoint(), SketchLine_29.startPoint())
SketchConstraintCoincidence_72 = Sketch_1.setCoincident(SketchArc_12.endPoint(), SketchLine_29.endPoint())
SketchLine_30 = Sketch_1.addLine(2.429999974807022, -3.674999999999994, 2.429999974807022, -3.71976244093269)
SketchLine_30.setName("SketchLine_31")
SketchLine_30.result().setName("SketchLine_31")
SketchConstraintCoincidence_73 = Sketch_1.setCoincident(SketchAPI_Arc(SketchArc_20).endPoint(), SketchLine_30.startPoint())
SketchConstraintCoincidence_74 = Sketch_1.setCoincident(SketchAPI_Arc(SketchArc_24).endPoint(), SketchLine_30.endPoint())
SketchLine_31 = Sketch_1.addLine(2.429999974807509, 2.369762445228637, 1.871489087529827, 3.334796428456965)
SketchLine_31.setName("SketchLine_32")
SketchLine_31.result().setName("SketchLine_32")
SketchLine_31.setAuxiliary(True)
SketchConstraintCoincidence_75 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_31.startPoint())
SketchConstraintCoincidence_76 = Sketch_1.setCoincident(SketchLine_31.endPoint(), SketchArc_1.results()[1])
SketchConstraintAngle_6 = Sketch_1.setAngle(SketchLine_31.result(), SketchLine_3.result(), 59.93999999988066)
SketchLine_32 = Sketch_1.addLine(1.871489087529827, 3.334796428456965, 1.871489087529827, 3.598813126524882)
SketchLine_32.setName("SketchLine_33")
SketchLine_32.result().setName("SketchLine_33")
SketchConstraintCoincidence_77 = Sketch_1.setCoincident(SketchLine_31.endPoint(), SketchLine_32.startPoint())
SketchConstraintCoincidence_78 = Sketch_1.setCoincident(SketchLine_32.endPoint(), SketchArc_17.results()[1])
SketchConstraintVertical_6 = Sketch_1.setVertical(SketchLine_32.result())
SketchLine_33 = Sketch_1.addLine(1.871489087529829, -3.334796428460793, 1.871489087529829, -3.598813126524861)
SketchLine_33.setName("SketchLine_34")
SketchLine_33.result().setName("SketchLine_34")
SketchConstraintCoincidence_79 = Sketch_1.setCoincident(SketchLine_33.startPoint(), SketchArc_12.results()[1])
SketchConstraintCoincidence_80 = Sketch_1.setCoincident(SketchLine_33.endPoint(), SketchArc_24.results()[1])
SketchConstraintCoincidence_81 = Sketch_1.setCoincident(SketchLine_33.startPoint(), SketchLine_32.result())
SketchConstraintVertical_7 = Sketch_1.setVertical(SketchLine_33.result())
model.do()
Extrusion_1_objects = [model.selection("WIRE", "Sketch_1/Face-SketchLine_5f-SketchArc_1_2r-SketchLine_33f-SketchArc_11_2f-SketchLine_2f-SketchLine_14r-SketchArc_9_2f-SketchLine_34r-SketchArc_15_2r-SketchLine_16r-SketchLine_15f-SketchLine_1r_wire"), model.selection("WIRE", "Sketch_1/Face-SketchArc_2_2r-SketchArc_3_2r-SketchLine_11f-SketchArc_4_2r-SketchArc_5_2r-SketchArc_16_2r-SketchLine_30r-SketchArc_10_2f-SketchArc_14_2f-SketchArc_13_2f-SketchArc_12_2f-SketchLine_29f-SketchArc_1_2r_wire"), model.selection("WIRE", "Sketch_1/Face-SketchLine_20f-SketchArc_17_2r-SketchLine_33f-SketchArc_23_2f-SketchLine_23r-SketchLine_22r-SketchLine_24f-SketchLine_25f-SketchArc_27_2f-SketchLine_34r-SketchArc_20_2r-SketchLine_21r_wire"), model.selection("WIRE", "Sketch_1/Face-SketchArc_19_2r-SketchArc_22_2r-SketchArc_21_2r-SketchLine_31f-SketchArc_28_2f-SketchArc_26_2f-SketchArc_25_2f-SketchArc_24_2f-SketchLine_28f-SketchArc_18_2r_wire"), model.selection("WIRE", "Sketch_1/Face-SketchLine_28r-SketchArc_23_2f-SketchLine_33r-SketchArc_17_2r_wire"), model.selection("WIRE", "Sketch_1/Face-SketchLine_29r-SketchArc_11_2f-SketchLine_33r-SketchArc_1_2r_wire"), model.selection("FACE", "Sketch_1/Face-SketchArc_9_2f-SketchLine_30f-SketchArc_15_2r-SketchLine_34f"), model.selection("WIRE", "Sketch_1/Face-SketchLine_34f-SketchArc_27_2f-SketchLine_31r-SketchArc_20_2r_wire")]
Extrusion_1 = model.addExtrusion(Part_1_doc, Extrusion_1_objects, model.selection(), 0.1785, 0.1785)
Extrusion_2_objects = [model.selection("WIRE", "Sketch_1/Face-SketchArc_13_2r-SketchArc_14_2r-SketchArc_10_2r-SketchLine_26f-SketchArc_21_2f-SketchArc_22_2f-SketchArc_19_2f-SketchArc_18_2f-SketchLine_27r-SketchArc_12_2r_wire"), model.selection("WIRE", "Sketch_1/Face-SketchArc_2_2r-SketchArc_3_2r-SketchLine_11f-SketchArc_4_2r-SketchArc_5_2r-SketchArc_16_2r-SketchLine_30r-SketchArc_10_2f-SketchArc_14_2f-SketchArc_13_2f-SketchArc_12_2f-SketchLine_29f-SketchArc_1_2r_wire"), model.selection("FACE", "Sketch_1/Face-SketchArc_19_2r-SketchArc_22_2r-SketchArc_21_2r-SketchLine_31f-SketchArc_28_2f-SketchArc_26_2f-SketchArc_25_2f-SketchArc_24_2f-SketchLine_28f-SketchArc_18_2r")]
Extrusion_2 = model.addExtrusion(Part_1_doc, Extrusion_2_objects, model.selection(), model.selection("FACE", "Extrusion_1_2_2/From_Face"), -0.05, model.selection("FACE", "Extrusion_1_2_2/From_Face"), 0)
Extrusion_3_objects = [model.selection("FACE", "Sketch_1/Face-SketchArc_13_2r-SketchArc_14_2r-SketchArc_10_2r-SketchLine_26f-SketchArc_21_2f-SketchArc_22_2f-SketchArc_19_2f-SketchArc_18_2f-SketchLine_27r-SketchArc_12_2r"), model.selection("FACE", "Sketch_1/Face-SketchArc_19_2r-SketchArc_22_2r-SketchArc_21_2r-SketchLine_31f-SketchArc_28_2f-SketchArc_26_2f-SketchArc_25_2f-SketchArc_24_2f-SketchLine_28f-SketchArc_18_2r"), model.selection("WIRE", "Sketch_1/Face-SketchArc_2_2r-SketchArc_3_2r-SketchLine_11f-SketchArc_4_2r-SketchArc_5_2r-SketchArc_16_2r-SketchLine_30r-SketchArc_10_2f-SketchArc_14_2f-SketchArc_13_2f-SketchArc_12_2f-SketchLine_29f-SketchArc_1_2r_wire")]
Extrusion_3 = model.addExtrusion(Part_1_doc, Extrusion_3_objects, model.selection(), model.selection("FACE", "Extrusion_1_1_1/To_Face"), 0.05, model.selection("FACE", "Extrusion_1_2_1/To_Face"), 0)
Extrusion_4_objects = [model.selection("FACE", "Sketch_1/Face-SketchLine_28r-SketchArc_23_2f-SketchLine_33r-SketchArc_17_2r"), model.selection("WIRE", "Sketch_1/Face-SketchLine_29r-SketchArc_11_2f-SketchLine_33r-SketchArc_1_2r_wire"), model.selection("WIRE", "Sketch_1/Face-SketchLine_27f-SketchArc_17_2f-SketchLine_33r-SketchArc_11_2r_wire"), model.selection("FACE", "Sketch_1/Face-SketchLine_34f-SketchArc_27_2f-SketchLine_31r-SketchArc_20_2r"), model.selection("FACE", "Sketch_1/Face-SketchLine_34f-SketchArc_20_2f-SketchLine_26r-SketchArc_9_2r"), model.selection("WIRE", "Sketch_1/Face-SketchArc_9_2f-SketchLine_30f-SketchArc_15_2r-SketchLine_34f_wire")]
Extrusion_4 = model.addExtrusion(Part_1_doc, Extrusion_4_objects, model.selection(), model.selection("FACE", "Extrusion_1_1_2/To_Face"), 0.15, model.selection("FACE", "Extrusion_1_1_2/To_Face"), 0)
Extrusion_4.setName("Extrusion_6")
Extrusion_4.result().setName("Extrusion_6_1")
Extrusion_4.result().subResult(0).setName("Extrusion_6_1_1")
Extrusion_4.result().subResult(1).setName("Extrusion_6_1_2")
Extrusion_4.result().subResult(2).setName("Extrusion_6_1_3")
Extrusion_4.results()[1].setName("Extrusion_6_2")
Extrusion_4.results()[1].subResult(0).setName("Extrusion_6_2_1")
Extrusion_4.results()[1].subResult(1).setName("Extrusion_6_2_2")
Extrusion_4.results()[1].subResult(2).setName("Extrusion_6_2_3")
Axis_4 = model.addAxis(Part_1_doc, model.selection("VERTEX", "[Extrusion_3_1_3/To_Face][Extrusion_3_1_3/Generated_Face&Sketch_1/SketchLine_28][Extrusion_3_1_3/Generated_Face&Sketch_1/SketchArc_24_2]"), model.selection("VERTEX", "[Extrusion_6_1_1/To_Face][Extrusion_6_1_1/Generated_Face&Sketch_1/SketchArc_23_2][Extrusion_6_1_1/Generated_Face&Sketch_1/SketchLine_33]"))
Plane_4 = model.addPlane(Part_1_doc, model.selection("EDGE", "Axis_1"), model.selection("VERTEX", "[Extrusion_6_1_3/Generated_Face&Sketch_1/SketchArc_1_2][Extrusion_6_1_3/To_Face][Extrusion_6_1_3/Generated_Face&Sketch_1/SketchLine_33]"), False)
Partition_1_objects = [model.selection("COMPSOLID", "Extrusion_6_1"), model.selection("FACE", "Plane_1"), model.selection("COMPSOLID", "Extrusion_6_2")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
model.do()

# check names of partition results
assert(Partition_1.name() + "_1" == Partition_1.result().name())
assert(Partition_1.name() + "_2" == Partition_1.results()[1].name())

model.end()

assert(model.checkPythonDump())
