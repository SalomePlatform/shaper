#=========================================================================
# Initialization of the test
#=========================================================================
from GeomDataAPI import *
from ModelAPI import *

__updated__ = "2014-10-28"

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
#=========================================================================
# Creation of a sketch
#=========================================================================
aSession.startOperation()
aSketchCommonFeature = aDocument.addFeature("Sketch")
aSketchFeature = featureToCompositeFeature(aSketchCommonFeature)
assert (aSketchFeature.getKind() == "Sketch")
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSession.finishOperation()
# check that values have been changed
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
assert (origin.x() == 0)
assert (origin.y() == 0)
assert (origin.z() == 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
assert (dirx.x() == 1)
assert (dirx.y() == 0)
assert (dirx.z() == 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
assert (norm.x() == 0)
assert (norm.y() == 0)
assert (norm.z() == 1)
#=========================================================================
# Creation of a point
#=========================================================================
aSession.startOperation()
aSketchReflist = aSketchFeature.reflist("Features")
assert (not aSketchReflist.isInitialized())
assert(aSketchReflist.size() == 0)
assert (len(aSketchReflist.list()) == 0)
# aSketchPoint = aDocument.addFeature("SketchPoint")
aSketchPoint = aSketchFeature.addFeature("SketchPoint")
assert (aSketchPoint.getKind() == "SketchPoint")
coords = geomDataAPI_Point2D(aSketchPoint.attribute("PointCoordinates"))
assert (coords.x() == 0)
assert (coords.y() == 0)
assert (not coords.isInitialized())
# Simulate SketchPlugin_Point::move(...)
coords.setValue(10., 10.)
assert (coords.isInitialized())
aSession.finishOperation()
# check that values have been changed
aSketchReflist = aSketchFeature.reflist("Features")
assert (aSketchReflist.size() == 1)
assert (len(aSketchReflist.list()) == 1)
coords = geomDataAPI_Point2D(aSketchPoint.attribute("PointCoordinates"))
assert (coords.x() == 10.0)
assert (coords.y() == 10.0)
#=========================================================================
# Creation of a line
#=========================================================================
aSession.startOperation()
# aSketchLine = aDocument.addFeature("SketchLine")
aSketchLine = aSketchFeature.addFeature("SketchLine")
assert (aSketchLine.getKind() == "SketchLine")
assert (aSketchReflist.size() == 2)
assert (len(aSketchReflist.list()) == 2)
aLineStartPoint = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
aLineEndPoint = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
assert (aLineStartPoint.x() == 0)
assert (aLineStartPoint.y() == 0)
assert (not aLineStartPoint.isInitialized())
assert (aLineEndPoint.x() == 0)
assert (aLineEndPoint.y() == 0)
assert (not aLineEndPoint.isInitialized())
# Simulate SketchPlugin_Line::move(...)
aLineStartPoint.setValue(50., 50.)
aLineEndPoint.setValue(60., 60.)
assert (aLineStartPoint.isInitialized())
assert (aLineEndPoint.isInitialized())
aSession.finishOperation()
# check that values have been changed
aLineStartPoint = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
aLineEndPoint = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
assert (aLineStartPoint.x() == 50.0)
assert (aLineStartPoint.y() == 50.0)
assert (aLineEndPoint.x() == 60.0)
assert (aLineEndPoint.y() == 60.0)
#=========================================================================
# Check the results
#=========================================================================
aResult = aSketchLine.firstResult()
assert (aResult is not None)
aResultConstruction = modelAPI_ResultConstruction(aResult)
aShape = aResultConstruction.shape()
assert (aShape is not None)
assert (not aShape.isNull())
