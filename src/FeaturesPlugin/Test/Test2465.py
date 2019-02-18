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

# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Ellipsoid_1 = model.addEllipsoid(Part_1_doc, 10, 20, 40)
Torus_1 = model.addTorus(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 15, 3)
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Ellipsoid_1_1")], model.selection("EDGE", "[Ellipsoid_1_1/Face_1][weak_name_3]"), 10, 2)
model.do()
model.end()

# check that for the linear copy direction the elliptical edge is invalid
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(not aFactory.validate(LinearCopy_1.feature()))
