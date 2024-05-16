# Copyright (C) 2018-2024  CEA, EDF
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
from GeomAPI import *
from SketchAPI import *

import math

def checkMiddlePoint(shape, x, y, z, tolerance = 1.e-7):
    assert(shape is not None)
    middlePoint = shape.middlePoint()
    assert(math.fabs(middlePoint.x() - x) < tolerance), "{} != {}".format(middlePoint.x(), x)
    assert(math.fabs(middlePoint.y() - y) < tolerance), "{} != {}".format(middlePoint.y(), y)
    assert(math.fabs(middlePoint.z() - z) < tolerance), "{} != {}".format(middlePoint.z(), z)

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Axis_4 = model.addAxis(Part_1_doc, 10, 10, 10)

ExtrusionFuse_1 = model.addExtrusionFuse(Part_1_doc, [], model.selection("FACE", "Box_1_1/Right"), 0, model.selection(), 0, [model.selection("SOLID", "Box_1_1")])
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Left"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(5, 10, 2)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.result(), SketchCircle_1.center())
ExtrusionFuse_1.setNestedSketch(Sketch_1)
model.do()
Shape = ExtrusionFuse_1.results()[0].resultSubShapePair()[0].shape()
checkMiddlePoint(Shape, 5, 5, 5.3457661)

ExtrusionFuse_2 = model.addExtrusionFuse(Part_1_doc, [], model.selection("EDGE", "Axis_1"), model.selection(), 0, model.selection("FACE", "ExtrusionFuse_1_1/Modified_Face&Box_1_1/Left&ExtrusionFuse_1_1/From_Face"), 0, [model.selection("SOLID", "ExtrusionFuse_1_1")])
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Front"))
SketchCircle_2 = Sketch_2.addCircle(10, 10, 2.040695131136049)
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "[(Box_1_1/Back)(Box_1_1/Bottom)(Box_1_1/Front)(ExtrusionFuse_1_1/Modified_Face&Box_1_1/Right&ExtrusionFuse_1_1/To_Face)][Box_1_1/Bottom][Box_1_1/Front]"), False)
SketchPoint_1 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchCircle_2.center(), SketchAPI_Point(SketchPoint_1).coordinates())
ExtrusionFuse_2.setNestedSketch(Sketch_2)
model.do()
Shape = ExtrusionFuse_2.results()[0].resultSubShapePair()[0].shape()
checkMiddlePoint(Shape, 4.97994444, 4.995858131, 4.254077951)

model.end()

assert(model.checkPythonDump())
