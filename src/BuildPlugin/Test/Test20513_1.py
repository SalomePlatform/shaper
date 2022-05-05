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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()

### Create SketchBSpline
SketchBSpline_1_poles = [(0, 0),
                         (64.69989977297864, 3.820449445888325),
                         (63.227018959685, 42.02181716036146),
                         (19.7442263567145, 66.94290132940253),
                         (0, 0)
                        ]
SketchBSpline_1 = Sketch_1.addSpline(poles = SketchBSpline_1_poles)
[SketchPoint_2, SketchPoint_3, SketchPoint_4, SketchPoint_5, SketchPoint_6] = SketchBSpline_1.controlPoles(auxiliary = [0, 1, 2, 3, 4])
[SketchLine_1, SketchLine_2, SketchLine_3, SketchLine_4] = SketchBSpline_1.controlPolygon(auxiliary = [0, 1, 2, 3])
Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_2).coordinates(), SketchPoint_1.result())
Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_6).coordinates(), SketchPoint_1.result())
model.do()

### Create Plane
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Sketch_1/Face-SketchBSpline_1f"), 10, False)

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))

### Create SketchProjection
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_7 = SketchProjection_2.createdFeature()

### Create SketchBSpline
SketchBSpline_2_poles = [(0, 0),
                         (8.839087148173476, 45.21301708610518),
                         (53.18579907801947, 39.22586299994072),
                         (47.99568029170977, 4.506542817571567),
                         (0, 0)
                        ]
SketchBSpline_2 = Sketch_2.addSpline(poles = SketchBSpline_2_poles)
[SketchPoint_8, SketchPoint_9, SketchPoint_10, SketchPoint_11, SketchPoint_12] = SketchBSpline_2.controlPoles(auxiliary = [0, 1, 2, 3, 4])
[SketchLine_5, SketchLine_6, SketchLine_7, SketchLine_8] = SketchBSpline_2.controlPolygon(auxiliary = [0, 1, 2, 3])
Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_8).coordinates(), SketchPoint_7.result())
Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_12).coordinates(), SketchPoint_7.result())
model.do()

### Create Wire
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchBSpline_1")], False)

### Create Wire
Wire_2 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchBSpline_2")], False)

### Create Filling
Filling_1 = model.addFilling(Part_1_doc, [model.selection("WIRE", "Wire_1_1"), model.selection("WIRE", "Wire_2_1")])

model.end()

from GeomAPI import *

model.testNbResults(Filling_1, 1)
model.testNbSubResults(Filling_1, [0])
model.testNbSubShapes(Filling_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Filling_1, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Filling_1, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Filling_1, GeomAPI_Shape.VERTEX, [8])
model.testResultsAreas(Filling_1, [1872.0403629667237])

assert(model.checkPythonDump())
