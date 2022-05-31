# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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
    Test2376.py
    Test case for issue #2376 "Wrong DoF when creating the sketch"
"""


from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(20, 25, 50, 25)
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "OX"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_1.endPoint(), SketchLine_2.result(), 25, True)
model.do()

SKETCH_DOF = 2
model.checkSketch(Sketch_1, SKETCH_DOF)

# add one more distance constraint, DoF should be the same
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_1.startPoint(), SketchLine_2.result(), 25, True)
model.do()
assert Sketch_1.solverError().value() != "", "FAILED: Sketch should report over-constrained situation"

# remove last constraint
partSet.removeFeature(SketchConstraintDistance_2.feature())
model.do()
model.checkSketch(Sketch_1, SKETCH_DOF)

model.end()

assert(model.checkPythonDump())
