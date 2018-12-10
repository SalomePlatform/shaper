## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

# -*- coding: utf-8 -*-

from salome.shaper import model
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
model.do()
# select a shell of a box : this test checks selection of a shell
aBoxResult = Box_1.feature().results()[0]
aShell = GeomAPI_ShapeExplorer(aBoxResult.shape(), GeomAPI_Shape.SHELL)
aGroup = Part_1_doc.addFeature("Group")
aGroup.selectionList("group_list").append(aBoxResult, aShell.current())
model.end()

# check that the resulting group is correct
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(aGroup))

assert(model.checkPythonDump())
