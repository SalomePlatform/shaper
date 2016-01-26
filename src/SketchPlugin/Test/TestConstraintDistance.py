"""
    TestConstraintDistance.py
    Unit test of SketchPlugin_ConstraintDistance class
    
    SketchPlugin_Constraint
        static const std::string MY_CONSTRAINT_VALUE("ConstraintValue");
        static const std::string MY_FLYOUT_VALUE_PNT("ConstraintFlyoutValuePnt");
        static const std::string MY_ENTITY_A("ConstraintEntityA");
        static const std::string MY_ENTITY_B("ConstraintEntityB");
        static const std::string MY_ENTITY_C("ConstraintEntityC");
        static const std::string MY_ENTITY_D("ConstraintEntityD");
        
    SketchPlugin_ConstraintDistance
        static const std::string MY_CONSTRAINT_DISTANCE_ID("SketchConstraintDistance");
        data()->addAttribute(SketchPlugin_Constraint::VALUE(),    ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());
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


def distancePointPoint(pointA, pointB):
    """
    subroutine to calculate distance between two points
    result of calculated distance is has 10**-5 precision
    """
    xdiff = math.pow((pointA.x() - pointB.x()), 2)
    ydiff = math.pow((pointA.y() - pointB.y()), 2)
    return round(math.sqrt(xdiff + ydiff), 5)

def distancePointLine(point, line):
    """
    subroutine to calculate distance between point and line
    result of calculated distance is has 10**-5 precision
    """
    aStartPoint = geomDataAPI_Point2D(line.attribute("StartPoint"))
    aEndPoint = geomDataAPI_Point2D(line.attribute("EndPoint"))
    # orthogonal direction
    aDirX = -(aEndPoint.y() - aStartPoint.y())
    aDirY = (aEndPoint.x() - aStartPoint.x())
    aLen = math.sqrt(aDirX**2 + aDirY**2)
    aDirX = aDirX / aLen
    aDirY = aDirY / aLen
    aVecX = point.x() - aStartPoint.x()
    aVecY = point.y() - aStartPoint.y()
    return round(math.fabs(aVecX * aDirX + aVecY * aDirY), 5)

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
# Create a point and a line
#=========================================================================
aSession.startOperation()
aSketchPoint = aSketchFeature.addFeature("SketchPoint")
aSketchPointCoords = geomDataAPI_Point2D(
    aSketchPoint.attribute("PointCoordindates"))
aSketchPointCoords.setValue(50., 50.)
aSketchLine = aSketchFeature.addFeature("SketchLine")
aLineAStartPoint = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
aLineAStartPoint.setValue(0., 25.)
aLineAEndPoint.setValue(100., 25.)
aSession.finishOperation()
#=========================================================================
# Make a constraint to keep the distance
#=========================================================================
PT_PT_DIST = 25.
aDist = distancePointPoint(aSketchPointCoords, aLineAStartPoint);
assert (aDist != PT_PT_DIST)
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintDistance")
aDistance = aConstraint.real("ConstraintValue")
refattrA = aConstraint.refattr("ConstraintEntityA")
refattrB = aConstraint.refattr("ConstraintEntityB")
assert (not aDistance.isInitialized())
assert (not refattrA.isInitialized())
assert (not refattrB.isInitialized())
aLineResult = aSketchLine.firstResult()
assert (aLineResult is not None)
# the following line is necessary to check automatic calculation of a distance
aConstraint.execute()
refattrA.setAttr(aSketchPointCoords)
refattrB.setAttr(aLineAStartPoint)
aSession.finishOperation()
assert (refattrA.isInitialized())
assert (refattrB.isInitialized())
assert (aDistance.isInitialized())
assert math.fabs(aDistance.value() - aDist) < 1.e-4, "Distance values are different: {0} != {1}".format(aDistance.value(), aDist)
#=========================================================================
# Change distance value
#=========================================================================
aSession.startOperation()
aDistance.setValue(PT_PT_DIST)
aSession.finishOperation()
assert (math.fabs(distancePointPoint(aSketchPointCoords, aLineAStartPoint) - PT_PT_DIST) < 1.e-10)
#=========================================================================
# Move line, check that distance is constant
#=========================================================================
aSession.startOperation()
aLineAStartPoint.setValue(0., 40.)
aLineAEndPoint.setValue(100., 40.)
aSession.finishOperation()
assert (math.fabs(distancePointPoint(aSketchPointCoords, aLineAStartPoint) - PT_PT_DIST) < 1.e-10)
#=========================================================================
# Remove constraint, check the points are unconstrained now
#=========================================================================
aSession.startOperation()
aDocument.removeFeature(aConstraint)
aSession.finishOperation()
aSession.startOperation()
aSketchPointCoords.setValue(0., 0.)
aSession.finishOperation()
assert (math.fabs(distancePointPoint(aSketchPointCoords, aLineAStartPoint) - PT_PT_DIST) > 1.e-10)

#=========================================================================
# Add distance between point and line
#=========================================================================
PT_LINE_DIST = 50.
aDist = distancePointLine(aSketchPointCoords, aSketchLine)
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintDistance")
aDistance = aConstraint.real("ConstraintValue")
refattrA = aConstraint.refattr("ConstraintEntityA")
refattrB = aConstraint.refattr("ConstraintEntityB")
assert (not aDistance.isInitialized())
assert (not refattrA.isInitialized())
assert (not refattrB.isInitialized())
aLineResult = aSketchLine.firstResult()
assert (aLineResult is not None)
refattrA.setObject(aLineResult)
refattrB.setAttr(aSketchPointCoords)
aSession.finishOperation()
assert (refattrA.isInitialized())
assert (refattrB.isInitialized())
assert (aDistance.isInitialized())
assert math.fabs(aDistance.value() - aDist) < 1.e-4, "Distance values are different: {0} != {1}".format(aDistance.value(), aDist)
#=========================================================================
# Change distance value
#=========================================================================
aSession.startOperation()
aDistance.setValue(PT_LINE_DIST)
aSession.finishOperation()
assert (math.fabs(distancePointLine(aSketchPointCoords, aSketchLine) - PT_LINE_DIST) < 1.e-10)
#=========================================================================
# Set distance between line boundaries
#=========================================================================
aSession.startOperation()
refattrA.setAttr(aLineAStartPoint)
refattrB.setAttr(aLineAEndPoint)
aSession.finishOperation()
assert (math.fabs(distancePointPoint(aLineAStartPoint, aLineAEndPoint) - PT_LINE_DIST) < 1.e-10)
#=========================================================================
# End of test
#=========================================================================
