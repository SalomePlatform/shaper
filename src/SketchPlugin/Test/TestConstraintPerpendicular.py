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
# Create two lines which are already perpendicular
#=========================================================================
aSession.startOperation()
# line A
aSketchLineA = aSketchFeature.addFeature("SketchLine")
aLineAStartPoint = geomDataAPI_Point2D(
    aSketchLineA.data().attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(
    aSketchLineA.data().attribute("EndPoint"))
# line B
aSketchLineB = aSketchFeature.addFeature("SketchLine")
aLineAStartPoint.setValue(0., 25)
aLineAEndPoint.setValue(85., 25)
aLineBStartPoint = geomDataAPI_Point2D(
    aSketchLineB.data().attribute("StartPoint"))
aLineBEndPoint = geomDataAPI_Point2D(
    aSketchLineB.data().attribute("EndPoint"))
aLineBStartPoint.setValue(25., 40.)
aLineBEndPoint.setValue(25., 125.)
aSession.finishOperation()
#=========================================================================
# Make a constraint to keep the length of the line constant
# to prevent perpendicular constraint collapsing line to point
#=========================================================================

for eachFeature in [aSketchLineA, aSketchLineB]:
    aSession.startOperation()
    aLengthConstraint = aSketchFeature.addFeature("SketchConstraintLength")
    aLengthConstraintData = aLengthConstraint.data()
    refattrA = aLengthConstraintData.refattr("ConstraintEntityA")
    aResultA = modelAPI_ResultConstruction(eachFeature.firstResult())
    assert (aResultA is not None)
    refattrA.setObject(aResultA)
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
aPerpendicularConstraint = aSketchFeature.addFeature(
    "SketchConstraintPerpendicular")
aConstraintData = aPerpendicularConstraint.data()
refattrA = aConstraintData.refattr("ConstraintEntityA")
refattrB = aConstraintData.refattr("ConstraintEntityB")
# aResultA is already defined for the length constraint
aResultB = modelAPI_ResultConstruction(aSketchLineB.firstResult())
assert (aResultB is not None)
refattrA.setObject(aResultA)
refattrB.setObject(aResultB)
aPerpendicularConstraint.execute()
aSession.finishOperation()
#=========================================================================
# Check values and move one constrainted object
#=========================================================================
# print "Check values and move one constrainted object"
# print "assert (aLineAStartPoint.x() == %d)" % aLineAStartPoint.x()
# print "assert (aLineAStartPoint.y() == %d)" % aLineAStartPoint.y()
# print "assert (aLineAEndPoint.x() == %d)" % aLineAEndPoint.x()
# print "assert (aLineAEndPoint.y() == %d)" % aLineAEndPoint.y()
# print "assert (aLineBStartPoint.x() == %d)" % aLineBStartPoint.x()
# print "assert (aLineBStartPoint.y() == %d)" % aLineBStartPoint.y()
# print "assert (aLineBEndPoint.x() == %d)" % aLineBEndPoint.x()
# print "assert (aLineBEndPoint.y() == %d)" % aLineBEndPoint.y()
deltaX = deltaY = 5.
# move line without rotation,
# check that reference's line points are not changed also
aLineAStartPoint.setValue(aLineAStartPoint.x() + deltaX,
                          aLineAStartPoint.y() + deltaY)
aLineAEndPoint.setValue(aLineAEndPoint.x() + deltaX,
                        aLineAEndPoint.y() + deltaY)
# print "move line without rotation"
# print "assert (aLineAStartPoint.x() == %d)" % aLineAStartPoint.x()
# print "assert (aLineAStartPoint.y() == %d)" % aLineAStartPoint.y()
# print "assert (aLineAEndPoint.x() == %d)" % aLineAEndPoint.x()
# print "assert (aLineAEndPoint.y() == %d)" % aLineAEndPoint.y()
# print "assert (aLineBStartPoint.x() == %d)" % aLineBStartPoint.x()
# print "assert (aLineBStartPoint.y() == %d)" % aLineBStartPoint.y()
# print "assert (aLineBEndPoint.x() == %d)" % aLineBEndPoint.x()
# print "assert (aLineBEndPoint.y() == %d)" % aLineBEndPoint.y()

# rotate line, check that reference's line points are moved also
aLineAStartPoint.setValue(aLineAStartPoint.x() + deltaX,
                          aLineAStartPoint.y() + deltaY)
aLineAEndPoint.setValue(aLineAEndPoint.x() - deltaX,
                        aLineAEndPoint.y() - deltaY)
# print "Rotate line, check that reference's line points are moved also"
# print "assert (aLineAStartPoint.x() == %d)" % aLineAStartPoint.x()
# print "assert (aLineAStartPoint.y() == %d)" % aLineAStartPoint.y()
# print "assert (aLineAEndPoint.x() == %d)" % aLineAEndPoint.x()
# print "assert (aLineAEndPoint.y() == %d)" % aLineAEndPoint.y()
# print "assert (aLineBStartPoint.x() == %d)" % aLineBStartPoint.x()
# print "assert (aLineBStartPoint.y() == %d)" % aLineBStartPoint.y()
# print "assert (aLineBEndPoint.x() == %d)" % aLineBEndPoint.x()
# print "assert (aLineBEndPoint.y() == %d)" % aLineBEndPoint.y()
#=========================================================================
# End of test
#=========================================================================
