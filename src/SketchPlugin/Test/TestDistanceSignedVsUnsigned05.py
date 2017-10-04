## Copyright (C) 2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

from salome.shaper import model
from SketchAPI import *
import math

TOLERANCE = 1.e-5

model.begin()
partSet = model.moduleDocument()
widthParam = model.addParameter(partSet, "w", "200")
distParam = model.addParameter(partSet, "d", "30")
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
Rectangle_1 = Sketch_1.addRectangle(0, 0, 200, 100)
[Line_1, Line_2, Line_3, Line_4] = Rectangle_1.lines()
Origin = Sketch_1.addPoint(model.selection("VERTEX", "Origin"))
Sketch_1.setCoincident(SketchAPI_Line(Line_1).endPoint(), Origin.result())
Sketch_1.setLength(SketchAPI_Line(Line_1).result(), "w")
Point_1 = Sketch_1.addPoint(230, 40)
Sketch_1.setSignedDistance(Point_1.result(), SketchAPI_Line(Line_4).result(), "d")
Point_2 = Sketch_1.addPoint(230, 80)
Sketch_1.setUnsignedDistance(Point_2.result(), SketchAPI_Line(Line_4).result(), "d")
Line_5 = Sketch_1.addLine(260, 0, 260, 100)
Sketch_1.setSignedDistance(Point_1.result(), Line_5.result(), "d")
Sketch_1.setUnsignedDistance(Point_2.result(), Line_5.result(), "d")
model.do()
assert Sketch_1.solverError().value() == "", "FAILED: Sketch should NOT report over-constrained situation"

line4 = model.toSketchFeature(Line_4)
line5 = model.toSketchFeature(Line_5)
signedDist14 = model.signedDistancePointLine(Point_1, line4)
signedDist15 = model.signedDistancePointLine(Point_1, line5)
signedDist24 = model.signedDistancePointLine(Point_2, line4)
signedDist25 = model.signedDistancePointLine(Point_2, line5)

# change rectangle width and check distances
widthParam.setValue(2000)
model.do()
curDist = model.signedDistancePointLine(Point_1, line4)
assert(math.fabs(signedDist14 - curDist) < TOLERANCE), "Expected {}, actual {}".format(signedDist14, curDist)
curDist = model.signedDistancePointLine(Point_2, line4)
assert(math.fabs(math.fabs(signedDist24) - math.fabs(curDist)) < TOLERANCE), "Expected {}, actual {}".format(signedDist24, -curDist)
curDist = model.signedDistancePointLine(Point_1, line5)
assert(math.fabs(signedDist15 - curDist) < TOLERANCE), "Expected {}, actual {}".format(signedDist15, curDist)
curDist = model.signedDistancePointLine(Point_2, line5)
assert(math.fabs(math.fabs(signedDist25) - math.fabs(curDist)) < TOLERANCE), "Expected {}, actual {}".format(signedDist25, -curDist)
assert Sketch_1.solverError().value() == "", "FAILED: Sketch should NOT report over-constrained situation"

# revert rectangle width and check distances again
widthParam.setValue(200)
model.do()
curDist = model.signedDistancePointLine(Point_1, line4)
assert(math.fabs(signedDist14 - curDist) < TOLERANCE), "Expected {}, actual {}".format(signedDist14, curDist)
curDist = model.signedDistancePointLine(Point_2, line4)
assert(math.fabs(math.fabs(signedDist24) - math.fabs(curDist)) < TOLERANCE), "Expected {}, actual {}".format(signedDist24, -curDist)
curDist = model.signedDistancePointLine(Point_1, line5)
assert(math.fabs(signedDist15 - curDist) < TOLERANCE), "Expected {}, actual {}".format(signedDist15, curDist)
curDist = model.signedDistancePointLine(Point_2, line5)
assert(math.fabs(math.fabs(signedDist25) - math.fabs(curDist)) < TOLERANCE), "Expected {}, actual {}".format(signedDist25, -curDist)
assert Sketch_1.solverError().value() == "", "FAILED: Sketch should NOT report over-constrained situation"

model.end()
