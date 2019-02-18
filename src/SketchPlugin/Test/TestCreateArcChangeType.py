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
    TestCreateArcChangeType.py

    Test attributes reset when changing creation method of an arc on-the-fly
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

__updated__ = "2017-04-06"


##=========================================================================
## Auxiliary functions
##=========================================================================

def assertNotInitializedByCenterAndPassed(theMacroArc):
    # check points
    aCenterPoint = geomDataAPI_Point2D(theMacroArc.attribute("center_point"))
    aStartPoint = geomDataAPI_Point2D(theMacroArc.attribute("start_point_1"))
    aEndPoint = geomDataAPI_Point2D(theMacroArc.attribute("end_point_1"))
    assert (not aCenterPoint.isInitialized())
    assert (not aStartPoint.isInitialized())
    assert (not aEndPoint.isInitialized())
    # check references
    aCenterPointRef = theMacroArc.refattr("center_point_ref")
    aStartPointRef = theMacroArc.refattr("start_point_ref")
    aEndPointRef = theMacroArc.refattr("end_point_ref")
    assert (not aCenterPointRef.isInitialized())
    assert (not aStartPointRef.isInitialized())
    assert (not aEndPointRef.isInitialized())

def assertNotInitializedByThreePoints(theMacroArc):
    # check points
    aStartPoint = geomDataAPI_Point2D(theMacroArc.attribute("start_point_2"))
    aEndPoint = geomDataAPI_Point2D(theMacroArc.attribute("end_point_2"))
    aPassedPoint = geomDataAPI_Point2D(theMacroArc.attribute("passed_point"))
    assert (not aStartPoint.isInitialized())
    assert (not aEndPoint.isInitialized())
    assert (not aPassedPoint.isInitialized())
    # check references
    aStartPointRef = theMacroArc.refattr("start_point_ref")
    aEndPointRef = theMacroArc.refattr("end_point_ref")
    aPassedPointRef = theMacroArc.refattr("passed_point_ref")
    assert (not aStartPointRef.isInitialized())
    assert (not aEndPointRef.isInitialized())
    assert (not aPassedPointRef.isInitialized())

def assertNotInitializedByTangentEdge(theMacroArc):
    # check end point
    aEndPoint = geomDataAPI_Point2D(theMacroArc.attribute("end_point_3"))
    assert (not aEndPoint.isInitialized())
    # check references
    aTangentPointRef = theMacroArc.refattr("passed_point_ref")
    aEndPointRef = theMacroArc.refattr("end_point_ref")
    assert (not aTangentPointRef.isInitialized())
    assert (not aEndPointRef.isInitialized())

