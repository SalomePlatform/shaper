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

from salome.shaper import model
assert(model.checkPythonDump())
