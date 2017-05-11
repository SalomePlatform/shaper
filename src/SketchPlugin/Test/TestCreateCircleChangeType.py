"""
    TestCreateCircleChangeType.py

    Test attributes reset when changing creation method of a circle on-the-fly
"""

#=========================================================================
# Initialization of the test
#=========================================================================
from GeomDataAPI import *
from GeomAPI import *
from ModelAPI import *
from SketchAPI import SketchAPI_Sketch
from salome.shaper import model
import math

__updated__ = "2017-03-22"


##=========================================================================
## Auxiliary functions
##=========================================================================

def assertNotInitializedByCenterAndPassed(theMacroCircle):
    # check points
    aPassedPoint = geomDataAPI_Point2D(theMacroCircle.attribute("passed_point"))
    assert (not aPassedPoint.isInitialized())
    # check references
    aCenterPointRef = theMacroCircle.refattr("center_point_ref")
    aPassedPointRef = theMacroCircle.refattr("passed_point_ref")
    assert (not aCenterPointRef.isInitialized())
    assert (not aPassedPointRef.isInitialized())

def assertNotInitializedByThreePoints(theMacroCircle):
    # check points
    aFirstPoint = geomDataAPI_Point2D(theMacroCircle.attribute("first_point"))
    aSecondPoint = geomDataAPI_Point2D(theMacroCircle.attribute("second_point"))
    aThirdPoint = geomDataAPI_Point2D(theMacroCircle.attribute("third_point"))
    assert (not aFirstPoint.isInitialized())
    assert (not aSecondPoint.isInitialized())
    assert (not aThirdPoint.isInitialized())
    # check references
    aFirstPointRef = theMacroCircle.refattr("first_point_ref")
    aSecondPointRef = theMacroCircle.refattr("second_point_ref")
    aThirdPointRef = theMacroCircle.refattr("third_point_ref")
    assert (not aFirstPointRef.isInitialized())
    assert (not aSecondPointRef.isInitialized())
    assert (not aThirdPointRef.isInitialized())

def verifyLastCircle(theSketch, theX, theY, theR):
    """
    subroutine to verify position of last circle in the sketch
    """
    aLastCircle = model.lastSubFeature(theSketch, "SketchCircle")
    model.assertCircle(aLastCircle, [theX, theY], theR)


#=========================================================================
# Start of test
#=========================================================================

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
#=========================================================================
# Creation of a sketch
#=========================================================================
aSession.startOperation()
aSketchCommonFeature = aDocument.addFeature("Sketch")
aSketchFeature = featureToCompositeFeature(aSketchCommonFeature)
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSession.finishOperation()
aSketch = SketchAPI_Sketch(aSketchFeature)

#=========================================================================
# Creation of auxiliary features
#=========================================================================
aSession.startOperation()
aSession.startOperation()
aLine = aSketchFeature.addFeature("SketchLine")
aLineStart = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
aLineStart.setValue(10., 0.)
aLineEnd = geomDataAPI_Point2D(aLine.attribute("EndPoint"))
aLineEnd.setValue(10., 50.)
aSession.finishOperation()

aSession.startOperation()
#=========================================================================
# Test 1. Create a circle as a macro-feature and check all attributes are not initialized
#=========================================================================
aCircle = aSketchFeature.addFeature("SketchMacroCircle")
assert (aCircle.getKind() == "SketchMacroCircle")
aCenterPoint = geomDataAPI_Point2D(aCircle.attribute("center_point"))
aPassedPoint = geomDataAPI_Point2D(aCircle.attribute("passed_point"))
aFirstPoint = geomDataAPI_Point2D(aCircle.attribute("first_point"))
aSecondPoint = geomDataAPI_Point2D(aCircle.attribute("second_point"))
aThirdPoint = geomDataAPI_Point2D(aCircle.attribute("third_point"))
aCenterPointRef = aCircle.refattr("center_point_ref")
aPassedPointRef = aCircle.refattr("passed_point_ref")
aFirstPointRef = aCircle.refattr("first_point_ref")
aSecondPointRef = aCircle.refattr("second_point_ref")
aThirdPointRef = aCircle.refattr("third_point_ref")
assertNotInitializedByCenterAndPassed(aCircle)
assertNotInitializedByThreePoints(aCircle)
aCircleType = aCircle.string("circle_type")
assert (not aCircleType.isInitialized())
#=========================================================================
# Test 2. Initialize center of circle, check the three points are not initialized
#=========================================================================
aCircleType.setValue("circle_type_by_center_and_passed_points")
aCenterPoint.setValue(-25., -25.)
assertNotInitializedByThreePoints(aCircle)
#=========================================================================
# Test 3. Change type of circle and check the attributes related to center and passed point became uninitilized
#=========================================================================
aCircleType.setValue("circle_type_by_three_points")
assertNotInitializedByCenterAndPassed(aCircle)
#=========================================================================
# Test 4. Initialize two points and change type, they should became uninitialized
#=========================================================================
aFirstPoint.setValue(-10., 10.)
aSecondPoint.setValue(10., 10.)
aCircleType.setValue("circle_type_by_center_and_passed_points")
assertNotInitializedByThreePoints(aCircle)
#=========================================================================
# Test 5. Initialize center and passed point then change the type
#=========================================================================
aCenterPoint.setValue(-25., -25.)
aPassedPoint.setValue(0., 0.)
aCircleType.setValue("circle_type_by_three_points")
assertNotInitializedByCenterAndPassed(aCircle)
#=========================================================================
# Test 6. Initialize all three points then change the type twice
#=========================================================================
aFirstPoint.setValue(-10., 10.)
aSecondPoint.setValue(10., 10.)
aThirdPoint.setValue(0., 0.)
aCircleType.setValue("circle_type_by_center_and_passed_points")
assertNotInitializedByThreePoints(aCircle)
aCircleType.setValue("circle_type_by_three_points")
assertNotInitializedByCenterAndPassed(aCircle)
#=========================================================================
# Test 7. Initialize first and third points then change the type
#=========================================================================
aFirstPointRef.setAttr(aLineStart)
aFirstPoint.setValue(aLineStart.pnt())
aThirdPointRef.setObject(aLine.lastResult())
aThirdPoint.setValue(aLineEnd.pnt())
aCircleType.setValue("circle_type_by_center_and_passed_points")
assertNotInitializedByThreePoints(aCircle)
#=========================================================================
# Test 8. Initialize center and passed points and finish operation
#=========================================================================
aCenterPointRef.setAttr(aLineStart)
aCenterPoint.setValue(aLineStart.pnt())
aPassedPointRef.setAttr(aLineEnd)
aPassedPoint.setValue(aLineEnd.pnt())
aSession.finishOperation()

aRadius = model.distancePointPoint(aLineStart, aLineEnd)
NB_FEATURES_EXPECTED = 4 # line, circle and two coincidences
assert (aSketchFeature.numberOfSubs() == NB_FEATURES_EXPECTED), "Number of features in sketch {}, expected {}".format(aSketchFeature.numberOfSubs(), NB_FEATURES_EXPECTED)
verifyLastCircle(aSketchFeature, aLineStart.x(), aLineStart.y(), aRadius)

#=========================================================================
# End of test
#=========================================================================

from salome.shaper import model
assert(model.checkPythonDump())
