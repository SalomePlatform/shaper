# Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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
    Test2824.py
    Test case for issue #2824 "Constraints at wrong positions when editing the sketch (BARCOM)"
"""

from salome.shaper import model
from GeomAPI import *
from GeomDataAPI import *

import math

TOLERANCE = 1.e-6

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOZ"))
SketchArc_1 = Sketch_1.addArc(7.554548355024374, 9.322927740745062, 20, 30.60489708262655, 31.5458454490763, 15, True)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "OZ"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchArc_1.startPoint(), SketchLine_1.result(), 20, True)
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "OX"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchArc_1.endPoint(), SketchLine_2.result(), 15, True)
model.do()
# move the arc like the user does
for i in range(0, 10):
    Sketch_1.move(SketchArc_1.startPoint(), SketchArc_1.startPoint().x(), SketchArc_1.startPoint().y() + 5)
model.end()
model.undo()

# compare the arc arguments and the resulting shape
aStartAttrY = SketchArc_1.startPoint().y()
aEndAttrX = SketchArc_1.endPoint().x()

anArcShape = SketchArc_1.feature().lastResult().shape()
aShapeExplorer = GeomAPI_ShapeExplorer(anArcShape, GeomAPI_Shape.VERTEX)

aPoint = aShapeExplorer.current().vertex().point()
aEndShapeX = aPoint.x()
assert(math.fabs(aEndAttrX - aEndShapeX) < TOLERANCE)

aShapeExplorer.next()
aPoint = aShapeExplorer.current().vertex().point()
aStartShapeY = aPoint.z() # in 3D sketch y iz Z
assert(math.fabs(aStartAttrY - aStartShapeY) < TOLERANCE)
