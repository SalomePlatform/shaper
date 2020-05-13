# Copyright (C) 2020  CEA/DEN, EDF R&D
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
SketchLine_1 = Sketch_1.addLine(102.3744998618265, 0, 0, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 83.57481045461809)
SketchLine_3 = Sketch_1.addLine(0, 83.57481045461809, 102.3744998618265, 83.57481045461809)
SketchLine_4 = Sketch_1.addLine(102.3744998618265, 83.57481045461809, 102.3744998618265, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(0, 83.57481045461809, 102.3744998618265, 0)
SketchLine_5.setAuxiliary(True)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.endPoint())
SketchLine_6 = Sketch_1.addLine(0, 0, 102.3744998618265, 83.57481045461809)
SketchLine_6.setAuxiliary(True)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_6.endPoint())
SketchPoint_2 = Sketch_1.addPoint(51.18724993091325, 41.78740522730904)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_5.result())
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_6.result())
model.do()
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r"), 100, False)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchCircle_1 = Sketch_2.addCircle(46.03714617613203, 29.83511540776045, 23.21299794073264)
model.do()
Edge_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchLine_3")]
Edge_1 = model.addEdge(Part_1_doc, Edge_1_objects, False)
Edge_2 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchCircle_1_2")], False)
Edge_3 = model.addEdge(Part_1_doc, model.selection("VERTEX", "Sketch_1/SketchPoint_2"), model.selection("VERTEX", "Sketch_2/SketchCircle_1"))
Pipe_1 = model.addPipe(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r"), model.selection("FACE", "Sketch_2/Face-SketchCircle_1_2r")], model.selection("EDGE", "Edge_3_1"), [])
Wire_1_objects = [model.selection("EDGE", "Edge_1_1"), model.selection("EDGE", "Edge_1_2"), model.selection("EDGE", "Edge_1_3"), model.selection("EDGE", "Edge_1_4")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects, False)
Recover_1 = model.addRecover(Part_1_doc, Pipe_1, [Edge_3.result()])
Pipe_2 = model.addPipe(Part_1_doc, [model.selection("EDGE", "Edge_2_1"), model.selection("WIRE", "Wire_1_1")], model.selection("EDGE", "Recover_1_1"), [])
Pipe_2.result().setColor(255, 0, 0)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SHELL", "Pipe_2_1")], [model.selection("SOLID", "Pipe_1_1")], keepSubResults = True)

model.end()

from GeomAPI import *

model.testNbResults(Pipe_1, 1)
model.testNbSubResults(Pipe_1, [0])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.FACE, [7])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.EDGE, [30])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.VERTEX, [60])
model.testResultsVolumes(Pipe_1, [470603.7881551325])

model.testNbResults(Pipe_2, 1)
model.testNbSubResults(Pipe_2, [0])
model.testNbSubShapes(Pipe_2, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Pipe_2, GeomAPI_Shape.FACE, [5])
model.testNbSubShapes(Pipe_2, GeomAPI_Shape.EDGE, [20])
model.testNbSubShapes(Pipe_2, GeomAPI_Shape.VERTEX, [40])
model.testResultsVolumes(Pipe_2, [26549.49767585])

# Cut should have no results
model.testNbResults(Cut_1, 0)

assert(model.checkPythonDump())
