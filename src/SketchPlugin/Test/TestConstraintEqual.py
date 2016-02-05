"""
    TestConstraintEqual.py
    Unit test of SketchPlugin_ConstraintEqual class
        
    SketchPlugin_ConstraintEqual
        static const std::string MY_CONSTRAINT_EQAUL_ID("SketchConstraintEqual");
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());

"""
from GeomDataAPI import *
from ModelAPI import *
import math
#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2015-03-16"

#=========================================================================
# Auxiliary functions
#=========================================================================
def externalSketch(theDoc):
    aSketchFeature = featureToCompositeFeature(theDoc.addFeature("Sketch"))
    origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
    origin.setValue(0, 0, 0)
    dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
    dirx.setValue(1, 0, 0)
    norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
    norm.setValue(0, 0, 1)
    # add circle defined by 3 points
    circle = aSketchFeature.addFeature("SketchCircle")
    circle.string("CircleType").setValue("ThreePoints")
    geomDataAPI_Point2D(circle.attribute("FirstPoint")).setValue(-40., 50.)
    geomDataAPI_Point2D(circle.attribute("SecondPoint")).setValue(-50., 60.)
    geomDataAPI_Point2D(circle.attribute("ThirdPoint")).setValue(-60., 50.)
    # add line
    line = aSketchFeature.addFeature("SketchLine")
    lineStart = geomDataAPI_Point2D(line.attribute("StartPoint"))
    lineEnd = geomDataAPI_Point2D(line.attribute("EndPoint"))
    lineStart.setValue(-40., 35.)
    lineEnd.setValue(-60., 25.)

def lineLength(theLine):
    aLineStart = geomDataAPI_Point2D(theLine.attribute("StartPoint"))
    aLineEnd = geomDataAPI_Point2D(theLine.attribute("EndPoint"))
    aVecX = aLineStart.x() - aLineEnd.x()
    aVecY = aLineStart.y() - aLineEnd.y()
    return math.hypot(aVecX, aVecY)


#=========================================================================
# Start of test
#=========================================================================
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
#=========================================================================
# Creation external sketch
#=========================================================================
aSession.startOperation()
externalSketch(aDocument)
aSession.finishOperation()
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
# Creation of an arc and a circle
#=========================================================================
aSession.startOperation()
aSketchArc = aSketchFeature.addFeature("SketchArc")
anArcCentr = geomDataAPI_Point2D(aSketchArc.attribute("ArcCenter"))
anArcCentr.setValue(10., 10.)
anArcStartPoint = geomDataAPI_Point2D(aSketchArc.attribute("ArcStartPoint"))
anArcStartPoint.setValue(0., 50.)
anArcEndPoint = geomDataAPI_Point2D(aSketchArc.attribute("ArcEndPoint"))
anArcEndPoint.setValue(50., 0.)
aSession.finishOperation()
# Circle
aSession.startOperation()
aSketchCircle = aSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
aCircleRadius = aSketchCircle.real("CircleRadius")
anCircleCentr.setValue(-25., -25.)
aCircleRadius.setValue(25.)
aSession.finishOperation()
#=========================================================================
# A constraint to make equal radii of arc and circle
#=========================================================================
aSession.startOperation()
aConstraintEqRad = aSketchFeature.addFeature("SketchConstraintEqual")
aRefObjectA = aConstraintEqRad.refattr("ConstraintEntityA")
aRefObjectB = aConstraintEqRad.refattr("ConstraintEntityB")
aResultA = modelAPI_ResultConstruction(aSketchArc.lastResult())
aResultB = modelAPI_ResultConstruction(aSketchCircle.lastResult())
assert (aResultA is not None)
assert (aResultB is not None)
aRefObjectA.setObject(aResultA)
aRefObjectB.setObject(aResultB)
aConstraintEqRad.execute()
aSession.finishOperation()
aCircRadius = aCircleRadius.value();
anArcVecX = anArcStartPoint.x() - anArcCentr.x()
anArcVecY = anArcStartPoint.y() - anArcCentr.y()
anArcRadius = math.sqrt(anArcVecX**2 + anArcVecY**2)
assert (math.fabs(aCircRadius - anArcRadius) <= 1.e-10)
#=========================================================================
# A constraint to make equal radii of arc and external circle
#=========================================================================
aSession.startOperation()
anExtCircRes = modelAPI_Result(aDocument.objectByName("Construction", "SketchCircle_1"))
assert(anExtCircRes)
anExtCircShape = anExtCircRes.shape()
assert(anExtCircShape)
anExtCircle = aSketchFeature.addFeature("SketchCircle")
anExtCircleCenter = geomDataAPI_Point2D(anExtCircle.attribute("CircleCenter"))
anExtCircleRadius = anExtCircle.real("CircleRadius")
anExtCircleCenter.setValue(-50., 50.)
anExtCircleRadius.setValue(10.)
anExtCircle.selection("External").setValue(anExtCircRes, anExtCircShape)
aSession.finishOperation()
aSession.startOperation()
aConstraintEqRad2 = aSketchFeature.addFeature("SketchConstraintEqual")
aRefObjectA = aConstraintEqRad2.refattr("ConstraintEntityA")
aRefObjectB = aConstraintEqRad2.refattr("ConstraintEntityB")
aRefObjectA.setObject(aSketchCircle.lastResult())
aRefObjectB.setObject(anExtCircle.lastResult())
aSession.finishOperation()
assert (math.fabs(anExtCircleRadius.value() - aCircleRadius.value()) < 1.e-10)
anArcVecX = anArcStartPoint.x() - anArcCentr.x()
anArcVecY = anArcStartPoint.y() - anArcCentr.y()
anArcRadius = math.sqrt(anArcVecX**2 + anArcVecY**2)
assert (math.fabs(anExtCircleRadius.value() - anArcRadius) <= 1.e-10)

