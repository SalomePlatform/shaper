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

from salome.shaper import model
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Filters = model.filters(Part_1_doc, [model.addFilter(name = "BelongsTo", args = [model.selection("SOLID", "Box_1_1")]), model.addFilter(name = "HorizontalFaces"), model.addFilter(name = "OnGeometry", args = [model.selection("FACE", "PartSet/XOY")])])
Group_1 = model.addGroup(Part_1_doc, [model.selection("FACE", "Box_1_1/Bottom"), Filters])
model.end()

aFactory = ModelAPI_Session.get().validators()

GroupFeature = Group_1.feature()
assert(aFactory.validate(GroupFeature))
assert(GroupFeature.selectionList("group_list").size() == 1)

assert(model.checkPythonDump())
