"""
    TestCreateCircleByCenterAndPassed.py

    Test creation methods of a circle built by a center and a passed point
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


#=========================================================================
# Auxiliary functions
#=========================================================================
TOLERANCE = 1.e-7

def getLastCircle(theSketch):
    """
    obtains last feature from the sketch and generates error if the feature is not a circle
    """
    expectedKind = "SketchCircle"
    for anIndex in range(theSketch.numberOfSubs() - 1, -1, -1):
        aSub = theSketch.subFeature(anIndex)
        if (aSub.getKind() == expectedKind):
            return aSub

def verifyLastCircle(theSketch, theX, theY, theR):
    """
    subroutine to verify position of last circle in the sketch
    """
    aLastCircle = getLastCircle(theSketch)
    aCenter = geomDataAPI_Point2D(aLastCircle.attribute("circle_center"))
    verifyPointCoordinates(aCenter, theX, theY)
    aRadius = aLastCircle.real("circle_radius")
    assert aRadius.value() == theR, "Wrong radius {0}, expected {1}".format(aRadius.value(), theR)

def verifyPointCoordinates(thePoint, theX, theY):
    assert thePoint.x() == theX and thePoint.y() == theY, "Wrong '{0}' point ({1}, {2}), expected ({3}, {4})".format(thePoint.attributeType(), thePoint.x(), thePoint.y(), theX, theY)

def verifyPointOnLine(thePoint, theLine):
    aDistance = distancePointLine(thePoint, theLine)
    assert aDistance < TOLERANCE, "Point is not on Line, distance: {0}".format(aDistance)

def verifyTangentCircles(theCircle1, theCircle2):
    aCenter1 = geomDataAPI_Point2D(theCircle1.attribute("circle_center"))
    aCenter2 = geomDataAPI_Point2D(theCircle2.attribute("circle_center"))
    aDistCC = distancePointPoint(aCenter1, aCenter2)
    aRadius1 = theCircle1.real("circle_radius").value()
    aRadius2 = theCircle2.real("circle_radius").value()
    aRSum = aRadius1 + aRadius2
    aRDiff = math.fabs(aRadius1 - aRadius2)
    assert math.fabs(aRSum - aDistCC) < TOLERANCE or math.fabs(aRDiff - aDistCC) < TOLERANCE, "Circles do not tangent"

def distancePointPoint(thePoint1, thePoint2):
    return thePoint1.pnt().distance(thePoint2.pnt())

def distancePointLine(thePoint, theLine):
    aLineStart = geomDataAPI_Point2D(theLine.attribute("StartPoint")).pnt().xy()
    aLineEnd = geomDataAPI_Point2D(theLine.attribute("EndPoint")).pnt().xy()
    aLineDir = aLineEnd.decreased(aLineStart)
    aLineLen = aLineEnd.distance(aLineStart)
    aPntDir = thePoint.pnt().xy().decreased(aLineStart)
    return math.fabs(aPntDir.cross(aLineDir) / aLineLen)


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
# Test 1. Create a circle by center and radius
#=========================================================================
aSession.startOperation()
aCircle = aSketchFeature.addFeature("SketchCircle")
assert (aCircle.getKind() == "SketchCircle")
aCircleCenter = geomDataAPI_Point2D(aCircle.attribute("circle_center"))
assert (not aCircleCenter.isInitialized())
aCircleRadius = aCircle.real("circle_radius")
assert (type(aCircleRadius) == ModelAPI_AttributeDouble)
# ModelAPI_AttributeDouble.typeId() is checked in ModelAPI_TestConstants
assert (aCircleRadius.attributeType() == ModelAPI_AttributeDouble.typeId())
aCircleCenter.setValue(-25., -25)
aCircleRadius.setValue(25.)
aSession.finishOperation()
verifyLastCircle(aSketchFeature, -25., -25., 25.)
#=========================================================================
# Edit the Circle
# 1. check that changing the center of a circle does not affect radius and vise versa
# 2. also check that int is acceptable as well as a real
#=========================================================================
aSession.startOperation()
aCircleCenter.setValue(10, 60)
aSession.finishOperation()
verifyLastCircle(aSketchFeature, 10., 60., 25.)
aSession.startOperation()
aCircleRadius.setValue(int(20))
aSession.finishOperation()
verifyLastCircle(aSketchFeature, 10., 60., 20.)

#=========================================================================
# Test 2. Create a circle as a macro-feature by center and passed point
#=========================================================================
aSession.startOperation()
aCircle = aSketchFeature.addFeature("SketchMacroCircle")
assert (aCircle.getKind() == "SketchMacroCircle")
aCircleCenter = geomDataAPI_Point2D(aCircle.attribute("center_point"))
assert (not aCircleCenter.isInitialized())
aCirclePassed = geomDataAPI_Point2D(aCircle.attribute("passed_point"))
assert (not aCirclePassed.isInitialized())
aCircleType = aCircle.string("circle_type")
assert (not aCircleType.isInitialized())
aCircleType.setValue("circle_type_by_center_and_passed_points")
aCircleCenter.setValue(-25., -25)
aCirclePassed.setValue(0., -25.)
aRadius = distancePointPoint(aCircleCenter, aCirclePassed)
aSession.finishOperation()
assert (aSketchFeature.numberOfSubs() == 2)
verifyLastCircle(aSketchFeature, -25., -25., aRadius)

#=========================================================================
# Test 3. Create a circle as a macro-feature by center and passed point coincident to other points
#=========================================================================
# get previous circle
aPrevCircle = getLastCircle(aSketchFeature)
aPrevCenter = geomDataAPI_Point2D(aPrevCircle.attribute("circle_center"))
# create additional point
aPointCoodinates = [0., 0.]
aSession.startOperation()
aPoint = aSketchFeature.addFeature("SketchPoint")
aPointCoord = geomDataAPI_Point2D(aPoint.attribute("PointCoordindates"))
aPointCoord.setValue(aPointCoodinates[0], aPointCoodinates[1])
aSession.finishOperation()
# create new circle
aSession.startOperation()
aCircle = aSketchFeature.addFeature("SketchMacroCircle")
aCenter = geomDataAPI_Point2D(aCircle.attribute("center_point"))
aCenterRef = aCircle.refattr("center_point_ref")
assert (not aCenterRef.isInitialized())
aPassed = geomDataAPI_Point2D(aCircle.attribute("passed_point"))
aPassedRef = aCircle.refattr("passed_point_ref")
assert (not aPassedRef.isInitialized())
aCircleType = aCircle.string("circle_type")
assert (not aCircleType.isInitialized())
# initialize attributes
aCircleType.setValue("circle_type_by_center_and_passed_points")
aCenterRef.setObject(aPoint.lastResult())
aCenter.setValue(aPointCoord.pnt())
aPassedRef.setAttr(aPrevCenter)
aPassed.setValue(aPrevCenter.pnt())
aRadius = distancePointPoint(aPrevCenter, aPointCoord)
aSession.finishOperation()
assert (aSketchFeature.numberOfSubs() == 6)
verifyPointCoordinates(aPointCoord, aPointCoodinates[0], aPointCoodinates[1])
verifyLastCircle(aSketchFeature, aPointCoord.x(), aPointCoord.y(), aRadius)
model.testNbSubFeatures(aSketch, "SketchConstraintCoincidence", 2)

#=========================================================================
# Test 4. Create a circle as a macro-feature by center on a line and passed point on another circle
#=========================================================================
# get previous circle
aPrevCircle = getLastCircle(aSketchFeature)
aPrevCenter = geomDataAPI_Point2D(aPrevCircle.attribute("circle_center"))
aPrevCenterXY = [aPrevCenter.x(), aPrevCenter.y()]
aPrevRadius = aPrevCircle.real("circle_radius").value()
# create additional line
aLineStart = [10., 0.]
aLineEnd = [10., 50.]
aSession.startOperation()
aLine = aSketchFeature.addFeature("SketchLine")
aStartPnt = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
aStartPnt.setValue(aLineStart[0], aLineStart[1])
aEndPnt = geomDataAPI_Point2D(aLine.attribute("EndPoint"))
aEndPnt.setValue(aLineEnd[0], aLineEnd[1])
aSession.finishOperation()
# create new circle
aSession.startOperation()
aCircle = aSketchFeature.addFeature("SketchMacroCircle")
aCenter = geomDataAPI_Point2D(aCircle.attribute("center_point"))
aCenterRef = aCircle.refattr("center_point_ref")
aPassed = geomDataAPI_Point2D(aCircle.attribute("passed_point"))
aPassedRef = aCircle.refattr("passed_point_ref")
aCircleType = aCircle.string("circle_type")
# initialize attributes
aCircleType.setValue("circle_type_by_center_and_passed_points")
aCenterRef.setObject(aLine.lastResult())
anExpectedCenter = [(aLineStart[0] + aLineEnd[0]) * 0.5, (aLineStart[1] + aLineEnd[1]) * 0.5]
aCenter.setValue(anExpectedCenter[0], anExpectedCenter[1])
aPassedRef.setObject(aPrevCircle.lastResult())
aPassed.setValue(aPrevCenter.x() + aPrevRadius, aPrevCenter.y())
aRadius = distancePointPoint(aCenter, aPassed)
aSession.finishOperation()
assert (aSketchFeature.numberOfSubs() == 10)
# check connected features do not change their positions
verifyPointCoordinates(aPrevCenter, aPrevCenterXY[0], aPrevCenterXY[1])
assert(aPrevCircle.real("circle_radius").value() == aPrevRadius)
verifyPointCoordinates(aStartPnt, aLineStart[0], aLineStart[1])
verifyPointCoordinates(aEndPnt, aLineEnd[0], aLineEnd[1])
# verify newly created circle
aCircle = getLastCircle(aSketchFeature)
aCenter = geomDataAPI_Point2D(aCircle.attribute("circle_center"))
verifyPointCoordinates(aCenter, anExpectedCenter[0], anExpectedCenter[1])
verifyPointOnLine(aCenter, aLine)
verifyTangentCircles(aCircle, aPrevCircle)
model.testNbSubFeatures(aSketch, "SketchConstraintCoincidence", 3)
model.testNbSubFeatures(aSketch, "SketchConstraintTangent", 1)

#=========================================================================
# Test 5. Create a circle as a macro-feature by center and passed point placed on the same line
#         Check the circle is not created
#=========================================================================
aSession.startOperation()
aCircle = aSketchFeature.addFeature("SketchMacroCircle")
aCenter = geomDataAPI_Point2D(aCircle.attribute("center_point"))
aCenterRef = aCircle.refattr("center_point_ref")
aPassed = geomDataAPI_Point2D(aCircle.attribute("passed_point"))
aPassedRef = aCircle.refattr("passed_point_ref")
aCircleType = aCircle.string("circle_type")
# initialize attributes
aCircleType.setValue("circle_type_by_center_and_passed_points")
aCenterRef.setObject(aLine.lastResult())
aCenter.setValue(anExpectedCenter[0], anExpectedCenter[1])
aPassedRef.setObject(aLine.lastResult())
aPassed.setValue(aLineStart[0], aLineStart[1])
aSession.finishOperation()
aLastFeature = aSketchFeature.subFeature(aSketchFeature.numberOfSubs() - 1)
assert aLastFeature == aCircle, "ERROR: SketchMacroCircle has NOT expected to be valid"
aDocument.removeFeature(aCircle)
assert (aSketchFeature.numberOfSubs() == 10)

#=========================================================================
# End of test
#=========================================================================

from salome.shaper import model
assert(model.checkPythonDump())
