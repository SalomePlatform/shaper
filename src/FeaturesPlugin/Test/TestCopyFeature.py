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

# Checks selection of the whole featurte and move to the end of the group created on results of this feature..

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(11.02869497636673, 9.8764247475525, 3.312248077480665)
SketchCircle_2 = Sketch_1.addCircle(4.278198729238611, 4.677840612715367, 1.794922837237287)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection(), 10, 0)
Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_1_2")])
Copy_1 = model.addCopy(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_1")], 1)
model.do()
# move the group feature to the end - through copy of the whole feature
Part_1_doc.moveFeature(Group_1.feature(), Copy_1.feature())
model.end()

from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Group_1.feature()))
selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 4) # two original solids plus two copies
assert(selectionList.value(3).namingName() == "Extrusion_1_1_1_1")
assert(selectionList.value(2).namingName() == "Extrusion_1_1_1_2")

assert(model.checkPythonDump())
