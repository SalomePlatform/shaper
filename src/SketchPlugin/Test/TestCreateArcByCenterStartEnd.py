## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

"""
    TestCreateArc.py

    static const std::string MY_SKETCH_ARC_ID("SketchArc");
    static const std::string MY_CENTER_ID = "center_point";
    static const std::string MY_START_ID = "start_point";
    static const std::string MY_END_ID = "end_point";
    data()->addAttribute(SketchPlugin_Arc::CENTER_ID(), GeomDataAPI_Point2D::typeId());
    data()->addAttribute(SketchPlugin_Arc::START_ID(),  GeomDataAPI_Point2D::typeId());
    data()->addAttribute(SketchPlugin_Arc::END_ID(),    GeomDataAPI_Point2D::typeId());
"""

#=========================================================================
# Initialization of the test
#=========================================================================
from GeomDataAPI import *
from ModelAPI import *
from SketchAPI import SketchAPI_Sketch
import math
from salome.shaper import model

__updated__ = "2017-03-28"

TOLERANCE = 1.e-7

#=========================================================================
# Auxiliary functions
#=========================================================================

def verifyLastArc(theSketch, theCenter, theStart, theEnd):
    """
    subroutine to verify position of last arc in the sketch
    """
    aLastArc = model.lastSubFeature(theSketch, "SketchArc")
    model.assertArc(aLastArc, theCenter, theStart, theEnd)

def verifyPointOnLine(thePoint, theLine):
    aDistance = model.distancePointLine(thePoint, theLine)
    assert aDistance < TOLERANCE, "Point is not on Line, distance: {0}".format(aDistance)

def verifyPointOnCircle(thePoint, theCircular):
    if theCircular.getKind() == "SketchArc":
        aCenterPoint = geomDataAPI_Point2D(theCircular.attribute("center_point"))
        aStartPoint = geomDataAPI_Point2D(theCircular.attribute("start_point"))
        aRadius = model.distancePointPoint(aCenterPoint, aStartPoint)
    elif theCircular.getKind() == "SketchCircle":
        aCenterPoint = geomDataAPI_Point2D(theCircular.attribute("circle_center"))
        aRadius = theCircular.real("circle_radius").value()
    else:
        return
    assert math.fabs(model.distancePointPoint(aCenterPoint, thePoint) - aRadius) < TOLERANCE


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
# Test 1. Create an arc by center, start and end points
#=========================================================================
aCenter = [10., 10.]
aStart  = [0., 50.]
aEnd    = [50., 0.]
aSession.startOperation()
aSketchArc = aSketchFeature.addFeature("SketchArc")
assert (aSketchArc.getKind() == "SketchArc")
anArcCenter = geomDataAPI_Point2D(aSketchArc.attribute("center_point"))
assert (not anArcCenter.isInitialized())
anArcCenter.setValue(aCenter[0], aCenter[1])
anArcStartPoint = geomDataAPI_Point2D(aSketchArc.attribute("start_point"))
assert (not anArcStartPoint.isInitialized())
anArcStartPoint.setValue(aStart[0], aStart[1])
anArcEndPoint = geomDataAPI_Point2D(aSketchArc.attribute("end_point"))
assert (not anArcEndPoint.isInitialized())
anArcEndPoint.setValue(aEnd[0], aEnd[1])
aSession.finishOperation()
verifyLastArc(aSketchFeature, aCenter, aStart, aEnd)
# Move center
aCenter = [15., 20.]
aSession.startOperation()
anArcCenter.setValue(aCenter[0], aCenter[1])
aSession.finishOperation()
verifyLastArc(aSketchFeature, [], [], [])
# Move start point
deltaX, deltaY = 5., 2.
aStart = [anArcStartPoint.x() + deltaX, anArcStartPoint.y() + deltaY]
aSession.startOperation()
anArcStartPoint.setValue(aStart[0], aStart[1])
aSession.finishOperation()
verifyLastArc(aSketchFeature, [], [], [])
# Move end point
aEnd = [anArcEndPoint.x() - deltaX, anArcEndPoint.y() - deltaY]
aSession.startOperation()
anArcEndPoint.setValue(aEnd[0], aEnd[1])
aSession.finishOperation()
verifyLastArc(aSketchFeature, [], [], [])
# Check that changing the radius does not affect arc
aSession.startOperation()
anArcRadius = aSketchArc.real("radius")
aPrevRadius = anArcRadius.value();
anArcRadius.setValue(aPrevRadius + 10.)
aSession.finishOperation()
assert (math.fabs(anArcRadius.value() - aPrevRadius) < TOLERANCE)
verifyLastArc(aSketchFeature, [], [], [])
# Check that changing the angle does not affect arc
aSession.startOperation()
anArcAngle = aSketchArc.real("angle")
aPrevAngle = anArcAngle.value()
anArcAngle.setValue(aPrevAngle + 10.)
aSession.finishOperation()
assert (math.fabs(anArcAngle.value() - aPrevAngle) < TOLERANCE)
verifyLastArc(aSketchFeature, [], [], [])

