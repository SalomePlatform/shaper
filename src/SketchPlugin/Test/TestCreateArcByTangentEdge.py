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

__updated__ = "2017-04-06"

TOLERANCE = 1.e-7

#=========================================================================
# Auxiliary functions
#=========================================================================

def verifyLastArc(theSketch, theCenter, theStart, theEnd):
    """
    subroutine to verify position of last arc in the sketch
    """
    aLastArc = model.lastSubFeature(theSketch, "SketchArc")
    aCenterPnt = geomDataAPI_Point2D(aLastArc.attribute("center_point"))
    aStartPnt = geomDataAPI_Point2D(aLastArc.attribute("start_point"))
    aEndPnt = geomDataAPI_Point2D(aLastArc.attribute("end_point"))
    if len(theCenter):
        verifyPointCoordinates(aCenterPnt, theCenter[0], theCenter[1])
    if len(theStart):
        verifyPointCoordinates(aStartPnt, theStart[0], theStart[1])
    if len(theEnd):
        verifyPointCoordinates(aEndPnt, theEnd[0], theEnd[1])
    model.assertSketchArc(aLastArc)

def verifyPointCoordinates(thePoint, theX, theY):
    assert thePoint.x() == theX and thePoint.y() == theY, "Wrong '{0}' point ({1}, {2}), expected ({3}, {4})".format(thePoint.id(), thePoint.x(), thePoint.y(), theX, theY)

def verifyTangent(theFeature1, theFeature2):
    anArcs = []
    aLines = []
    aFeatures = [theFeature1, theFeature2]
    for feat in aFeatures:
        if feat.getKind() == "SketchLine":
            aLines.append(feat)
        elif feat.getKind() == "SketchArc":
            anArcs.append(feat)
    if len(anArcs) == 2:
        verifyArcArcTangent(anArcs[0], anArcs[1])
    elif len(anArcs) == 1 and len(aLines) == 1:
        verifyArcLineTangent(anArcs[0], aLines[0])

def verifyArcArcTangent(theArc1, theArc2):
    aCenter1 = geomDataAPI_Point2D(theArc1.attribute("center_point"))
    aStart1  = geomDataAPI_Point2D(theArc1.attribute("start_point"))
    aRadius1 = model.distancePointPoint(aStart1, aCenter1)

    aCenter2 = geomDataAPI_Point2D(theArc2.attribute("center_point"))
    aStart2  = geomDataAPI_Point2D(theArc2.attribute("start_point"))
    aRadius2 = model.distancePointPoint(aStart2, aCenter2)

    aDistCC = model.distancePointPoint(aCenter1, aCenter2)
    aRSum = aRadius1 + aRadius2
    aRDiff = math.fabs(aRadius1 - aRadius2)
    assert math.fabs(aRSum - aDistCC) < TOLERANCE or math.fabs(aRDiff - aDistCC) < TOLERANCE, "Arcs do not tangent"

def verifyArcLineTangent(theArc, theLine):
    aCenter = geomDataAPI_Point2D(theArc.attribute("center_point"))
    aStart  = geomDataAPI_Point2D(theArc.attribute("start_point"))
    aRadius = model.distancePointPoint(aStart, aCenter)

    aDistCL = distancePointLine(aCenter, theLine)
    assert math.fabs(aDistCL - aRadius) < TOLERANCE, "Arc and line do not tangent"

def distancePointLine(thePoint, theLine):
    aLineStart = geomDataAPI_Point2D(theLine.attribute("StartPoint")).pnt().xy()
    aLineEnd = geomDataAPI_Point2D(theLine.attribute("EndPoint")).pnt().xy()
    aLineDir = aLineEnd.decreased(aLineStart)
    aLineLen = aLineEnd.distance(aLineStart)
    aPntDir = thePoint.pnt().xy().decreased(aLineStart)
    return math.fabs(aPntDir.cross(aLineDir) / aLineLen)

