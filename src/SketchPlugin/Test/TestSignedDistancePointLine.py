# Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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
from salome.shaper import model
import math

TOLERANCE = 1.e-5

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "clearance", "210")
model.addParameter(Part_1_doc, "wheel_R", "203")
model.addParameter(Part_1_doc, "arc_R", "wheel_R+70")
model.addParameter(Part_1_doc, "hood_height", "290")
HeightParam = model.addParameter(Part_1_doc, "height", "900")
LengthParam = model.addParameter(Part_1_doc, "length", "1460")
PosParam = model.addParameter(Part_1_doc, "position", "71.99905090248758")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-281.378739745974, 78.99909999999998, -98.87873974597397, 78.99909999999998)
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchArc_1 = Sketch_1.addArc(174.031501908677, 71.9991, -98.87873974597397, 78.99909999999998, 446.941743563328, 78.9991, True)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchArc_1.startPoint())
SketchArc_2 = Sketch_1.addArc(-554.288981400625, 71.9991, -281.378739745974, 78.99909999999998, -827.1992230552759, 78.9991, False)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_1.result())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchArc_2.center(), SketchLine_1.result(), "clearance-wheel_R", True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchArc_1.center(), SketchLine_1.result(), "clearance-wheel_R", True)
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchArc_2.results()[1], SketchArc_1.results()[1])
SketchCircle_1 = Sketch_1.addCircle(-554.288981400625, 71.9991, 203)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_2.center(), SketchCircle_1.center())
SketchCircle_2 = Sketch_1.addCircle(174.031501908677, 71.9991, 203)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchArc_1.center(), SketchCircle_2.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], "wheel_R")
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchCircle_1.results()[1], SketchCircle_2.results()[1])
SketchLine_2 = Sketch_1.addLine(-827.1992230552759, 78.9991, -960.4001854579536, 78.9991)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-960.4001854579536, 78.9991, -960.4001854579536, 368.9991)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(-891.6236641114499, 437.7756213465037, -540.5628729569305, 437.7756213465037)
SketchLine_5 = Sketch_1.addLine(-540.5628729569305, 437.7756213465037, -349.330908394406, 768.9991)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(-349.330908394406, 768.9991, 308.3678499795216, 768.9991)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(308.3678499795216, 768.9991, 499.5998145420464, 437.7756213465038)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(499.5998145420464, 437.7756213465038, 499.5998145420464, 78.9991)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_1.addLine(499.5998145420464, 78.9991, 446.941743563328, 78.9991)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_9.endPoint())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintHorizontal_5 = Sketch_1.setHorizontal(SketchLine_9.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_8.result())
SketchArc_3 = Sketch_1.addArc(-891.6236641114499, 368.9991, -891.6236641114499, 437.7756213465037, -960.4001854579536, 368.9991, False)
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchArc_3.startPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_3.endPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_3.results()[1], SketchLine_3.result())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_3.results()[1], SketchLine_4.result())
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_5.result(), SketchLine_4.result(), 120.0000000000006)
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_8.startPoint(), SketchLine_4.result())
SketchConstraintAngle_2 = Sketch_1.setAngleBackward(SketchLine_7.result(), SketchLine_6.result(), 120.0000000000006)
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchArc_2.center(), SketchLine_2.startPoint(), "arc_R", True)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchLine_2.endPoint(), SketchLine_5.endPoint(), "height-clearance")
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchLine_2.endPoint(), SketchLine_9.startPoint(), "length", True)
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchLine_3.endPoint(), SketchLine_2.result(), "hood_height", True)
SketchLine_10 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OX"))
SketchConstraintDistance_6 = Sketch_1.setDistance(SketchCircle_2.center(), SketchLine_10.result(), "position", True)
SketchPoint_1 = Sketch_1.addPoint(-910.4001854579535, 347.7756213465037)
SketchPoint_2 = Sketch_1.addPoint(-910.9041532173604, 297.7781612460398)
SketchConstraintDistance_7 = Sketch_1.setDistance(SketchPoint_1.coordinates(), SketchLine_3.result(), 50, True)
SketchConstraintDistance_8 = Sketch_1.setDistance(SketchPoint_1.coordinates(), SketchLine_4.result(), 90, True)
SketchConstraintDistance_9 = Sketch_1.setDistance(SketchPoint_2.coordinates(), SketchPoint_1.coordinates(), 50, True)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), "length/8")
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchArc_1_2r-SketchLine_9r-SketchLine_8r-SketchLine_7r-SketchLine_6r-SketchLine_5r-SketchLine_4r-SketchArc_3_2f-SketchLine_3r-SketchLine_2r-SketchArc_2_2r")], model.selection(), 500, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f"), model.selection("FACE", "Sketch_1/Face-SketchCircle_2_2f")], model.selection(), 500, -400)
Extrusion_3 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f"), model.selection("FACE", "Sketch_1/Face-SketchCircle_2_2f")], model.selection(), 100, 0)
model.do()

