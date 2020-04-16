# Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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

def checkMiddlePoint(feature, x, y, z, tolerance = 1.e-7):
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
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Left"))
SketchCircle_1 = Sketch_1.addCircle(4, 5.52786404500042, 3)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_1.result(), 6, True)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 3)
SketchIntersectionPoint_1 = Sketch_1.addIntersectionPoint(model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]"), False)
[SketchPoint_1] = SketchIntersectionPoint_1.intersectionPoints()
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchCircle_1.center(), 6, True)
model.do()
ExtrusionFuse_1 = model.addExtrusionFuse(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchCircle_1_2r_wire")], model.selection(), 1, 0, [model.selection("SOLID", "Box_1_1")])
model.end()

SKETCH_DOF = 0
model.checkSketch(Sketch_1, SKETCH_DOF)

from GeomAPI import GeomAPI_Shape

model.testNbResults(ExtrusionFuse_1, 1)
model.testNbSubResults(ExtrusionFuse_1, [0])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.FACE, [8])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.EDGE, [30])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.VERTEX, [60])
model.testResultsVolumes(ExtrusionFuse_1, [1028.2743338823])

checkMiddlePoint(ExtrusionFuse_1, 4.972503122, 4.84876717, 5.0145146)

# change plane and check error
model.begin()
Sketch_1.setPlane(model.selection("FACE", "Box_1_1/Front"))
model.end()
assert(SketchIntersectionPoint_1.feature().error() != "")
# revert error
model.undo()

# change plane to get correct sketch
model.begin()
Sketch_1.setPlane(model.selection("FACE", "Box_1_1/Right"))
model.end()
model.checkSketch(Sketch_1, SKETCH_DOF)

model.testNbResults(ExtrusionFuse_1, 1)
model.testNbSubResults(ExtrusionFuse_1, [0])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.FACE, [8])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.EDGE, [30])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.VERTEX, [60])
model.testResultsVolumes(ExtrusionFuse_1, [1028.2743338823])

checkMiddlePoint(ExtrusionFuse_1, 4.972503122, 5.15123283, 5.0145146)

# change plane removing features related to external objects
model.begin()
Sketch_1.setPlane(model.selection("FACE", "Box_1_1/Top"), True)
model.end()
SKETCH_DOF = 6
model.checkSketch(Sketch_1, SKETCH_DOF)

model.testNbResults(ExtrusionFuse_1, 1)
model.testNbSubResults(ExtrusionFuse_1, [2])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.FACE, [9])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.EDGE, [30])
model.testNbSubShapes(ExtrusionFuse_1, GeomAPI_Shape.VERTEX, [60])
model.testResultsVolumes(ExtrusionFuse_1, [1028.2743338823])

checkMiddlePoint(ExtrusionFuse_1, 5.0, 0.73606797, 5.5)

assert(model.checkPythonDump())
