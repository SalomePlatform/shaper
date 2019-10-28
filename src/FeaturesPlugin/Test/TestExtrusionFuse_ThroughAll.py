# Copyright (C) 2018-2019  CEA/DEN, EDF R&D
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
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Box_3 = model.addBox(Part_1_doc, 20, 20, 20)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], 20, 10, 0)
Translation_2 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_3_1")], 40, 20, 0)
Edge_1 = model.addEdge(Part_1_doc, model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Bottom]"), model.selection("VERTEX", "[Translation_2_1/MF:Translated&Box_3_1/Front][Translation_2_1/MF:Translated&Box_3_1/Right][Translation_2_1/MF:Translated&Box_3_1/Top]"))
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Back"))
SketchCircle_1 = Sketch_1.addCircle(2.134236344973221, -2.430731739079631, 1.564909384334321)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")])
ExtrusionFuse_1_objects_2 = [model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Translation_1_1"), model.selection("SOLID", "Translation_2_1")]
ExtrusionFuse_1 = model.addExtrusionFuse(Part_1_doc, [model.selection("FACE", "Face_1_1")], model.selection("EDGE", "Edge_1_1"), ExtrusionFuse_1_objects_2)

model.do()
Shape = ExtrusionFuse_1.results()[0].resultSubShapePair()[0].shape()
checkMiddlePoint(Shape, 37.46245068, 23.05267081, 8.52187757)

model.end()

assert(model.checkPythonDump())
