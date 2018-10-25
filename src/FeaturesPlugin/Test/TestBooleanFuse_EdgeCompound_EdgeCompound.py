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

# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-71.60846560846564, -5.878306878306883, -32.06349206349206, 30.99470899470901)
SketchLine_2 = Sketch_1.addLine(17.63492063492066, -11.75661375661376, 49.16402116402119, 24.04761904761905)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_3 = Sketch_2.addLine(-70.53968253968257, 29.92592592592593, -31.52910052910054, -5.343915343915344)
SketchLine_4 = Sketch_2.addLine(14.96296296296294, 26.18518518518519, 50.23280423280426, -8.015873015873021)
model.do()
Edge_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_2/SketchLine_3"), model.selection("EDGE", "Sketch_2/SketchLine_4")]
Edge_1 = model.addEdge(Part_1_doc, Edge_1_objects)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("EDGE", "Edge_1_1"), model.selection("EDGE", "Edge_1_2")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("EDGE", "Edge_1_3"), model.selection("EDGE", "Edge_1_4")])
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1"), model.selection("COMPOUND", "Compound_2_1")])
model.testHaveNamingSubshapes(Fuse_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [8])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [16])

assert(model.checkPythonDump())
