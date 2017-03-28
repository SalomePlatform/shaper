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
from salome.shaper import model

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

def checkArcLineTangency(theArc, theLine):
    """
    subroutine to check that the line is tangent to arc/circle
    """
    if (theArc.getKind() == "SketchCircle"):
        aCenter = geomDataAPI_Point2D(theArc.attribute("circle_center"))
        aRadius = theArc.real("circle_radius").value()
    else:
        aCenter = geomDataAPI_Point2D(theArc.attribute("center_point"))
        aStartPnt = geomDataAPI_Point2D(theArc.attribute("start_point"))
        aRadius = model.distancePointPoint(aStartPnt, aCenter)
    aDist = distancePointLine(aCenter, theLine)
    assert math.fabs(aDist - aRadius) < 2.e-5, "aDist = {0}, aRadius = {1}".format(aDist, aRadius)

def checkArcArcTangency(theArc1, theArc2):
    """
    subroutine to check that arcs/circles arc tangent
    """
    anArcs = [theArc1, theArc2]
    aCenters = []
    aRadii = []
    for anArc in anArcs:
        if (anArc.getKind() == "SketchCircle"):
            aCenter = geomDataAPI_Point2D(anArc.attribute("circle_center"))
            aRadius = anArc.real("circle_radius").value()
        else:
            aCenter = geomDataAPI_Point2D(anArc.attribute("center_point"))
            aStartPnt = geomDataAPI_Point2D(anArc.attribute("start_point"))
            aRadius = model.distancePointPoint(aStartPnt, aCenter)
        aCenters.append(aCenter)
        aRadii.append(aRadius)
    aDist = model.distancePointPoint(aCenters[0], aCenters[1])
    aRSum = aRadii[0] + aRadii[1]
    aRDiff = math.fabs(aRadii[0] - aRadii[1])
    assert math.fabs(aDist - aRSum) < 2.e-5 or math.fabs(aDist - aRDiff) < 2.e-5, "aDist = {0}, aRSum = {1}, aRDiff = {2}".format(aDist, aRSum, aRDiff)


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
anArcCentr = geomDataAPI_Point2D(aSketchArc1.attribute("center_point"))
anArcCentr.setValue(10., 10.)
anArcStartPoint = geomDataAPI_Point2D(aSketchArc1.attribute("start_point"))
anArcStartPoint.setValue(0., 50.)
anArcEndPoint = geomDataAPI_Point2D(aSketchArc1.attribute("end_point"))
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
assert (model.dof(aSketchFeature) == 13)
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
assert (model.dof(aSketchFeature) == 9)
#=========================================================================
# Add tangency constraint and check correctness
#=========================================================================
aSession.startOperation()
aTangency = aSketchFeature.addFeature("SketchConstraintTangent")
aRefObjectA = aTangency.refattr("ConstraintEntityA")
aRefObjectB = aTangency.refattr("ConstraintEntityB")
anObjectA = modelAPI_ResultConstruction(aSketchArc1.lastResult())
anObjectB = modelAPI_ResultConstruction(aSketchLine1.lastResult())
assert (anObjectA is not None)
assert (anObjectB is not None)
aRefObjectA.setObject(anObjectA)
aRefObjectB.setObject(anObjectB)
aTangency.execute()
aSession.finishOperation()
checkArcLineTangency(aSketchArc1, aSketchLine1)
assert (model.dof(aSketchFeature) == 8)
#=========================================================================
# Add tangency constraint for arc and second line and check correctness
#=========================================================================
aSession.startOperation()
aTangency = aSketchFeature.addFeature("SketchConstraintTangent")
aRefObjectA = aTangency.refattr("ConstraintEntityA")
aRefObjectB = aTangency.refattr("ConstraintEntityB")
anObjectA = modelAPI_ResultConstruction(aSketchArc1.lastResult())
anObjectB = modelAPI_ResultConstruction(aSketchLine2.lastResult())
assert (anObjectA is not None)
assert (anObjectB is not None)
aRefObjectA.setObject(anObjectA)
aRefObjectB.setObject(anObjectB)
aTangency.execute()
aSession.finishOperation()
checkArcLineTangency(aSketchArc1, aSketchLine2)
assert (model.dof(aSketchFeature) == 7)

