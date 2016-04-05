"""
    TestConstraintCoincidence.py
    Unit test of SketchPlugin_ConstraintCoincidence class
    
    SketchPlugin_Constraint
        static const std::string MY_CONSTRAINT_VALUE("ConstraintValue");
        static const std::string MY_FLYOUT_VALUE_PNT("ConstraintFlyoutValuePnt");
        static const std::string MY_ENTITY_A("ConstraintEntityA");
        static const std::string MY_ENTITY_B("ConstraintEntityB");
        static const std::string MY_ENTITY_C("ConstraintEntityC");
        static const std::string MY_ENTITY_D("ConstraintEntityD");
        
    SketchPlugin_ConstraintCoincidence
        static const std::string MY_CONSTRAINT_COINCIDENCE_ID("SketchConstraintCoincidence");
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());

"""
from GeomDataAPI import *
from ModelAPI import *
import math
#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2014-10-28"
TOLERANCE = 1.e-7


#=========================================================================
# Auxiliary functions
#=========================================================================
def checkPointOnLine(point, line):
    aStart = geomDataAPI_Point2D(line.attribute("StartPoint"))
    aEnd   = geomDataAPI_Point2D(line.attribute("EndPoint"))
    aDirX = aEnd.x() - aStart.x()
    aDirY = aEnd.y() - aStart.y()
    aVecX = point.x() - aStart.x()
    aVecY = point.y() - aStart.y()
    assert (math.fabs(aDirX * aVecY - aDirY * aVecX) <= TOLERANCE)

def checkPointOnCircle(point, circle):
    aCenter = geomDataAPI_Point2D(circle.attribute("CircleCenter"))
    aRadius = circle.real("CircleRadius").value()
    aDist = math.hypot(point.x() - aCenter.x(), point.y() - aCenter.y())
    assert (math.fabs(aDist - aRadius) <= TOLERANCE)

def checkPointOnArc(point, arc):
    aStart  = geomDataAPI_Point2D(arc.attribute("ArcStartPoint"))
    aCenter = geomDataAPI_Point2D(arc.attribute("ArcCenter"))
    aRadius = math.hypot(aStart.x() - aCenter.x(), aStart.y() - aCenter.y())
    aDist = math.hypot(point.x() - aCenter.x(), point.y() - aCenter.y())
    assert (math.fabs(aDist - aRadius) <= TOLERANCE)


#=========================================================================
# Start of test
#=========================================================================
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
# add an origin
aSession.startOperation()
aFeature = aDocument.addFeature("Point")
aFeature.real("x").setValue(0.)
aFeature.real("y").setValue(0.)
aFeature.real("z").setValue(0.)
anOriginName = aFeature.name()
aSession.finishOperation()
#=========================================================================
# Creation of a sketch
#=========================================================================
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aDocument.addFeature("Sketch"))
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSession.finishOperation()
#=========================================================================
# Create a line and an arc
#=========================================================================
aSession.startOperation()
aSketchArc = aSketchFeature.addFeature("SketchArc")
anArcCentr = geomDataAPI_Point2D(aSketchArc.attribute("ArcCenter"))
anArcStartPoint = geomDataAPI_Point2D(
    aSketchArc.attribute("ArcStartPoint"))
anArcEndPoint = geomDataAPI_Point2D(aSketchArc.attribute("ArcEndPoint"))
anArcCentr.setValue(10., 10.)
anArcStartPoint.setValue(0., 50.)
anArcEndPoint.setValue(50., 0.)
aSketchLine = aSketchFeature.addFeature("SketchLine")
aLineStartPoint = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
aLineEndPoint = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
# Lets initialize line start at circle's end:
aLineStartPoint.setValue(50., 0.)
aLineEndPoint.setValue(100., 25.)
aSession.finishOperation()
#=========================================================================
# Link arc's end and line's start points with concidence constraint
#=========================================================================
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintCoincidence")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setAttr(anArcEndPoint)
reflistB.setAttr(aLineStartPoint)
aConstraint.execute()
aSession.finishOperation()
#=========================================================================
# Check values and move one constrainted object
#=========================================================================
assert (anArcEndPoint.x() == 50)
assert (anArcEndPoint.y() == 0)
assert (aLineStartPoint.x() == 50)
assert (aLineStartPoint.y() == 0)
deltaX = deltaY = 40.
#  move line
aSession.startOperation()
aLineStartPoint.setValue(aLineStartPoint.x() + deltaX,
                         aLineStartPoint.y() + deltaY)
aLineEndPoint.setValue(aLineEndPoint.x() + deltaX,
                       aLineEndPoint.y() + deltaY)
aSession.finishOperation()
# check that arc's points are moved also
assert (anArcEndPoint.x() == aLineStartPoint.x())
assert (anArcEndPoint.y() == aLineStartPoint.y())
#=========================================================================
# Remove coincidence and move the line
#=========================================================================
aSession.startOperation()
aDocument.removeFeature(aConstraint)
aSession.finishOperation()
aSession.startOperation()
aLineStartPoint.setValue(70., 0.)
aSession.finishOperation()
assert (anArcEndPoint.x() != aLineStartPoint.x() or anArcEndPoint.y() != aLineStartPoint.y())

#=========================================================================
# Add constraint point-on-line
#=========================================================================
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintCoincidence")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setAttr(anArcStartPoint)
reflistB.setObject(aSketchLine.lastResult())
aConstraint.execute()
aSession.finishOperation()
checkPointOnLine(anArcStartPoint, aSketchLine)
#=========================================================================
# Add constraint point-on-circle
#=========================================================================
aSession.startOperation()
# create circle with center coincident with origin
aSketchCircle = aSketchFeature.addFeature("SketchCircle")
aCircleCenter = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
aCircleRadius = aSketchCircle.real("CircleRadius")
aCircleCenter.setValue(10., 10.)
aCircleRadius.setValue(25.)
aSession.finishOperation()
# create origin
aSession.startOperation()
anOrigRes = modelAPI_Result(aDocument.objectByName("Construction", anOriginName))
assert (anOrigRes)
anOrigShape = anOrigRes.shape()
assert (anOrigShape)
anOrigin = aSketchFeature.addFeature("SketchPoint")
anOriginCoord = geomDataAPI_Point2D(anOrigin.attribute("PointCoordindates"))
anOriginCoord.setValue(0., 0.)
anOrigin.selection("External").setValue(anOrigRes, anOrigShape)
aSession.finishOperation()
# coincidence between center of circle and the origin
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintCoincidence")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setAttr(aCircleCenter)
reflistB.setObject(anOrigin.lastResult())
aSession.finishOperation()
# point-on-circle
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintCoincidence")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setObject(aSketchCircle.lastResult())
reflistB.setAttr(aLineEndPoint)
aConstraint.execute()
aSession.finishOperation()
checkPointOnCircle(aLineEndPoint, aSketchCircle)
#=========================================================================
# Add constraint point-on-arc
#=========================================================================
aSession.startOperation("constraint point-on-arc")
aConstraint = aSketchFeature.addFeature("SketchConstraintCoincidence")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setAttr(aCircleCenter)
reflistB.setObject(aSketchArc.lastResult())
aConstraint.execute()
aSession.finishOperation()
#checkPointOnArc(aCircleCenter, aSketchArc)
#=========================================================================
# Check center of circle is still in origin
#=========================================================================
#assert (aCircleCenter.x() == 0. and aCircleCenter.y() == 0.)
#=========================================================================
# End of test
#=========================================================================
