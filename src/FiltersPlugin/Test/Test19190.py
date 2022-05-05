# Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(87.62648221343873, 42.39493620569578, -76.44861660079052, 42.39493620569578)
SketchLine_2 = Sketch_1.addLine(-76.44861660079052, 42.39493620569578, -76.44861660079052, -42.23747486149789)
SketchLine_3 = Sketch_1.addLine(-76.44861660079052, -42.23747486149789, 87.62648221343873, -42.23747486149789)
SketchLine_4 = Sketch_1.addLine(87.62648221343873, -42.23747486149789, 87.62648221343873, 42.39493620569578)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(-32.93478260869566, 42.39493620569578, -17.36561264822134, -42.23747486149789)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_1.result())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_3.result())
SketchLine_6 = Sketch_1.addLine(34.93083003952569, -42.23747486149789, 44.11264822134387, 42.39493620569579)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchLine_3.result())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_1.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection(), 100, 0)
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 35, 120)
Fuse_1_objects_1 = [model.selection("SOLID", "Extrusion_1_1_1"), model.selection("SOLID", "Cylinder_1_1"), model.selection("SOLID", "Extrusion_1_1_3")]
Fuse_1 = model.addFuse(Part_1_doc, Fuse_1_objects_1, removeEdges = True, keepSubResults = True)
Fuse_1.result().setColor(76, 76, 153)
Fuse_1.result().subResult(0).setColor(254, 127, 127)
Fuse_1.result().subResult(1).setColor(153, 153, 76)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Fuse_1_1_2/Modified_Face&Sketch_1/SketchLine_2"), 75, True)
Filters = model.filters(Part_1_doc, [model.addFilter(name = "RelativeToSolid", args = [model.selection("SOLID", "Fuse_1_1_2"), "on"])])
model.end()

from GeomAPI import *

model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [2])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.FACE, [14])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.EDGE, [54])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [108])
model.testResultsVolumes(Fuse_1, [1465576.14])

solid1 = Fuse_1.result().subResult(0).resultSubShapePair()[0]
solid2 = Fuse_1.result().subResult(1).resultSubShapePair()[0]

Reference = {}
exp = GeomAPI_ShapeExplorer(solid1.shape(), GeomAPI_Shape.FACE)
Reference[model.selection(solid1, exp.current())] = False; exp.next()
Reference[model.selection(solid1, exp.current())] = True;  exp.next()
while exp.more():
    Reference[model.selection(solid1, exp.current())] = False
    exp.next()
exp = GeomAPI_ShapeExplorer(solid2.shape(), GeomAPI_Shape.FACE)
while exp.more():
    Reference[model.selection(solid1, exp.current())] = True
    exp.next()

model.checkFilter(Part_1_doc, model, Filters, Reference)