# Verify point-line signed distance
dist1 = model.signedDistancePointLine(SketchArc_1.center(), SketchLine_1)
dist2 = model.signedDistancePointLine(SketchArc_2.center(), SketchLine_1)
dist3 = model.signedDistancePointLine(SketchLine_3.endPoint(), SketchLine_2)
dist4 = model.signedDistancePointLine(SketchPoint_1, SketchLine_3)
dist5 = model.signedDistancePointLine(SketchPoint_1, SketchLine_4)

PosParam.setValue(300)
model.do()
curDist1 = model.signedDistancePointLine(SketchArc_1.center(), SketchLine_1)
curDist2 = model.signedDistancePointLine(SketchArc_2.center(), SketchLine_1)
curDist3 = model.signedDistancePointLine(SketchLine_3.endPoint(), SketchLine_2)
curDist4 = model.signedDistancePointLine(SketchPoint_1, SketchLine_3)
curDist5 = model.signedDistancePointLine(SketchPoint_1, SketchLine_4)
assert(math.fabs(dist1 - curDist1) < TOLERANCE), "Expected {}, actual {}".format(dist1, curDist1)
assert(math.fabs(dist2 - curDist2) < TOLERANCE), "Expected {}, actual {}".format(dist2, curDist2)
assert(math.fabs(dist3 - curDist3) < TOLERANCE), "Expected {}, actual {}".format(dist3, curDist3)
assert(math.fabs(dist4 - curDist4) < TOLERANCE), "Expected {}, actual {}".format(dist4, curDist4)
assert(math.fabs(dist5 - curDist5) < TOLERANCE), "Expected {}, actual {}".format(dist5, curDist5)

LengthParam.setValue(2000)
model.do()
curDist1 = model.signedDistancePointLine(SketchArc_1.center(), SketchLine_1)
curDist2 = model.signedDistancePointLine(SketchArc_2.center(), SketchLine_1)
curDist3 = model.signedDistancePointLine(SketchLine_3.endPoint(), SketchLine_2)
curDist4 = model.signedDistancePointLine(SketchPoint_1, SketchLine_3)
curDist5 = model.signedDistancePointLine(SketchPoint_1, SketchLine_4)
assert(math.fabs(dist1 - curDist1) < TOLERANCE), "Expected {}, actual {}".format(dist1, curDist1)
assert(math.fabs(dist2 - curDist2) < TOLERANCE), "Expected {}, actual {}".format(dist2, curDist2)
assert(math.fabs(dist3 - curDist3) < TOLERANCE), "Expected {}, actual {}".format(dist3, curDist3)
assert(math.fabs(dist4 - curDist4) < TOLERANCE), "Expected {}, actual {}".format(dist4, curDist4)
assert(math.fabs(dist5 - curDist5) < TOLERANCE), "Expected {}, actual {}".format(dist5, curDist5)

HeightParam.setValue(1200)
model.do()
curDist1 = model.signedDistancePointLine(SketchArc_1.center(), SketchLine_1)
curDist2 = model.signedDistancePointLine(SketchArc_2.center(), SketchLine_1)
curDist3 = model.signedDistancePointLine(SketchLine_3.endPoint(), SketchLine_2)
curDist4 = model.signedDistancePointLine(SketchPoint_1, SketchLine_3)
curDist5 = model.signedDistancePointLine(SketchPoint_1, SketchLine_4)
assert(math.fabs(dist1 - curDist1) < TOLERANCE), "Expected {}, actual {}".format(dist1, curDist1)
assert(math.fabs(dist2 - curDist2) < TOLERANCE), "Expected {}, actual {}".format(dist2, curDist2)
assert(math.fabs(dist3 - curDist3) < TOLERANCE), "Expected {}, actual {}".format(dist3, curDist3)
assert(math.fabs(dist4 - curDist4) < TOLERANCE), "Expected {}, actual {}".format(dist4, curDist4)
assert(math.fabs(dist5 - curDist5) < TOLERANCE), "Expected {}, actual {}".format(dist5, curDist5)

model.end()
