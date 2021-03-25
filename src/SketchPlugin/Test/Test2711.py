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
    Test2711.py
    Test case for issue #2711 "Crash when calling split"
"""

from salome.shaper import model
from salome.shaper import geom

from ModelAPI import *
from SketchAPI import *

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-0.4, -0.45, 3.6, -0.45)
SketchLine_2 = Sketch_1.addLine(3.6, -0.45, 3.6, 0.2)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(3.6, 0.2, 0.25, 0.2)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(0.25, 0.2, 0.25, 3.55)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(0.25, 3.55, -0.4, 3.55)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(-0.4, 3.55, -0.4, -0.45)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_6.endPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), 0.65)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_5.result(), 0.65)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_6.result(), 4)
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_1.result(), 4)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "OY"), False)
SketchLine_7 = SketchProjection_1.createdFeature()
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_6.startPoint(), SketchLine_7.result(), 0.4, True)
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "OX"), False)
SketchLine_8 = SketchProjection_2.createdFeature()
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_3.startPoint(), SketchLine_8.result(), 0.2, True)
model.do()

model.checkSketch(Sketch_1, 0)

SketchPoint_1 = Sketch_1.addPoint(2, 0.2)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_3.result())
model.do()

model.checkSketch(Sketch_1, 1)

Sketch_1.addSplit(SketchLine_3, geom.Pnt2d(3, 0.2))
model.do()

model.checkSketch(Sketch_1, 2)

SketchLine_9 = SketchAPI_Line(model.lastSubFeature(featureToCompositeFeature(Sketch_1.feature()), "SketchLine"))
SketchPoint_2 = Sketch_1.addPoint(3, 0.2)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_9.result())
model.do()

model.checkSketch(Sketch_1, 3)

Sketch_1.addSplit(SketchLine_9, geom.Pnt2d(3.2, 0.2))
model.do()

model.checkSketch(Sketch_1, 4)

model.end()

assert(model.checkPythonDump())
