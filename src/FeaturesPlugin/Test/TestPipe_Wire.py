# Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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
SketchLine_1 = Sketch_1.addLine(25, -20, -20, -20)
SketchLine_2 = Sketch_1.addLine(-20, -20, -20, 15)
SketchLine_3 = Sketch_1.addLine(-20, 15, 25, 15)
SketchLine_4 = Sketch_1.addLine(25, 15, 25, -20)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(75, 25, 95, 25)
SketchLine_6 = Sketch_1.addLine(95, 25, 95, 45)
SketchLine_7 = Sketch_1.addLine(95, 45, 75, 45)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
model.do()
Pipe_1 = model.addPipe(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r_wire")], model.selection("EDGE", "PartSet/OZ"))
Wire_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_5"), model.selection("EDGE", "Sketch_1/SketchLine_6"), model.selection("EDGE", "Sketch_1/SketchLine_7")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects)
Pipe_2 = model.addPipe(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], model.selection("EDGE", "PartSet/OZ"), [])
model.do()
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Pipe_1, 1)
model.testNbSubResults(Pipe_1, [0])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(Pipe_1, [157500])

model.testNbResults(Pipe_2, 1)
model.testNbSubResults(Pipe_2, [0])
model.testNbSubShapes(Pipe_2, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Pipe_2, GeomAPI_Shape.FACE, [3])
model.testNbSubShapes(Pipe_2, GeomAPI_Shape.EDGE, [12])
model.testNbSubShapes(Pipe_2, GeomAPI_Shape.VERTEX, [24])
model.testResultsAreas(Pipe_2, [6000])
