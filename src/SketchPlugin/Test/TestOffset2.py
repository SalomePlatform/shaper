# Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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

SKETCH_DOF = 9

model.begin()
partSet = model.moduleDocument()

### Create Sketch
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))

### Create SketchArc
SketchArc_1 = Sketch_1.addArc(10, 10, 50, 0, 0, 50, False)

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(0, 50, -20, 0)

### Create SketchLine
SketchLine_2 = Sketch_1.addLine(50, 0, -20, 0)
Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.startPoint())
Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_2.startPoint())
model.do()

assert(model.dof(Sketch_1) == SKETCH_DOF)

### Create SketchOffset
SketchOffset_1_objects = [SketchLine_1.result(), SketchArc_1.results()[1], SketchLine_2.result()]
SketchOffset_1 = Sketch_1.addOffset(SketchOffset_1_objects, 13, False)
model.do()

# DoF should not change
assert(model.dof(Sketch_1) == SKETCH_DOF)
# check number of features
assert(len(SketchOffset_1.offset()) == 5)
model.testNbSubFeatures(Sketch_1, "SketchPoint", 0)
model.testNbSubFeatures(Sketch_1, "SketchLine", 4)
model.testNbSubFeatures(Sketch_1, "SketchArc", 4)
model.testNbSubFeatures(Sketch_1, "SketchBSpline", 0)
model.testNbSubFeatures(Sketch_1, "SketchBSplinePeriodic", 0)

model.end()

assert(model.checkPythonDump())
