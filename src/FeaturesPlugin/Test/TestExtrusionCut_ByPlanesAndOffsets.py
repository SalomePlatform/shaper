## Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

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

ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection("FACE", "Box_1_1/Right"), 0, model.selection(), 0, [model.selection("SOLID", "Box_1_1")])
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Left"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(5, 10, 2)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.result(), SketchCircle_1.center())
ExtrusionCut_1.setNestedSketch(Sketch_1)
model.do()
Shape = ExtrusionCut_1.results()[0].resultSubShapePair()[0].shape()
checkMiddlePoint(Shape, 5, 5, 4.97049494)

ExtrusionCut_2 = model.addExtrusionCut(Part_1_doc, [], model.selection("EDGE", "Axis_1"), model.selection(), 0, model.selection("FACE", "ExtrusionCut_1_1/Modified_Face&Box_1_1/Left"), 0, [model.selection("SOLID", "ExtrusionCut_1_1")])
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Front"))
SketchCircle_2 = Sketch_2.addCircle(10, 10, 2.040695131136049)
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "[ExtrusionCut_1_1/Modified_Face&Box_1_1/Right][Box_1_1/Front][ExtrusionCut_1_1/Modified_Face&Box_1_1/Top&weak_name_2]"), False)
SketchPoint_1 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchCircle_2.center(), SketchAPI_Point(SketchPoint_1).coordinates())
ExtrusionCut_2.setNestedSketch(Sketch_2)
model.do()
Shape = ExtrusionCut_2.results()[0].resultSubShapePair()[0].shape()
checkMiddlePoint(Shape, 5.00107093476, 4.96064972557, 4.9770664027)

model.end()

assert(model.checkPythonDump())
