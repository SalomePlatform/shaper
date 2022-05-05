# Copyright (C) 2017-2022  CEA/DEN, EDF R&D
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
Point_1 = Sketch_1.addPoint(model.selection("VERTEX", "Origin"))
Sketch_1.setCoincident(SketchAPI_Line(Line_1).endPoint(), Point_1.result())
Sketch_1.setLength(SketchAPI_Line(Line_1).result(), "w")
Circle_1 = Sketch_1.addCircle(230, 40, 20)
Sketch_1.setSignedDistance(Circle_1.center(), SketchAPI_Line(Line_4).result(), "d")
Circle_2 = Sketch_1.addCircle(230, 80, 20)
Sketch_1.setUnsignedDistance(Circle_2.center(), SketchAPI_Line(Line_4).result(), "d")
model.do()
assert Sketch_1.solverError().value() == "", "FAILED: Sketch should NOT report over-constrained situation"

center1 = Circle_1.center()
center2 = Circle_2.center()
line = model.toSketchFeature(Line_4)
signedDist1 = model.signedDistancePointLine(center1, line)
signedDist2 = model.signedDistancePointLine(center2, line)

# change rectangle width and check distances
widthParam.setValue(2000)
model.do()
curDist = model.signedDistancePointLine(center1, line)
assert(math.fabs(signedDist1 - curDist) < TOLERANCE), "Expected {}, actual {}".format(signedDist1, curDist)
curDist = model.signedDistancePointLine(center2, line)
assert(math.fabs(math.fabs(signedDist2) - math.fabs(curDist)) < TOLERANCE), "Expected {}, actual {}".format(signedDist2, -curDist)
assert Sketch_1.solverError().value() == "", "FAILED: Sketch should NOT report over-constrained situation"

# revert rectangle width and check distances again
widthParam.setValue(200)
model.do()
curDist = model.signedDistancePointLine(center1, line)
assert(math.fabs(signedDist1 - curDist) < TOLERANCE), "Expected {}, actual {}".format(signedDist1, curDist)
curDist = model.signedDistancePointLine(center2, line)
assert(math.fabs(math.fabs(signedDist2) - math.fabs(curDist)) < TOLERANCE), "Expected {}, actual {}".format(signedDist2, -curDist)
assert Sketch_1.solverError().value() == "", "FAILED: Sketch should NOT report over-constrained situation"

model.end()
