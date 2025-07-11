# Copyright (C) 2014-2025  CEA, EDF
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
from ModelHighAPI import CHECK_NAMING

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Top"))
SketchCircle_1 = Sketch_1.addCircle(5, 5, 3)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_1.result(), 5, True)
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_2.result(), 5, True)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 3)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchCircle_1_2r_wire")], model.selection(), 5, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchCircle_1_2r_wire")], model.selection(), 10, -5)
Filters = model.filters(Part_1_doc, [model.addFilter(name = "RelativeToSolid", args = [model.selection("SOLID", "Box_1_1"), "not_in"]),
                                     model.addFilter(name = "TopoConnectedFaces", args = [model.selection("VERTEX", "[Extrusion_2_1/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_2_1/To_Face]")])])
model.end()

Reference = {}
# Faces of the box
ResultBox = Box_1.result().resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultBox.shape(), GeomAPI_Shape.FACE)
while exp.more():
  Reference[model.selection(ResultBox, exp.current())] = False
  exp.next()

# Faces of the first cylinder
ResultCylinder_1 = Extrusion_1.result().resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultCylinder_1.shape(), GeomAPI_Shape.FACE)
while exp.more():
  Reference[model.selection(ResultCylinder_1, exp.current())] = False
  exp.next()

# Faces of the second cylinder
ResultCylinder_2 = Extrusion_2.result().resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultCylinder_2.shape(), GeomAPI_Shape.FACE)
Reference[model.selection(ResultCylinder_2, exp.current())] = True;  exp.next()
Reference[model.selection(ResultCylinder_2, exp.current())] = False; exp.next()
Reference[model.selection(ResultCylinder_2, exp.current())] = True;  exp.next()

model.checkFilter(Part_1_doc, model, Filters, Reference)

model.begin()
Group_1 = model.addGroup(Part_1_doc, [model.selection("FACE", "Extrusion_2_1/To_Face"), model.selection("FACE", "Extrusion_2_1/Generated_Face&Sketch_1/SketchCircle_1_2"), Filters])
model.end()
assert(model.checkPythonDump(CHECK_NAMING))
