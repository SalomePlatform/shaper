"""
    TestConstraintTangent.py
    Unit test of SketchPlugin_ConstraintTangent class
        
    SketchPlugin_ConstraintTangent
        static const std::string MY_CONSTRAINT_TANGENT_ID("SketchConstraintTangent");
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::type());

"""
from GeomDataAPI import *
from GeomAPI import *
from ModelAPI import *
import math
#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2015-03-17"

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
# Creation of an arc and a line
#=========================================================================
# Arc
aSession.startOperation()
aSketchArc1 = aSketchFeature.addFeature("SketchArc")
anArcCentr = geomDataAPI_Point2D(aSketchArc1.attribute("ArcCenter"))
anArcCentr.setValue(10., 10.)
anArcStartPoint = geomDataAPI_Point2D(aSketchArc1.attribute("ArcStartPoint"))
anArcStartPoint.setValue(0., 50.)
anArcEndPoint = geomDataAPI_Point2D(aSketchArc1.attribute("ArcEndPoint"))
anArcEndPoint.setValue(50., 0.)
aSession.finishOperation()
# Line
aSession.startOperation()
aSketchLine1 = aSketchFeature.addFeature("SketchLine")
aLineStartPoint = geomDataAPI_Point2D(aSketchLine1.attribute("StartPoint"))
aLineEndPoint = geomDataAPI_Point2D(aSketchLine1.attribute("EndPoint"))
aLineStartPoint.setValue(0., 50.)
aLineEndPoint.setValue(0., 100.)
aSession.finishOperation()
#=========================================================================
# Link arc start point and line's point by the coincidence constraint
#=========================================================================
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintCoincidence")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setAttr(anArcStartPoint)
reflistB.setAttr(aLineStartPoint)
aConstraint.execute()
aSession.finishOperation()
#=========================================================================
# Add tangency constraint and check correctness
#=========================================================================
aSession.startOperation()
aTangency = aSketchFeature.addFeature("SketchConstraintTangent")
aRefObjectA = aTangency.refattr("ConstraintEntityA")
aRefObjectB = aTangency.refattr("ConstraintEntityB")
anObjectA = modelAPI_ResultConstruction(aSketchArc1.firstResult())
anObjectB = modelAPI_ResultConstruction(aSketchLine1.firstResult())
assert (anObjectA is not None)
assert (anObjectB is not None)
aRefObjectA.setObject(anObjectA)
aRefObjectB.setObject(anObjectB)
aTangency.execute()
aSession.finishOperation()
anArcVecX = anArcStartPoint.x() - anArcCentr.x()
anArcVecY = anArcStartPoint.y() - anArcCentr.y()
aLineVecX = aLineEndPoint.x() - aLineStartPoint.x()
aLineVecY = aLineEndPoint.y() - aLineStartPoint.y()
aDot = anArcVecX * aLineVecX + anArcVecY * aLineVecY
print anArcVecX, anArcVecY
print aLineVecX, aLineVecY
print aDot
print aLineStartPoint.x(), aLineStartPoint.y(), aLineEndPoint.x(), aLineEndPoint.y()
assert(aDot == 0.)
#=========================================================================
# End of test
#=========================================================================
