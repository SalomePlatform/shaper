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
### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
### Create SketchCircle
SketchCircle_1 = Sketch_1.addCircle(-10, 10, 20)
model.do()
### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
### Create SketchCircle
SketchCircle_2 = Sketch_2.addCircle(10, -10, 20)
model.do()
### Create Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection(), 20, 0)
### Create Extrusion
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2r")], model.selection(), 10, 0)
### Create Fuse
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_1"), model.selection("COMPOUND", "all-in-Extrusion_2")], removeEdges = True, keepSubResults = True)
### Create Fillet
Fillet_1 = model.addFillet(Part_1_doc, [model.selection("FACE", "Extrusion_1_1/To_Face")], 1, keepSubResults = True)
model.end()

# check the fillet validity
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Fillet_1.feature()))

assert(model.checkPythonDump())
