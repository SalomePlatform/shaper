# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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


#=========================================================================
# Initialization of the test
#=========================================================================
from GeomDataAPI import *
from ModelAPI import *
from SketchAPI import SketchAPI_Sketch
import math
from salome.shaper import model

__updated__ = "2019-08-16"

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

def verifyArcLineTransversal(theArc, theLine):
    aCenter = geomDataAPI_Point2D(theArc.attribute("center_point"))
    aDistCL = model.distancePointLine(aCenter, theLine)
    assert aDistCL < TOLERANCE, "Arc and line are not orthogonal"

def verifyPointOnArc(thePoint, theArc):
    aCenter = geomDataAPI_Point2D(theArc.attribute("center_point"))
    aStart = geomDataAPI_Point2D(theArc.attribute("start_point"))
    aRadius = model.distancePointPoint(aStart, aCenter)

    aDistPP = model.distancePointPoint(aCenter, thePoint)
    assert math.fabs(aRadius - aDistPP) < TOLERANCE, "Point is not on Circle, distance: {0}".format(aDistPP)



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

# auxiliary line
aLineStartPnt = [0., 0.]
aLineEndPnt = [50., 0.]
aSession.startOperation()
aSketchLine = aSketchFeature.addFeature("SketchLine")
aLineStart = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
aLineEnd = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
aLineStart.setValue(aLineStartPnt[0], aLineStartPnt[1])
aLineEnd.setValue(aLineEndPnt[0], aLineEndPnt[1])
aSession.finishOperation()

#=========================================================================
# Test 1. Create an arc, orthogonal to the line
#=========================================================================
anArcEndPnt = [80., 20.]
aSession.startOperation()
anArc = aSketchFeature.addFeature("SketchMacroArc")
assert (anArc.getKind() == "SketchMacroArc")
anArcTgPnt = anArc.refattr("transversal_point")
assert (not anArcTgPnt.isInitialized())
anArcEnd = geomDataAPI_Point2D(anArc.attribute("end_point_4"))
assert (not anArcEnd.isInitialized())
anArcType = anArc.string("arc_type")
assert (not anArcType.isInitialized())
# initialize attributes
anArcType.setValue("by_transversal_line")
anArcTgPnt.setAttr(aLineEnd)
anArcEnd.setValue(anArcEndPnt[0], anArcEndPnt[1])
aSession.finishOperation()
verifyLastArc(aSketchFeature, [], aLineEndPnt, anArcEndPnt)
aLastArc = model.lastSubFeature(aSketchFeature, "SketchArc")
verifyArcLineTransversal(aLastArc, aSketchLine)
model.testNbSubFeatures(aSketch, "SketchConstraintCoincidence", 1)
model.testNbSubFeatures(aSketch, "SketchConstraintPerpendicular", 1)

#=========================================================================
# Test 2. Create an arc, orthogonal to the previous arc (expect an error)
#=========================================================================
aPrevArc = aLastArc
aPrevArcEnd = geomDataAPI_Point2D(aPrevArc.attribute("end_point"))
anArcEndPnt = [50., 100.]
aSession.startOperation()
anArc = aSketchFeature.addFeature("SketchMacroArc")
anArcTgPnt = anArc.refattr("transversal_point")
anArcEnd = geomDataAPI_Point2D(anArc.attribute("end_point_4"))
anArcType = anArc.string("arc_type")
# initialize attributes
anArcType.setValue("by_transversal_line")
anArcTgPnt.setAttr(aPrevArcEnd)
anArcEnd.setValue(anArcEndPnt[0], anArcEndPnt[1])
aSession.finishOperation()
assert(anArc.error() != "")
# remove failed feature
aSession.startOperation()
aDocument.removeFeature(anArc)
aSession.finishOperation()

#=========================================================================
# Test 3. Create an arc, orthogonal to the line with end point on the arc
#=========================================================================
aPrevArc = model.lastSubFeature(aSketchFeature, "SketchArc")
aPrevArcEnd = geomDataAPI_Point2D(aPrevArc.attribute("end_point"))
aSession.startOperation()
anArc = aSketchFeature.addFeature("SketchMacroArc")
anArcTgPnt = anArc.refattr("transversal_point")
anArcEnd = geomDataAPI_Point2D(anArc.attribute("end_point_4"))
anArcEndRef = anArc.refattr("end_point_ref")
anArcType = anArc.string("arc_type")
# initialize attributes
anArcType.setValue("by_transversal_line")
anArcTgPnt.setAttr(aLineStart)
anArcEndRef.setObject(aPrevArc.lastResult())
anArcEnd.setValue(anArcEndPnt[0], anArcEndPnt[1])
aSession.finishOperation()
verifyLastArc(aSketchFeature, [], [aLineStart.x(), aLineStart.y()], [])
aLastArc = model.lastSubFeature(aSketchFeature, "SketchArc")
verifyArcLineTransversal(aLastArc, aSketchLine)
aLastArcEnd = geomDataAPI_Point2D(aLastArc.attribute("end_point"))
verifyPointOnArc(aLastArcEnd, aPrevArc)
model.testNbSubFeatures(aSketch, "SketchConstraintCoincidence", 3)
model.testNbSubFeatures(aSketch, "SketchConstraintPerpendicular", 2)

#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
