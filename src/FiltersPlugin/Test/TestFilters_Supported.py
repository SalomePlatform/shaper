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

from salome.shaper import model
from ModelAPI import *
from GeomAPI import *
from ModelHighAPI import *

# Whole list of supported filters
FILTER_BELONGS_TO = "BelongsTo"
FILTER_ON_PLANE = "OnPlane"
FILTER_ON_LINE = "OnLine"
FILTER_ON_GEOMETRY = "OnGeometry"
FILTER_ON_PLANE_SIDE = "OnPlaneSide"
FILTER_OPPOSITE_TO_EDGE = "OppositeToEdge"
FILTER_RELATIVE_TO_SOLID = "RelativeToSolid"
FILTER_EXTERNAL_FACES = "ExternalFaces"
FILTER_HORIZONTAL_FACES = "HorizontalFaces"
FILTER_VERTICAL_FACES = "VerticalFaces"
FILTER_CONNECTED_FACES = "TopoConnectedFaces"
FILTER_EDGE_SIZE = "EdgeSize"
FILTER_FACE_SIZE = "FaceSize"
FILTER_VOLUME_SIZE = "VolumeSize"
FILTER_FEATURE_EDGES = "FeatureEdges"
FILTER_CONTINUOUS_FACES= "ContinuousFaces"

# Reference data (supported filters) for each type of shape
Reference = {
    GeomAPI_Shape.VERTEX : [FILTER_BELONGS_TO, FILTER_ON_PLANE, FILTER_ON_LINE, FILTER_ON_PLANE_SIDE, FILTER_RELATIVE_TO_SOLID],
    GeomAPI_Shape.EDGE   : [FILTER_BELONGS_TO, FILTER_ON_PLANE, FILTER_ON_LINE, FILTER_ON_GEOMETRY, FILTER_ON_PLANE_SIDE, FILTER_OPPOSITE_TO_EDGE, FILTER_RELATIVE_TO_SOLID, FILTER_EDGE_SIZE, FILTER_FEATURE_EDGES],
    GeomAPI_Shape.WIRE   : [FILTER_BELONGS_TO, FILTER_ON_PLANE, FILTER_ON_PLANE_SIDE, FILTER_RELATIVE_TO_SOLID],
    GeomAPI_Shape.FACE   : [FILTER_BELONGS_TO, FILTER_ON_PLANE, FILTER_ON_GEOMETRY, FILTER_ON_PLANE_SIDE, FILTER_RELATIVE_TO_SOLID, FILTER_EXTERNAL_FACES, FILTER_HORIZONTAL_FACES, FILTER_VERTICAL_FACES, FILTER_CONNECTED_FACES, FILTER_FACE_SIZE, FILTER_CONTINUOUS_FACES],
    GeomAPI_Shape.SHELL  : [FILTER_BELONGS_TO, FILTER_ON_PLANE, FILTER_ON_PLANE_SIDE, FILTER_RELATIVE_TO_SOLID],
    GeomAPI_Shape.SOLID  : [FILTER_BELONGS_TO, FILTER_ON_PLANE_SIDE, FILTER_VOLUME_SIZE],
}

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
# load 'Filters' plugin
model.filters(Part_1_doc, [])

filtersFactory = ModelAPI_Session.get().filters()
for type, res in Reference.items():
  filtersList = filtersFactory.filters(type)
  for filter in filtersList:
    filterID = filtersFactory.id(filter)
    assert(filterID in res), "Filter \"{}\" is not applicable for {}".format(filterID, strByShapeType(type))
    res.remove(filterID)
  assert(len(res) == 0), "There are more filters {}, which should be supported for {}".format(res, strByShapeType(type))
model.end()
