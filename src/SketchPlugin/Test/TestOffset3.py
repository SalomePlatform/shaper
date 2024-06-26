# Copyright (C) 2020-2024  CEA, EDF
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

model.begin()
partSet = model.moduleDocument()

### Create Sketch
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(0, 0, 0, 130)

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchAPI_Point(SketchPoint_1).coordinates())
Sketch_1.setVertical(SketchLine_1.result())
Sketch_1.setLength(SketchLine_1.result(), 130)

### Create SketchLine
SketchLine_2 = Sketch_1.addLine(0, 130, 52.63824701112046, 101.2039073554021)
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
Sketch_1.setLength(SketchLine_2.result(), 60)

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(52.63824701112046, 101.2039073554021, 199.7207674554361, 130.6442229064876)
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
Sketch_1.setLength(SketchLine_3.result(), 150)

### Create SketchConstraintAngle
Sketch_1.setAngle(SketchLine_3.result(), SketchLine_2.result(), 140, type = "Direct")

### Create SketchLine
SketchLine_4 = Sketch_1.addLine(158.0815949372134, 7.493180231252017, 199.7207674554361, 130.6442229064876)
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.endPoint())
Sketch_1.setLength(SketchLine_4.result(), 130)

### Create SketchConstraintAngle
Sketch_1.setAngle(SketchLine_3.result(), SketchLine_4.result(), 60, type = "Direct")

### Create SketchLine
SketchLine_5 = Sketch_1.addLine(158.0815949372134, 7.493180231252017, 106.6596776583922, 54.98830022657437)
Sketch_1.setCoincident(SketchLine_4.startPoint(), SketchLine_5.startPoint())
Sketch_1.setLength(SketchLine_5.result(), 70)

### Create SketchLine
SketchLine_6 = Sketch_1.addLine(106.6596776583922, 54.98830022657437, 0, 0)
Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_1.startPoint())
Sketch_1.setLength(SketchLine_6.result(), 120)

### Create SketchConstraintAngle
Sketch_1.setAngle(SketchLine_6.result(), SketchLine_5.result(), 110, type = "Direct")

### SketchOffset objects
SketchOffset_objects = [SketchLine_1.result(), SketchLine_2.result(), SketchLine_3.result(), SketchLine_4.result(), SketchLine_5.result(), SketchLine_6.result()]

### 1. "KeepDistance" mode

SketchOffset_1 = Sketch_1.addOffset(SketchOffset_objects, 8, False, "KeepDistance") # outside
SketchOffset_2 = Sketch_1.addOffset(SketchOffset_objects, 8, True, "KeepDistance") # inside
model.do()

assert(len(SketchOffset_1.offset()) == 10)
assert(len(SketchOffset_2.offset()) == 8)

### 2. "Arcs" mode

SketchOffset_3 = Sketch_1.addOffset(SketchOffset_objects, 16, False, "Arcs") # outside
SketchOffset_4 = Sketch_1.addOffset(SketchOffset_objects, 16, True, "Arcs") # inside
model.do()

assert(len(SketchOffset_3.offset()) == 12)
assert(len(SketchOffset_4.offset()) == 12)

### 3. "Lines" mode

SketchOffset_5 = Sketch_1.addOffset(SketchOffset_objects, 24, False, "Lines") # outside
SketchOffset_6 = Sketch_1.addOffset(SketchOffset_objects, 24, True, "Lines") # inside
model.do()

assert(len(SketchOffset_5.offset()) == 6)
assert(len(SketchOffset_6.offset()) == 6)

model.end()

assert(model.checkPythonDump())
