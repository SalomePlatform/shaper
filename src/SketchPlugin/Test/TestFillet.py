"""
    TestFillet.py
    Unit test of SketchPlugin_ConstraintFillet class
        
    SketchPlugin_ConstraintFillet
        static const std::string MY_CONSTRAINT_FILLET_ID("SketchConstraintFillet");
        data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_C(), ModelAPI_AttributeRefList::typeId());

"""
from GeomDataAPI import *
from ModelAPI import *
import math

#=========================================================================
# Auxiliary functions
#=========================================================================
def createSketch(theSketch):
    # Initialize sketch by two lines with coincident boundary
    allFeatures = []
    # Line1
    aSketchLine1 = theSketch.addFeature("SketchLine")
    aStartPoint1 = geomDataAPI_Point2D(aSketchLine1.attribute("StartPoint"))
    aEndPoint1   = geomDataAPI_Point2D(aSketchLine1.attribute("EndPoint"))
    aStartPoint1.setValue(10., 5.)
    aEndPoint1.setValue(-10., 10.)
    allFeatures.append(aSketchLine1)
    # Line2
    aSketchLine2 = theSketch.addFeature("SketchLine")
    aStartPoint2 = geomDataAPI_Point2D(aSketchLine2.attribute("StartPoint"))
    aEndPoint2   = geomDataAPI_Point2D(aSketchLine2.attribute("EndPoint"))
    aStartPoint2.setValue(10., 5.)
    aEndPoint2.setValue(15., 20.)
    allFeatures.append(aSketchLine2)
    # Coincidence
    aCoincidence = theSketch.addFeature("SketchConstraintCoincidence")
    aCoincidence.refattr("ConstraintEntityA").setAttr(aStartPoint1)
    aCoincidence.refattr("ConstraintEntityB").setAttr(aStartPoint2)
    
    theSketch.execute()
    return allFeatures
    
def checkFillet(theObjects, theRadius):
    # Verify the arc and lines are connected smoothly
    aLine = []
    anArc = []
    aSize = theObjects.size()
    for i in range (0, aSize):
        feat = ModelAPI_Feature.feature(theObjects.object(i))
        assert(feat is not None)
        if (feat.getKind() == "SketchLine"):
            aLine.append(feat)
        elif (feat.getKind() == "SketchArc"):
            anArc.append(feat)
    assert(anArc)
    assert(anArc[0] is not None)
    
    anArcPoints = []
    aPoint = geomDataAPI_Point2D(anArc[0].attribute("ArcStartPoint"))
    anArcPoints.append((aPoint.x(), aPoint.y()))
    aPoint = geomDataAPI_Point2D(anArc[0].attribute("ArcEndPoint"))
    anArcPoints.append((aPoint.x(), aPoint.y()))
    aPoint = geomDataAPI_Point2D(anArc[0].attribute("ArcCenter"))
    aCenterX = aPoint.x()
    aCenterY = aPoint.y()
    
    for line in aLine:
        aStartPoint = geomDataAPI_Point2D(line.attribute("StartPoint"))
        aEndPoint = geomDataAPI_Point2D(line.attribute("EndPoint"))
        
        aLinePoints = []
        aLinePoints.append((aStartPoint.x(), aStartPoint.y()))
        aLinePoints.append((aEndPoint.x(), aEndPoint.y()))
        
        aLineDirX = aEndPoint.x() - aStartPoint.x()
        aLineDirY = aEndPoint.y() - aStartPoint.y()
        
        for arcPt in anArcPoints:
            for linePt in aLinePoints:
                if (math.hypot(linePt[0]-arcPt[0], linePt[1]-arcPt[1]) < 1.e-10):
                    aDirX = linePt[0] - aCenterX
                    aDirY = linePt[1] - aCenterY
                    assert(math.fabs(math.hypot(aDirX, aDirY) - theRadius) < 1.e-7)
                    aDot = aDirX * aLineDirX + aDirY * aLineDirY
                    assert(math.fabs(aDot) < 1.e-7)
                    break;


#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2015-09-18"

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
# Initialize sketch
#=========================================================================
aSession.startOperation()
aFeaturesList = createSketch(aSketchFeature)
aSession.finishOperation()
#=========================================================================
# Global variables
#=========================================================================
FILLET_RADIUS1 = 3.
FILLET_RADIUS2 = 5.
#=========================================================================
# Create the Fillet
#=========================================================================
aSession.startOperation()
aFillet = aSketchFeature.addFeature("SketchConstraintFillet")
aRefAttrA = aFillet.refattr("ConstraintEntityA");
aRefAttrB = aFillet.refattr("ConstraintEntityB");
aResConstr = modelAPI_ResultConstruction(aFeaturesList[0].lastResult())
assert(aResConstr)
aRefAttrA.setObject(aResConstr)
aResConstr = modelAPI_ResultConstruction(aFeaturesList[1].lastResult())
assert(aResConstr)
aRefAttrB.setObject(aResConstr)
aRadius = aFillet.real("ConstraintValue")
aRadius.setValue(FILLET_RADIUS1)
aFillet.execute()
aResObjects = aFillet.reflist("ConstraintEntityC")
#=========================================================================
# Verify the objects of fillet are created
#=========================================================================
assert(aResObjects)
checkFillet(aResObjects, FILLET_RADIUS1)
aSession.finishOperation()
#=========================================================================
# End of test
#=========================================================================
