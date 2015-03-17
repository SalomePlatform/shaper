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
# TEST 1. Arc-line tangency
#=========================================================================
# Creation of an arc and two lines
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
# Line 1
aSession.startOperation()
aSketchLine1 = aSketchFeature.addFeature("SketchLine")
aLine1StartPoint = geomDataAPI_Point2D(aSketchLine1.attribute("StartPoint"))
aLine1EndPoint = geomDataAPI_Point2D(aSketchLine1.attribute("EndPoint"))
aLine1StartPoint.setValue(0., 50.)
aLine1EndPoint.setValue(0., 100.)
aSession.finishOperation()
# Line 2
aSession.startOperation()
aSketchLine2 = aSketchFeature.addFeature("SketchLine")
aLine2StartPoint = geomDataAPI_Point2D(aSketchLine2.attribute("StartPoint"))
aLine2EndPoint = geomDataAPI_Point2D(aSketchLine2.attribute("EndPoint"))
aLine2StartPoint.setValue(50., 0.)
aLine2EndPoint.setValue(100., 0.)
aSession.finishOperation()
#=========================================================================
# Link arc points and lines points by the coincidence constraint
#=========================================================================
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintCoincidence")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setAttr(anArcStartPoint)
reflistB.setAttr(aLine1StartPoint)
aConstraint.execute()
aSession.finishOperation()
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintCoincidence")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setAttr(anArcEndPoint)
reflistB.setAttr(aLine2StartPoint)
aConstraint.execute()
aSession.finishOperation()
#=========================================================================
# Add tangency constraint and check correctness
#=========================================================================
aSession.startOperation()
aTangency = aSketchFeature.addFeature("SketchConstraintTangent")
aRefObjectA = aTangency.refattr("ConstraintEntityA")
aRefObjectB = aTangency.refattr("ConstraintEntityB")
anObjectA = modelAPI_ResultConstruction(aSketchArc1.lastResult())
anObjectB = modelAPI_ResultConstruction(aSketchLine1.firstResult())
assert (anObjectA is not None)
assert (anObjectB is not None)
aRefObjectA.setObject(anObjectA)
aRefObjectB.setObject(anObjectB)
aTangency.execute()
aSession.finishOperation()
anArcVecX = anArcStartPoint.x() - anArcCentr.x()
anArcVecY = anArcStartPoint.y() - anArcCentr.y()
aLineVecX = aLine1EndPoint.x() - aLine1StartPoint.x()
aLineVecY = aLine1EndPoint.y() - aLine1StartPoint.y()
aDot = anArcVecX * aLineVecX + anArcVecY * aLineVecY
assert(math.fabs(aDot) <= 1.e-12)
#=========================================================================
# Add tangency constraint for arc and second line and check correctness
#=========================================================================
aSession.startOperation()
aTangency = aSketchFeature.addFeature("SketchConstraintTangent")
aRefObjectA = aTangency.refattr("ConstraintEntityA")
aRefObjectB = aTangency.refattr("ConstraintEntityB")
anObjectA = modelAPI_ResultConstruction(aSketchArc1.lastResult())
anObjectB = modelAPI_ResultConstruction(aSketchLine2.firstResult())
assert (anObjectA is not None)
assert (anObjectB is not None)
aRefObjectA.setObject(anObjectA)
aRefObjectB.setObject(anObjectB)
aTangency.execute()
aSession.finishOperation()
anArcVecX = anArcEndPoint.x() - anArcCentr.x()
anArcVecY = anArcEndPoint.y() - anArcCentr.y()
aLineVecX = aLine2EndPoint.x() - aLine2StartPoint.x()
aLineVecY = aLine2EndPoint.y() - aLine2StartPoint.y()
aDot = anArcVecX * aLineVecX + anArcVecY * aLineVecY
assert(math.fabs(aDot) <= 1.e-12)