#=========================================================================
# Creation of two different lines
#=========================================================================
# First Line
aSession.startOperation()
aSketchLine1 = aSketchFeature.addFeature("SketchLine")
aLine1StartPoint = geomDataAPI_Point2D(aSketchLine1.attribute("StartPoint"))
aLine1EndPoint = geomDataAPI_Point2D(aSketchLine1.attribute("EndPoint"))
aLine1StartPoint.setValue(0., 15.)
aLine1EndPoint.setValue(20., 25.)
aSession.finishOperation()
# Second Line
aSession.startOperation()
aSketchLine2 = aSketchFeature.addFeature("SketchLine")
aLine2StartPoint = geomDataAPI_Point2D(aSketchLine2.attribute("StartPoint"))
aLine2EndPoint = geomDataAPI_Point2D(aSketchLine2.attribute("EndPoint"))
aLine2StartPoint.setValue(0., 0.)
aLine2EndPoint.setValue(-1., 10.)
aSession.finishOperation()
#=========================================================================
# A constraint to make equal lengths of lines
#=========================================================================
aSession.startOperation()
aConstraintEqLen = aSketchFeature.addFeature("SketchConstraintEqual")
aRefObjectA = aConstraintEqLen.refattr("ConstraintEntityA")
aRefObjectB = aConstraintEqLen.refattr("ConstraintEntityB")
aResultA = modelAPI_ResultConstruction(aSketchLine1.firstResult())
aResultB = modelAPI_ResultConstruction(aSketchLine2.firstResult())
assert (aResultA is not None)
assert (aResultB is not None)
aRefObjectA.setObject(aResultA)
aRefObjectB.setObject(aResultB)
aConstraintEqLen.execute()
aSession.finishOperation()
aLine1Len = lineLength(aSketchLine1)
aLine2Len = lineLength(aSketchLine2)
assert (math.fabs(aLine1Len - aLine2Len) < 1.e-10)
#=========================================================================
# A constraint to make equal length of line with external line
#=========================================================================
aSession.startOperation()
anExtLineRes = modelAPI_Result(aDocument.objectByName("Construction", "SketchLine_1"))
assert(anExtLineRes)
anExtLineShape = anExtLineRes.shape()
assert(anExtLineShape)
anExtLine = aSketchFeature.addFeature("SketchLine")
anExtLineStart = geomDataAPI_Point2D(anExtLine.attribute("StartPoint"))
anExtLineEnd = geomDataAPI_Point2D(anExtLine.attribute("EndPoint"))
anExtLineStart.setValue(-40., 35.)
anExtLineEnd.setValue(-60., 25.)
anExtLine.selection("External").setValue(anExtLineRes, anExtLineShape)
anExtLineLen = lineLength(anExtLine)
aSession.finishOperation()
aSession.startOperation()
aConstraintEqLen2 = aSketchFeature.addFeature("SketchConstraintEqual")
aRefObjectA = aConstraintEqLen2.refattr("ConstraintEntityA")
aRefObjectB = aConstraintEqLen2.refattr("ConstraintEntityB")
aRefObjectA.setObject(anExtLine.lastResult())
aRefObjectB.setObject(aSketchLine2.lastResult())
aSession.finishOperation()
aLine1Len = lineLength(aSketchLine1)
aLine2Len = lineLength(aSketchLine2)
assert (math.fabs(aLine1Len - anExtLineLen) < 1.e-10)
assert (math.fabs(aLine2Len - anExtLineLen) < 1.e-10)
#=========================================================================
# End of test
#=========================================================================
