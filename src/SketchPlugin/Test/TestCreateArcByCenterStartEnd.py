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
    aCenterPnt = geomDataAPI_Point2D(aLastArc.attribute("center_point"))
    aStartPnt = geomDataAPI_Point2D(aLastArc.attribute("start_point"))
    aEndPnt = geomDataAPI_Point2D(aLastArc.attribute("end_point"))
    if len(theCenter):
        verifyPointCoordinates(aCenterPnt, theCenter[0], theCenter[1])
    if len(theStart):
        verifyPointCoordinates(aStartPnt, theStart[0], theStart[1])
    if len(theEnd):
        verifyPointCoordinates(aEndPnt, theEnd[0], theEnd[1])
    aRadius = aLastArc.real("radius")
    aDistCS = model.distancePointPoint(aCenterPnt, aStartPnt)
    aDistCE = model.distancePointPoint(aCenterPnt, aEndPnt)
    assert math.fabs(aDistCS - aDistCE) < TOLERANCE, "Wrong arc: center-start distance {}, center-end distance {}".format(aDistCS, aDistCE)
    assert math.fabs(aRadius.value() -aDistCS) < TOLERANCE, "Wrong radius {0}, expected {1}".format(aRadius.value(), aDistCS)

def verifyPointCoordinates(thePoint, theX, theY):
    assert thePoint.x() == theX and thePoint.y() == theY, "Wrong '{0}' point ({1}, {2}), expected ({3}, {4})".format(thePoint.id(), thePoint.x(), thePoint.y(), theX, theY)


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
# End of test
#=========================================================================

assert(model.checkPythonDump())
