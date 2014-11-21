"""
    TestConstraintPerpendicular.py
    Unit test of SketchPlugin_ConstraintPerpendicular class
    
    SketchPlugin_Constraint
        static const std::string MY_CONSTRAINT_VALUE("ConstraintValue");
        static const std::string MY_FLYOUT_VALUE_PNT("ConstraintFlyoutValuePnt");
        static const std::string MY_ENTITY_A("ConstraintEntityA");
        static const std::string MY_ENTITY_B("ConstraintEntityB");
        static const std::string MY_ENTITY_C("ConstraintEntityC");
        static const std::string MY_ENTITY_D("ConstraintEntityD");
        
    SketchPlugin_ConstraintPerpendicular
        static const std::string MY_CONSTRAINT_PERPENDICULAR_ID("SketchConstraintPerpendicular");
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::type());
        data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::type()); 
"""
from GeomDataAPI import *
from ModelAPI import *
#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2014-10-28"

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
#=========================================================================
# Creation of a sketch
#=========================================================================
aSession.startOperation()
aSketchCommonFeature = aDocument.addFeature("Sketch")
aSketchFeature = modelAPI_CompositeFeature(aSketchCommonFeature)
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
diry = geomDataAPI_Dir(aSketchFeature.attribute("DirY"))
diry.setValue(0, 1, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSession.finishOperation()
#=========================================================================
# Create two lines which are already perpendicular
#=========================================================================
aSession.startOperation()
# line A
aSketchLineA = aSketchFeature.addFeature("SketchLine")
aLineAStartPoint = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(aSketchLineA.attribute("EndPoint"))
aLineAStartPoint.setValue(0., 25)
aLineAEndPoint.setValue(85., 25)
# line B
aSketchLineB = aSketchFeature.addFeature("SketchLine")
aLineBStartPoint = geomDataAPI_Point2D(aSketchLineB.attribute("StartPoint"))
aLineBEndPoint = geomDataAPI_Point2D(aSketchLineB.attribute("EndPoint"))
aLineBStartPoint.setValue(25., 40.)
aLineBEndPoint.setValue(25., 125.)
aSession.finishOperation()
#=========================================================================
# Make a constraint to keep the length of the line constant
# to prevent perpendicular constraint collapsing line to point
#=========================================================================
aSession.startOperation()
for eachFeature in [aSketchLineA, aSketchLineB]:
    aLengthConstraint = aSketchFeature.addFeature("SketchConstraintLength")
    eachRefattr = aLengthConstraint.refattr("ConstraintEntityA")
    eachResult = modelAPI_ResultConstruction(eachFeature.firstResult())
    assert (eachResult is not None)
    eachRefattr.setObject(eachResult)
    aLengthConstraint.execute()
aSession.finishOperation()

# Coordinates of lines should not be changed after this constraint
assert (aLineAStartPoint.x() == 0)
assert (aLineAStartPoint.y() == 25)
assert (aLineAEndPoint.x() == 85)
assert (aLineAEndPoint.y() == 25)
assert (aLineBStartPoint.x() == 25)
assert (aLineBStartPoint.y() == 40)
assert (aLineBEndPoint.x() == 25)
assert (aLineBEndPoint.y() == 125)
#=========================================================================
# Link lines with perpendicular constraint
#=========================================================================
aSession.startOperation()
aPerpendicularConstraint = aSketchFeature.addFeature("SketchConstraintPerpendicular")
refattrA = aPerpendicularConstraint.refattr("ConstraintEntityA")
refattrB = aPerpendicularConstraint.refattr("ConstraintEntityB")
aResultA = modelAPI_ResultConstruction(aSketchLineA.firstResult())
aResultB = modelAPI_ResultConstruction(aSketchLineB.firstResult())
assert (aResultA is not None)
assert (aResultB is not None)
refattrA.setObject(aResultA)
refattrB.setObject(aResultB)
aPerpendicularConstraint.execute()
aSession.finishOperation()
#=========================================================================
# Check values and move one constrainted object
#=========================================================================
aLineBStartPointPrev = (aLineBStartPoint.x(),aLineBStartPoint.y())
aLineBEndPointPrev = (aLineBEndPoint.x(),aLineBEndPoint.y())
deltaX = deltaY = 5.
# move line without rotation,
# check that reference's line points are not changed also
aSession.startOperation()
aLineAStartPoint.setValue(aLineAStartPoint.x() + deltaX, aLineAStartPoint.y() + deltaY)
aLineAEndPoint.setValue(aLineAEndPoint.x() + deltaX, aLineAEndPoint.y() + deltaY)
aSession.finishOperation()
assert (aLineBStartPointPrev == (aLineBStartPoint.x(), aLineBStartPoint.y()))
assert (aLineBEndPointPrev   == (aLineBEndPoint.x(),   aLineBEndPoint.y()))
aLineBStartPointPrev = (aLineBStartPoint.x(),aLineBStartPoint.y())
aLineBEndPointPrev = (aLineBEndPoint.x(),aLineBEndPoint.y())
# rotate line, 
# check that reference's line points are moved also
aSession.startOperation()
aLineAStartPoint.setValue(aLineAStartPoint.x() + deltaX, aLineAStartPoint.y() + deltaY)
aLineAEndPoint.setValue(aLineAEndPoint.x() - deltaX, aLineAEndPoint.y() - deltaY)
aSession.finishOperation()
assert (aLineBStartPointPrev != (aLineBStartPoint.x(), aLineBStartPoint.y()))
assert (aLineBEndPointPrev   != (aLineBEndPoint.x(),   aLineBEndPoint.y()))
#=========================================================================
# End of test
#=========================================================================
