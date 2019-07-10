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
from GeomAPI import *
from ModelHighAPI import *

# Map filter and emptyness of its XML representation
Reference = {
    "BelongsTo"          : False,
    "OnPlane"            : False,
    "OnLine"             : False,
    "OnGeometry"         : False,
    "OnPlaneSide"        : False,
    "OppositeToEdge"     : False,
    "RelativeToSolid"    : False,
    "ExternalFaces"      : True,
    "HorizontalFaces"    : True,
    "VerticalFaces"      : True,
    "TopoConnectedFaces" : False,
}

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
# load 'Filters' plugin
model.filters(Part_1_doc, [])

filtersFactory = ModelAPI_Session.get().filters()
for id, res in Reference.items():
  filter = filtersFactory.filter(id)
  xmlString = filter.xmlRepresentation()
  assert((len(xmlString) == 0) == res)
model.end()