def verifyLastArc(theSketch, theCenter, theStart, theEnd):
    """
    subroutine to verify position of last arc in the sketch
    """
    aLastArc = model.lastSubFeature(theSketch, "SketchArc")
    model.assertArc(aLastArc, theCenter, theStart, theEnd)


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
# Test 1. Create an arc as a macro-feature and check all attributes are not initialized
#=========================================================================
anArc = aSketchFeature.addFeature("SketchMacroArc")
assert (anArc.getKind() == "SketchMacroArc")
aCenterPoint = geomDataAPI_Point2D(anArc.attribute("center_point"))
aPassedPoint = geomDataAPI_Point2D(anArc.attribute("passed_point"))
aStartPoint1 = geomDataAPI_Point2D(anArc.attribute("start_point_1"))
aStartPoint2 = geomDataAPI_Point2D(anArc.attribute("start_point_2"))
aEndPoint1 = geomDataAPI_Point2D(anArc.attribute("end_point_1"))
aEndPoint2 = geomDataAPI_Point2D(anArc.attribute("end_point_2"))
aEndPoint3 = geomDataAPI_Point2D(anArc.attribute("end_point_3"))
aTangentPoint = anArc.refattr("tangent_point")
aCenterPointRef = anArc.refattr("center_point_ref")
aStartPointRef = anArc.refattr("start_point_ref")
aEndPointRef = anArc.refattr("end_point_ref")
aPassedPointRef = anArc.refattr("passed_point_ref")
assertNotInitializedByCenterAndPassed(anArc)
assertNotInitializedByThreePoints(anArc)
assertNotInitializedByTangentEdge(anArc)
anArcType = anArc.string("arc_type")
assert (not anArcType.isInitialized())
#=========================================================================
# Test 2. Initialize center of arc, check the three points are not initialized
#=========================================================================
anArcType.setValue("by_center_and_points")
aCenterPoint.setValue(-25., -25.)
assertNotInitializedByThreePoints(anArc)
assertNotInitializedByTangentEdge(anArc)
#=========================================================================
# Test 3. Change type of circle and check the attributes related to center and passed point became uninitilized
#=========================================================================
anArcType.setValue("by_three_points")
assertNotInitializedByCenterAndPassed(anArc)
assertNotInitializedByTangentEdge(anArc)
#=========================================================================
# Test 4. Initialize two points and change type, they should became uninitialized
#=========================================================================
aStartPoint2.setValue(-10., 10.)
aEndPoint2.setValue(10., 10.)
anArcType.setValue("by_center_and_points")
assertNotInitializedByThreePoints(anArc)
assertNotInitializedByTangentEdge(anArc)
#=========================================================================
# Test 5. Initialize center and passed points then change the type
#=========================================================================
aCenterPoint.setValue(-25., -25.)
aStartPoint1.setValue(0., 0.)
aEndPoint1.setValue(-50., 0.)
anArcType.setValue("by_three_points")
assertNotInitializedByCenterAndPassed(anArc)
assertNotInitializedByTangentEdge(anArc)
#=========================================================================
# Test 6. Initialize all three points then change the type twice
#=========================================================================
aStartPoint2.setValue(-10., 10.)
aEndPoint2.setValue(10., 10.)
aPassedPoint.setValue(0., 0.)
anArcType.setValue("by_tangent_edge")
assertNotInitializedByThreePoints(anArc)
anArcType.setValue("by_center_and_points")
assertNotInitializedByTangentEdge(anArc)
anArcType.setValue("by_three_points")
assertNotInitializedByCenterAndPassed(anArc)
#=========================================================================
# Test 7. Initialize first and third points then change the type
#=========================================================================
aStartPointRef.setAttr(aLineStart)
aStartPoint2.setValue(aLineStart.pnt())
aPassedPointRef.setObject(aLine.lastResult())
aPassedPoint.setValue(aLineEnd.pnt())
anArcType.setValue("by_tangent_edge")
assertNotInitializedByCenterAndPassed(anArc)
assertNotInitializedByThreePoints(anArc)
#=========================================================================
# Test 8. Initialize tangent point then change type
#=========================================================================
aTangentPoint.setAttr(aLineEnd)
anArcType.setValue("by_three_points")
assertNotInitializedByCenterAndPassed(anArc)
assertNotInitializedByTangentEdge(anArc)
#=========================================================================
# Test 9. Initialize tangent point and end point then change type
#=========================================================================
anArcType.setValue("by_tangent_edge")
aTangentPoint.setAttr(aLineEnd)
aEndPoint3.setValue(50., 50.)
anArcType.setValue("by_center_and_points")
assertNotInitializedByThreePoints(anArc)
assertNotInitializedByTangentEdge(anArc)
#=========================================================================
# Test 10. Initialize center and passed points and finish operation
#=========================================================================
aCenterPointRef.setObject(aLine.lastResult())
aCenterPoint.setValue((aLineStart.x() + aLineEnd.x()) * 0.5, (aLineStart.y() + aLineEnd.y()) * 0.5)
aStartPointRef.setAttr(aLineStart)
aStartPoint1.setValue(aLineStart.pnt())
aEndPointRef.setObject(aLine.lastResult())
aEndPoint1.setValue(aLineEnd.pnt())
aSession.finishOperation()

NB_FEATURES_EXPECTED = 5 # line, arc and three coincidences
assert (aSketchFeature.numberOfSubs() == NB_FEATURES_EXPECTED), "Number of features in sketch {}, expected {}".format(aSketchFeature.numberOfSubs(), NB_FEATURES_EXPECTED)
verifyLastArc(aSketchFeature, [], [aLineStart.x(), aLineStart.y()], [])

#=========================================================================
# End of test
#=========================================================================

from salome.shaper import model
assert(model.checkPythonDump())
