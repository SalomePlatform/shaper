# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
    Test1673.py
    Test case for issue #1673 "Drag and drop the extremity of an arc is too slow"
"""

from salome.shaper import model
from timeit import default_timer as timer
import math

# convenient ratio of time moving point to time creating sketch
MOVE_BUILD_RATIO = 0.1
# tolerance for comparison of reals
TOLERANCE = 1.e-7

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "ep_jupe", "0.188")
model.addParameter(Part_1_doc, "width_gallery", "1.5")
model.addParameter(Part_1_doc, "h_bas_dome", "9.025")
model.addParameter(Part_1_doc, "r_dome", "9.794")
model.addParameter(Part_1_doc, "demi_ep_dome", "0.082")

# time to build sketch
sketchTime = timer()

Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(-6.538, -3.45, -7.782, -3.45)
SketchLine_1.setName("SketchLine_2")
SketchLine_1.result().setName("SketchLine_2")
SketchLine_2 = Sketch_1.addLine(-7.782, -3.45, -7.782, -6.45)
SketchLine_2.setName("SketchLine_3")
SketchLine_2.result().setName("SketchLine_3")
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_1.setName("SketchConstraintCoincidence_2")
SketchLine_3 = Sketch_1.addLine(-7.782, -6.45, 0, -6.45)
SketchLine_3.setName("SketchLine_4")
SketchLine_3.result().setName("SketchLine_4")
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_2.setName("SketchConstraintCoincidence_3")
SketchLine_4 = Sketch_1.addLine(0, -6.45, 0, -3.45)
SketchLine_4.setName("SketchLine_5")
SketchLine_4.result().setName("SketchLine_5")
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_3.setName("SketchConstraintCoincidence_4")
SketchLine_5 = Sketch_1.addLine(0, -3.45, -6.188, -3.45)
SketchLine_5.setName("SketchLine_6")
SketchLine_5.result().setName("SketchLine_6")
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_4.setName("SketchConstraintCoincidence_5")
SketchLine_6 = Sketch_1.addLine(-6.188, -3.45, -6.188, 9.025)
SketchLine_6.setName("SketchLine_7")
SketchLine_6.result().setName("SketchLine_7")
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_5.setName("SketchConstraintCoincidence_6")
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintVertical_2.setName("SketchConstraintVertical_3")
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintVertical_3.setName("SketchConstraintVertical_4")
SketchLine_7 = Sketch_1.addLine(-6.376, 9.025, -6.376, -2.25)
SketchLine_7.setName("SketchLine_8")
SketchLine_7.result().setName("SketchLine_8")
SketchLine_8 = Sketch_1.addLine(-6.376, -2.25, -6.538, -2.4)
SketchLine_8.setName("SketchLine_9")
SketchLine_8.result().setName("SketchLine_9")
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_6.setName("SketchConstraintCoincidence_7")
SketchLine_9 = Sketch_1.addLine(-6.538, -2.4, -6.538, -3.45)
SketchLine_9.setName("SketchLine_10")
SketchLine_9.result().setName("SketchLine_10")
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_7.setName("SketchConstraintCoincidence_8")
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_9.endPoint())
SketchConstraintCoincidence_8.setName("SketchConstraintCoincidence_9")
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_7.result())
SketchConstraintVertical_4.setName("SketchConstraintVertical_5")
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_9.result())
SketchConstraintVertical_5.setName("SketchConstraintVertical_6")
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_4.result(), SketchLine_2.result())
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchPoint_1.coordinates(), SketchLine_5.result(), 3.45)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchPoint_1.coordinates(), SketchLine_6.result(), 6.188)
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_6.startPoint(), SketchLine_9.result(), 0.35)
SketchConstraintDistance_3.setName("SketchConstraintDistance_4")
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchLine_6.startPoint(), SketchLine_7.result(), "ep_jupe")
SketchConstraintDistance_4.setName("SketchConstraintDistance_5")
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_4.result(), 3)
SketchLine_10 = Sketch_1.addLine(-6.282, 0.5300728101808314, -6.282, -4.926040141145169)
SketchLine_10.setName("SketchLine_11")
SketchLine_10.result().setName("SketchLine_11")
SketchLine_10.setAuxiliary(True)
SketchConstraintVertical_6 = Sketch_1.setVertical(SketchLine_10.result())
SketchConstraintVertical_6.setName("SketchConstraintVertical_7")
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchLine_6.startPoint(), SketchLine_10.result(), "ep_jupe/2")
SketchConstraintDistance_5.setName("SketchConstraintDistance_6")
SketchLine_11 = Sketch_1.addLine(-5.532, -4.05, -7.032, -4.05)
SketchLine_11.setName("SketchLine_12")
SketchLine_11.result().setName("SketchLine_12")
SketchLine_12 = Sketch_1.addLine(-7.032, -4.05, -7.032, -5.85)
SketchLine_12.setName("SketchLine_13")
SketchLine_12.result().setName("SketchLine_13")
SketchLine_13 = Sketch_1.addLine(-7.032, -5.85, -5.532, -5.85)
SketchLine_13.setName("SketchLine_14")
SketchLine_13.result().setName("SketchLine_14")
SketchLine_14 = Sketch_1.addLine(-5.532, -5.85, -5.532, -4.05)
SketchLine_14.setName("SketchLine_15")
SketchLine_14.result().setName("SketchLine_15")
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_9.setName("SketchConstraintCoincidence_10")
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_10.setName("SketchConstraintCoincidence_11")
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_11.setName("SketchConstraintCoincidence_12")
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_12.setName("SketchConstraintCoincidence_13")
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_7 = Sketch_1.setVertical(SketchLine_12.result())
SketchConstraintVertical_7.setName("SketchConstraintVertical_8")
SketchConstraintHorizontal_5 = Sketch_1.setHorizontal(SketchLine_13.result())
SketchConstraintVertical_8 = Sketch_1.setVertical(SketchLine_14.result())
SketchConstraintVertical_8.setName("SketchConstraintVertical_9")
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_13.result(), "width_gallery")
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_14.result(), 1.8)
SketchConstraintDistance_6 = Sketch_1.setDistance(SketchLine_11.startPoint(), SketchLine_10.result(), "width_gallery/2")
SketchConstraintDistance_6.setName("SketchConstraintDistance_7")
SketchConstraintDistance_7 = Sketch_1.setDistance(SketchLine_12.startPoint(), SketchLine_1.result(), 0.6)
SketchConstraintDistance_7.setName("SketchConstraintDistance_8")
SketchConstraintDistance_8 = Sketch_1.setDistance(SketchLine_2.startPoint(), SketchLine_10.result(), "width_gallery")
SketchConstraintDistance_8.setName("SketchConstraintDistance_9")
SketchLine_15 = Sketch_1.addLine(model.selection("VERTEX", "PartSet/OZ"))
SketchLine_15.setName("Axis_Z")
SketchLine_15.result().setName("Axis_Z")
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_15.result())
SketchConstraintCoincidence_13.setName("SketchConstraintCoincidence_14")
SketchLine_16 = Sketch_1.addLine(model.selection("VERTEX", "PartSet/OX"))
SketchLine_16.setName("Axis_X")
SketchLine_16.result().setName("Axis_X")
SketchConstraintDistance_9 = Sketch_1.setDistance(SketchLine_8.startPoint(), SketchLine_16.result(), 2.25)
SketchConstraintDistance_9.setName("SketchConstraintDistance_10")
SketchLine_17 = Sketch_1.addLine(-6.376, -2.25, -7.479754760955, -2.25)
SketchLine_17.setAuxiliary(True)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_17.startPoint())
SketchConstraintCoincidence_14.setName("SketchConstraintCoincidence_15")
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_8.startPoint(), SketchLine_17.startPoint())
SketchConstraintCoincidence_15.setName("SketchConstraintCoincidence_16")
SketchConstraintHorizontal_6 = Sketch_1.setHorizontal(SketchLine_17.result())
SketchConstraintDistance_10 = Sketch_1.setDistance(SketchLine_9.startPoint(), SketchLine_17.result(), 0.15)
SketchConstraintDistance_10.setName("SketchConstraintDistance_11")
SketchConstraintDistance_11 = Sketch_1.setDistance(SketchLine_6.endPoint(), SketchLine_16.result(), "h_bas_dome")
SketchConstraintDistance_11.setName("SketchConstraintDistance_12")
SketchConstraintDistance_12 = Sketch_1.setDistance(SketchLine_7.startPoint(), SketchLine_16.result(), "h_bas_dome")
SketchConstraintDistance_12.setName("SketchConstraintDistance_13")
SketchLine_18 = Sketch_1.addLine(-6.188, 9.025, -6.082, 9.532)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_18.startPoint())
SketchConstraintCoincidence_16.setName("SketchConstraintCoincidence_17")
SketchLine_19 = Sketch_1.addLine(-6.376, 9.025, -6.538, 9.187)
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_7.startPoint(), SketchLine_19.startPoint())
SketchConstraintCoincidence_17.setName("SketchConstraintCoincidence_18")
SketchLine_20 = Sketch_1.addLine(-6.538, 9.187, -6.538, 10.256)
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_19.endPoint(), SketchLine_20.startPoint())
SketchConstraintCoincidence_18.setName("SketchConstraintCoincidence_19")
SketchLine_21 = Sketch_1.addLine(-6.538, 10.256, -6.188, 10.256)
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchLine_20.endPoint(), SketchLine_21.startPoint())
SketchConstraintCoincidence_19.setName("SketchConstraintCoincidence_20")
SketchConstraintDistance_13 = Sketch_1.setDistance(SketchLine_19.endPoint(), SketchLine_16.result(), 9.187)
SketchConstraintDistance_13.setName("SketchConstraintDistance_14")
SketchConstraintDistance_14 = Sketch_1.setDistance(SketchLine_6.endPoint(), SketchLine_20.result(), 0.35)
SketchConstraintDistance_14.setName("SketchConstraintDistance_15")
SketchConstraintVertical_9 = Sketch_1.setVertical(SketchLine_20.result())
SketchConstraintVertical_9.setName("SketchConstraintVertical_10")
SketchConstraintVertical_10 = Sketch_1.setVertical(SketchLine_20.result())
SketchConstraintVertical_10.setName("SketchConstraintVertical_11")
SketchLine_22 = Sketch_1.addLine(-6.376, 9.025, -7.650292531, 9.025)
SketchLine_22.setAuxiliary(True)
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchLine_7.startPoint(), SketchLine_22.startPoint())
SketchConstraintCoincidence_20.setName("SketchConstraintCoincidence_21")
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchLine_19.startPoint(), SketchLine_22.startPoint())
SketchConstraintCoincidence_21.setName("SketchConstraintCoincidence_22")
SketchConstraintHorizontal_7 = Sketch_1.setHorizontal(SketchLine_22.result())
SketchConstraintHorizontal_8 = Sketch_1.setHorizontal(SketchLine_21.result())
SketchLine_23 = Sketch_1.addLine(-6.188, 10.256, -4.938, 10.97786688777511)
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchLine_21.endPoint(), SketchLine_23.startPoint())
SketchConstraintCoincidence_22.setName("SketchConstraintCoincidence_23")
SketchArc_1 = Sketch_1.addArc(0, 2.425, -4.938, 10.97786688777511, 0, 12.301, True)
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_15.result())
SketchConstraintCoincidence_23.setName("SketchConstraintCoincidence_24")
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchLine_23.endPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_24.setName("SketchConstraintCoincidence_25")
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_15.result())
SketchConstraintCoincidence_25.setName("SketchConstraintCoincidence_26")
SketchConstraintDistance_15 = Sketch_1.setDistance(SketchLine_16.result(), SketchArc_1.center(), 2.425)
SketchConstraintDistance_15.setName("SketchConstraintDistance_16")
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], "r_dome+demi_ep_dome")
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_21.result(), 0.35)
SketchLine_24 = Sketch_1.addLine(-4.938, 10.97786688777511, 0, 2.425)
SketchLine_24.setAuxiliary(True)
SketchConstraintCoincidence_26 = Sketch_1.setCoincident(SketchLine_23.endPoint(), SketchLine_24.startPoint())
SketchConstraintCoincidence_26.setName("SketchConstraintCoincidence_27")
SketchConstraintCoincidence_27 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_24.startPoint())
SketchConstraintCoincidence_27.setName("SketchConstraintCoincidence_28")
SketchConstraintCoincidence_28 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_24.endPoint())
SketchConstraintCoincidence_28.setName("SketchConstraintCoincidence_29")
SketchConstraintAngle_1 = Sketch_1.setAngleBackward(SketchLine_24.result(), SketchLine_15.result(), 30)
SketchConstraintDistance_16 = Sketch_1.setDistance(SketchLine_20.endPoint(), SketchLine_16.result(), 10.256)
SketchConstraintDistance_16.setName("SketchConstraintDistance_17")
SketchConstraintDistance_17 = Sketch_1.setDistance(SketchLine_18.endPoint(), SketchLine_16.result(), 9.532)
SketchConstraintDistance_17.setName("SketchConstraintDistance_18")
SketchConstraintDistance_18 = Sketch_1.setDistance(SketchLine_18.endPoint(), SketchLine_6.result(), 0.106)
SketchConstraintDistance_18.setName("SketchConstraintDistance_19")
SketchArc_2 = Sketch_1.addArc(-4.79378612024245, 9.263, -6.082, 9.532, -6.022198038184096, 9.735080670922804, True)
SketchConstraintCoincidence_29 = Sketch_1.setCoincident(SketchLine_18.endPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_29.setName("SketchConstraintCoincidence_30")
SketchConstraintDistance_19 = Sketch_1.setDistance(SketchArc_2.center(), SketchLine_16.result(), 9.263)
SketchConstraintDistance_19.setName("SketchConstraintDistance_20")
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_2.results()[1], 1.316)
SketchPoint_2 = Sketch_1.addPoint(0, 0)
SketchLine_25 = Sketch_1.addLine(-4.79378612024245, 9.263, -4.793786120242411, 9.813514482867)
SketchLine_25.setAuxiliary(True)
SketchConstraintCoincidence_30 = Sketch_1.setCoincident(SketchArc_2.center(), SketchLine_25.startPoint())
SketchConstraintCoincidence_30.setName("SketchConstraintCoincidence_31")
SketchConstraintVertical_11 = Sketch_1.setVertical(SketchLine_25.result())
SketchConstraintVertical_11.setName("SketchConstraintVertical_12")
model.do()

sketchTime = timer() - sketchTime
print("Sketch creation time: {0}".format(sketchTime))
expectedTime = MOVE_BUILD_RATIO * sketchTime
averageTime = 0
nbMoves = 0

# Two vertical constraints: due to #1889 the sketch is invalid now, not overconstrained
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(not aFactory.validate(SketchConstraintVertical_10.feature()))
#assert Sketch_1.solverError().value() != "", "FAILED: Sketch should report over-constrained situation"

# Remove duplicated Vertical constraints
Part_1_doc.removeFeature(SketchConstraintVertical_10.feature())
model.do()
assert Sketch_1.solverError().value() == "", "FAILED: Sketch should NOT report over-constrained situation"

#=========================================================================
# Move end point of SketchArc_2
#=========================================================================
aCenter = SketchArc_2.center()
aEndPoint = SketchArc_2.endPoint()
aDeltaX = aEndPoint.x() - aCenter.x()
aDeltaY = aEndPoint.y() - aCenter.y()

anAngle = math.pi / 100.
aCos = math.cos(anAngle)
aSin = math.sin(anAngle)

for ang in range(0, 50):
    movementTime = timer()
    X = aCenter.x() + aDeltaX * aCos + aDeltaY * aSin
    Y = aCenter.y() - aDeltaX * aSin + aDeltaY * aCos
    aEndPoint.setValue(X, Y)
    model.do()
    movementTime = timer() - movementTime
    assert movementTime < expectedTime, "Time to move point {0} is greater than expected {1}".format(movementTime, expectedTime)
    assert math.fabs(aEndPoint.x() - X) < TOLERANCE and math.fabs(aEndPoint.y() - Y) < TOLERANCE, "({0}, {1}) != ({2}, {3})".format(aEndPoint.x(), aEndPoint.y(), X, Y)
    averageTime += movementTime
    nbMoves += 1
    aDeltaX = aEndPoint.x() - aCenter.x()
    aDeltaY = aEndPoint.y() - aCenter.y()
print("Movement average time: {0}".format(averageTime / nbMoves))

model.end()


assert(model.checkPythonDump())
