# Copyright (C) 2014-2024  CEA, EDF
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
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(25, 20, 10)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection(), 20, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/To_Face"))
SketchProjection_1 = Sketch_2.addProjection(model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1/To_Face]__cc"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_2 = Sketch_2.addCircle(25, 20, 5)
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchPoint_1.result(), SketchCircle_2.center())
model.end()
# check the sketch_2 projection and the whole sketch is valid after selection of the circle center from another part
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(SketchProjection_1.feature()))
assert(aFactory.validate(Sketch_2.feature()))

assert(model.checkPythonDump())
