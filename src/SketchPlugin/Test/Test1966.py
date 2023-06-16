# Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

"""
    Test1966.py
    Test case for issue #1966 "Sketch solving is 25 times slower between two studies with middle point"
"""

from salome.shaper import model
from timeit import default_timer as timer
import math

# convenient ratio of time moving point to time creating sketch
MOVE_BUILD_RATIO = 0.2
# tolerance for comparison of reals
TOLERANCE = 1.e-7

#=========================================================================
# Auxiliary functions
#=========================================================================
def angle(theStartPoint1, theEndPoint1, theStartPoint2, theEndPoint2):
    # subroutine to calculate angle between two lines
    aDirX1 = theEndPoint1.x() - theStartPoint1.x()
    aDirY1 = theEndPoint1.y() - theStartPoint1.y()
    aLen1 = math.hypot(aDirX1, aDirY1)
    aDirX2 = theEndPoint2.x() - theStartPoint2.x()
    aDirY2 = theEndPoint2.y() - theStartPoint2.y()
    aLen2 = math.hypot(aDirX2, aDirY2)

    aDot = aDirX1 * aDirX2 + aDirY1 * aDirY2

    anAngle = math.acos(aDot / aLen1 / aLen2)
    return round(anAngle * 180. / math.pi, 6)

#=========================================================================
# Initial model
#=========================================================================

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "ep_jupe", "0.188")

