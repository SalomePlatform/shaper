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

# Cheching for selection of the whole feature (Sketch) in PartSet from the Part

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(16, 15, 3)
model.do()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Edge_1 = model.addEdge(Part_1_doc, [model.selection("COMPOUND", "PartSet/all-in-Sketch_1")], False)
model.end()

# check the result validity
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Edge_1.feature()))

assert(model.checkPythonDump())
