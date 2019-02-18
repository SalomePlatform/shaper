# Copyright (C) 2018-2019  CEA/DEN, EDF R&D
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
    Test2034_1.py
    Test case for issue #2034 "Conflicting constraint not raised (but found by planegcs and shaper's dof computation)"
"""

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 30, 30, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "OY"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_2.result())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "OX"), False)
SketchLine_3 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchAPI_Line(SketchLine_2).startPoint(), SketchLine_1.endPoint(), 30, True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchAPI_Line(SketchLine_2).startPoint(), SketchLine_1.startPoint(), 30, True)
SketchArc_1 = Sketch_1.addArc(-1.010399194398717, -1.010399194382958, 0, 30, 30, 0, False)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchArc_1.endPoint())
model.do()

# check sketch is correct
assert(Sketch_1.solverError().value() == "")

# add coincidence between center of arc and an origin
Sketch_1.setCoincident(SketchArc_1.center(), SketchAPI_Line(SketchLine_2).startPoint())
model.do()
assert(Sketch_1.solverError().value() != "")

model.end()
