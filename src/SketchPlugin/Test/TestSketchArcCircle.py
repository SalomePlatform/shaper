"""
    TestSketchArcCircle.py
    
    static const std::string MY_SKETCH_ARC_ID("SketchArc");
    static const std::string MY_CENTER_ID = "ArcCenter";
    static const std::string MY_START_ID = "ArcStartPoint";
    static const std::string MY_END_ID = "ArcEndPoint";
    data()->addAttribute(SketchPlugin_Arc::CENTER_ID(), GeomDataAPI_Point2D::typeId());
    data()->addAttribute(SketchPlugin_Arc::START_ID(),  GeomDataAPI_Point2D::typeId());
    data()->addAttribute(SketchPlugin_Arc::END_ID(),    GeomDataAPI_Point2D::typeId());
    
    static const std::string MY_CIRCLE_ID("SketchCircle");
    static const std::string MY_CIRCLE_CENTER_ID("CircleCenter");
    static const std::string MY_CIRCLE_RADIUS_ID("CircleRadius");
    data()->addAttribute(SketchPlugin_Circle::CENTER_ID(), GeomDataAPI_Point2D::typeId());
    data()->addAttribute(SketchPlugin_Circle::RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
"""

#=========================================================================
# Initialization of the test
#=========================================================================
from GeomDataAPI import *
from ModelAPI import *
import math

__updated__ = "2014-10-28"


#=========================================================================
# Auxiliary functions
#=========================================================================

def angle(theCenter, theFirst, theLast):
    """
    subroutine to calculate angle given by 3 points
    """
    aDirX1 = theFirst.x() - theCenter.x()
    aDirY1 = theFirst.y() - theCenter.y()
    aLen1 = math.hypot(aDirX1, aDirY1)
    aDirX2 = theLast.x() - theCenter.x()
    aDirY2 = theLast.y() - theCenter.y()
    aLen2 = math.hypot(aDirX2, aDirY2)
    aDot = aDirX1 * aDirX2 + aDirY1 * aDirY2
    anAngle = math.acos(aDot / aLen1 / aLen2)
    return round(anAngle * 180. / math.pi, 6)

def distancePointPoint(thePointA, thePointB):
    """
    subroutine to calculate distance between two points
    result of calculated distance is has 10**-5 precision
    """
    xdiff = math.pow((thePointA.x() - thePointB.x()), 2)
    ydiff = math.pow((thePointA.y() - thePointB.y()), 2)
    return round(math.sqrt(xdiff + ydiff), 5)


aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
#=========================================================================
# Creation of a sketch
#=========================================================================
aSession.startOperation()
#aSketchFeature = aDocument.addFeature("Sketch")
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
# Creation of an arc
# 1. Test SketchPlugin_Arc attributes
# 2.
#=========================================================================
aSession.startOperation()
aSketchReflist = aSketchFeature.reflist("Features")
assert (not aSketchReflist.isInitialized())
assert (aSketchReflist.size() == 0)
assert (len(aSketchReflist.list()) == 0)
aSketchArc = aSketchFeature.addFeature("SketchArc")
assert (aSketchArc.getKind() == "SketchArc")
anArcCentr = geomDataAPI_Point2D(aSketchArc.attribute("ArcCenter"))
assert (anArcCentr.x() == 0)
assert (anArcCentr.y() == 0)
assert (not anArcCentr.isInitialized())
anArcCentr.setValue(10., 10.)
anArcStartPoint = geomDataAPI_Point2D(
    aSketchArc.attribute("ArcStartPoint"))
