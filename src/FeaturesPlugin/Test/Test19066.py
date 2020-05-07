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
SketchLine_1 = Sketch_1.addLine(19.10387596899224, 5.793798449612405, 19.10387596899224, 27.4031007751938)
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchLine_2 = Sketch_1.addLine(19.10387596899224, 27.4031007751938, 74.84961240310078, 27.4031007751938)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchLine_3 = Sketch_1.addLine(74.84961240310078, 27.4031007751938, 74.84961240310078, 54.0232558139535)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_3.result())
SketchLine_4 = Sketch_1.addLine(74.84961240310078, 54.0232558139535, 102.0961240310078, 54.0232558139535)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(102.0961240310078, 54.0232558139535, 102.0961240310078, 26.77674418604651)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_5.result())
SketchLine_6 = Sketch_1.addLine(102.0961240310078, 26.77674418604651, 136.2325581395349, 26.77674418604651)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchLine_7 = Sketch_1.addLine(136.2325581395349, 26.77674418604651, 136.2325581395349, 0.4697674418604749)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_7.result())
SketchLine_8 = Sketch_1.addLine(136.2325581395349, 0.4697674418604749, 103.9751937984496, 0.4697674418604749)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_8.result())
SketchLine_9 = Sketch_1.addLine(103.9751937984496, 0.4697674418604749, 103.9751937984496, -20.82635658914729)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_9.result())
SketchLine_10 = Sketch_1.addLine(103.9751937984496, -20.82635658914729, 77.35503875968993, -18.63410852713179)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchLine_11 = Sketch_1.addLine(77.35503875968993, -18.63410852713179, 76.10232558139535, 9.238759689922478)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchLine_12 = Sketch_1.addLine(76.10232558139535, 9.238759689922478, 19.10387596899224, 5.793798449612405)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_12.endPoint())
SketchPoint_1 = Sketch_1.addPoint(88.97935811509896, 16.06459344588224)
model.do()
Wire_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_7"), model.selection("EDGE", "Sketch_1/SketchLine_9"), model.selection("EDGE", "Sketch_1/SketchLine_6"), model.selection("EDGE", "Sketch_1/SketchLine_8"), model.selection("EDGE", "Sketch_1/SketchLine_10"), model.selection("EDGE", "Sketch_1/SketchLine_5"), model.selection("EDGE", "Sketch_1/SketchLine_11"), model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_12"), model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("WIRE", "Sketch_1/Face-SketchLine_12r-SketchLine_11r-SketchLine_10r-SketchLine_9r-SketchLine_8r-SketchLine_7r-SketchLine_6r-SketchLine_5r-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r_wire")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects, False)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), 100, False)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchLine_13 = Sketch_2.addLine(54.51293454413378, 26.90644687116999, 78.22337943042922, 26.90644687116999)
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_13.result())
SketchLine_14 = Sketch_2.addLine(78.22337943042922, 26.90644687116999, 78.22337943042922, 42.49687638544646)
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintVertical_6 = Sketch_2.setVertical(SketchLine_14.result())
SketchLine_15 = Sketch_2.addLine(78.22337943042922, 42.49687638544646, 92.18980587030188, 42.49687638544646)
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchConstraintHorizontal_6 = Sketch_2.setHorizontal(SketchLine_15.result())
SketchLine_16 = Sketch_2.addLine(92.18980587030188, 42.49687638544646, 92.18980587030188, 26.25684564140846)
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchConstraintVertical_7 = Sketch_2.setVertical(SketchLine_16.result())
SketchLine_17 = Sketch_2.addLine(92.18980587030188, 26.25684564140846, 103.5578273911285, 24.63284256700466)
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchLine_16.endPoint(), SketchLine_17.startPoint())
SketchLine_18 = Sketch_2.addLine(103.5578273911285, 24.63284256700466, 103.5578273911285, 15.21362473546265)
SketchConstraintCoincidence_17 = Sketch_2.setCoincident(SketchLine_17.endPoint(), SketchLine_18.startPoint())
SketchConstraintVertical_8 = Sketch_2.setVertical(SketchLine_18.result())
SketchLine_19 = Sketch_2.addLine(103.5578273911285, 15.21362473546265, 94.13860955958643, 16.83762780986643)
SketchConstraintCoincidence_18 = Sketch_2.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
SketchLine_20 = Sketch_2.addLine(94.13860955958643, 16.83762780986643, 94.78821078934796, 9.367213667608976)
SketchConstraintCoincidence_19 = Sketch_2.setCoincident(SketchLine_19.endPoint(), SketchLine_20.startPoint())
SketchLine_21 = Sketch_2.addLine(94.78821078934796, 9.367213667608976, 83.42018926852137, 11.96561858665505)
SketchConstraintCoincidence_20 = Sketch_2.setCoincident(SketchLine_20.endPoint(), SketchLine_21.startPoint())
SketchLine_22 = Sketch_2.addLine(83.42018926852137, 11.96561858665505, 83.09538865364063, 18.13683026938948)
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchLine_21.endPoint(), SketchLine_22.startPoint())
SketchLine_23 = Sketch_2.addLine(83.09538865364063, 18.13683026938948, 60.03454499710668, 20.08563395867403)
SketchConstraintCoincidence_22 = Sketch_2.setCoincident(SketchLine_22.endPoint(), SketchLine_23.startPoint())
SketchLine_24 = Sketch_2.addLine(60.03454499710668, 20.08563395867403, 54.51293454413378, 26.90644687116999)
SketchConstraintCoincidence_23 = Sketch_2.setCoincident(SketchLine_23.endPoint(), SketchLine_24.startPoint())
SketchConstraintCoincidence_24 = Sketch_2.setCoincident(SketchLine_13.startPoint(), SketchLine_24.endPoint())
SketchPoint_2 = Sketch_2.addPoint(86.98316963342577, 22.7185550514595)
model.do()
Wire_2_objects = [model.selection("EDGE", "Sketch_2/SketchLine_13"), model.selection("EDGE", "Sketch_2/SketchLine_24"), model.selection("EDGE", "Sketch_2/SketchLine_23"), model.selection("EDGE", "Sketch_2/SketchLine_22"), model.selection("EDGE", "Sketch_2/SketchLine_21"), model.selection("EDGE", "Sketch_2/SketchLine_20"), model.selection("EDGE", "Sketch_2/SketchLine_19"), model.selection("EDGE", "Sketch_2/SketchLine_18"), model.selection("EDGE", "Sketch_2/SketchLine_17"), model.selection("EDGE", "Sketch_2/SketchLine_16"), model.selection("EDGE", "Sketch_2/SketchLine_15"), model.selection("EDGE", "Sketch_2/SketchLine_14")]
Wire_2 = model.addWire(Part_1_doc, Wire_2_objects, False)
Edge_1 = model.addEdge(Part_1_doc, model.selection("VERTEX", "Sketch_1/SketchPoint_1"), model.selection("VERTEX", "Sketch_2/SketchPoint_2"))
Pipe_1 = model.addPipe(Part_1_doc, [model.selection("WIRE", "Wire_1_1"), model.selection("WIRE", "Wire_2_1")], model.selection("EDGE", "Edge_1_1"), [])

model.end()

from GeomAPI import *

model.testNbResults(Pipe_1, 1)
model.testNbSubResults(Pipe_1, [0])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.FACE, [12])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.EDGE, [48])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.VERTEX, [96])
model.testResultsVolumes(Pipe_1, [27370.29819])

assert(model.checkPythonDump())
