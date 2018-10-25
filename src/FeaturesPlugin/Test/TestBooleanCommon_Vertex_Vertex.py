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
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchPoint_1 = Sketch_1.addPoint(15.05904629113425, 12.50050543761873)
SketchPoint_2 = Sketch_1.addPoint(47.25352258215165, 31.63008715695855)
model.do()
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_1/SketchPoint_1"), model.selection("VERTEX", "Sketch_1/SketchPoint_2")])
Common_1 = model.addCommon(Part_1_doc, [model.selection("VERTEX", "Vertex_1_1"), model.selection("VERTEX", "Vertex_1_2")])
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchPoint_3 = Sketch_2.addPoint(15.05904629113425, 12.50050543761873)
SketchProjection_1 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchPoint_1"), False)
SketchPoint_4 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchPoint_3.coordinates(), SketchPoint_4.result())
model.do()
Vertex_2 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_2/SketchPoint_3")])
Vertex_3 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_1/SketchPoint_1")])
Common_2 = model.addCommon(Part_1_doc, [model.selection("VERTEX", "Vertex_2_1"), model.selection("VERTEX", "Vertex_3_1")])
model.testHaveNamingSubshapes(Common_2, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Common_1, 0)
model.testNbSubResults(Common_1, [])
model.testNbSubShapes(Common_1, GeomAPI_Shape.SOLID, [])
model.testNbSubShapes(Common_1, GeomAPI_Shape.FACE, [])
model.testNbSubShapes(Common_1, GeomAPI_Shape.EDGE, [])
model.testNbSubShapes(Common_1, GeomAPI_Shape.VERTEX, [])

model.testNbResults(Common_2, 1)
model.testNbSubResults(Common_2, [0])
model.testNbSubShapes(Common_2, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Common_2, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Common_2, GeomAPI_Shape.EDGE, [0])
model.testNbSubShapes(Common_2, GeomAPI_Shape.VERTEX, [1])

assert(model.checkPythonDump())
