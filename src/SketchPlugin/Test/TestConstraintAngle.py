"""
    TestConstraintAngle.py
    Unit test of SketchPlugin_ConstraintAngle class
        
    SketchPlugin_ConstraintAngle
        static const std::string MY_CONSTRAINT_ANGLE_ID("SketchConstraintAngle");
        data()->addAttribute(SketchPlugin_Constraint::VALUE(),    ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());
        
    
"""
from GeomDataAPI import *
from ModelAPI import *
import os
import math

#=========================================================================
# Auxiliary functions
#=========================================================================

def angle(theLine1, theLine2):
    # subroutine to angle between two lines
    aStartPoint1 = geomDataAPI_Point2D(theLine1.attribute("StartPoint"))
    aEndPoint1   = geomDataAPI_Point2D(theLine1.attribute("EndPoint"))
    aStartPoint2 = geomDataAPI_Point2D(theLine2.attribute("StartPoint"))
    aEndPoint2   = geomDataAPI_Point2D(theLine2.attribute("EndPoint"))
    
    aDirX1 = aEndPoint1.x() - aStartPoint1.x()
    aDirY1 = aEndPoint1.y() - aStartPoint1.y()
    aLen1 = math.hypot(aDirX1, aDirY1)
    aDirX2 = aEndPoint2.x() - aStartPoint2.x()
    aDirY2 = aEndPoint2.y() - aStartPoint2.y()
    aLen2 = math.hypot(aDirX2, aDirY2)
    
    aDot = aDirX1 * aDirX2 + aDirY1 * aDirY2
    
    anAngle = math.acos(aDot / aLen1 / aLen2)
    return round(anAngle * 180. / math.pi, 6)


#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2015-09-18"

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
# Create two lines
#=========================================================================
aSession.startOperation()
aSketchLineA = aSketchFeature.addFeature("SketchLine")
aStartPoint = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aEndPoint = geomDataAPI_Point2D(aSketchLineA.attribute("EndPoint"))
aStartPoint.setValue(-10., 25.)
aEndPoint.setValue(100., 25.)

aSketchLineB = aSketchFeature.addFeature("SketchLine")
aStartPoint = geomDataAPI_Point2D(aSketchLineB.attribute("StartPoint"))
aEndPoint = geomDataAPI_Point2D(aSketchLineB.attribute("EndPoint"))
aStartPoint.setValue(-20., 15.)
aEndPoint.setValue(80., 50.)
aSession.finishOperation()
#=========================================================================
# Make a constraint to keep the angle
#=========================================================================
ANGLE_DEGREE = 30.
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintAngle")
anAngleVal = aConstraint.real("ConstraintValue")
refattrA = aConstraint.refattr("ConstraintEntityA")
refattrB = aConstraint.refattr("ConstraintEntityB")
assert (not anAngleVal.isInitialized())
assert (not refattrA.isInitialized())
assert (not refattrB.isInitialized())
anAngleVal.setValue(ANGLE_DEGREE)
refattrA.setObject(aSketchLineA.firstResult())
refattrB.setObject(aSketchLineB.firstResult())
aConstraint.execute()
aSession.finishOperation()
assert (anAngleVal.isInitialized())
assert (refattrA.isInitialized())
assert (refattrB.isInitialized())
assert (angle(aSketchLineA, aSketchLineB) == ANGLE_DEGREE)
#=========================================================================
# Move line, check that angle is constant
#=========================================================================
aSession.startOperation()
aStartPoint = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aStartPoint.setValue(0., 30.)
aConstraint.execute()
aSession.finishOperation()
assert (angle(aSketchLineA, aSketchLineB) == ANGLE_DEGREE)
#=========================================================================
# Change angle value and check the lines are moved
#=========================================================================
NEW_ANGLE_DEGREE = 60.
aSession.startOperation()
anAngleVal.setValue(NEW_ANGLE_DEGREE)
aConstraint.execute()
aSession.finishOperation()
assert (angle(aSketchLineA, aSketchLineB) == NEW_ANGLE_DEGREE)
#=========================================================================
# TODO: improve test
# 1. remove constraint, move line's start point to
#    check that constraint are not applied
# 2. check constrained distance between:
#    * point and line
#    * two lines
#=========================================================================
#=========================================================================
# End of test
#=========================================================================
