"""
    TestConstraintEqual.py
    Unit test of SketchPlugin_ConstraintEqual class
        
    SketchPlugin_ConstraintEqual
        static const std::string MY_CONSTRAINT_EQAUL_ID("SketchConstraintEqual");
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::type());

"""
from GeomDataAPI import *
from ModelAPI import *
import math
#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2015-03-16"

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
anCircleCentr.setValue(-25., -25)
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
anArcVecX = anArcStartPoint.x() - anArcCentr.x();
anArcVecY = anArcStartPoint.y() - anArcCentr.y();
anArcRadius = math.sqrt(anArcVecX**2 + anArcVecY**2)
assert (math.fabs(aCircRadius - anArcRadius) <= 1.e-10)
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
aVecX = aLine1StartPoint.x() - aLine1EndPoint.x();
aVecY = aLine1StartPoint.y() - aLine1EndPoint.y();
aLine1Len = math.sqrt(anArcVecX * anArcVecX + anArcVecY * anArcVecY)
aVecX = aLine2StartPoint.x() - aLine2EndPoint.x();
aVecY = aLine2StartPoint.y() - aLine2EndPoint.y();
aLine2Len = math.sqrt(anArcVecX**2 + anArcVecY**2)
assert (aLine1Len == aLine2Len)
#=========================================================================
# End of test
#=========================================================================
