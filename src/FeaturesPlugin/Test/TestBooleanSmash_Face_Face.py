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
SketchCircle_1 = Sketch_1.addCircle(-31.7032590051458, 14.72555746140652, 29.35516607310447)
SketchCircle_1.result().setColor(225, 0, 0)
SketchCircle_1.results()[1].setColor(225, 0, 0)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_2 = Sketch_2.addCircle(12.30017152658664, 14.37907375643225, 29.61574408620473)
SketchCircle_2.result().setColor(225, 0, 0)
SketchCircle_2.results()[1].setColor(225, 0, 0)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2f")])
Smash_1 = model.addSmash(Part_1_doc, [model.selection("FACE", "Face_1_1")], [model.selection("FACE", "Face_1_2")])
model.testHaveNamingSubshapes(Smash_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Smash_1, 1)
model.testNbSubResults(Smash_1, [2])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.EDGE, [5])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.VERTEX, [10])

assert(model.checkPythonDump())
