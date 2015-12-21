"""
    TestMultiTranslation.py
    Unit test of SketchPlugin_MultiTranslation class
        
    SketchPlugin_MultiTranslation
        static const std::string MY_CONSTRAINT_TRANSLATION_ID("SketchMultiTranslation");
        data()->addAttribute(VALUE_TYPE(), ModelAPI_AttributeString::typeId());
        data()->addAttribute(START_POINT_ID(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(END_POINT_ID(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(NUMBER_OF_OBJECTS_ID(), ModelAPI_AttributeInteger::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefList::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefList::typeId());
        data()->addAttribute(TRANSLATION_LIST_ID(), ModelAPI_AttributeRefList::typeId());

"""
from GeomDataAPI import *
from ModelAPI import *

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
    
def checkTranslation(theObjects, theNbObjects, theDeltaX, theDeltaY):
    # Verify distances of the objects and the number of copies
    aFeatures = []
    aSize = theObjects.size()
    for i in range (0, aSize):
        feat = ModelAPI_Feature.feature(theObjects.object(i))
        assert(feat is not None)
        aFeatures.append(feat)
        
    anInd = 0 
    for feat, next in zip(aFeatures[:-1], aFeatures[1:]):
        anInd = anInd + 1
        if (anInd > theNbObjects):
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
            
        #for attr in anAttributes:
            #aPoint1 = geomDataAPI_Point2D(feat.attribute(attr))
            #aPoint2 = geomDataAPI_Point2D(next.attribute(attr))
            #aDiffX = aPoint2.x() - aPoint1.x() - theDeltaX
            #aDiffY = aPoint2.y() - aPoint1.y() - theDeltaY
            #assert(aDiffX**2 + aDiffY**2 < 1.e-15)
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
START_X = 0.
START_Y = 15.
DIR_X = 20.
DIR_Y = 10.
DELTA_X = -5.
DELTA_Y = 3.
#=========================================================================
# Create translation line
#=========================================================================
aSession.startOperation()
aTransLine = aSketchFeature.addFeature("SketchLine")
aTransLineStartPoint = geomDataAPI_Point2D(aTransLine.attribute("StartPoint"))
aTransLineEndPoint = geomDataAPI_Point2D(aTransLine.attribute("EndPoint"))
aTransLineStartPoint.setValue(START_X, START_Y)
aTransLineEndPoint.setValue(START_X + DELTA_X, START_Y + DELTA_Y)
aSession.finishOperation()
#=========================================================================
# Create the Translation constraint
#=========================================================================
aSession.startOperation()
aMultiTranslation = aSketchFeature.addFeature("SketchMultiTranslation")
aTransList = aMultiTranslation.reflist("MultiTranslationList")
for aFeature in aFeaturesList:
    aResult = modelAPI_ResultConstruction(aFeature.lastResult())
    assert(aResult is not None)
    aTransList.append(aResult)

aValueType = aMultiTranslation.string("ValueType")
aValueType.setValue("SingleValue")
aStartPoint = aMultiTranslation.refattr("MultiTranslationStartPoint")
aEndPoint = aMultiTranslation.refattr("MultiTranslationEndPoint")
aStartPoint.setAttr(aTransLineStartPoint)
aEndPoint.setAttr(aTransLineEndPoint)
aNbCopies = aMultiTranslation.integer("MultiTranslationObjects")
aNbCopies.setValue(2)
aMultiTranslation.execute()
aSession.finishOperation()
#=========================================================================
# Verify the objects are moved for the specified distance
#=========================================================================
aTranslated = aMultiTranslation.reflist("ConstraintEntityB")
checkTranslation(aTranslated, aNbCopies.value(), DELTA_X, DELTA_Y)
#=========================================================================
# Change number of copies and verify translation
#=========================================================================
aSession.startOperation()
aNbCopies.setValue(3)
aSession.finishOperation()
aTranslated = aMultiTranslation.reflist("ConstraintEntityB")
checkTranslation(aTranslated, aNbCopies.value(), DELTA_X, DELTA_Y)
#=========================================================================
# TODO: improve test
# 1. Add more features into translation
# 2. Move one of initial features and check the translated is moved too
#=========================================================================
#=========================================================================
# End of test
#=========================================================================
