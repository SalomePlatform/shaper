# Copyright (C) 2019  CEA/DEN, EDF R&D
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
    Test constraint "Equal" applied to the pair of ellipses
"""

from salome.shaper import model
import math

DOF = 0

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchEllipse_1 = Sketch_1.addEllipse(-27.88698315421018, 6.197107367602265, -8.725072906579975, 15.87998754592604, 11.10896680773502)
[SketchPoint_1, SketchPoint_2, SketchPoint_3, SketchPoint_4, SketchPoint_5, SketchPoint_6, SketchPoint_7, SketchLine_1, SketchLine_2] = SketchEllipse_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
DOF += 5
SketchEllipse_2 = Sketch_1.addEllipse(15.14848467636108, -15.95181340919842, 21.12194589112931, -20.27742325437541, 9.877448119278471)
[SketchPoint_8, SketchPoint_9, SketchPoint_10, SketchPoint_11, SketchPoint_12, SketchPoint_13, SketchPoint_14, SketchLine_3, SketchLine_4] = SketchEllipse_2.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
DOF += 5
SketchEllipticArc_1 = Sketch_1.addEllipticArc(49.62123971365138, 13.34935433264426, 64.40153327705804, 5.234651852264014, 68.29270956846837, 8.653290073592997, 32.00833375829566, 14.82599483073829, False)
[SketchPoint_15, SketchPoint_16, SketchPoint_17, SketchPoint_18, SketchPoint_19, SketchPoint_20, SketchPoint_21, SketchLine_5, SketchLine_6] = SketchEllipticArc_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
DOF += 7
SketchEllipticArc_2 = Sketch_1.addEllipticArc(7.849720447428027, 32.28934430567138, 19.71732573395684, 29.13862828729395, 12.53096585507117, 17.93622281956947, 4.288678376456463, 46.71874313598852, True)
[SketchPoint_22, SketchPoint_23, SketchPoint_24, SketchPoint_25, SketchPoint_26, SketchPoint_27, SketchPoint_28, SketchLine_7, SketchLine_8] = SketchEllipticArc_2.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
DOF += 7
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchEllipse_1.result(), SketchEllipse_2.result())
DOF -= 2
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchEllipse_1.result(), SketchEllipticArc_1.result())
DOF -= 2
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchEllipticArc_1.result(), SketchEllipticArc_2.result())
DOF -= 2
model.do()
model.end()

TOLERANCE = 1.e-7

dist1 = model.distancePointPoint(SketchEllipse_1.majorAxisNegative(), SketchEllipse_1.majorAxisPositive())
dist2 = model.distancePointPoint(SketchEllipse_2.majorAxisNegative(), SketchEllipse_2.majorAxisPositive())
dist3 = model.distancePointPoint(SketchEllipticArc_1.majorAxisNegative(), SketchEllipticArc_1.majorAxisPositive())
dist4 = model.distancePointPoint(SketchEllipticArc_2.majorAxisNegative(), SketchEllipticArc_2.majorAxisPositive())
assert(math.fabs(dist1 - dist2) < TOLERANCE)
assert(math.fabs(dist1 - dist3) < TOLERANCE)
assert(math.fabs(dist1 - dist4) < TOLERANCE)

dist1 = model.distancePointPoint(SketchEllipse_1.minorAxisNegative(), SketchEllipse_1.minorAxisPositive())
dist2 = model.distancePointPoint(SketchEllipse_2.minorAxisNegative(), SketchEllipse_2.minorAxisPositive())
dist3 = model.distancePointPoint(SketchEllipticArc_1.minorAxisNegative(), SketchEllipticArc_1.minorAxisPositive())
dist4 = model.distancePointPoint(SketchEllipticArc_2.minorAxisNegative(), SketchEllipticArc_2.minorAxisPositive())
assert(math.fabs(dist1 - dist2) < TOLERANCE)
assert(math.fabs(dist1 - dist3) < TOLERANCE)
assert(math.fabs(dist1 - dist4) < TOLERANCE)

assert(model.dof(Sketch_1) == DOF)

assert(model.checkPythonDump())
