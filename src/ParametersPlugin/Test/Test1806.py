"""
      Test1806.py
      Unit test for testing the expression evaluation even no parameters are created

"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *

__updated__ = "2016-11-23"

aSession = ModelAPI_Session.get()
aPartSet = aSession.moduleDocument()

#=========================================================================
# Create a part
#=========================================================================
aSession.startOperation()
aPartFeature = aPartSet.addFeature("Part")
aSession.finishOperation()

aPart = aSession.activeDocument()
#=========================================================================
# Create a sketch with a line and length
#=========================================================================
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
aXOYPlane = objectToResult(aPartSet.objectByName("Construction", "XOY"))
aSketchFeature.selection("External").setValue(aXOYPlane, None)
# Create lines
aLine = aSketchFeature.addFeature("SketchLine")
aStart = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
aStart.setValue(0, 0)
anEnd = geomDataAPI_Point2D(aLine.attribute("EndPoint"))
anEnd.setValue(200, 0)

aDistance = aSketchFeature.addFeature("SketchConstraintDistance")
aDistance.refattr("ConstraintEntityA").setAttr(aStart)
aDistance.refattr("ConstraintEntityB").setAttr(anEnd)
aDistance.real("ConstraintValue").setText("25/2")

aSession.finishOperation()

assert(aDistance.real("ConstraintValue").value() == 12.5)
#=========================================================================
# End of test
#=========================================================================
import model
assert(model.checkPythonDump())