def verifyPointOnLine(thePoint, theLine):
    aDistance = distancePointLine(thePoint, theLine)
    assert aDistance < TOLERANCE, "Point is not on Line, distance: {0}".format(aDistance)



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
# Test 1. Create an arc, tangent to the line
#=========================================================================
anArcEndPnt = [80., 20.]
aSession.startOperation()
anArc = aSketchFeature.addFeature("SketchMacroArc")
assert (anArc.getKind() == "SketchMacroArc")
anArcTgPnt = anArc.refattr("tangent_point")
assert (not anArcTgPnt.isInitialized())
anArcEnd = geomDataAPI_Point2D(anArc.attribute("end_point_3"))
assert (not anArcEnd.isInitialized())
anArcType = anArc.string("arc_type")
assert (not anArcType.isInitialized())
# initialize attributes
anArcType.setValue("by_tangent_edge")
anArcTgPnt.setAttr(aLineEnd)
anArcEnd.setValue(anArcEndPnt[0], anArcEndPnt[1])
aSession.finishOperation()
verifyLastArc(aSketchFeature, [], aLineEndPnt, anArcEndPnt)
aLastArc = model.lastSubFeature(aSketchFeature, "SketchArc")
verifyTangent(aLastArc, aSketchLine)
model.testNbSubFeatures(aSketch, "SketchConstraintCoincidence", 1)
model.testNbSubFeatures(aSketch, "SketchConstraintTangent", 1)

#=========================================================================
# Test 2. Create an arc, tangent to the previous arc
#=========================================================================
aPrevArc = aLastArc
aPrevArcEnd = geomDataAPI_Point2D(aPrevArc.attribute("end_point"))
anArcEndPnt = [50., 100.]
aSession.startOperation()
anArc = aSketchFeature.addFeature("SketchMacroArc")
anArcTgPnt = anArc.refattr("tangent_point")
anArcEnd = geomDataAPI_Point2D(anArc.attribute("end_point_3"))
anArcType = anArc.string("arc_type")
# initialize attributes
anArcType.setValue("by_tangent_edge")
anArcTgPnt.setAttr(aPrevArcEnd)
anArcEnd.setValue(anArcEndPnt[0], anArcEndPnt[1])
aSession.finishOperation()
verifyLastArc(aSketchFeature, [], [aPrevArcEnd.x(), aPrevArcEnd.y()], anArcEndPnt)
aLastArc = model.lastSubFeature(aSketchFeature, "SketchArc")
verifyTangent(aLastArc, aPrevArc)
model.testNbSubFeatures(aSketch, "SketchConstraintCoincidence", 2)
model.testNbSubFeatures(aSketch, "SketchConstraintTangent", 2)

#=========================================================================
# Test 3. Create an arc, tangent to the previous arc with end point on the line
#=========================================================================
aPrevArc = aLastArc
aPrevArcEnd = geomDataAPI_Point2D(aPrevArc.attribute("end_point"))
aSession.startOperation()
anArc = aSketchFeature.addFeature("SketchMacroArc")
anArcTgPnt = anArc.refattr("tangent_point")
anArcEnd = geomDataAPI_Point2D(anArc.attribute("end_point_3"))
anArcEndRef = anArc.refattr("end_point_ref")
anArcType = anArc.string("arc_type")
# initialize attributes
anArcType.setValue("by_tangent_edge")
anArcTgPnt.setAttr(aPrevArcEnd)
anArcEndRef.setObject(aSketchLine.lastResult())
anArcEnd.setValue(aLineStartPnt[0], aLineStartPnt[1])
aSession.finishOperation()
verifyLastArc(aSketchFeature, [], [aPrevArcEnd.x(), aPrevArcEnd.y()], [])
aLastArc = model.lastSubFeature(aSketchFeature, "SketchArc")
verifyTangent(aLastArc, aPrevArc)
aLastArcEnd = geomDataAPI_Point2D(aLastArc.attribute("end_point"))
verifyPointOnLine(aLastArcEnd, aSketchLine)
model.testNbSubFeatures(aSketch, "SketchConstraintCoincidence", 4)
model.testNbSubFeatures(aSketch, "SketchConstraintTangent", 3)

#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
