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

# Checks that the Copy feature produces correct results and names if the whole result and feature are copied.

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-19.47177806477657, -9.723714972297476, -32.14322074518937, 18.42485654704614)
SketchLine_2 = Sketch_1.addLine(-32.14322074518937, 18.42485654704614, -13.96909823036938, 5.527027954588926)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-13.96909823036938, 5.527027954588926, -19.47177806477657, -9.723714972297476)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
SketchLine_4 = Sketch_1.addLine(-19.47177806477657, -9.723714972297476, -38.20802856635906, 4.439747463237282)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(-38.20802856635906, 4.439747463237282, -32.14322074518937, 18.42485654704614)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_5.endPoint())
SketchCircle_1 = Sketch_1.addCircle(15.45187411494798, 10.57784256870842, 6.036432809751617)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_5r-SketchLine_4r"), model.selection("FACE", "Sketch_1/Face-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection(), 10, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchCircle_1_2f_wire")], model.selection(), 10, 0)
Copy_1 = model.addCopy(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_1_1"), model.selection("COMPOUND", "all-in-Extrusion_2")], 1)
model.end()

assert(Copy_1.feature().results().size() == 2)

from ModelAPI import *
assert(Copy_1.feature().results()[0].data().name() == "Extrusion_1_1_1")
assert(modelAPI_ResultBody(Copy_1.feature().results()[0]).subResult(0).data().name() == "Extrusion_1_1_1_1")
assert(modelAPI_ResultBody(Copy_1.feature().results()[0]).subResult(1).data().name() == "Extrusion_1_1_1_2")
assert(Copy_1.feature().results()[1].data().name() == "Extrusion_2_1_1")

# Check that copy of the whole feature that contains several results produce compound of all results
model.begin()
Extrusion_3 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection(), 10, 0)
Copy_2 = model.addCopy(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_3")], 1)
model.end()

assert(Copy_2.feature().results()[0].data().name() == "Extrusion_3_1_1")
assert(Copy_2.feature().results()[0].shape().shapeTypeStr() == "COMPOUND")
aSub1 = modelAPI_ResultBody(Copy_2.feature().results()[0]).subResult(0)
assert(aSub1.shape().shapeTypeStr() == "COMPSOLID")
aSub2 = modelAPI_ResultBody(Copy_2.feature().results()[0]).subResult(1)
assert(aSub2.shape().shapeTypeStr() == "SOLID")

model.begin()
model.testHaveNamingSubshapes(Copy_1, model, Part_1_doc)
model.testHaveNamingSubshapes(Copy_2, model, Part_1_doc)
model.end()

assert(model.checkPythonDump())
