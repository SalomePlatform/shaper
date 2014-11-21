"""
    TestConstraintRigid.py
    Unit test of SketchPlugin_ConstraintRigid class
    
    SketchPlugin_Constraint
        static const std::string MY_CONSTRAINT_VALUE("ConstraintValue");
        static const std::string MY_FLYOUT_VALUE_PNT("ConstraintFlyoutValuePnt");
        static const std::string MY_ENTITY_A("ConstraintEntityA");
        static const std::string MY_ENTITY_B("ConstraintEntityB");
        static const std::string MY_ENTITY_C("ConstraintEntityC");
        static const std::string MY_ENTITY_D("ConstraintEntityD");
        
    SketchPlugin_ConstraintRigid
        static const std::string MY_CONSTRAINT_RIGID_ID("SketchConstraintRigid");
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());

"""
from GeomDataAPI import *
from ModelAPI import *
from GeomAPI import *
#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2014-11-21"

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
# Create a triangle ABC
#=========================================================================
aSession.startOperation()
aSketchLineA = aSketchFeature.addFeature("SketchLine")
aSketchLineB = aSketchFeature.addFeature("SketchLine")
aSketchLineC = aSketchFeature.addFeature("SketchLine")
aLineAStartPoint = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(aSketchLineA.attribute("EndPoint"))
aLineBStartPoint = geomDataAPI_Point2D(aSketchLineB.attribute("StartPoint"))
aLineBEndPoint = geomDataAPI_Point2D(aSketchLineB.attribute("EndPoint"))
aLineCStartPoint = geomDataAPI_Point2D(aSketchLineC.attribute("StartPoint"))
aLineCEndPoint = geomDataAPI_Point2D(aSketchLineC.attribute("EndPoint"))
aLineAStartPoint.setValue(25., 25.)
aLineAEndPoint.setValue(100., 25.)
aLineBStartPoint.setValue(100., 25.)
aLineBEndPoint.setValue(60., 75.)
aLineCStartPoint.setValue(60., 75.)
aLineCEndPoint.setValue(25., 25.)
aSession.finishOperation()
# Store initial values of lines
kLineAStart = (aLineAStartPoint.x(), aLineAStartPoint.y())
kLineAEnd = (aLineAEndPoint.x(),   aLineAEndPoint.y())
kLineBStart = (aLineBStartPoint.x(), aLineBStartPoint.y())
kLineBEnd = (aLineBEndPoint.x(),   aLineBEndPoint.y())
kLineCStart = (aLineCStartPoint.x(), aLineCStartPoint.y())
kLineCEnd = (aLineCEndPoint.x(),   aLineCEndPoint.y())
#=========================================================================
# Link triange lines with concidence
#=========================================================================
concidenceLinks = zip([aLineBStartPoint, aLineCStartPoint, aLineAStartPoint],
                      [aLineAEndPoint, aLineBEndPoint, aLineCEndPoint])
aSession.startOperation()
for eachLink in concidenceLinks:
    aConstraint = aSketchFeature.addFeature("SketchConstraintCoincidence")
    reflistA = aConstraint.refattr("ConstraintEntityA")
    reflistB = aConstraint.refattr("ConstraintEntityB")
    reflistA.setAttr(eachLink[0])
    reflistB.setAttr(eachLink[1])
    aConstraint.execute()
aSession.finishOperation()

# Check that constarints doesn't affected lines' values
assert (kLineAStart == (aLineAStartPoint.x(), aLineAStartPoint.y()))
assert (kLineAEnd == (aLineAEndPoint.x(),   aLineAEndPoint.y()))
assert (kLineBStart == (aLineBStartPoint.x(), aLineBStartPoint.y()))
assert (kLineBEnd == (aLineBEndPoint.x(),   aLineBEndPoint.y()))
assert (kLineCStart == (aLineCStartPoint.x(), aLineCStartPoint.y()))
assert (kLineCEnd == (aLineCEndPoint.x(),   aLineCEndPoint.y()))
#=========================================================================
# Make line A rigid
#=========================================================================
aSession.startOperation()
aRigidConstraint = aSketchFeature.addFeature("SketchConstraintRigid")
eachRefattr = aRigidConstraint.refattr("ConstraintEntityA")
lineResults = aSketchLineA.results()
for eachResult in lineResults:
    if eachResult.shape().isEdge():
        break
aResult = modelAPI_ResultConstruction(eachResult)
assert (aResult is not None)
eachRefattr.setObject(aResult)
# aRigidConstraint.execute()
aSession.finishOperation()

# Check that constarints doesn't affected lines' values
assert (kLineAStart == (aLineAStartPoint.x(), aLineAStartPoint.y()))
assert (kLineAEnd == (aLineAEndPoint.x(),   aLineAEndPoint.y()))
assert (kLineBStart == (aLineBStartPoint.x(), aLineBStartPoint.y()))
assert (kLineBEnd == (aLineBEndPoint.x(),   aLineBEndPoint.y()))
assert (kLineCStart == (aLineCStartPoint.x(), aLineCStartPoint.y()))
assert (kLineCEnd == (aLineCEndPoint.x(),   aLineCEndPoint.y()))
#=========================================================================
# Check that
#=========================================================================
aSession.startOperation()
aLineBEndPoint.setValue(90., 150.)
aSession.finishOperation()
# Check that constarint keep features' values
assert (kLineAStart == (aLineAStartPoint.x(), aLineAStartPoint.y()))
assert (kLineAEnd == (aLineAEndPoint.x(),   aLineAEndPoint.y()))
assert (kLineBStart == (aLineBStartPoint.x(), aLineBStartPoint.y()))
assert (kLineBEnd != (aLineBEndPoint.x(),   aLineBEndPoint.y()))
assert (kLineCStart != (aLineCStartPoint.x(), aLineCStartPoint.y()))
assert (kLineCEnd == (aLineCEndPoint.x(),   aLineCEndPoint.y()))
#=========================================================================
# TODO: improve test
# 1. remove constraint, move line to check that constraint are not applied
#=========================================================================
#=========================================================================
# End of test
#=========================================================================
