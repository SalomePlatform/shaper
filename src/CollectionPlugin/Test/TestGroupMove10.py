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
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-8, 20, 25)
SketchLine_1 = Sketch_1.addLine(40, 32, -5, 32)
SketchLine_2 = Sketch_1.addLine(-5, 32, -5, 5)
SketchLine_3 = Sketch_1.addLine(-5, 5, 40, 5)
SketchLine_4 = Sketch_1.addLine(40, 5, 40, 32)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
Group_1_objects = [model.selection("SOLID", "Extrusion_1_1_2"), model.selection("SOLID", "Extrusion_1_1_1"), model.selection("SOLID", "Extrusion_1_1_3")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
# to create groups of faces, edges and vertices
model.testHaveNamingSubshapes(Extrusion_1, model, Part_1_doc)
model.do()
Remove_SubShapes_1 = model.addRemoveSubShapes(Part_1_doc, model.selection("COMPSOLID", "Extrusion_1_1"))
Remove_SubShapes_1.setSubShapesToRemove([model.selection("SOLID", "Extrusion_1_1_2"), model.selection("SOLID", "Extrusion_1_1_3")])
model.do()
# move groups to the end
LastFeature = Remove_SubShapes_1.feature()
for i in range(Part_1_doc.size("Groups")):
  GroupFeature = Part_1_doc.feature(objectToResult(Part_1_doc.object("Groups", 0))) # move always the very first group
  Part_1_doc.moveFeature(GroupFeature, LastFeature)
  LastFeature = GroupFeature
model.end()

aFactory = ModelAPI_Session.get().validators()

# Check groups
a = 0
num_in_groups = [1, 10, 15, 7]
for i in range(Part_1_doc.size("Groups")):
  GroupFeature = Part_1_doc.feature(objectToResult(Part_1_doc.object("Groups", i)))
  assert(aFactory.validate(GroupFeature))
  assert(GroupFeature.selectionList("group_list").size() == num_in_groups[a])
  a = a + 1

assert(model.checkPythonDump())