# time to build sketch
sketchTime = timer()

Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(-6.376, 9.025, -6.376, -2.25)
SketchLine_2 = Sketch_1.addLine(-6.376, -2.25, -6.538, -2.4)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-6.538, -2.4, -6.538, -3.45)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(-6.538, -3.45, -7.782, -3.45)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(-7.782, -3.45, -7.782, -6.45)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(-7.782, -6.45, 0, -6.45)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(0, -6.45, 0, -3.45)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(0, -3.45, -6.188, -3.45)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_1.addLine(-6.188, -3.45, -6.188, 9.025)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_9.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_7.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_8.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_4.result())
SketchLine_10 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OZ"))
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_8.startPoint(), SketchLine_10.result())
SketchLine_11 = Sketch_1.addLine(-6.376, 9.025, -6.538, 9.187)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_11.startPoint())
SketchLine_12 = Sketch_1.addLine(-6.538, 9.187, -6.538, 10.256)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchLine_13 = Sketch_1.addLine(-6.538, 10.256, -6.188, 10.256)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchLine_14 = Sketch_1.addLine(-6.188, 9.025, -6.084266646261868, 9.523738867722166)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_14.startPoint())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_13.result())
SketchConstraintVertical_6 = Sketch_1.setVertical(SketchLine_12.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_7.result(), 3)
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_2.startPoint(), SketchLine_9.result(), "ep_jupe")
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_4.startPoint(), SketchLine_9.result(), 0.35)
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_8.endPoint(), SketchLine_10.result(), 6.188)
SketchLine_15 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OX"))
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchLine_1.startPoint(), SketchLine_15.result(), 9.025)
SketchLine_16 = Sketch_1.addLine(-6.376, 9.025, -6.188, 9.025)
SketchLine_16.setAuxiliary(True)
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_16.startPoint())
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_11.startPoint(), SketchLine_16.startPoint())
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_16.endPoint())
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_14.startPoint(), SketchLine_16.endPoint())
SketchConstraintHorizontal_5 = Sketch_1.setHorizontal(SketchLine_16.result())
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchLine_2.startPoint(), SketchLine_15.result(), 2.25)
SketchConstraintDistance_6 = Sketch_1.setDistance(SketchLine_3.startPoint(), SketchLine_15.result(), 2.4)
SketchConstraintDistance_7 = Sketch_1.setDistance(SketchLine_4.startPoint(), SketchLine_15.result(), 3.45)
SketchLine_17 = Sketch_1.addLine(-6.282, 9.025, -6.282, -4.050005366189866)
SketchLine_17.setAuxiliary(True)
SketchConstraintVertical_7 = Sketch_1.setVertical(SketchLine_17.result())
SketchPoint_1 = Sketch_1.addPoint(-6.282, 9.025)
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchLine_17.startPoint(), SketchPoint_1.coordinates())
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_16.result())
SketchConstraintMiddle_1 = Sketch_1.setMiddlePoint(SketchPoint_1.coordinates(), SketchLine_16.result())
SketchLine_18 = Sketch_1.addLine(-5.532, -4.050005366189866, -7.032, -4.050005366189866)
SketchLine_19 = Sketch_1.addLine(-7.032, -4.050005366189866, -7.032, -5.850005366189865)
SketchLine_20 = Sketch_1.addLine(-7.032, -5.850005366189865, -5.532, -5.850005366189865)
SketchLine_21 = Sketch_1.addLine(-5.532, -5.850005366189865, -5.532, -4.050005366189866)
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchLine_21.endPoint(), SketchLine_18.startPoint())
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchLine_19.endPoint(), SketchLine_20.startPoint())
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchLine_20.endPoint(), SketchLine_21.startPoint())
SketchConstraintHorizontal_6 = Sketch_1.setHorizontal(SketchLine_18.result())
SketchConstraintVertical_8 = Sketch_1.setVertical(SketchLine_19.result())
SketchConstraintHorizontal_7 = Sketch_1.setHorizontal(SketchLine_20.result())
SketchConstraintVertical_9 = Sketch_1.setVertical(SketchLine_21.result())
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_20.result(), 1.5)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_19.result(), 1.8)
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchLine_17.endPoint(), SketchLine_18.result())
SketchConstraintMiddle_2 = Sketch_1.setMiddlePoint(SketchLine_18.result(), SketchLine_17.endPoint())
SketchConstraintDistance_8 = Sketch_1.setDistance(SketchLine_17.endPoint(), SketchLine_5.result(), 1.5)
SketchConstraintDistance_9 = Sketch_1.setDistance(SketchLine_11.endPoint(), SketchLine_17.result(), 0.256)
SketchConstraintDistance_10 = Sketch_1.setDistance(SketchLine_11.endPoint(), SketchLine_15.result(), 9.187)
SketchConstraintDistance_11 = Sketch_1.setDistance(SketchLine_12.endPoint(), SketchLine_15.result(), 10.256)
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_13.result(), 0.35)
SketchLine_22 = Sketch_1.addLine(-6.188, 10.256, -4.938, 10.97786688777511)
SketchConstraintCoincidence_26 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_22.startPoint())
SketchArc_1 = Sketch_1.addArc(-4.794548536090929, 9.26204592494933, -6.084266646261868, 9.523738867722166, -5.676829966640709, 10.23848613167324, True)
SketchConstraintCoincidence_27 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchArc_1.startPoint())
SketchLine_23 = Sketch_1.addLine(-5.676829966640709, 10.23848613167324, -4.794548536090929, 9.26204592494933)
SketchLine_23.setAuxiliary(True)
SketchConstraintCoincidence_28 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_23.startPoint())
SketchConstraintCoincidence_39 = Sketch_1.setCoincident(SketchLine_23.endPoint(), SketchArc_1.center())
SketchArc_2 = Sketch_1.addArc(0, 2.425, -4.938, 10.97786688777511, 0, 12.301, True)
SketchConstraintCoincidence_30 = Sketch_1.setCoincident(SketchArc_2.center(), SketchLine_10.result())
SketchConstraintCoincidence_31 = Sketch_1.setCoincident(SketchLine_22.endPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_32 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_10.result())
SketchConstraintDistance_12 = Sketch_1.setDistance(SketchArc_2.center(), SketchLine_15.result(), 2.425)
SketchLine_24 = Sketch_1.addLine(0, 12.301, 0, 12.137)
SketchConstraintCoincidence_33 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_24.startPoint())
SketchArc_3 = Sketch_1.addArc(0, 2.425, -5.877900790328188, 10.25907418263698, 2.067397663006541, 11.99831201324782, True)
SketchArc_3.setName("SketchArc_4")
SketchArc_3.result().setName("SketchArc_4")
SketchArc_3.results()[1].setName("SketchArc_4_2")
SketchArc_3.setAuxiliary(True)
SketchConstraintCoincidence_34 = Sketch_1.setCoincident(SketchArc_2.center(), SketchArc_3.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_3.results()[1], 9.794)
SketchPoint_2 = Sketch_1.addPoint(0, 12.219)
SketchConstraintCoincidence_35 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_24.result())
SketchConstraintCoincidence_36 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchArc_3.results()[1])
SketchConstraintMiddle_3 = Sketch_1.setMiddlePoint(SketchPoint_2.coordinates(), SketchLine_24.result())
SketchLine_25 = Sketch_1.addLine(-5.676829966640709, 10.23848613167324, -5.417460848824602, 10.45671600291324)
SketchConstraintCoincidence_37 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_25.startPoint())
SketchConstraintCoincidence_38 = Sketch_1.setCoincident(SketchLine_23.startPoint(), SketchLine_25.startPoint())
SketchLine_26 = Sketch_1.addLine(-5.417460848824602, 10.45671600291324, -5.143396378647499, 10.65615482129426)
SketchConstraintCoincidence_39 = Sketch_1.setCoincident(SketchLine_25.endPoint(), SketchLine_26.startPoint())
SketchLine_27 = Sketch_1.addLine(-5.143396378647499, 10.65615482129426, -4.856000000000008, 10.83583872155447)
SketchConstraintCoincidence_40 = Sketch_1.setCoincident(SketchLine_26.endPoint(), SketchLine_27.startPoint())
SketchArc_4 = Sketch_1.addArc(0, 2.425, -4.856, 10.83583872155447, 0, 12.137, True)
SketchArc_4.setName("SketchArc_5")
SketchArc_4.result().setName("SketchArc_5")
SketchArc_4.results()[1].setName("SketchArc_5_2")
SketchConstraintCoincidence_41 = Sketch_1.setCoincident(SketchArc_2.center(), SketchArc_4.center())
SketchConstraintCoincidence_42 = Sketch_1.setCoincident(SketchArc_3.center(), SketchArc_4.center())
SketchConstraintCoincidence_43 = Sketch_1.setCoincident(SketchLine_27.endPoint(), SketchArc_4.startPoint())
SketchConstraintCoincidence_44 = Sketch_1.setCoincident(SketchLine_24.endPoint(), SketchArc_4.endPoint())
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_24.result(), "0.082*2")
SketchLine_28 = Sketch_1.addLine(-4.938, 10.97786688777511, 0, 2.425)
SketchLine_28.setAuxiliary(True)
SketchConstraintCoincidence_45 = Sketch_1.setCoincident(SketchLine_22.endPoint(), SketchLine_28.startPoint())
SketchConstraintCoincidence_46 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_28.startPoint())
SketchConstraintCoincidence_47 = Sketch_1.setCoincident(SketchArc_2.center(), SketchLine_28.endPoint())
SketchConstraintCoincidence_48 = Sketch_1.setCoincident(SketchArc_3.center(), SketchLine_28.endPoint())
SketchConstraintCoincidence_49 = Sketch_1.setCoincident(SketchArc_4.center(), SketchLine_28.endPoint())
SketchConstraintCoincidence_50 = Sketch_1.setCoincident(SketchLine_28.result(), SketchLine_27.endPoint())
SketchConstraintAngle_1 = Sketch_1.setAngleBackward(SketchLine_28.result(), SketchLine_10.result(), 30)
SketchLine_29 = Sketch_1.addLine(-5.143396378647499, 10.65615482129426, 0, 2.425)
SketchLine_29.setAuxiliary(True)
SketchConstraintCoincidence_51 = Sketch_1.setCoincident(SketchArc_2.center(), SketchLine_29.endPoint())
SketchConstraintCoincidence_52 = Sketch_1.setCoincident(SketchArc_3.center(), SketchLine_29.endPoint())
SketchConstraintCoincidence_53 = Sketch_1.setCoincident(SketchArc_4.center(), SketchLine_29.endPoint())
SketchConstraintCoincidence_54 = Sketch_1.setCoincident(SketchLine_28.endPoint(), SketchLine_29.endPoint())
SketchConstraintCoincidence_55 = Sketch_1.setCoincident(SketchLine_26.endPoint(), SketchLine_29.startPoint())
SketchConstraintAngle_2 = Sketch_1.setAngleBackward(SketchLine_29.result(), SketchLine_10.result(), 32)
SketchLine_30 = Sketch_1.addLine(-5.417460848824602, 10.45671600291324, 0, 2.425)
SketchLine_30.setAuxiliary(True)
SketchConstraintCoincidence_56 = Sketch_1.setCoincident(SketchArc_2.center(), SketchLine_30.endPoint())
SketchConstraintCoincidence_57 = Sketch_1.setCoincident(SketchArc_3.center(), SketchLine_30.endPoint())
SketchConstraintCoincidence_58 = Sketch_1.setCoincident(SketchArc_4.center(), SketchLine_30.endPoint())
SketchConstraintCoincidence_59 = Sketch_1.setCoincident(SketchLine_28.endPoint(), SketchLine_30.endPoint())
SketchConstraintCoincidence_60 = Sketch_1.setCoincident(SketchLine_29.endPoint(), SketchLine_30.endPoint())
SketchConstraintCoincidence_61 = Sketch_1.setCoincident(SketchLine_25.endPoint(), SketchLine_30.startPoint())
SketchConstraintAngle_3 = Sketch_1.setAngleBackward(SketchLine_30.result(), SketchLine_10.result(), 34)
SketchLine_31 = Sketch_1.addLine(0, 2.425, -5.676829966640709, 10.23848613167324)
SketchLine_31.setName("SketchLine_35")
SketchLine_31.result().setName("SketchLine_35")
SketchLine_31.setAuxiliary(True)
SketchConstraintCoincidence_62 = Sketch_1.setCoincident(SketchArc_2.center(), SketchLine_31.startPoint())
SketchConstraintCoincidence_63 = Sketch_1.setCoincident(SketchArc_3.center(), SketchLine_31.startPoint())
SketchConstraintCoincidence_64 = Sketch_1.setCoincident(SketchArc_4.center(), SketchLine_31.startPoint())
SketchConstraintCoincidence_65 = Sketch_1.setCoincident(SketchLine_28.endPoint(), SketchLine_31.startPoint())
SketchConstraintCoincidence_66 = Sketch_1.setCoincident(SketchLine_29.endPoint(), SketchLine_31.startPoint())
SketchConstraintCoincidence_67 = Sketch_1.setCoincident(SketchLine_30.endPoint(), SketchLine_31.startPoint())
SketchConstraintCoincidence_68 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_31.endPoint())
SketchConstraintAngle_4 = Sketch_1.setAngleBackward(SketchLine_31.result(), SketchLine_10.result(), 36)
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_1.results()[1], 1.316)
SketchLine_32 = Sketch_1.addLine(-4.794548536090929, 9.26204592494933, -4.794548536090929, 9.764655753971969)
SketchLine_32.setName("SketchLine_38")
SketchLine_32.result().setName("SketchLine_38")
SketchLine_32.setAuxiliary(True)
SketchConstraintCoincidence_69 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_32.startPoint())
SketchConstraintCoincidence_70 = Sketch_1.setCoincident(SketchLine_23.endPoint(), SketchLine_32.startPoint())
SketchConstraintVertical_10 = Sketch_1.setVertical(SketchLine_32.result())
SketchConstraintAngle_5 = Sketch_1.setAngleBackward(SketchLine_23.result(), SketchLine_32.result(), 42.1)
SketchLine_33 = Sketch_1.addLine(-4.794548536090929, 9.26204592494933, -5.10267883886141, 9.26204592494933)
SketchLine_33.setName("SketchLine_39")
SketchLine_33.result().setName("SketchLine_39")
SketchLine_33.setAuxiliary(True)
SketchConstraintCoincidence_71 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_33.startPoint())
SketchConstraintCoincidence_72 = Sketch_1.setCoincident(SketchLine_23.endPoint(), SketchLine_33.startPoint())
SketchConstraintCoincidence_73 = Sketch_1.setCoincident(SketchLine_32.startPoint(), SketchLine_33.startPoint())
SketchConstraintHorizontal_8 = Sketch_1.setHorizontal(SketchLine_33.result())
SketchLine_34 = Sketch_1.addLine(-6.084266646261868, 9.523738867722166, -4.794548536090929, 9.26204592494933)
SketchLine_34.setName("SketchLine_40")
SketchLine_34.result().setName("SketchLine_40")
SketchLine_34.setAuxiliary(True)
SketchConstraintCoincidence_74 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_34.startPoint())
SketchConstraintCoincidence_75 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_34.startPoint())
SketchConstraintCoincidence_76 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_34.endPoint())
SketchConstraintCoincidence_77 = Sketch_1.setCoincident(SketchLine_23.endPoint(), SketchLine_34.endPoint())
SketchConstraintCoincidence_78 = Sketch_1.setCoincident(SketchLine_32.startPoint(), SketchLine_34.endPoint())
SketchConstraintCoincidence_79 = Sketch_1.setCoincident(SketchLine_33.startPoint(), SketchLine_34.endPoint())
SketchConstraintCoincidence_80 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchLine_10.result())
SketchConstraintAngle_6 = Sketch_1.setAngleBackward(SketchLine_33.result(), SketchLine_34.result(), 11.47)
SketchLine_35 = Sketch_1.addLine(-5.676829966640709, 10.23848613167324, -5.756768760952487, 10.34851244290823)
SketchLine_35.setName("SketchLine_36")
SketchLine_35.result().setName("SketchLine_36")
SketchLine_35.setAuxiliary(True)
SketchConstraintCoincidence_81 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_35.startPoint())
SketchConstraintCoincidence_82 = Sketch_1.setCoincident(SketchLine_23.startPoint(), SketchLine_35.startPoint())
SketchConstraintCoincidence_83 = Sketch_1.setCoincident(SketchLine_25.startPoint(), SketchLine_35.startPoint())
SketchConstraintCoincidence_84 = Sketch_1.setCoincident(SketchLine_31.endPoint(), SketchLine_35.startPoint())
SketchConstraintCoincidence_85 = Sketch_1.setCoincident(SketchLine_35.endPoint(), SketchArc_3.results()[1])
SketchLine_36 = Sketch_1.addLine(-5.417460848824602, 10.45671600291324, -5.476735296592498, 10.54459398560407)
SketchLine_36.setName("SketchLine_37")
SketchLine_36.result().setName("SketchLine_37")
SketchLine_36.setAuxiliary(True)
SketchConstraintCoincidence_86 = Sketch_1.setCoincident(SketchLine_25.endPoint(), SketchLine_36.startPoint())
SketchConstraintCoincidence_87 = Sketch_1.setCoincident(SketchLine_26.startPoint(), SketchLine_36.startPoint())
SketchConstraintCoincidence_88 = Sketch_1.setCoincident(SketchLine_30.startPoint(), SketchLine_36.startPoint())
SketchConstraintCoincidence_89 = Sketch_1.setCoincident(SketchLine_36.endPoint(), SketchArc_3.results()[1])
SketchLine_37 = Sketch_1.addLine(-5.143396378647499, 10.65615482129426, -5.190029273900022, 10.73078305375603)
SketchLine_37.setName("SketchLine_41")
SketchLine_37.result().setName("SketchLine_41")
SketchLine_37.setAuxiliary(True)
SketchConstraintCoincidence_90 = Sketch_1.setCoincident(SketchLine_26.endPoint(), SketchLine_37.startPoint())
SketchConstraintCoincidence_91 = Sketch_1.setCoincident(SketchLine_27.startPoint(), SketchLine_37.startPoint())
SketchConstraintCoincidence_92 = Sketch_1.setCoincident(SketchLine_29.startPoint(), SketchLine_37.startPoint())
SketchConstraintCoincidence_93 = Sketch_1.setCoincident(SketchLine_37.endPoint(), SketchArc_3.results()[1])
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_35.result(), SketchLine_31.result())
SketchConstraintParallel_2 = Sketch_1.setParallel(SketchLine_36.result(), SketchLine_30.result())
SketchConstraintParallel_3 = Sketch_1.setParallel(SketchLine_37.result(), SketchLine_29.result())
SketchConstraintLength_6 = Sketch_1.setLength(SketchLine_35.result(), 0.136)
SketchConstraintLength_7 = Sketch_1.setLength(SketchLine_36.result(), 0.106)
SketchConstraintLength_8 = Sketch_1.setLength(SketchLine_37.result(), 0.088)
model.do()

