"""
    TestMultiRotation.py
    Unit test of SketchPlugin_MultiRotation class
        
    SketchPlugin_MultiRotation
        static const std::string MY_CONSTRAINT_ROTATION_ID("SketchMultiRotation");
        data()->addAttribute(ANGLE_TYPE(), ModelAPI_AttributeString::typeId());
        data()->addAttribute(ANGLE_ID(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(ANGLE_FULL_ID(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(NUMBER_OF_OBJECTS_ID(), ModelAPI_AttributeInteger::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefList::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefList::typeId());
        data()->addAttribute(ROTATION_LIST_ID(), ModelAPI_AttributeRefList::typeId());

"""
from GeomDataAPI import *
from ModelAPI import *
import math

#=========================================================================
# Auxiliary functions
#=========================================================================
def createSketch(theSketch):
    # Initialize sketch by arc
    allFeatures = []
    # Create arc
    aSketchArc = theSketch.addFeature("SketchArc")
    aCenter     = geomDataAPI_Point2D(aSketchArc.attribute("ArcCenter"))
    aStartPoint = geomDataAPI_Point2D(aSketchArc.attribute("ArcStartPoint"))
    aEndPoint   = geomDataAPI_Point2D(aSketchArc.attribute("ArcEndPoint"))
    aCenter.setValue(5., 5.)
    aStartPoint.setValue(10., 5.)
    aEndPoint.setValue(5., 10.)
    allFeatures.append(aSketchArc)
    theSketch.execute()
    return allFeatures
    
def checkRotation(theObjects, theNbObjects, theCenterX, theCenterY, theAngle):
    # Verify distances of the objects and the number of copies
    aFeatures = []
    aSize = theObjects.size()
    for i in range (0, aSize):
        feat = ModelAPI_Feature.feature(theObjects.object(i))
        assert(feat is not None)
        aFeatures.append(feat)
        
    cosA = math.cos(theAngle * math.pi / 180.)
        
    anInd = 0 
    for feat, next in zip(aFeatures[:-1], aFeatures[1:]):
        anInd = anInd + 1
        if (anInd > theNbObjects-1):
            anInd = 0
            continue
        assert(feat.getKind() == next.getKind())
        
        anAttributes = []
        if (feat.getKind() == "SketchLine"):
            anAttributes.append('StartPoint')
            anAttributes.append('EndPoint')
        elif (feat.getKind() == "SketchArc"):
            anAttributes.append('ArcCenter')
            anAttributes.append('ArcStartPoint')
            anAttributes.append('ArcEndPoint')
            
        for attr in anAttributes:
            aPoint1 = geomDataAPI_Point2D(feat.attribute(attr))
            aPoint2 = geomDataAPI_Point2D(next.attribute(attr))
            aDirX1 = aPoint1.x() - theCenterX
            aDirY1 = aPoint1.y() - theCenterY
            aLen1 = math.hypot(aDirX1, aDirY1)
            aDirX2 = aPoint2.x() - theCenterX
            aDirY2 = aPoint2.y() - theCenterY
            aLen2 = math.hypot(aDirX2, aDirY2)
            aLocCosA = (aDirX1 * aDirX2 + aDirY1 * aDirY2) / aLen1 / aLen2
            assert(math.fabs(aLocCosA - cosA) < 1.e-10)
    # Check the number of copies is as planed
    assert(anInd == theNbObjects-1)


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
CENTER_X = 0.
CENTER_Y = 0.
ANGLE = 30.
#=========================================================================
# Create rotation point
#=========================================================================
aSession.startOperation()
aRotationPoint = aSketchFeature.addFeature("SketchPoint")
aRotationPointPoint = geomDataAPI_Point2D(aRotationPoint.attribute("PointCoordindates"))
aRotationPointPoint.setValue(CENTER_X, CENTER_Y)
aSession.finishOperation()
#=========================================================================
# Create the Rotation constraint
#=========================================================================
aSession.startOperation()
aMultiRotation = aSketchFeature.addFeature("SketchMultiRotation")
aRotList = aMultiRotation.reflist("MultiRotationList")
for aFeature in aFeaturesList:
    aResult = modelAPI_ResultConstruction(aFeature.lastResult())
    assert(aResult is not None)
    aRotList.append(aResult)

aValueType = aMultiRotation.string("AngleType")
aValueType.setValue("SingleValue")

aCenter = aMultiRotation.refattr("MultiRotationCenter")
aCenter.setAttr(aRotationPointPoint)

anAngle = aMultiRotation.real("MultiRotationAngle")
anAngle.setValue(ANGLE)

anAngle = aMultiRotation.string("AngleType")
anAngle.setValue("SingleAngle")

aNbCopies = aMultiRotation.integer("MultiRotationObjects")
aNbCopies.setValue(2)
aMultiRotation.execute()
aSession.finishOperation()
#=========================================================================
# Verify the objects are moved for the specified distance
#=========================================================================
aRotated = aMultiRotation.reflist("ConstraintEntityB")
checkRotation(aRotated, aNbCopies.value(), CENTER_X, CENTER_Y, ANGLE)
#=========================================================================
# Change number of copies and verify Rotation
#=========================================================================
aSession.startOperation()
aNbCopies.setValue(3)
aSession.finishOperation()
aRotated = aMultiRotation.reflist("ConstraintEntityB")
checkRotation(aRotated, aNbCopies.value(), CENTER_X, CENTER_Y, ANGLE)
#=========================================================================
# TODO: improve test
# 1. Add more features into Rotation
# 2. Move one of initial features and check the Rotated is moved too
#=========================================================================
#=========================================================================
# End of test
#=========================================================================
