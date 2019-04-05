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
SketchLine_1 = Sketch_1.addLine(-55.80357142857143, 252.2321428571429, -250, 252.2321428571429)
SketchLine_2 = Sketch_1.addLine(-250, 252.2321428571429, -250, 4.464285714285698)
SketchLine_3 = Sketch_1.addLine(-250, 4.464285714285698, -55.80357142857143, 4.464285714285698)
SketchLine_4 = Sketch_1.addLine(-55.80357142857143, 4.464285714285698, -55.80357142857143, 252.2321428571429)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Face_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchLine_1")]
Face_1 = model.addFace(Part_1_doc, Face_1_objects)
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_5 = Sketch_2.addLine(-247.7678571428571, 71.42857142857137, -160.7142857142857, 167.4107142857142)
SketchLine_6 = Sketch_2.addLine(-160.7142857142857, 167.4107142857142, -276.7857142857143, 267.8571428571428)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_2.addLine(-276.7857142857143, 267.8571428571428, -154.0178571428571, 350.4464285714285)
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_2.addLine(-154.0178571428571, 350.4464285714285, -294.6428571428572, 428.5714285714284)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
model.do()
Wire_1_objects = [model.selection("EDGE", "Sketch_2/SketchLine_5"), model.selection("EDGE", "Sketch_2/SketchLine_6"), model.selection("EDGE", "Sketch_2/SketchLine_7"), model.selection("EDGE", "Sketch_2/SketchLine_8")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects)
Pipe_1 = model.addPipe(Part_1_doc, [model.selection("FACE", "Face_1_1")], model.selection("WIRE", "Wire_1_1"))
model.do()
model.testHaveNamingSubshapes(Pipe_1, model, Part_1_doc)
model.end()

assert(model.checkPythonDump())
