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

# Tests python API for the whole result (by feature) in group selection
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("COMPOUND", "all-in-Box_1")])
Group_2 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Box_1_1/Left"), model.selection("FACE", "Box_1_1/Top")])
GroupSubstraction_1 = model.addGroupSubstraction(Part_1_doc, [model.selection("COMPOUND", "Group_1")], [model.selection("COMPOUND", "Group_2")])
model.end()

aResultShape = GroupSubstraction_1.feature().firstResult().shape()

from GeomAPI import GeomAPI_ShapeIterator
anIter = GeomAPI_ShapeIterator(aResultShape)
aNum = 0
while anIter.more():
  aFace = anIter.current()
  assert(aFace.isFace())
  aNum = aNum + 1
  anIter.next()

assert(aNum == 4) # 6 from the whole result minus 2 from the second group (local selection)


assert(model.checkPythonDump())
