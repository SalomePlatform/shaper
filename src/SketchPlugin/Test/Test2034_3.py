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
    Test2034_3.py
    Test case for issue #2034 "Conflicting constraint not raised (but found by planegcs and shaper's dof computation)"
"""

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(5, 0, 0, 5)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "OX"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_2.result())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "OZ"), False)
SketchLine_3 = SketchProjection_2.createdFeature()
SketchLine_4 = Sketch_1.addLine(0, 5, -5, 0)
SketchLine_5 = Sketch_1.addLine(-5, 0, 5, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_2.result())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_1.startPoint())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_1.result(), SketchLine_4.result())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_4.startPoint(), SketchLine_3.result())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_4.startPoint())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_4.result(), SketchLine_1.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_5.result(), 10)
model.do()
Sketch_2 = model.addSketch(partSet, model.defaultPlane("XOZ"))
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_4_EndVertex"), False)
SketchPoint_1 = SketchProjection_3.createdFeature()
SketchArc_1 = Sketch_2.addArc(0, 5, -5, 0, 5, 0, False)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchPoint_1.result(), SketchArc_1.startPoint())
SketchProjection_4 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_1_StartVertex"), False)
SketchPoint_2 = SketchProjection_4.createdFeature()
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchArc_1.endPoint(), SketchAPI_Point(SketchPoint_2).coordinates())
SketchProjection_5 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_1_EndVertex"), False)
SketchPoint_3 = SketchProjection_5.createdFeature()
model.do()

# check sketch is correct
assert(Sketch_2.solverError().value() == "")

# add coincidence between center of arc and a point from the first sketch
Sketch_2.setCoincident(SketchArc_1.center(), SketchAPI_Point(SketchPoint_3).coordinates())
model.do()
assert(Sketch_2.solverError().value() != "")

model.end()
