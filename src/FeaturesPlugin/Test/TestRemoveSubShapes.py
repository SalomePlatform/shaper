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

#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *
import math

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

# Create a part for extrusion
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
assert (len(aPartFeature.results()) == 1)

aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()

#=========================================================================
# Create a sketch to extrude
#=========================================================================
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)

# Create circles
aSketchCircle = aSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("circle_center"))
aCircleRadius = aSketchCircle.real("circle_radius")
anCircleCentr.setValue(-25, 0)
aCircleRadius.setValue(50)
aSketchCircle = aSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("circle_center"))
aCircleRadius = aSketchCircle.real("circle_radius")
anCircleCentr.setValue(25, 0)
aCircleRadius.setValue(50)
aSession.finishOperation()
aSketchResult = aSketchFeature.firstResult()

#=========================================================================
# Make extrusion on sketch
#=========================================================================
# Create extrusion
aSession.startOperation()
anExtrusionFeature = aPart.addFeature("Extrusion")
anExtrusionFeature.selectionList("base").append(aSketchResult, None)
anExtrusionFeature.string("CreationMethod").setValue("BySizes")
anExtrusionFeature.real("to_size").setValue(50)
anExtrusionFeature.real("from_size").setValue(0)
anExtrusionFeature.real("to_offset").setValue(0) #TODO: remove
anExtrusionFeature.real("from_offset").setValue(0) #TODO: remove
anExtrusionFeature.execute()
aSession.finishOperation()
anExtrusionResult = modelAPI_ResultCompSolid(modelAPI_ResultBody(anExtrusionFeature.firstResult()))

#=========================================================================
# Remove sub-shapes
#=========================================================================
aSession.startOperation()
aRemoveSubShapesFeature = aPart.addFeature("Remove_SubShapes")
aRemoveSubShapesFeature.selection("base_shape").setValue(anExtrusionResult, None)
aRemoveSubShapesFeature.string("creation_method").setValue("by_keep_subshapes")
aRemoveSubShapesFeature.selectionList("subshapes_to_keep").removeLast();
aSession.finishOperation()
assert (len(aRemoveSubShapesFeature.results()) > 0)
anUnionResult = modelAPI_ResultCompSolid(modelAPI_ResultBody(aRemoveSubShapesFeature.firstResult()))
assert (anUnionResult.numberOfSubs() == 2)

from salome.shaper import model
assert(model.checkPythonDump())
