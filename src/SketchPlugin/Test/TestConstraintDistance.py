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
        data()->addAttribute(SketchPlugin_Constraint::VALUE(),    ModelAPI_AttributeDouble::type());
        data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::type());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::type());
        
    
"""
from GeomDataAPI import *
from ModelAPI import *
import math
#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2014-10-28"


def distance(pointA, pointB):
    """
    subroutine to calculate distance between two points
    result of calculated distance is has 10**-5 precision
    """
    xdiff = math.pow((pointA.x() - pointB.x()), 2)
    ydiff = math.pow((pointA.y() - pointB.y()), 2)
    return round(math.sqrt(xdiff + ydiff), 5)

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
#=========================================================================
# Creation of a sketch
#=========================================================================
aSession.startOperation()
aSketchCommonFeature = aDocument.addFeature("Sketch")
aSketchFeature = modelAPI_CompositeFeature(aSketchCommonFeature)
aSketchFeatureData = aSketchFeature.data()
origin = geomDataAPI_Point(aSketchFeatureData.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeatureData.attribute("DirX"))
dirx.setValue(1, 0, 0)
diry = geomDataAPI_Dir(aSketchFeatureData.attribute("DirY"))
diry.setValue(0, 1, 0)
norm = geomDataAPI_Dir(aSketchFeatureData.attribute("Norm"))
norm.setValue(0, 0, 1)
aSession.finishOperation()
#=========================================================================
# Create a point and a line
#=========================================================================
aSession.startOperation()
aSketchPoint = aSketchFeature.addFeature("SketchPoint")
aSketchPointData = aSketchPoint.data()
aSketchPointCoords = geomDataAPI_Point2D(
    aSketchPointData.attribute("PointCoordindates"))
aSketchPointCoords.setValue(50., 50.)
aSketchLine = aSketchFeature.addFeature("SketchLine")
aLineAStartPoint = geomDataAPI_Point2D(
    aSketchLine.data().attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(aSketchLine.data().attribute("EndPoint"))
aLineAStartPoint.setValue(0., 25.)
aLineAEndPoint.setValue(100., 25.)
aSession.finishOperation()
#=========================================================================
# Make a constraint to keep the distance
#=========================================================================
assert (distance(aSketchPointCoords, aLineAStartPoint) != 25.)
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintDistance")
aConstraintData = aConstraint.data()
aDistance = aConstraintData.real("ConstraintValue")
refattrA = aConstraintData.refattr("ConstraintEntityA")
refattrB = aConstraintData.refattr("ConstraintEntityB")
assert (not aDistance.isInitialized())
assert (not refattrA.isInitialized())
assert (not refattrB.isInitialized())
aDistance.setValue(25.)
aLineResult = aSketchLine.firstResult()
assert (aLineResult is not None)
refattrA.setAttr(aSketchPointCoords)
refattrB.setAttr(aLineAStartPoint)
aSession.finishOperation()
assert (aDistance.isInitialized())
assert (refattrA.isInitialized())
assert (refattrB.isInitialized())
#=========================================================================
# Move line, check that distance is constant
#=========================================================================
assert (distance(aSketchPointCoords, aLineAStartPoint) == 25.)
aSession.startOperation()
aLineAStartPoint.setValue(0., 40.)
aLineAEndPoint.setValue(100., 40.)
aSession.finishOperation()
assert (distance(aSketchPointCoords, aLineAStartPoint) == 25.)
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