#=========================================================================
# TEST 2. Arc-arc tangency
#=========================================================================
# Creation of arcs
#=========================================================================
# Arc 1
aSession.startOperation()
aSketchArc1 = aSketchFeature.addFeature("SketchArc")
anArc1Centr = geomDataAPI_Point2D(aSketchArc1.attribute("center_point"))
anArc1Centr.setValue(10., 10.)
anArc1StartPoint = geomDataAPI_Point2D(aSketchArc1.attribute("start_point"))
anArc1StartPoint.setValue(50., 0.)
anArc1EndPoint = geomDataAPI_Point2D(aSketchArc1.attribute("end_point"))
anArc1EndPoint.setValue(0., 50.)
aSession.finishOperation()
# Arc 2
aSession.startOperation()
aSketchArc2 = aSketchFeature.addFeature("SketchMacroArc")
aSketchArc2.string("arc_type").setValue("by_three_points")
anArc2StartPoint = geomDataAPI_Point2D(aSketchArc2.attribute("start_point_2"))
anArc2StartPoint.setValue(0., 50.)
anArc2EndPoint = geomDataAPI_Point2D(aSketchArc2.attribute("end_point_2"))
anArc2EndPoint.setValue(-50., 0.)
anArc2PassedPoint = geomDataAPI_Point2D(aSketchArc2.attribute("passed_point"))
anArc2PassedPoint.setValue(-40., 40.)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 17)
aSketchArc2 = model.lastSubFeature(aSketchFeature, "SketchArc")
anArc2Centr = geomDataAPI_Point2D(aSketchArc2.attribute("center_point"))
anArc2StartPoint = geomDataAPI_Point2D(aSketchArc2.attribute("start_point"))
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
assert (model.dof(aSketchFeature) == 15)
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
checkArcArcTangency(aSketchArc1, aSketchArc2)
assert (model.dof(aSketchFeature) == 14)

#=========================================================================
# TEST 3. Tangency between non-connected objects should work
#=========================================================================
# 3.1 tangency between arc2 and line2
aSession.startOperation()
aTangency = aSketchFeature.addFeature("SketchConstraintTangent")
aRefObjectA = aTangency.refattr("ConstraintEntityA")
aRefObjectB = aTangency.refattr("ConstraintEntityB")
anObjectA = modelAPI_ResultConstruction(aSketchArc2.lastResult())
anObjectB = modelAPI_ResultConstruction(aSketchLine2.lastResult())
assert (anObjectA is not None)
assert (anObjectB is not None)
aRefObjectA.setObject(anObjectA)
aRefObjectB.setObject(anObjectB)
aTangency.execute()
aSession.finishOperation()
checkArcLineTangency(aSketchArc2, aSketchLine2)
assert (model.dof(aSketchFeature) == 13)

aSession.startOperation()
aDocument.removeFeature(aTangency)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 14)

# 3.2  tangency between non-connected arcs
aSession.startOperation()
aSketchArc3 = aSketchFeature.addFeature("SketchArc")
anArc3Centr = geomDataAPI_Point2D(aSketchArc3.attribute("center_point"))
anArc3Centr.setValue(100., -10.)
anArc3StartPoint = geomDataAPI_Point2D(aSketchArc3.attribute("start_point"))
anArc3StartPoint.setValue(70., -10.)
anArc3EndPoint = geomDataAPI_Point2D(aSketchArc3.attribute("end_point"))
anArc3EndPoint.setValue(100., 20.)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 19)

aSession.startOperation()
aTangency = aSketchFeature.addFeature("SketchConstraintTangent")
aTangency.refattr("ConstraintEntityA").setObject(modelAPI_ResultConstruction(aSketchArc2.lastResult()))
aTangency.refattr("ConstraintEntityB").setObject(modelAPI_ResultConstruction(aSketchArc3.lastResult()))
aSession.finishOperation()
checkArcArcTangency(aSketchArc2, aSketchArc3)
assert (model.dof(aSketchFeature) == 18)