#=========================================================================
# Test 2. Create an arc as a macro-feature by center, start and end points
#=========================================================================
aSession.startOperation()
anArc = aSketchFeature.addFeature("SketchMacroArc")
assert (anArc.getKind() == "SketchMacroArc")
anArcCenter = geomDataAPI_Point2D(anArc.attribute("center_point"))
assert (not anArcCenter.isInitialized())
anArcStart = geomDataAPI_Point2D(anArc.attribute("start_point_1"))
assert (not anArcStart.isInitialized())
anArcEnd = geomDataAPI_Point2D(anArc.attribute("end_point_1"))
assert (not anArcEnd.isInitialized())
anArcType = anArc.string("arc_type")
assert (not anArcType.isInitialized())
anArcType.setValue("by_center_and_points")
anArcCenter.setValue(aCenter[0], aCenter[1])
anArcStart.setValue(aStart[0], aStart[1])
anArcEnd.setValue(aEnd[0], aEnd[1])
aSession.finishOperation()
assert (aSketchFeature.numberOfSubs() == 2)
verifyLastArc(aSketchFeature, [], [], [])

#=========================================================================
# Test 3. Create an arc by center and two points coincident to other points
#=========================================================================
# get previous arc
aPrevArc = model.lastSubFeature(aSketchFeature, "SketchArc")
aPrevArcStart = geomDataAPI_Point2D(aPrevArc.attribute("start_point"))
aPrevArcEnd = geomDataAPI_Point2D(aPrevArc.attribute("end_point"))
# create additional point
aPointCoordinates = [0., 0.]
aSession.startOperation()
aPoint = aSketchFeature.addFeature("SketchPoint")
aPointCoord = geomDataAPI_Point2D(aPoint.attribute("PointCoordinates"))
aPointCoord.setValue(aPointCoordinates[0], aPointCoordinates[1])
aPoint.selection("External").selectSubShape("VERTEX", "Origin")
aSession.finishOperation()
# create additional line
aLineStart = [20., -5.]
aLineEnd = [20., 20]
aSession.startOperation()
aLine = aSketchFeature.addFeature("SketchLine")
aLineStartPoint = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
aLineEndPoint = geomDataAPI_Point2D(aLine.attribute("EndPoint"))
aLineStartPoint.setValue(aLineStart[0], aLineStart[1])
aLineEndPoint.setValue(aLineEnd[0], aLineEnd[1])
aSession.finishOperation()
# create new arc
aSession.startOperation()
anArc = aSketchFeature.addFeature("SketchMacroArc")
aCenter = geomDataAPI_Point2D(anArc.attribute("center_point"))
aCenterRef = anArc.refattr("center_point_ref")
assert (not aCenterRef.isInitialized())
aStart = geomDataAPI_Point2D(anArc.attribute("start_point_1"))
aStartRef = anArc.refattr("start_point_ref")
assert (not aStartRef.isInitialized())
aEnd = geomDataAPI_Point2D(anArc.attribute("end_point_1"))
aEndRef = anArc.refattr("end_point_ref")
assert (not aEndRef.isInitialized())
anArcType = anArc.string("arc_type")
assert (not anArcType.isInitialized())
# initialize attributes
anArcType.setValue("by_center_and_points")
aCenterRef.setAttr(aPrevArcStart)
aCenter.setValue(aPrevArcStart.pnt())
aStartRef.setObject(aPoint.lastResult())
aStart.setValue(aPointCoord.pnt())
aEndRef.setAttr(aPrevArcEnd)
aEnd.setValue(aPrevArcEnd.pnt())
aSession.finishOperation()
# check the MacroArc is not valid (selection of end point is not possible)
aLastFeature = aSketchFeature.subFeature(aSketchFeature.numberOfSubs() - 1)
assert aLastFeature.getKind() == "SketchMacroArc", "ERROR: SketchMacroArc has NOT expected to be valid"
# change end point reference to a line
aSession.startOperation()
aEndRef.setObject(aLine.lastResult())
aEnd.setValue(aLineStart[0], aLineStart[1])
aSession.finishOperation()
assert (aSketchFeature.numberOfSubs() == 8), "Number of subs {}".format(aSketchFeature.numberOfSubs())
model.assertPoint(aPointCoord, aPointCoordinates)
verifyLastArc(aSketchFeature, [aPrevArcStart.x(), aPrevArcStart.y()], aPointCoordinates, [])
model.testNbSubFeatures(aSketch, "SketchConstraintCoincidence", 3)

