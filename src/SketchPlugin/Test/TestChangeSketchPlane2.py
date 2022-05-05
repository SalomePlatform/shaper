# Copyright (C) 2019-2022  CEA/DEN, EDF R&D
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
import math

def checkMiddlePoint(feature, x, y, z, tolerance = 1.e-6):
    shape = feature.results()[0].resultSubShapePair()[0].shape()
    assert(shape is not None)
    middlePoint = shape.middlePoint()
    assert(math.fabs(middlePoint.x() - x) < tolerance), "X: {} != {}".format(middlePoint.x(), x)
    assert(math.fabs(middlePoint.y() - y) < tolerance), "Y: {} != {}".format(middlePoint.y(), y)
    assert(math.fabs(middlePoint.z() - z) < tolerance), "Z: {} != {}".format(middlePoint.z(), z)

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 0, 20, 0)
SketchLine_2 = Sketch_1.addLine(20, 0, 30, 20)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(30, 20, 50, 20)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(50, 20, 50, 40)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(50, 40, 0, 40)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(0, 40, 0, 0)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_6.endPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_5.result(), 50)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_6.result(), 40)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_4.result(), 20)
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_3.result(), 20)
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_1.result(), 20)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchAPI_Point(SketchPoint_1).coordinates())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f-SketchLine_5f-SketchLine_6f")], model.selection(), 30, 0)
ExtrusionFuse_1 = model.addExtrusionFuse(Part_1_doc, [], model.selection(), 10, 0, [model.selection("SOLID", "Extrusion_1_1")])
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3"))
SketchCircle_1 = Sketch_2.addCircle(33, 25, 5)
SketchConstraintRadius_1 = Sketch_2.setRadius(SketchCircle_1.results()[1], 5)
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3]"), False)
SketchLine_7 = SketchProjection_2.createdFeature()
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchCircle_1.center(), SketchLine_7.result(), 3, True)
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/To_Face]"), False)
SketchLine_8 = SketchProjection_3.createdFeature()
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchCircle_1.center(), SketchLine_8.result(), 5, True)
ExtrusionFuse_1.setNestedSketch(Sketch_2)
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "ExtrusionFuse_1_1/Modified_Face&Extrusion_1_1/To_Face"))
SketchIntersectionPoint_1 = Sketch_3.addIntersectionPoint(model.selection("EDGE", "[ExtrusionFuse_1_1/Generated_Face&Sketch_2/SketchCircle_1_2][ExtrusionFuse_1_1/To_Face]"), False)
[SketchPoint_2] = SketchIntersectionPoint_1.intersectionPoints()
SketchLine_9 = Sketch_3.addLine(33.5, 10, 32.5, 10)
SketchLine_10 = Sketch_3.addLine(32.5, 10, 32.5, 17)
SketchLine_11 = Sketch_3.addLine(32.5, 17, 33.5, 17)
SketchLine_12 = Sketch_3.addLine(33.5, 17, 33.5, 10)
SketchConstraintCoincidence_8 = Sketch_3.setCoincident(SketchLine_12.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_9 = Sketch_3.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_10 = Sketch_3.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_11 = Sketch_3.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintHorizontal_4 = Sketch_3.setHorizontal(SketchLine_9.result())
SketchConstraintVertical_3 = Sketch_3.setVertical(SketchLine_10.result())
SketchConstraintHorizontal_5 = Sketch_3.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_4 = Sketch_3.setVertical(SketchLine_12.result())
SketchConstraintMiddle_1 = Sketch_3.setMiddlePoint(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_9.result())
SketchConstraintLength_6 = Sketch_3.setLength(SketchLine_9.result(), 1)
SketchConstraintLength_7 = Sketch_3.setLength(SketchLine_12.result(), 7)
model.do()
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchLine_12r-SketchLine_11r-SketchLine_10r-SketchLine_9r")], model.selection(), 0, 1, [model.selection("SOLID", "ExtrusionFuse_1_1")])
model.end()

SKETCH_DOF = 0
model.checkSketch(Sketch_1, SKETCH_DOF)
model.checkSketch(Sketch_2, SKETCH_DOF)
model.checkSketch(Sketch_3, SKETCH_DOF)

from GeomAPI import GeomAPI_Shape

model.testNbResults(ExtrusionCut_1, 1)
model.testNbSubResults(ExtrusionCut_1, [0])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [15])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [78])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [156])
model.testResultsVolumes(ExtrusionCut_1, [45760.2175339])

checkMiddlePoint(ExtrusionCut_1, 21.09174756, 23.41211647, 15.165448698)

# change plane of Fuse's sketch
model.begin()
Sketch_2.setPlane(model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_5"))
model.end()

model.checkSketch(Sketch_1, SKETCH_DOF)
model.checkSketch(Sketch_2, SKETCH_DOF)
model.checkSketch(Sketch_3, SKETCH_DOF)

model.testNbResults(ExtrusionCut_1, 1)
model.testNbSubResults(ExtrusionCut_1, [0])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [14])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [68])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [136])
model.testResultsVolumes(ExtrusionCut_1, [45778.45658456])

checkMiddlePoint(ExtrusionCut_1, 21.0948367, 23.919988, 15.169366862)

# undo the change
model.undo()
# and set another
model.begin()
Sketch_2.setPlane(model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2"))
model.end()

model.checkSketch(Sketch_1, SKETCH_DOF)
model.checkSketch(Sketch_2, SKETCH_DOF)
model.checkSketch(Sketch_3, SKETCH_DOF)

model.testNbResults(ExtrusionCut_1, 1)
model.testNbSubResults(ExtrusionCut_1, [0])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [18])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [97])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [194])
model.testResultsVolumes(ExtrusionCut_1, [45338.366732291])

checkMiddlePoint(ExtrusionCut_1, 21.00046446, 23.51391548, 15.073978)

# undo the change again
model.undo()
# and set another which fails the projection
model.begin()
Sketch_2.setPlane(model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6"))
model.end()
assert(SketchProjection_3.feature().error() != "")
# undo the error
model.undo()

model.begin()
Sketch_3.setPlane(model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4"), True)
model.end()

model.checkSketch(Sketch_1, SKETCH_DOF)
model.checkSketch(Sketch_2, SKETCH_DOF)
model.checkSketch(Sketch_3, 2)

model.testNbResults(ExtrusionCut_1, 1)
model.testNbSubResults(ExtrusionCut_1, [0])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [16])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [76])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [152])
model.testResultsVolumes(ExtrusionCut_1, [45760.15911273])

checkMiddlePoint(ExtrusionCut_1, 21.08920837, 23.40914618, 15.16787737)

assert(model.checkPythonDump())
