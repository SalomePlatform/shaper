"""
    TestConstraintCollinear.py
    Unit test of SketchPlugin_ConstraintCollinear class
        
    SketchPlugin_ConstraintCollinear
        static const std::string MY_CONSTRAINT_COLLINEAR_ID("SketchConstraintCollinear");
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());

"""
from GeomDataAPI import *
from ModelAPI import *
import math
#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2016-01-28"


def checkCollinearVec(theX1, theY1, theX2, theY2):
    TOL = 1.e-6
    aLen1 = math.hypot(theX1, theY1)
    aLen2 = math.hypot(theX2, theY2)
    aDot = theX1 * theX2 + theY1 * theY2
    assert math.fabs(math.fabs(aDot) - aLen1 * aLen2) < TOL**2, "aDot = {0}, aLen1 = {1}, aLen2 = {2}, aDiff = {3}".format(aDot, aLen1, aLen2, math.fabs(aDot) - aLen1 * aLen2)

def checkCollinear(theLine1, theLine2):
    aStartPoint1 = geomDataAPI_Point2D(theLine1.attribute("StartPoint"))
    aEndPoint1   = geomDataAPI_Point2D(theLine1.attribute("EndPoint"))
    aStartPoint2 = geomDataAPI_Point2D(theLine2.attribute("StartPoint"))
    aEndPoint2   = geomDataAPI_Point2D(theLine2.attribute("EndPoint"))
    
    aDir1x, aDir1y = aEndPoint1.x() - aStartPoint1.x(), aEndPoint1.y() - aStartPoint1.y()
    aDir2x, aDir2y = aEndPoint2.x() - aStartPoint1.x(), aEndPoint2.y() - aStartPoint1.y()
    aDir3x, aDir3y = aStartPoint2.x() - aStartPoint1.x(), aStartPoint2.y() - aStartPoint1.y()
    checkCollinearVec(aDir1x, aDir1y, aDir2x, aDir2y)
    checkCollinearVec(aDir1x, aDir1y, aDir3x, aDir3y)



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
# line A
aSketchLineA = aSketchFeature.addFeature("SketchLine")
aLineAStartPoint = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(aSketchLineA.attribute("EndPoint"))
aSketchLineB = aSketchFeature.addFeature("SketchLine")
aLineAStartPoint.setValue(0., 25)
aLineAEndPoint.setValue(85., 25)
# line B
aLineBStartPoint = geomDataAPI_Point2D(aSketchLineB.attribute("StartPoint"))
aLineBEndPoint = geomDataAPI_Point2D(aSketchLineB.attribute("EndPoint"))
aLineBStartPoint.setValue(0., 50)
aLineBEndPoint.setValue(80., 75)
aSession.finishOperation()
#=========================================================================
# Link lines with collinear constraint
#=========================================================================
aSession.startOperation()
aParallelConstraint = aSketchFeature.addFeature("SketchConstraintCollinear")
refattrA = aParallelConstraint.refattr("ConstraintEntityA")
refattrB = aParallelConstraint.refattr("ConstraintEntityB")
refattrA.setObject(aSketchLineA.firstResult())
refattrB.setObject(aSketchLineB.firstResult())
aParallelConstraint.execute()
aSession.finishOperation()
checkCollinear(aSketchLineA, aSketchLineB)
#=========================================================================
# Check values and move one constrainted object
#=========================================================================
deltaX = deltaY = 10.
# rotate line, check that reference's line points are moved also
aLineBStartPointPrev = (aLineBStartPoint.x(), aLineBStartPoint.y())
aLineBEndPointPrev = (aLineBEndPoint.x(), aLineBEndPoint.y())
aSession.startOperation()
aLineAStartPoint.setValue(aLineAStartPoint.x() + deltaX,
                          aLineAStartPoint.y() + deltaY)
aLineAEndPoint.setValue(aLineAEndPoint.x() - deltaX,
                        aLineAEndPoint.y() - deltaY)
aSession.finishOperation()
checkCollinear(aSketchLineA, aSketchLineB)
#=========================================================================
# End of test
#=========================================================================