sketchTime = timer() - sketchTime
print("Sketch creation time: {0}".format(sketchTime))
expectedTime = MOVE_BUILD_RATIO * sketchTime
averageTime = 0
nbMoves = 0

#=========================================================================
# Move point under the Middle point constraint and check execution time.
# This point is fully constrained, so its coordinates should not be changed.
#=========================================================================
aPoint = SketchPoint_1.coordinates()
X = aPoint.x()
Y = aPoint.y()
deltas = [(0.0, 0.1), (0.1, 0.0), (-0.1, 0.1), (1.0, 0.0), (0.0, -1.0), (-0.5, -0.2), (-0.3, 0.3)]
for deltaX, deltaY in deltas:
    movementTime = timer()
    aPoint.setValue(X, Y + 0.1)
    model.do()
    movementTime = timer() - movementTime
    assert movementTime < expectedTime, "Time to move point {0} is greater than expected {1}".format(movementTime, expectedTime)
    assert math.fabs(aPoint.x() - X) < TOLERANCE and math.fabs(aPoint.y() - Y) < TOLERANCE, "({0}, {1}) != ({2}, {3})".format(aPoint.x(), aPoint.y(), X, Y)
    averageTime += movementTime
    nbMoves += 1
print("Movement average time: {0}".format(averageTime / nbMoves))

#=========================================================================
# Change value of SketchConstraintAngle_6 and check execution time.
# Check constrained objects are satisfy this constraint.
# Check the middle point stays on its place.
#=========================================================================
averageTime = 0
nbMoves = 0
for ang in range(12, 30):
    movementTime = timer()
    SketchConstraintAngle_6.feature().real("AngleValue").setValue(ang)
    model.do()
    movementTime = timer() - movementTime
    assert movementTime < expectedTime, "Time to change angle {0} is greater than expected {1}".format(movementTime, expectedTime)
    curAngle = angle(SketchLine_33.startPoint(), SketchLine_33.endPoint(), SketchLine_34.endPoint(), SketchLine_34.startPoint())
    assert math.fabs(curAngle - ang) < TOLERANCE, "angle {0} != {1} expected".format(curAngle, ang)
    averageTime += movementTime
    nbMoves += 1
print("Changing angle average time: {0}".format(averageTime / nbMoves))

model.end()


assert(model.checkPythonDump())
