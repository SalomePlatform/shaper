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

Revolution_1 = model.addRevolution(Part_1_doc, [], model.selection("EDGE", "PartSet/OZ"), 90)
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchCircle_1 = Sketch_1.addCircle(30, 20, 15)
Revolution_1.setNestedSketch(Sketch_1)
model.do()
Shape = Revolution_1.results()[0].resultSubShapePair()[0].shape()
checkMiddlePoint(Shape, -19.9197187127138, 19.9197187127138, 20)

Revolution_1.setAxis(model.selection("EDGE", "PartSet/OY"))
model.do()
Shape = Revolution_1.results()[0].resultSubShapePair()[0].shape()
checkMiddlePoint(Shape, 14.2731178230633, 30, 14.2731178230633)

model.end()

assert(model.checkPythonDump())
