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
SketchCircle_1 = Sketch_1.addCircle(10, 25, 7)
SketchCircle_2 = Sketch_1.addCircle(10, 25, 3)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchCircle_2.center())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r-SketchCircle_2_2r"), model.selection("FACE", "Sketch_1/Face-SketchCircle_2_2f")], model.selection(), 10, 0)
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), 0, 10, [model.selection("SOLID", "Extrusion_1_1_1"), model.selection("SOLID", "Extrusion_1_1_2")])
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1_1/To_Face"))
SketchCircle_3 = Sketch_2.addCircle(2, 30, 10)
ExtrusionCut_1.setNestedSketch(Sketch_2)
model.end()

# check the extrusion-cut result is correct
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(ExtrusionCut_1.feature()))
model.testNbResults(ExtrusionCut_1, 1)

assert(model.checkPythonDump())
