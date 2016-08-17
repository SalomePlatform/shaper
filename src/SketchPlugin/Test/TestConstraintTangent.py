"""
    TestConstraintTangent.py
    Unit test of SketchPlugin_ConstraintTangent class
        
    SketchPlugin_ConstraintTangent
        static const std::string MY_CONSTRAINT_TANGENT_ID("SketchConstraintTangent");
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());

"""
from GeomDataAPI import *
from GeomAPI import *
from ModelAPI import *
import math
#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2015-03-17"

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
aLen = math.sqrt(anArcVecX**2 + anArcVecY**2)
aLineVecX = aLine1EndPoint.x() - aLine1StartPoint.x()
aLineVecY = aLine1EndPoint.y() - aLine1StartPoint.y()
aLen = aLen * math.sqrt(aLineVecX**2 + aLineVecY**2)
aDot = anArcVecX * aLineVecX + anArcVecY * aLineVecY
assert math.fabs(aDot) <= 2.e-6 * aLen, "Observed dot product: {0}".format(aDot)
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
aLen = math.sqrt(anArcVecX**2 + anArcVecY**2)
aLineVecX = aLine2EndPoint.x() - aLine2StartPoint.x()
aLineVecY = aLine2EndPoint.y() - aLine2StartPoint.y()
aLen = aLen * math.sqrt(aLineVecX**2 + aLineVecY**2)
aDot = anArcVecX * aLineVecX + anArcVecY * aLineVecY
assert math.fabs(aDot) <= 2.e-6 * aLen, "Observed dot product: {0}".format(aDot)

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
aSketchArc2.string("ArcType").setValue("ThreePoints")
anArc2Centr = geomDataAPI_Point2D(aSketchArc2.attribute("ArcCenter"))
anArc2StartPoint = geomDataAPI_Point2D(aSketchArc2.attribute("ArcStartPoint"))
anArc2StartPoint.setValue(0., 50.)
anArc2EndPoint = geomDataAPI_Point2D(aSketchArc2.attribute("ArcEndPoint"))
anArc2EndPoint.setValue(-50., 0.)
anArc2PassedPoint = geomDataAPI_Point2D(aSketchArc2.attribute("ArcPassedPoint"))
anArc2PassedPoint.setValue(-40., 40.)
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
aLen = math.sqrt(anArc1VecX**2 + anArc1VecY**2)
anArc2VecX = anArc2StartPoint.x() - anArc2Centr.x()
anArc2VecY = anArc2StartPoint.y() - anArc2Centr.y()
aLen = aLen * math.sqrt(anArc2VecX**2 + anArc2VecY**2)
aCross = anArc1VecX * anArc2VecY - anArc1VecY * anArc2VecX
assert math.fabs(aCross) <= 2.e-6 * aLen, "Observed cross product: {0}".format(aCross)

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
aSession.startOperation()
aDocument.removeFeature(aTangency)
aSession.finishOperation()

#=========================================================================
# TEST 4. Creating of tangency arc by the option of the SketchArc feature
#=========================================================================
aSession.startOperation()
aSketchArc3 = aSketchFeature.addFeature("SketchArc")
aSketchArc3.string("ArcType").setValue("Tangent")
anArc3Start = aSketchArc3.refattr("ArcTangentPoint")
anArc3Start.setAttr(anArc1StartPoint)
anArc3EndPoint = geomDataAPI_Point2D(aSketchArc3.attribute("ArcEndPoint"))
anArc3EndPoint.setValue(100., 0.)
aSession.finishOperation()
anArc3Center = geomDataAPI_Point2D(aSketchArc2.attribute("ArcCenter"))
anArc3StartPoint = geomDataAPI_Point2D(aSketchArc2.attribute("ArcStartPoint"))

anArc1VecX = anArc1EndPoint.x() - anArc1Centr.x()
anArc1VecY = anArc1EndPoint.y() - anArc1Centr.y()
aLen = math.sqrt(anArc1VecX**2 + anArc1VecY**2)
anArc3VecX = anArc3StartPoint.x() - anArc3Center.x()
anArc3VecY = anArc3StartPoint.y() - anArc3Center.y()
aLen = aLen * math.sqrt(anArc3VecX**2 + anArc3VecY**2)
aCross = anArc1VecX * anArc3VecY - anArc1VecY * anArc3VecX
assert math.fabs(aCross) <= 2.e-6 * aLen, "Observed cross product: {0}".format(aCross)

#=========================================================================
# TEST 5. Creating of tangency between line and circle
#=========================================================================
aSession.startOperation()
aLine = aSketchFeature.addFeature("SketchLine")
aLineStart = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
aLineEnd = geomDataAPI_Point2D(aLine.attribute("EndPoint"))
aLineStart.setValue(100., 100.)
aLineEnd.setValue(200., 200.)
aCircle = aSketchFeature.addFeature("SketchCircle")
aCircleCenter = geomDataAPI_Point2D(aCircle.attribute("CircleCenter"))
aCircleRadius = aCircle.real("CircleRadius")
aCircleCenter.setValue(150., 100.)
aCircleRadius.setValue(20.)
aSession.finishOperation()

aSession.startOperation()
aTangency = aSketchFeature.addFeature("SketchConstraintTangent")
aRefObjectA = aTangency.refattr("ConstraintEntityA")
aRefObjectB = aTangency.refattr("ConstraintEntityB")
anObjectA = modelAPI_ResultConstruction(aLine.lastResult())
anObjectB = modelAPI_ResultConstruction(aCircle.lastResult())
assert (anObjectA is not None)
assert (anObjectB is not None)
aRefObjectA.setObject(anObjectA)
aRefObjectB.setObject(anObjectB)
aTangency.execute()
aSession.finishOperation()

assert(math.fabs(distancePointLine(aCircleCenter, aLine) - round(aCircleRadius.value(), 5)) < 1.e-10)
#=========================================================================
# End of test
#=========================================================================

import model
assert(model.checkPythonDump())
