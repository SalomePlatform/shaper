# Copyright (C) 2018-2023  CEA, EDF
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
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), 5)

Rotation_1 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Bottom]"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Bottom]"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Bottom]"))
Shape = Rotation_1.results()[0].resultSubShapePair()[0].shape()
checkMiddlePoint(Shape, 0, 7.071067811865, 5)

model.end()

assert(model.checkPythonDump())
