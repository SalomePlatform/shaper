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

from ConfigAPI import *
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
model.do()

# load 'Filters' plugin
FiltersFeature = model.filters(Part_1_doc, [model.addFilter(name = "OnGeometry", args = [model.selection("FACE", "Box_1_1/Left")])])

FiltersFactory = ModelAPI_Session.get().filters()
Filter = FiltersFactory.filter("OnGeometry")

# read XML representation to load validator
ValidatorReader = Config_ValidatorReader(Filter.xmlRepresentation(), True)
ValidatorReader.setFeatureId("FiltersSelection")
ValidatorReader.readAll()

aFactory = ModelAPI_Session.get().validators()
# no Group, thus not valid yet
assert(not aFactory.validate(FiltersFeature.feature()))

Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Box_1_1/Left"), FiltersFeature])
assert(aFactory.validate(FiltersFeature.feature()))

model.end()
