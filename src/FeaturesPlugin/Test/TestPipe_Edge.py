# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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
SketchArc_1 = Sketch_1.addArc(23.47558064206828, 28.1849874113521, 68.07028657666974, 20.97621888383109, 20.24058919303746, 73.24260576165612, False)
SketchLine_1 = Sketch_1.addLine(67.41776169413798, 4.173199186912264, 102.8479480835664, 10.72004206910377)
model.do()
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1")])
Pipe_1 = model.addPipe(Part_1_doc, [model.selection("EDGE", "Edge_1_1")], model.selection("EDGE", "Sketch_1/SketchArc_1_2"))
Edge_2 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1")])
Pipe_2 = model.addPipe(Part_1_doc, [model.selection("EDGE", "Edge_2_1")], model.selection("EDGE", "PartSet/OY"), model.selection("EDGE", "PartSet/OZ"))
model.do()
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Pipe_1, 1)
model.testNbSubResults(Pipe_1, [0])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.VERTEX, [8])
model.testResultsAreas(Pipe_1, [3693.366575752712378744035959244])

model.testNbResults(Pipe_2, 1)
model.testNbSubResults(Pipe_2, [0])
model.testNbSubShapes(Pipe_2, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Pipe_2, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Pipe_2, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Pipe_2, GeomAPI_Shape.VERTEX, [8])
model.testResultsAreas(Pipe_2, [3543.018638942841789685189723969])
