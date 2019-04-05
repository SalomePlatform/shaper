# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
SketchPoint_1 = Sketch_1.addPoint(0, 50)
SketchPoint_2 = Sketch_1.addPoint(0, 25)
model.do()
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_1/SketchPoint_1")])
Vertex_2 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_1/SketchPoint_1")])
Vertex_3 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_1/SketchPoint_1")])
Vertex_4 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_1/SketchPoint_2")])
Cut_1 = model.addCut(Part_1_doc, [model.selection("VERTEX", "Vertex_1_1")], [model.selection("VERTEX", "Vertex_2_1")])
Cut_2 = model.addCut(Part_1_doc, [model.selection("VERTEX", "Vertex_3_1")], [model.selection("VERTEX", "Vertex_4_1")])
model.testHaveNamingSubshapes(Cut_2, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Cut_1, 0)
model.testNbSubResults(Cut_1, [])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [])

model.testNbResults(Cut_2, 1)
model.testNbSubResults(Cut_2, [0])
model.testNbSubShapes(Cut_2, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Cut_2, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Cut_2, GeomAPI_Shape.EDGE, [0])
model.testNbSubShapes(Cut_2, GeomAPI_Shape.VERTEX, [1])

assert(model.checkPythonDump())
