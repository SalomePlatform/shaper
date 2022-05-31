# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
SketchLine_1 = Sketch_1.addLine(-27.76647174128943, 72.31029012920524, 42.2686420251427, 59.94944420697596)
SketchLine_2 = Sketch_1.addLine(42.2686420251427, 59.94944420697596, 59.0507718766223, 21.43074552356853)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(59.0507718766223, 21.43074552356853, 56.1909608273397, -69.919416180974)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(56.1909608273397, -69.919416180974, -29.91225626113892, -65.09334567012574)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(-29.91225626113892, -65.09334567012574, -57.18321726979322, -2.500460309588558)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(-57.18321726979322, -2.500460309588558, -27.76647174128943, 72.31029012920524)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_1.startPoint())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchLine_7 = Sketch_2.addLine(24.33758278426026, -52.48127186534471, 29.40566289183227, 59.45054433956496)
model.do()
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchLine_7")])
Face_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_6"), model.selection("EDGE", "Sketch_1/SketchLine_5"), model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_2")]
Face_1 = model.addFace(Part_1_doc, Face_1_objects)
Split_1 = model.addSplit(Part_1_doc, [model.selection("FACE", "Face_1_1")], [model.selection("EDGE", "Edge_1_1")])
model.do()

model.checkResult(Split_1,model,1,[0],[0],[1],[6],[13])
model.testHaveNamingSubshapes(Split_1,model,Part_1_doc)

model.end()

assert(model.checkPythonDump())
