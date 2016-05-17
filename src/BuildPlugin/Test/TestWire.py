# Initialization of the test
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

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
aSketchLineFeature = aSketchFeature.addFeature("SketchLine")
aSketchLineStartPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("StartPoint"))
aSketchLineEndPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("EndPoint"))
aSketchLineStartPoint.setValue(0, 0)
aSketchLineEndPoint.setValue(0, 50)
aSketchLineFeature = aSketchFeature.addFeature("SketchLine")
aSketchLineStartPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("StartPoint"))
aSketchLineEndPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("EndPoint"))
aSketchLineStartPoint.setValue(0, 50)
aSketchLineEndPoint.setValue(50, 50)
aSketchLineFeature = aSketchFeature.addFeature("SketchLine")
aSketchLineStartPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("StartPoint"))
aSketchLineEndPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("EndPoint"))
aSketchLineStartPoint.setValue(50, 50)
aSketchLineEndPoint.setValue(50, 0)
aSketchLineFeature = aSketchFeature.addFeature("SketchLine")
aSketchLineStartPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("StartPoint"))
aSketchLineEndPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("EndPoint"))
aSketchLineStartPoint.setValue(50, 0)
aSketchLineEndPoint.setValue(0, 0)
aSession.finishOperation()
aSketchResult = aSketchFeature.firstResult()
aSketchShape = aSketchResult.shape()

# Create wire
aSession.startOperation()
aWireFeature = aPart.addFeature("Wire")
aBaseObjectsList = aWireFeature.selectionList("base_objects")
aShapeExplorer = GeomAPI_ShapeExplorer(aSketchShape, GeomAPI_Shape.EDGE)
if aShapeExplorer.more():
    aBaseObjectsList.append(aSketchResult, aShapeExplorer.current())
aWireFeature.customAction("add_contour")
aSession.finishOperation()

# Test results
assert (len(aWireFeature.results()) > 0)
