# Initialization of the test
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

import random

def createLine(theSketchFeature):
    aSketchLineFeature = theSketchFeature.addFeature("SketchLine")
    aSketchLineStartPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("StartPoint"))
    aSketchLineEndPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("EndPoint"))
    aSketchLineStartPoint.setValue(random.uniform(0, 100), random.uniform(0, 100))
    aSketchLineEndPoint.setValue(random.uniform(0, 100), random.uniform(0, 100))

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

# Create lines
aNumOfLines = 10
for i in range(aNumOfLines):
    createLine(aSketchFeature)
aSession.finishOperation()
aSketchResult = aSketchFeature.firstResult()
aSketchShape = aSketchResult.shape()

# Create edges
aSession.startOperation()
anEdgeFeature = aPart.addFeature("Edge")
aBaseObjectsList = anEdgeFeature.selectionList("base_objects")
aShapeExplorer = GeomAPI_ShapeExplorer(aSketchShape, GeomAPI_Shape.EDGE)
while aShapeExplorer.more():
    aBaseObjectsList.append(aSketchResult, aShapeExplorer.current())
    aShapeExplorer.next()
aSession.finishOperation()

# Test results
assert (len(anEdgeFeature.results()) == aNumOfLines)

from salome.shaper import model
assert(model.checkPythonDump())
