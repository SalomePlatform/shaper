# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

"""
    TestCreateArcByThreePoints.py

    Test creation methods of arc built by three points
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

def verifyLastArc(theSketch, theX, theY, theR):
    """
    subroutine to verify position of last arc in the sketch
    """
    aLastArc = model.lastSubFeature(theSketch, "SketchArc")
    aCenter = geomDataAPI_Point2D(aLastArc.attribute("center_point"))
    model.assertPoint(aCenter, [theX, theY])
    aRadius = aLastArc.real("radius")
    assert aRadius.value() == theR, "Wrong radius {0}, expected {1}".format(aRadius.value(), theR)

def verifyPointOnArc(thePoint, theArc):
    aCenter = geomDataAPI_Point2D(theArc.attribute("center_point"))
    aDistCP = model.distancePointPoint(aCenter, thePoint)
    aRadius = theArc.real("radius").value()
    assert math.fabs(aDistCP - aRadius) < TOLERANCE, "Point is not on arc, distance: {0}, radius of arc: {1}".format(aDistCP, aRadius)

def verifyTangentCircleArc(theCircle, theArc):
    aCircleCenter = geomDataAPI_Point2D(theCircle.attribute("circle_center"))
    anArcCenter = geomDataAPI_Point2D(theArc.attribute("center_point"))
    anArcStart = geomDataAPI_Point2D(theArc.attribute("start_point"))
    aDistCC = model.distancePointPoint(aCircleCenter, anArcCenter)
    aCircleRadius = theCircle.real("circle_radius").value()
    anArcRadius = model.distancePointPoint(anArcCenter, anArcStart)
    verifyTangentCircular(aDistCC, aCircleRadius, anArcRadius)

def verifyTangentCircular(theDistBetweenCenters, theRadius1, theRadius2):
    aRSum = theRadius1 + theRadius2
    aRDiff = math.fabs(theRadius1 - theRadius2)
    assert math.fabs(aRSum - theDistBetweenCenters) < TOLERANCE or math.fabs(aRDiff - theDistBetweenCenters) < TOLERANCE, "Two circulars are not tangent"

def verifyTangentArcLine(theArc, theLine):
    aCenter = geomDataAPI_Point2D(theArc.attribute("center_point"))
    aRadius = theArc.real("radius").value()
    aDistCL = model.distancePointLine(aCenter, theLine)
    assert math.fabs(aDistCL - aRadius) < TOLERANCE, "Circle and line are not tangent"


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
# Test 1. Create an arc by three points
#=========================================================================
expectedCenter = [0., 0.]
expectedRadius = 10.
aSession.startOperation()
anArc = aSketchFeature.addFeature("SketchMacroArc")
assert (anArc.getKind() == "SketchMacroArc")
anArcPnt1 = geomDataAPI_Point2D(anArc.attribute("start_point_2"))
anArcPnt2 = geomDataAPI_Point2D(anArc.attribute("end_point_2"))
anArcPnt3 = geomDataAPI_Point2D(anArc.attribute("passed_point"))
assert (not anArcPnt1.isInitialized())
assert (not anArcPnt2.isInitialized())
assert (not anArcPnt3.isInitialized())
anArcType = anArc.string("arc_type")
assert (not anArcType.isInitialized())
anArcType.setValue("by_three_points")
anArcPnt1.setValue(expectedCenter[0] - expectedRadius, expectedCenter[1])
anArcPnt2.setValue(expectedCenter[0] + expectedRadius, expectedCenter[1])
anArcPnt3.setValue(expectedCenter[0], expectedCenter[1] + expectedRadius)
aSession.finishOperation()
assert (aSketchFeature.numberOfSubs() == 1)
verifyLastArc(aSketchFeature, expectedCenter[0], expectedCenter[1], expectedRadius)

#=========================================================================
# Test 2. Create an arc by three points coincident to other points
#=========================================================================
# get previous arc
aPrevArc = model.lastSubFeature(aSketchFeature, "SketchArc")
aPrevCenter = geomDataAPI_Point2D(aPrevArc.attribute("center_point"))
aPrevCenterXY = [aPrevCenter.x(), aPrevCenter.y()]
aPrevArcRadius = aPrevArc.real("radius").value()
# create additional point and line
aPointCoodinates = [5., 20.]
aLineStart = [10., 0.]
aLineEnd = [10., 50.]
aSession.startOperation()
aPoint = aSketchFeature.addFeature("SketchPoint")
aPointCoord = geomDataAPI_Point2D(aPoint.attribute("PointCoordinates"))
aPointCoord.setValue(aPointCoodinates[0], aPointCoodinates[1])
aLine = aSketchFeature.addFeature("SketchLine")
aStartPnt = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
aStartPnt.setValue(aLineStart[0], aLineStart[1])
aEndPnt = geomDataAPI_Point2D(aLine.attribute("EndPoint"))
aEndPnt.setValue(aLineEnd[0], aLineEnd[1])
aSession.finishOperation()
# create new arc
aSession.startOperation()
anArc = aSketchFeature.addFeature("SketchMacroArc")
anArcPnt1 = geomDataAPI_Point2D(anArc.attribute("start_point_2"))
anArcPnt2 = geomDataAPI_Point2D(anArc.attribute("end_point_2"))
anArcPnt3 = geomDataAPI_Point2D(anArc.attribute("passed_point"))
anArcPnt1Ref = anArc.refattr("start_point_ref")
anArcPnt2Ref = anArc.refattr("end_point_ref")
anArcPnt3Ref = anArc.refattr("passed_point_ref")
anArcType = anArc.string("arc_type")
# initialize attributes
anArcType.setValue("by_three_points")
anArcPnt1Ref.setAttr(aPrevCenter)
anArcPnt1.setValue(aPrevCenter.pnt())
anArcPnt2Ref.setObject(aPoint.lastResult())
anArcPnt2.setValue(aPointCoord.pnt())
anArcPnt3Ref.setAttr(aStartPnt)
anArcPnt3.setValue(aLineStart[0], aLineStart[1])
aSession.finishOperation()
assert (aSketchFeature.numberOfSubs() == 7)
# check the points do not change their positions
model.assertPoint(aPrevCenter, aPrevCenterXY)
model.assertPoint(aPointCoord, aPointCoodinates)
model.assertPoint(aStartPnt, aLineStart)
# check newly created arc passes through the points
anArc = model.lastSubFeature(aSketchFeature, "SketchArc")
verifyPointOnArc(aPrevCenter, anArc)
verifyPointOnArc(aPointCoord, anArc)
verifyPointOnArc(aStartPnt, anArc)
model.testNbSubFeatures(aSketch, "SketchConstraintCoincidence", 3)
model.testNbSubFeatures(aSketch, "SketchConstraintTangent", 0)

#=========================================================================
# Test 3. Create an arc by three points an tangent with line
#=========================================================================
# create new arc
aSession.startOperation()
anArc = aSketchFeature.addFeature("SketchMacroArc")
anArcPnt1 = geomDataAPI_Point2D(anArc.attribute("start_point_2"))
anArcPnt2 = geomDataAPI_Point2D(anArc.attribute("end_point_2"))
anArcPnt3 = geomDataAPI_Point2D(anArc.attribute("passed_point"))
anArcPnt3Ref = anArc.refattr("passed_point_ref")
anArcType = anArc.string("arc_type")
# initialize attributes
anArcType.setValue("by_three_points")
anArcPnt1.setValue(20, 10)
anArcPnt2.setValue(20, 40)
anArcPnt3Ref.setObject(aLine.lastResult())
anArcPnt3.setValue(20, 25)
aSession.finishOperation()
assert (aSketchFeature.numberOfSubs() == 9)
# check the points do not change their positions
model.assertPoint(aPrevCenter, aPrevCenterXY)
model.assertPoint(aPointCoord, aPointCoodinates)
model.assertPoint(aStartPnt, aLineStart)
# check sub-features
model.testNbSubFeatures(aSketch, "SketchConstraintCoincidence", 3)
model.testNbSubFeatures(aSketch, "SketchConstraintTangent", 1)

#=========================================================================
# Test 4. Create a arc by three points:
#         a. first two points are coincident to extremities of the line
#         b. check that this line is not selectable by third point
#=========================================================================
aSession.startOperation()
anArc = aSketchFeature.addFeature("SketchMacroArc")
anArcPnt1 = geomDataAPI_Point2D(anArc.attribute("start_point_2"))
anArcPnt2 = geomDataAPI_Point2D(anArc.attribute("end_point_2"))
anArcPnt3 = geomDataAPI_Point2D(anArc.attribute("passed_point"))
anArcPnt1Ref = anArc.refattr("start_point_ref")
anArcPnt2Ref = anArc.refattr("end_point_ref")
anArcPnt3Ref = anArc.refattr("passed_point_ref")
anArcType = anArc.string("arc_type")
# initialize attributes
anArcType.setValue("by_three_points")
anArcPnt1Ref.setAttr(aStartPnt)
anArcPnt1.setValue(aStartPnt.pnt())
anArcPnt2Ref.setAttr(aEndPnt)
anArcPnt2.setValue(aEndPnt.pnt())
anArcPnt3Ref.setObject(aLine.lastResult())
anArcPnt3.setValue(aLineEnd[0], aLineEnd[1])
aSession.finishOperation()
aLastFeature = aSketchFeature.subFeature(aSketchFeature.numberOfSubs() - 1)
assert aLastFeature.getKind() == "SketchMacroArc", "ERROR: SketchMacroArc has NOT expected to be valid"
aDocument.removeFeature(anArc)
assert (aSketchFeature.numberOfSubs() == 9)

#=========================================================================
# Test 5. Create a arc by three points:
#         a. first two points are placed on both sides from line
#         b. check that this line is not selectable by third point
#=========================================================================
aDistanceFromLine = 20.
aSession.startOperation()
anArc = aSketchFeature.addFeature("SketchMacroArc")
anArcPnt1 = geomDataAPI_Point2D(anArc.attribute("start_point_2"))
anArcPnt2 = geomDataAPI_Point2D(anArc.attribute("end_point_2"))
anArcPnt3 = geomDataAPI_Point2D(anArc.attribute("passed_point"))
anArcPnt1Ref = anArc.refattr("start_point_ref")
anArcPnt2Ref = anArc.refattr("end_point_ref")
anArcPnt3Ref = anArc.refattr("passed_point_ref")
anArcType = anArc.string("arc_type")
# initialize attributes
anArcType.setValue("by_three_points")
anArcPnt1.setValue(aLineStart[0] + aDistanceFromLine, aLineStart[1])
anArcPnt2.setValue(aLineStart[0] - aDistanceFromLine, aLineStart[1])
anArcPnt3Ref.setObject(aLine.lastResult())
anArcPnt3.setValue(aLineEnd[0], aLineEnd[1])
aSession.finishOperation()
aLastFeature = aSketchFeature.subFeature(aSketchFeature.numberOfSubs() - 1)
assert aLastFeature.getKind() == "SketchMacroArc", "ERROR: SketchMacroArc has NOT expected to be valid"
aDocument.removeFeature(anArc)
assert (aSketchFeature.numberOfSubs() == 9)

#=========================================================================
# Test 6. Create an arc by three points:
#         a. check that one point IS NOT selectable as first and second points simultaneously
#         b. check that one segment IS selectable by first and second points
#=========================================================================
aSession.startOperation()
anArc = aSketchFeature.addFeature("SketchMacroArc")
anArcPnt1 = geomDataAPI_Point2D(anArc.attribute("start_point_2"))
anArcPnt2 = geomDataAPI_Point2D(anArc.attribute("end_point_2"))
anArcPnt3 = geomDataAPI_Point2D(anArc.attribute("passed_point"))
anArcPnt1Ref = anArc.refattr("start_point_ref")
anArcPnt2Ref = anArc.refattr("end_point_ref")
anArcPnt3Ref = anArc.refattr("passed_point_ref")
anArcType = anArc.string("arc_type")
# initialize attributes
anArcType.setValue("by_three_points")
anArcPnt1Ref.setAttr(aStartPnt)
anArcPnt1.setValue(aStartPnt.pnt())
anArcPnt2Ref.setAttr(aStartPnt)
anArcPnt2.setValue(aStartPnt.pnt())
anArcPnt3.setValue(0., 0.)
aSession.finishOperation()
# check the macro arc is not valid
aLastFeature = aSketchFeature.subFeature(aSketchFeature.numberOfSubs() - 1)
assert aLastFeature.getKind() == "SketchMacroArc", "ERROR: SketchMacroArc has NOT expected to be valid"
# reselect first points
aSession.startOperation()
anArcPnt1Ref.setObject(aLine.lastResult())
anArcPnt1.setValue(aStartPnt.pnt())
anArcPnt2Ref.setObject(aLine.lastResult())
anArcPnt2.setValue(aEndPnt.pnt())
aSession.finishOperation()
# check the macro arc is valid
aLastFeature = aSketchFeature.subFeature(aSketchFeature.numberOfSubs() - 1)
assert aLastFeature.getKind() != "SketchMacroArc", "ERROR: SketchMacroArc is expected to be valid"
assert (aSketchFeature.numberOfSubs() == 12)
# check sub-features
model.testNbSubFeatures(aSketch, "SketchConstraintCoincidence", 5)
model.testNbSubFeatures(aSketch, "SketchConstraintTangent", 1)

model.do()
model.end()

#=========================================================================
# End of test
#=========================================================================

from salome.shaper import model
assert(model.checkPythonDump())
