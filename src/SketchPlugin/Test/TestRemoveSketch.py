"""
    TestRemoveSketch.py
"""
from GeomDataAPI import *
from ModelAPI import *

#=========================================================================
# Initialization of the test
#=========================================================================
__updated__ = "2017-02-01"

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

# create a sketch
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aDocument.addFeature("Sketch"))
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSession.finishOperation()

# create a line
aSession.startOperation()
aSketchLine = aSketchFeature.addFeature("SketchLine")
aLineStartPoint = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
aLineEndPoint = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
aLineStartPoint.setValue(50., 0.)
aLineEndPoint.setValue(100., 25.)
aSession.finishOperation()

# remove sketch
aSession.startOperation()
aDocument.removeFeature(aSketchFeature)
aSession.finishOperation()

#=========================================================================
# End of test
#=========================================================================
