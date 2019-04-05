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
SketchPoint_1 = Sketch_1.addPoint(-28.13426853707415, 23.27655310621242)
SketchPoint_2 = Sketch_1.addPoint(10.32264529058116, 23.27655310621242)
SketchPoint_3 = Sketch_1.addPoint(47.56513026052106, 23.27655310621242)
model.do()
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_1/SketchPoint_1"), model.selection("VERTEX", "Sketch_1/SketchPoint_2")])
Vertex_2 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_1/SketchPoint_2"), model.selection("VERTEX", "Sketch_1/SketchPoint_3")])
Compound_1 = model.addCompound(Part_1_doc, [model.selection("VERTEX", "Vertex_1_1"), model.selection("VERTEX", "Vertex_1_2")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("VERTEX", "Vertex_2_1"), model.selection("VERTEX", "Vertex_2_2")])
Common_1 = model.addCommon(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1"), model.selection("COMPOUND", "Compound_2_1")])
model.testHaveNamingSubshapes(Common_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Common_1, 1)
model.testNbSubResults(Common_1, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.EDGE, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.VERTEX, [1])

assert(model.checkPythonDump())
