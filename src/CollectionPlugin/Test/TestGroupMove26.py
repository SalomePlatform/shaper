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

# Check that if particular result of group is concealed, other results are not concealed
# and don't treated like that by the move to the history algorithm

from salome.shaper import model
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(10.07512315270936, 19.52832512315271, 5.646825151450413)
SketchCircle_2 = Sketch_1.addCircle(-17.04064039408868, -4.229064039408867, 10.0065552781921)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_2_2f"), model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection(), 10, 0)
Group_1 = model.addGroup(Part_1_doc, "Solids", [model.selection("SOLID", "Extrusion_1_1")])
RemoveResults_1 = model.addRemoveResults(Part_1_doc, [model.selection("SOLID", "Extrusion_1_2")])
model.do()
Part_1_doc.moveFeature(Group_1.feature(), RemoveResults_1.feature())
model.end()

aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Group_1.feature()))
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 1)

assert(model.checkPythonDump())
