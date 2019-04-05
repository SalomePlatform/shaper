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
SketchLine_1 = Sketch_1.addLine(-30.96793587174349, 41.08817635270543, 39.4689378757515, 41.4929859719439)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_2 = Sketch_2.addLine(13.96593186372747, 52.01803607214429, -9.917835671342695, 52.01803607214429)
SketchLine_3 = Sketch_2.addLine(-9.917835671342695, 52.01803607214429, -9.917835671342695, 27.32464929859719)
SketchLine_4 = Sketch_2.addLine(-9.917835671342695, 27.32464929859719, 13.96593186372747, 27.32464929859719)
SketchLine_5 = Sketch_2.addLine(13.96593186372747, 27.32464929859719, 13.96593186372747, 52.01803607214429)
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_2.result())
SketchConstraintVertical_1 = Sketch_2.setVertical(SketchLine_3.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_4.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_5.result())
model.do()
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1")])
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchLine_2r-SketchLine_3f-SketchLine_4f-SketchLine_5f_wire")])
Cut_1 = model.addCut(Part_1_doc, [model.selection("EDGE", "Edge_1_1")], [model.selection("FACE", "Face_1_1")])
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [2])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [2])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [4])

assert(model.checkPythonDump())
