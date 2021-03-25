# Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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

from ModelAPI import *
from GeomDataAPI import *

# Get document
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

# Create a part
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()

# Create a sketch with circle for pipe profile
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)

# Create circle
aSketchCircleFeature = aSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircleFeature.attribute("circle_center"))
aCircleRadius = aSketchCircleFeature.real("circle_radius")
anCircleCentr.setValue(0, 0)
aCircleRadius.setValue(10)
aSession.finishOperation()

aSketchResult = aSketchFeature.firstResult()
aSketchShape = aSketchResult.shape()

aSession.startOperation()
aPipeFeature = aPart.addFeature("Pipe")
aPipeFeature.string("creation_method").setValue("WrongCreationMethod")
aPipeFeature.execute()
assert(aPipeFeature.error() != "")
aSession.finishOperation()

aSession.startOperation()
aBaseObjectsList = aPipeFeature.selectionList("base_objects")
aBaseObjectsList.append(aSketchResult, None)
aPipeFeature.execute()
assert(aPipeFeature.error() != "")
aSession.finishOperation()

aSession.startOperation()
aPathObjectSelection = aPipeFeature.selection("path_object")
aPathObjectSelection.selectSubShape("EDGE", "PartSet/OZ")
aPipeFeature.execute()
assert(aPipeFeature.error() != "")
aSession.finishOperation()

aSession.startOperation()
aPipeFeature.string("creation_method").setValue("binormal")
aPipeFeature.execute()
assert(aPipeFeature.error() != "")
aSession.finishOperation()

aSession.startOperation()
aPipeFeature.string("creation_method").setValue("locations")
aBaseObjectsList = aPipeFeature.selectionList("locations_objects")
aBaseObjectsList.append(None, None)
aPipeFeature.execute()
assert(aPipeFeature.error() != "")
aSession.finishOperation()
