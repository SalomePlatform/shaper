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

from GeomAPI import *
from SketchAPI import *

from salome.shaper import model

ORIGIN = GeomAPI_Pnt(0, 0, 0)
NB_COPIES = 8
TOLERANCE = 1.e-7

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "OX"), True)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchMultiRotation_1 = Sketch_1.addRotation([SketchLine_1.result()], SketchAPI_Line(SketchLine_1).startPoint(), 360, NB_COPIES, True)
[SketchLine_2, SketchLine_3, SketchLine_4, SketchLine_5, SketchLine_6, SketchLine_7, SketchLine_8] = SketchMultiRotation_1.rotated()
model.do()
model.end()

model.testNbResults(Sketch_1, 1)
model.testNbSubShapes(Sketch_1, GeomAPI_Shape.EDGE, [NB_COPIES])
model.testNbSubShapes(Sketch_1, GeomAPI_Shape.VERTEX, [2 * NB_COPIES])

midPnt = Sketch_1.defaultResult().shape().middlePoint()
assert(midPnt.distance(ORIGIN) < TOLERANCE)

assert(model.checkPythonDump())
