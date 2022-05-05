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
from GeomAPI import *
from ModelHighAPI import *
import math

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Box
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
model.do()

### Create Filters
filter_1 = model.addFilter(name = "OnPlane",
                           args = [model.selection("FACE", "Box_1_1/Left")])

filter_2 = model.addFilter(name = "OnPlane",
                           args = [model.selection("FACE", "Box_1_1/Top")])

filters = model.filters(Part_1_doc, [filter_1, filter_2])

### Select all (one) suitable edges
selected_edges = filters.select("Edges")

group_1 = model.addGroup(Part_1_doc, "Edges", selected_edges)
assert(group_1.feature().results().size() == 1)

# Check the selected edge
aResult = group_1.results()[0].resultSubShapePair()[0]
aShape = aResult.shape()
aShapeExplorer = GeomAPI_ShapeExplorer(aShape, GeomAPI_Shape.EDGE)
assert(aShapeExplorer.more())
anEdge = aShapeExplorer.current()
assert(anEdge.edge().isLine() and math.fabs(anEdge.edge().line().direction().x() - 1.0) < 1.e-7)
aLoc = anEdge.edge().line().location()
assert(math.fabs(aLoc.x()) < 1.e-7)
assert(math.fabs(aLoc.y()) < 1.e-7)
assert(math.fabs(aLoc.z() - 10.0) < 1.e-7)
aShapeExplorer.next()
assert(not aShapeExplorer.more())

model.end()