aSession.startOperation()
aDocument.removeFeature(aSketchArc3)
aDocument.removeFeature(aTangency)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 14)

# 3.3  tangency between arc and circle
aSession.startOperation()
aCircle1 = aSketchFeature.addFeature("SketchCircle")
aCircleCenter = geomDataAPI_Point2D(aCircle1.attribute("circle_center"))
aCircleRadius = aCircle1.real("circle_radius")
aCircleCenter.setValue(150., 100.)
aCircleRadius.setValue(50.)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 17)

aSession.startOperation()
aTangency1 = aSketchFeature.addFeature("SketchConstraintTangent")
aTangency1.refattr("ConstraintEntityA").setObject(modelAPI_ResultConstruction(aSketchArc2.lastResult()))
aTangency1.refattr("ConstraintEntityB").setObject(modelAPI_ResultConstruction(aCircle1.lastResult()))
aSession.finishOperation()
checkArcArcTangency(aSketchArc2, aCircle1)
assert (model.dof(aSketchFeature) == 16)

# 3.4  tangency between two circles
aSession.startOperation()
aCircle2 = aSketchFeature.addFeature("SketchCircle")
aCircleCenter = geomDataAPI_Point2D(aCircle2.attribute("circle_center"))
aCircleRadius = aCircle2.real("circle_radius")
aCircleCenter.setValue(120., 70.)
aCircleRadius.setValue(20.)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 19)

aSession.startOperation()
aTangency2 = aSketchFeature.addFeature("SketchConstraintTangent")
aTangency2.refattr("ConstraintEntityA").setObject(modelAPI_ResultConstruction(aCircle1.lastResult()))
aTangency2.refattr("ConstraintEntityB").setObject(modelAPI_ResultConstruction(aCircle2.lastResult()))
aSession.finishOperation()
checkArcArcTangency(aCircle1, aCircle2)
assert (model.dof(aSketchFeature) == 18)

#=========================================================================
# TEST 4. Creating of tangency arc by the option of the SketchArc feature
#=========================================================================
aSession.startOperation()
aSketchArc3 = aSketchFeature.addFeature("SketchMacroArc")
aSketchArc3.string("arc_type").setValue("by_tangent_edge")
anArc3Start = aSketchArc3.refattr("tangent_point")
anArc3Start.setAttr(anArc1StartPoint)
anArc3EndPoint = geomDataAPI_Point2D(aSketchArc3.attribute("end_point_3"))
anArc3EndPoint.setValue(anArc1StartPoint.x()-5, anArc1StartPoint.y()-30)
aSession.finishOperation()
aSketchArc3 = model.lastSubFeature(aSketchFeature, "SketchArc")
checkArcArcTangency(aSketchArc1, aSketchArc3)
# freeze radius of tangent arc
aSession.startOperation()
aConstraintRadius = aSketchFeature.addFeature("SketchConstraintRadius")
aConstraintRadius.refattr("ConstraintEntityA").setObject(modelAPI_ResultConstruction(aSketchArc3.lastResult()))
aConstraintRadius.real("ConstraintValue").setValue(30.)
aSession.finishOperation()
checkArcArcTangency(aSketchArc1, aSketchArc3)
# do not check DoF here because it is unstable for tangent arcs,
# remove tangent arc to avoid instability while dumping
aSession.startOperation()
ModelAPI.removeFeaturesAndReferences(FeatureSet([aSketchArc3]))
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 18)

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
aCircleCenter = geomDataAPI_Point2D(aCircle.attribute("circle_center"))
aCircleRadius = aCircle.real("circle_radius")
aCircleCenter.setValue(150., 100.)
aCircleRadius.setValue(20.)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 25)

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

checkArcLineTangency(aCircle, aLine)
assert (model.dof(aSketchFeature) == 24)
#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
