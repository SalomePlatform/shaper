#=========================================================================
# Initialization of the test
#=========================================================================
from GeomDataAPI import *
from ModelAPI import *

__updated__ = "2014-07-24"

aPluginManager = ModelAPI_PluginManager.get()
aDocument = aPluginManager.rootDocument()
#===============================================================================
# Test ModelAPI static methods
# TODO: Move this test in the ModelAPI progect 
#===============================================================================
assert (ModelAPI_Feature.group() == "Features")
assert (ModelAPI_AttributeDocRef.type() == "DocRef")
assert (ModelAPI_AttributeDouble.type() == "Double")
assert (ModelAPI_AttributeReference.type() == "Reference")
assert (ModelAPI_AttributeRefAttr.type() == "RefAttr")
assert (ModelAPI_AttributeRefList.type() == "RefList")
#===============================================================================
# Test GeomDataAPI static methods
# TODO: Move this test in the GeomDataAPI progect 
#===============================================================================
assert (GeomDataAPI_Point.type() == "Point")
assert (GeomDataAPI_Dir.type() == "Dir")  
assert (GeomDataAPI_Point2D.type() == "Point2D")  
#=========================================================================
# Creation of a sketch
#=========================================================================
aDocument.startOperation()
aSketchFeature = aDocument.addFeature("Sketch")
assert (aSketchFeature.getKind() == "Sketch")
aSketchFeatureData = aSketchFeature.data()
origin = geomDataAPI_Point(aSketchFeatureData.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeatureData.attribute("DirX"))
dirx.setValue(1, 0, 0)
diry = geomDataAPI_Dir(aSketchFeatureData.attribute("DirY"))
diry.setValue(0, 1, 0)
norm = geomDataAPI_Dir(aSketchFeatureData.attribute("Norm"))
norm.setValue(0, 0, 1)
# check that values have been changed
origin = geomDataAPI_Point(aSketchFeatureData.attribute("Origin"))
assert (origin.x() == 0)
assert (origin.y() == 0)
assert (origin.z() == 0)
dirx = geomDataAPI_Dir(aSketchFeatureData.attribute("DirX"))
assert (dirx.x() == 1)
assert (dirx.y() == 0)
assert (dirx.z() == 0)
diry = geomDataAPI_Dir(aSketchFeatureData.attribute("DirY"))
assert (diry.x() == 0)
assert (diry.y() == 1)
assert (diry.z() == 0)
norm = geomDataAPI_Dir(aSketchFeatureData.attribute("Norm"))
assert (norm.x() == 0)
assert (norm.y() == 0)
assert (norm.z() == 1)
aDocument.finishOperation()
#=========================================================================
# Creation of a point
#=========================================================================
aDocument.startOperation()
aSketchReflist = aSketchFeatureData.reflist("Features")
assert (not aSketchReflist.isInitialized())
assert(aSketchReflist.size() == 0)
assert (len(aSketchReflist.list()) == 0)
aSketchPoint = aDocument.addFeature("SketchPoint")
assert (aSketchPoint.getKind() == "SketchPoint")
aSketchReflist.append(aSketchPoint)
aSketchPointData = aSketchPoint.data()
coords = geomDataAPI_Point2D(aSketchPointData.attribute("PointCoordindates"))
assert (coords.x() == 0)
assert (coords.y() == 0)
assert (not coords.isInitialized())
# Simulate SketchPlugin_Point::move(...)
coords.setValue(10., 10.)
assert (coords.isInitialized())
# check that values have been changed
aSketchReflist = aSketchFeatureData.reflist("Features")
assert (aSketchReflist.size() == 1)
assert (len(aSketchReflist.list()) == 1)
aSketchPointData = aSketchPoint.data()
coords = geomDataAPI_Point2D(aSketchPointData.attribute("PointCoordindates"))
assert (coords.x() == 10.0)
assert (coords.y() == 10.0)
aDocument.finishOperation()
#===============================================================================
# Creation of a line
#===============================================================================
aDocument.startOperation()
aSketchLine = aDocument.addFeature("SketchLine")
aSketchReflist.append(aSketchLine)
assert (aSketchReflist.size() == 2)
assert (len(aSketchReflist.list()) == 2)
aSketchLineData = aSketchLine.data()
aLineStartPoint = geomDataAPI_Point2D(aSketchLineData.attribute("StartPoint"))
aLineEndPoint= geomDataAPI_Point2D(aSketchLineData.attribute("EndPoint"))
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
# check that values have been changed
aSketchLineData = aSketchLine.data()
aLineStartPoint = geomDataAPI_Point2D(aSketchLineData.attribute("StartPoint"))
aLineEndPoint= geomDataAPI_Point2D(aSketchLineData.attribute("EndPoint"))
assert (aLineStartPoint.x() == 50.0)
assert (aLineStartPoint.y() == 50.0)
assert (aLineEndPoint.x() == 60.0)
assert (aLineEndPoint.y() == 60.0)
aDocument.finishOperation()
#===============================================================================
# Check results
#===============================================================================
aResult = aSketchLine.firstResult()
aResultConstruction = modelAPI_ResultConstruction(aResult)
aShape = aResultConstruction.shape()
assert (aShape is not None)
assert (not aShape.isNull())
 #==============================================================================
 # Finish the test
 #==============================================================================
