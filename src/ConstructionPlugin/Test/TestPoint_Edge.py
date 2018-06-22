## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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

"""
Test case for Construction Point feature by coordinates.
"""

from salome.shaper import model
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 0, 100, 100)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchPoint_1.result())
model.do()
Point_2 = model.addPoint(Part_1_doc, model.selection("EDGE", "Sketch_1/Edge-SketchLine_1"), 50, False, False)
Point_3 = model.addPoint(Part_1_doc, model.selection("EDGE", "Sketch_1/Edge-SketchLine_1"), 0.4, True, False)
Point_4 = model.addPoint(Part_1_doc, model.selection("EDGE", "Sketch_1/Edge-SketchLine_1"), 0.4, True, True)
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 50, 10)
Point_5 = model.addPoint(Part_1_doc, model.selection("EDGE", "Cylinder_1_1/Face_1&Cylinder_1_1/Face_2"), 0.25, True, False)
Point_6 = model.addPoint(Part_1_doc, model.selection("EDGE", "Cylinder_1_1/Face_1&Cylinder_1_1/Face_2"), 0.25, True, True)
model.do()
model.end()

assert (len(Point_2.results()) > 0)
rightPosition = GeomAPI_Vertex(35.3553390593, 35.3553390593, 0)
assert(rightPosition.isEqual(Point_2.results()[0].resultSubShapePair()[0].shape()))

assert (len(Point_3.results()) > 0)
rightPosition = GeomAPI_Vertex(40, 40, 0)
assert(rightPosition.isEqual(Point_3.results()[0].resultSubShapePair()[0].shape()))

assert (len(Point_4.results()) > 0)
rightPosition = GeomAPI_Vertex(60, 60, 0)
assert(rightPosition.isEqual(Point_4.results()[0].resultSubShapePair()[0].shape()))

assert (len(Point_5.results()) > 0)
rightPosition = GeomAPI_Vertex(0, 50, 10)
assert(rightPosition.isEqual(Point_5.results()[0].resultSubShapePair()[0].shape()))

assert (len(Point_6.results()) > 0)
rightPosition = GeomAPI_Vertex(0, -50, 10)
assert(rightPosition.isEqual(Point_6.results()[0].resultSubShapePair()[0].shape()))

assert(model.checkPythonDump())
