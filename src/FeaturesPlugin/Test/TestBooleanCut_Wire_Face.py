# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
SketchLine_1 = Sketch_1.addLine(-26.15951972555746, 50.06689536878219, -17.15094339622642, 14.37907375643225)
SketchLine_2 = Sketch_1.addLine(-17.15094339622642, 14.37907375643225, 26.85248713550602, 14.72555746140652)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(26.85248713550602, 14.72555746140652, 34.47512864493997, 50.41337907375645)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_2.addCircle(3.984562607204112, 58.72898799313897, 38.16357190022508)
model.do()
Wire_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchLine_3")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects)
Face_1 = model.addFace(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchCircle_1_2")])
Cut_1 = model.addCut(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], [model.selection("FACE", "Face_1_1")])
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [3])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [6])

assert(model.checkPythonDump())