#=========================================================================
# Test 4. Create an arc by center and points coincident to other features
#=========================================================================
# get previous arc
aPrevArc = model.lastSubFeature(aSketchFeature, "SketchArc")
aPrevArcCenter = geomDataAPI_Point2D(aPrevArc.attribute("center_point"))
aPrevArcStart = geomDataAPI_Point2D(aPrevArc.attribute("start_point"))
aPrevArcEnd = geomDataAPI_Point2D(aPrevArc.attribute("end_point"))
aPrevArcCenterXY = [aPrevArcCenter.x(), aPrevArcCenter.y()]
aPrevArcStartXY = [aPrevArcStart.x(), aPrevArcStart.y()]
aPrevArcEndXY = [aPrevArcEnd.x(), aPrevArcEnd.y()]
# create new arc
aSession.startOperation()
anArc = aSketchFeature.addFeature("SketchMacroArc")
aCenter = geomDataAPI_Point2D(anArc.attribute("center_point"))
aCenterRef = anArc.refattr("center_point_ref")
aStart = geomDataAPI_Point2D(anArc.attribute("start_point_1"))
aStartRef = anArc.refattr("start_point_ref")
aEnd = geomDataAPI_Point2D(anArc.attribute("end_point_1"))
aEndRef = anArc.refattr("end_point_ref")
anArcType = anArc.string("arc_type")
# initialize attributes
anArcType.setValue("by_center_and_points")
aCenterRef.setObject(aLine.lastResult())
aCenter.setValue(aLineStartPoint.pnt())
aStartRef.setObject(aPrevArc.lastResult())
aStart.setValue(aPrevArcStart.pnt())
aEndRef.setObject(aLine.lastResult())
aEnd.setValue(aLineEndPoint.pnt())
aSession.finishOperation()
assert (aSketchFeature.numberOfSubs() == 12), "Number of subs {}".format(aSketchFeature.numberOfSubs())
# check connected features do not change their positions
model.assertArc(aPrevArc, aPrevArcCenterXY, aPrevArcStartXY, aPrevArcEndXY)
model.assertLine(aLine, aLineStart, aLineEnd)
# verify newly created arc
anArc = model.lastSubFeature(aSketchFeature, "SketchArc")
aCenter = geomDataAPI_Point2D(anArc.attribute("center_point"))
aStart = geomDataAPI_Point2D(anArc.attribute("start_point"))
aEnd = geomDataAPI_Point2D(anArc.attribute("end_point"))
verifyPointOnLine(aCenter, aLine)
verifyPointOnLine(aEnd, aLine)
verifyPointOnCircle(aStart, aPrevArc)
model.testNbSubFeatures(aSketch, "SketchConstraintCoincidence", 6)
model.testNbSubFeatures(aSketch, "SketchConstraintTangent", 0)

#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