assert (anArcStartPoint.x() == 0)
assert (anArcStartPoint.y() == 0)
assert (not anArcStartPoint.isInitialized())
anArcStartPoint.setValue(0., 50.)
anArcEndPoint = geomDataAPI_Point2D(aSketchArc.attribute("ArcEndPoint"))
assert (anArcEndPoint.x() == 0)
assert (anArcEndPoint.y() == 0)
assert (not anArcEndPoint.isInitialized())
anArcEndPoint.setValue(50., 0.)
aSession.finishOperation()
# check that values have been changed
aSketchReflist = aSketchFeature.reflist("Features")
assert (aSketchReflist.size() == 1)
assert (len(aSketchReflist.list()) == 1)
assert (anArcCentr.x() == 10.0)
assert (anArcCentr.y() == 10.0)
assert (anArcStartPoint.x() == 0.0)
assert (anArcStartPoint.y() == 50.0)
assert (anArcEndPoint.x() == 50.0)
assert (anArcEndPoint.y() == 0.0)
#=========================================================================
# Edit the arc:
# 1. Move whole arc
# 2. Change the start point
# 3. Change the radius of arc
# 4. Change arc's angle
#=========================================================================
aSession.startOperation()
deltaX, deltaY = 5., 10.
anArcCentr.setValue(anArcCentr.x() + deltaX, anArcCentr.y() + deltaY)
aSession.finishOperation()
assert (anArcCentr.x() == 15)
assert (anArcCentr.y() == 20)
assert (math.fabs(distancePointPoint(anArcCentr, anArcStartPoint) - distancePointPoint(anArcCentr, anArcEndPoint)) < 1.e-10)
# Change the start point
aSession.startOperation()
anArcStartPoint.setValue(anArcStartPoint.x() + deltaX, anArcStartPoint.y())
aSession.finishOperation()
assert (math.fabs(distancePointPoint(anArcCentr, anArcStartPoint) - distancePointPoint(anArcCentr, anArcEndPoint)) < 1.e-10)
# Change radius
RADIUS = 50
aSession.startOperation()
anArcRadius = aSketchArc.real("ArcRadius")
anArcRadius.setValue(RADIUS)
aSession.finishOperation()
assert (math.fabs(distancePointPoint(anArcCentr, anArcStartPoint) - RADIUS) < 1.e-10)
assert (math.fabs(distancePointPoint(anArcCentr, anArcEndPoint) - RADIUS) < 1.e-10)
# Change angle
ANGLE = 120
aSession.startOperation()
anArcAngle = aSketchArc.real("ArcAngle")
anArcAngle.setValue(ANGLE)
aSession.finishOperation()
assert (math.fabs(angle(anArcCentr, anArcStartPoint, anArcEndPoint) - ANGLE) < 1.e-7)
#=========================================================================
# Check results of the Arc
#=========================================================================
aResult = aSketchArc.firstResult()
aResultConstruction = modelAPI_ResultConstruction(aResult)
aShape = aResultConstruction.shape()
assert (aShape is not None)
assert (not aShape.isNull())
#=========================================================================
# Create a circle
# 1. Test SketchPlugin_Circle.h attributes
# 2. ModelAPI_AttributeDouble attribute
#=========================================================================
aSession.startOperation()
aSketchReflist = aSketchFeature.reflist("Features")
# Arc is already added
assert (aSketchReflist.size() == 1)
assert (len(aSketchReflist.list()) == 1)
aSketchCircle = aSketchFeature.addFeature("SketchCircle")
assert (aSketchCircle.getKind() == "SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
assert (anCircleCentr.x() == 0)
assert (anCircleCentr.y() == 0)
assert (not anCircleCentr.isInitialized())
aCircleRadius = aSketchCircle.real("CircleRadius")
assert (type(aCircleRadius) == ModelAPI_AttributeDouble)
# ModelAPI_AttributeDouble.typeId() is checked in ModelAPI_TestConstants
assert (aCircleRadius.attributeType() == ModelAPI_AttributeDouble.typeId())
assert (aCircleRadius.value() == 0)
anCircleCentr.setValue(-25., -25)
aCircleRadius.setValue(25.)
assert (anCircleCentr.x() == -25)
assert (anCircleCentr.y() == -25)
assert (aCircleRadius.value() == 25)
aSession.finishOperation()
#=========================================================================
# Edit the Cricle
# 1. Check that changing the centr of a circle does not affects radius
# 2. and vise versa; also check that int is acceptable as well as a real
#=========================================================================
aSession.startOperation()
anCircleCentr.setValue(10, 60)
aSession.finishOperation()
assert (anCircleCentr.x() == 10)
assert (anCircleCentr.y() == 60)
assert (aCircleRadius.value() == 25)
aSession.startOperation()
aCircleRadius.setValue(int(20))
aSession.finishOperation()
assert (anCircleCentr.x() == 10)
assert (anCircleCentr.y() == 60)
assert (aCircleRadius.value() == 20)
#=========================================================================
# Create an arc, tangent to the line
#=========================================================================
aSession.startOperation()
aSketchLine = aSketchFeature.addFeature("SketchLine")
aLineStart = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
aLineEnd = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
aLineStart.setValue(0., 0.)
aLineEnd.setValue(50., 0.)
aSession.finishOperation()
aSession.startOperation()
aSketchArcTangent = aSketchFeature.addFeature("SketchArc")
aSketchArcTangent.string("ArcType").setValue("Tangent")
anArcEndPoint = geomDataAPI_Point2D(aSketchArcTangent.attribute("ArcEndPoint"))
aTangent = aSketchArcTangent.refattr("ArcTangentPoint")
aTangent.setAttr(aLineEnd)
anArcEndPoint.setValue(51., 1.)
anArcEndPoint.setValue(100., 25.)
aSession.finishOperation()
anArcCenter = geomDataAPI_Point2D(aSketchArcTangent.attribute("ArcCenter"))
assert(anArcCenter.x() == 50.)
#=========================================================================
# Create an arc, tangent to the previous arc
#=========================================================================
aSession.startOperation()
aSketchArcTangent2 = aSketchFeature.addFeature("SketchArc")
aSketchArcTangent2.string("ArcType").setValue("Tangent")
anArcEndPoint2 = geomDataAPI_Point2D(aSketchArcTangent2.attribute("ArcEndPoint"))
aTangent = aSketchArcTangent2.refattr("ArcTangentPoint")
aTangent.setAttr(anArcEndPoint)
anArcEndPoint2.setValue(anArcEndPoint.x() + 1, anArcEndPoint.y() + 1)
anArcEndPoint2.setValue(50., 50.)
aSession.finishOperation()
#=========================================================================
# End of test
#=========================================================================
