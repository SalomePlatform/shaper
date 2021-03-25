# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-53.93497380447579, 58.11303515552675, 60.01215395145899, 58.11303515552675)
SketchLine_2 = Sketch_1.addLine(60.01215395145899, 58.11303515552675, 60.01215395145899, 11.01488901640705)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_3 = Sketch_2.addLine(31.90519577166173, 58.11303515552675, 60.012153951459, 58.11303515552675)
SketchProjection_1 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_1"), False)
SketchLine_4 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_3.startPoint(), SketchLine_4.result())
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchAPI_Line(SketchLine_4).endPoint(), SketchLine_3.endPoint())
SketchLine_5 = Sketch_2.addLine(60.012153951459, 58.11303515552675, 60.012153951459, 33.0446670492211)
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_5.startPoint())
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_2"), False)
SketchLine_6 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.result())
model.do()
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2")])
Edge_2 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchLine_3"), model.selection("EDGE", "Sketch_2/SketchLine_5")])
Cut_1 = model.addCut(Part_1_doc, [model.selection("EDGE", "Edge_1_1"), model.selection("EDGE", "Edge_1_2")], [model.selection("EDGE", "Edge_2_1"), model.selection("EDGE", "Edge_2_2")])
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Cut_1, 2)
model.testNbSubResults(Cut_1, [0, 0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [0, 0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [0, 0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [1, 1])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [2, 2])

assert(model.checkPythonDump())
