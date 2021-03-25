# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

from SketchAPI import *
from salome.shaper import model
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 2)
Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Box_1_1/Top")])
model.do()
Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Cylinder_1 = model.addCylinder(Part_2_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10, 45)
Group_2 = model.addGroup(Part_2_doc, "Faces", [model.selection("FACE", "Cylinder_1_1/Face_4"), model.selection("FACE", "Cylinder_1_1/Face_2")])
model.do()
Placement_1 = model.addPlacement(partSet, [model.selection("COMPOUND", "Part_2/")], model.selection("FACE", "Part_2/Cylinder_1_1/Face_3"), model.selection("FACE", "Part_1/Box_1_1/Top"), False, True)
model.do()

aSession = ModelAPI_Session.get()
aSession.setActiveDocument(partSet, False)
model.do()

model.exportToGEOM(partSet)
model.end()

# check what is exported to GEOM
import salome
from salome.geom import geomBuilder

import os
import tempfile

salome.salome_init(1)
aComp = salome.myStudy.FindComponent("GEOM")
iterator = salome.myStudy.NewChildIterator(aComp)
aComponents = []
while iterator.More():
  aComponents.append(iterator.Value())
  iterator.Next()

assert len(aComponents) == 2

ok, group_0_1 = aComponents[0].FindSubObject(1)
assert ok
assert group_0_1.GetName() == "Group_1"

ok, group_0_2 = aComponents[0].FindSubObject(2)
assert not ok


ok, group_1_1 = aComponents[1].FindSubObject(1)
assert ok
assert group_1_1.GetName() == "Group_1"

ok, group_1_2 = aComponents[1].FindSubObject(2)
assert not ok
