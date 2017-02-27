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
from salome.shaper import model

#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2016-01-29"
TOLERANCE = 1.e-7


#=========================================================================
# Auxiliary functions
#=========================================================================
def checkMiddlePoint(point, line):
    aStart = geomDataAPI_Point2D(line.attribute("StartPoint"))
    aEnd   = geomDataAPI_Point2D(line.attribute("EndPoint"))
    assert math.fabs(aStart.x() + aEnd.x() - 2.0 * point.x()) <= TOLERANCE, "x1={0}, x2={1}, mid={2}".format(aStart.x(), aEnd.x(), point.x())
    assert math.fabs(aStart.y() + aEnd.y() - 2.0 * point.y()) <= TOLERANCE, "y1={0}, y2={1}, mid={2}".format(aStart.y(), aEnd.y(), point.y())


#=========================================================================
# Start of test
#=========================================================================
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
# add an origin
aSession.startOperation()
aFeature = aDocument.addFeature("Point")
# aFeature.string("creation_method").setValue("by_xyz")
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
# Create a two lines
#=========================================================================
aSession.startOperation()
aLine1 = aSketchFeature.addFeature("SketchLine")
aStartPoint1 = geomDataAPI_Point2D(aLine1.attribute("StartPoint"))
aEndPoint1 = geomDataAPI_Point2D(aLine1.attribute("EndPoint"))
aStartPoint1.setValue(50., 0.)
aEndPoint1.setValue(100., 25.)
aLine2 = aSketchFeature.addFeature("SketchLine")
aStartPoint2 = geomDataAPI_Point2D(aLine2.attribute("StartPoint"))
aEndPoint2 = geomDataAPI_Point2D(aLine2.attribute("EndPoint"))
aStartPoint2.setValue(10., 100.)
aEndPoint2.setValue(100., 25.)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 8)
#=========================================================================
# Make end point of second line middle point on first line
#=========================================================================
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintMiddle")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setAttr(aEndPoint2)
reflistB.setObject(aLine1.lastResult())
aConstraint.execute()
aSession.finishOperation()
#=========================================================================
# Check error message (this message is not a error but a limitation of PlaneGCS)
# If the problem will be resolved, following block may be removed
#=========================================================================
assert aSketchFeature.string("SolverError").value() != "", "PlaneGCS limitation: if you see this message, then PlaneGCS has solved conflicting constraints when applying Middle constraint for the point out of the segment"
aSession.startOperation()
aDocument.removeFeature(aConstraint)
aSession.finishOperation()
aSession.startOperation()
aEndPoint2.setValue(80., 25.)
aConstraint = aSketchFeature.addFeature("SketchConstraintMiddle")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setAttr(aEndPoint2)
reflistB.setObject(aLine1.lastResult())
aConstraint.execute()
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 6)

#=========================================================================
# Check values and move one constrainted object
#=========================================================================
checkMiddlePoint(aEndPoint2, aLine1)
deltaX, deltaY = -80.0, -40.0
aSession.startOperation()
aStartPoint1.setValue(aStartPoint1.x() + deltaX, aStartPoint1.y() + deltaY)
aSession.finishOperation()
checkMiddlePoint(aEndPoint2, aLine1)
assert (model.dof(aSketchFeature) == 6)
#=========================================================================
# Remove constraint and move the line
#=========================================================================
aCurX, aCurY = aEndPoint2.x(), aEndPoint2.y()
aSession.startOperation()
aDocument.removeFeature(aConstraint)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 8)
aSession.startOperation()
aEndPoint1.setValue(90., 0.)
aSession.finishOperation()
assert (aEndPoint2.x() == aCurX and aEndPoint2.y() == aCurY)
assert (model.dof(aSketchFeature) == 8)

#=========================================================================
# Set external point as a middle point
#=========================================================================
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
assert (model.dof(aSketchFeature) == 8)
# middle point constraint
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintMiddle")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setObject(aLine2.lastResult())
reflistB.setObject(anOrigin.lastResult())
aSession.finishOperation()
checkMiddlePoint(anOriginCoord, aLine2)
assert (model.dof(aSketchFeature) == 6)
#=========================================================================
# Check origin coordinates does not changed
#=========================================================================
assert (anOriginCoord.x() == 0. and anOriginCoord.y() == 0.)

#=========================================================================
# Add other line with one extremity coincident to the first line
#=========================================================================
aSession.startOperation()
aLine3 = aSketchFeature.addFeature("SketchLine")
aStartPoint3 = geomDataAPI_Point2D(aLine3.attribute("StartPoint"))
aEndPoint3 = geomDataAPI_Point2D(aLine3.attribute("EndPoint"))
aStartPoint3.setValue(50., 50.)
aEndPoint3.setValue(50., 0.)
aCoincidence = aSketchFeature.addFeature("SketchConstraintCoincidence")
reflistA = aCoincidence.refattr("ConstraintEntityA")
reflistB = aCoincidence.refattr("ConstraintEntityB")
reflistA.setAttr(aEndPoint3)
reflistB.setObject(aLine1.lastResult())
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 9)
#=========================================================================
# Set Middle point
#=========================================================================
aSession.startOperation()
aMiddle = aSketchFeature.addFeature("SketchConstraintMiddle")
reflistA = aMiddle.refattr("ConstraintEntityA")
reflistB = aMiddle.refattr("ConstraintEntityB")
reflistA.setAttr(aEndPoint3)
reflistB.setObject(aLine1.lastResult())
aSession.finishOperation()
# check the point, and no error message
assert aSketchFeature.string("SolverError").value() == ""
assert (model.dof(aSketchFeature) == 8)
checkMiddlePoint(aEndPoint3, aLine1)
#=========================================================================
# Remove coincidence and move one line
#=========================================================================
aSession.startOperation()
aDocument.removeFeature(aCoincidence)
aSession.finishOperation()
deltaX, deltaY = 10.0, -10.0
aSession.startOperation()
aStartPoint1.setValue(aStartPoint1.x() + deltaX, aStartPoint1.y() + deltaY)
aEndPoint1.setValue(aEndPoint1.x() + deltaX, aEndPoint1.y() + deltaY)
aSession.finishOperation()
checkMiddlePoint(aEndPoint3, aLine1)
assert (model.dof(aSketchFeature) == 8)

#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
