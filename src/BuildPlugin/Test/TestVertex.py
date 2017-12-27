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

# Initialization of the test
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

import random

def createPoint(theSketchFeature):
    aSketchPointFeature = theSketchFeature.addFeature("SketchPoint")
    aPointCoordinates = geomDataAPI_Point2D(aSketchPointFeature.attribute("PointCoordinates"))
    aPointCoordinates.setValue(random.uniform(0, 100), random.uniform(0, 100))
    return aSketchPointFeature

# Get document
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

# Create a part
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()

# Create a sketch
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
anOrigin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
anOrigin.setValue(0, 0, 0)
aDirX = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
aDirX.setValue(1, 0, 0)
aNorm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
aNorm.setValue(0, 0, 1)

# Create points
aNumOfPoints = 10
aPoints = []
for i in range(aNumOfPoints):
    aSession.startOperation()
    aSketchPointFeature = createPoint(aSketchFeature)
    aSession.finishOperation()
    aPoints.append(aSketchPointFeature.firstResult().shape())
aSession.finishOperation()
aSketchResult = aSketchFeature.firstResult()

# Create vertices
aSession.startOperation()
aVertexFeature = aPart.addFeature("Vertex")
aBaseObjectsList = aVertexFeature.selectionList("base_objects")

for aPoint in aPoints:
    aBaseObjectsList.append(aSketchResult, aPoint)
aSession.finishOperation()

# Test results
assert (len(aVertexFeature.results()) == aNumOfPoints)

# Check Vertex feature failed on incorrect input
aSession.startOperation()
aVertexFeature2 = aPart.addFeature("Vertex")
aBaseObjectsList = aVertexFeature2.selectionList("base_objects")
aBaseObjectsList.append(aSketchResult, None)
aSession.finishOperation()
assert (len(aVertexFeature2.results()) == 0)

aSession.startOperation()
aLine = aSketchFeature.addFeature("SketchLine")
geomDataAPI_Point2D(aLine.attribute("StartPoint")).setValue(0, 0)
geomDataAPI_Point2D(aLine.attribute("EndPoint")).setValue(100, 100)
aSession.finishOperation()
aSession.startOperation()
aBaseObjectsList.clear()
aBaseObjectsList.append(aSketchResult, aLine.lastResult().shape())
aSession.finishOperation()
assert (len(aVertexFeature2.results()) == 0)

# remove failed feature
aSession.startOperation()
aPart.removeFeature(aVertexFeature2)
aPart.setCurrentFeature(aVertexFeature, True)
aSession.finishOperation()

from salome.shaper import model
assert(model.checkPythonDump())
