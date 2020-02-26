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
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 2, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1")], 5, 5, 0)

RevolutionFuse_1 = model.addRevolutionFuse(Part_1_doc, [], model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]"), [model.selection("SOLID", "Box_1_1")])

Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Translation_1_1/MF:Translated&Cylinder_1_1/Face_2"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "[Translation_1_1/MF:Translated&Cylinder_1_1/Face_1][Translation_1_1/MF:Translated&Cylinder_1_1/Face_2]__cc"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "[Translation_1_1/MF:Translated&Cylinder_1_1/Face_1][Translation_1_1/MF:Translated&Cylinder_1_1/Face_2]"), False)
SketchCircle_1 = SketchProjection_2.createdFeature()
SketchCircle_2 = Sketch_1.addCircle(5, 5, 2)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_2.center())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchCircle_1.results()[1], SketchCircle_2.results()[1])

RevolutionFuse_1.setNestedSketch(Sketch_1)

model.do()

Shape = RevolutionFuse_1.results()[0].resultSubShapePair()[0].shape()
checkMiddlePoint(Shape, 6.3892968486, 5.0, 6.3892968486)

model.end()

assert(model.checkPythonDump())