#=========================================================================
# TEST 2. Arc-arc tangency
#=========================================================================
# Creation of arcs
#=========================================================================
# Arc 1
aSession.startOperation()
aSketchArc1 = aSketchFeature.addFeature("SketchArc")
anArc1Centr = geomDataAPI_Point2D(aSketchArc1.attribute("ArcCenter"))
anArc1Centr.setValue(10., 10.)
anArc1StartPoint = geomDataAPI_Point2D(aSketchArc1.attribute("ArcStartPoint"))
anArc1StartPoint.setValue(50., 0.)
anArc1EndPoint = geomDataAPI_Point2D(aSketchArc1.attribute("ArcEndPoint"))
anArc1EndPoint.setValue(0., 50.)
aSession.finishOperation()
# Arc 2
aSession.startOperation()
aSketchArc2 = aSketchFeature.addFeature("SketchArc")
anArc2Centr = geomDataAPI_Point2D(aSketchArc2.attribute("ArcCenter"))
anArc2Centr.setValue(-10., 10.)
anArc2StartPoint = geomDataAPI_Point2D(aSketchArc2.attribute("ArcStartPoint"))
anArc2StartPoint.setValue(0., 50.)
anArc2EndPoint = geomDataAPI_Point2D(aSketchArc2.attribute("ArcEndPoint"))
anArc2EndPoint.setValue(-50., 0.)
aSession.finishOperation()
#=========================================================================
# Link points of arcs by the coincidence constraint
#=========================================================================
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintCoincidence")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setAttr(anArc1EndPoint)
reflistB.setAttr(anArc2StartPoint)
aConstraint.execute()
aSession.finishOperation()
#=========================================================================
# Add tangency constraint and check correctness
#=========================================================================
aSession.startOperation()
aTangency = aSketchFeature.addFeature("SketchConstraintTangent")
aRefObjectA = aTangency.refattr("ConstraintEntityA")
aRefObjectB = aTangency.refattr("ConstraintEntityB")
anObjectA = modelAPI_ResultConstruction(aSketchArc1.lastResult())
anObjectB = modelAPI_ResultConstruction(aSketchArc2.lastResult())
assert (anObjectA is not None)
assert (anObjectB is not None)
aRefObjectA.setObject(anObjectA)
aRefObjectB.setObject(anObjectB)
aTangency.execute()
aSession.finishOperation()
anArc1VecX = anArc1EndPoint.x() - anArc1Centr.x()
anArc1VecY = anArc1EndPoint.y() - anArc1Centr.y()
anArc2VecX = anArc2StartPoint.x() - anArc2Centr.x()
anArc2VecY = anArc2StartPoint.y() - anArc2Centr.y()
aCross = anArc1VecX * anArc2VecY - anArc1VecY * anArc2VecX
assert(math.fabs(aCross) <= 1.e-12)

#=========================================================================
# TEST 3. Tangency between non-connected objects should be wrong
#=========================================================================
# Store data
aLine2StartPointPrev = (aLine2StartPoint.x(), aLine2StartPoint.y())
aLine2EndPointPrev = (aLine2EndPoint.x(), aLine2EndPoint.y())
anArc2CenterPrev = (anArc2Centr.x(), anArc2Centr.y())
anArc2StartPointPrev = (anArc2StartPoint.x(), anArc2StartPoint.y())
anArc2EndPointPrev = (anArc2EndPoint.x(), anArc2EndPoint.y())
#=========================================================================
# Add tangency between arc2 and line2
#=========================================================================
aSession.startOperation()
aTangency = aSketchFeature.addFeature("SketchConstraintTangent")
aRefObjectA = aTangency.refattr("ConstraintEntityA")
aRefObjectB = aTangency.refattr("ConstraintEntityB")
anObjectA = modelAPI_ResultConstruction(aSketchArc2.lastResult())
anObjectB = modelAPI_ResultConstruction(aSketchLine2.firstResult())
assert (anObjectA is not None)
assert (anObjectB is not None)
aRefObjectA.setObject(anObjectA)
aRefObjectB.setObject(anObjectB)
aTangency.execute()
aSession.finishOperation()
# Check that nothing is changed
aLine2StartPointNew = (aLine2StartPoint.x(), aLine2StartPoint.y())
aLine2EndPointNew = (aLine2EndPoint.x(), aLine2EndPoint.y())
anArc2CenterNew = (anArc2Centr.x(), anArc2Centr.y())
anArc2StartPointNew = (anArc2StartPoint.x(), anArc2StartPoint.y())
anArc2EndPointNew = (anArc2EndPoint.x(), anArc2EndPoint.y())
assert(aLine2StartPointNew == aLine2StartPointPrev)
assert(aLine2EndPointNew == aLine2EndPointPrev)
assert(anArc2CenterNew == anArc2CenterPrev)
assert(anArc2StartPointNew == anArc2StartPointPrev)
assert(anArc2EndPointNew == anArc2EndPointPrev)
#=========================================================================
# End of test
#=========================================================================
