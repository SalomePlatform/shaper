## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-32.04974271012008, 18.88336192109778, 25.20058663660772)
SketchCircle_1.result().setColor(225, 0, 0)
SketchCircle_1.results()[1].setColor(225, 0, 0)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_2.addLine(22.00171526586623, 64.27272727272731, -86.7941680960549, 64.27272727272731)
SketchLine_1.result().setColor(225, 0, 0)
SketchLine_2 = Sketch_2.addLine(-86.7941680960549, 64.27272727272731, -86.7941680960549, -27.54545454545455)
SketchLine_2.result().setColor(225, 0, 0)
SketchLine_3 = Sketch_2.addLine(-86.7941680960549, -27.54545454545455, 22.00171526586623, -27.54545454545455)
SketchLine_3.result().setColor(225, 0, 0)
SketchLine_4 = Sketch_2.addLine(22.00171526586623, -27.54545454545455, 22.00171526586623, 64.27272727272731)
SketchLine_4.result().setColor(225, 0, 0)
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_2.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_4.result())
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f"), model.selection("FACE", "Sketch_2/Face-SketchLine_1f-SketchLine_2f-SketchLine_3f-SketchLine_4f")])
Smash_1 = model.addSmash(Part_1_doc, [model.selection("FACE", "Face_1_1")], [model.selection("FACE", "Face_1_2")])
model.testHaveNamingSubshapes(Smash_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Smash_1, 1)
model.testNbSubResults(Smash_1, [0])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.VERTEX, [8])

assert(model.checkPythonDump())
