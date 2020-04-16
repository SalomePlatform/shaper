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
SketchCircle_1 = Sketch_1.addCircle(-32.92409574173262, 1.332083115848444, 52.41665939244462)
SketchLine_1 = Sketch_1.addLine(-32.3899127495535, 53.7460204836366, -32.9246838934834, -51.08457627329644)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchCircle_1.results()[1])
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_2 = Sketch_2.addCircle(52.19056504290929, 1.630574981545157, 59.89949304384059)
SketchLine_2 = Sketch_2.addLine(112.0898160449407, 1.460291967886176, -7.70480537031941, 0.9278161529026914)
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_2.startPoint(), SketchCircle_2.results()[1])
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchCircle_2.results()[1])
model.do()
Shell_1_objects = [model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2r-SketchCircle_2_2r-SketchLine_2r"), model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2r-SketchLine_2f"), model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r-SketchLine_1r"), model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r-SketchCircle_1_2r-SketchLine_1f")]
Shell_1 = model.addShell(Part_1_doc, Shell_1_objects)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SHELL", "Shell_1_2")], [model.selection("SHELL", "Shell_1_1")])
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [7])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [14])

assert(model.checkPythonDump())
