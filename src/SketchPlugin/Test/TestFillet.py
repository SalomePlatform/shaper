"""
    TestFillet.py
    Unit test of SketchPlugin_ConstraintFillet class

    SketchPlugin_ConstraintFillet
        static const std::string MY_CONSTRAINT_FILLET_ID("SketchConstraintFillet");
        data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttrList::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefList::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_C(), ModelAPI_AttributeRefAttrList::typeId());

"""
from GeomDataAPI import *
from ModelAPI import *
import math

#=========================================================================
# Auxiliary functions
#=========================================================================
aStartPoint1 = []

def createSketch1(theSketch):
    global aEndPoint1, aEndPoint2
    # Initialize sketch by two lines with coincident boundary
    allFeatures = []
    # Line1
    aSketchLine1 = theSketch.addFeature("SketchLine")
    aStartPoint1 = geomDataAPI_Point2D(aSketchLine1.attribute("StartPoint"))
    aEndPoint1   = geomDataAPI_Point2D(aSketchLine1.attribute("EndPoint"))
    aStartPoint1.setValue(-10., -10.)
    aEndPoint1.setValue(-10., 10.)
    allFeatures.append(aSketchLine1)
    # Line2
    aSketchLine2 = theSketch.addFeature("SketchLine")
    aStartPoint2 = geomDataAPI_Point2D(aSketchLine2.attribute("StartPoint"))
    aEndPoint2   = geomDataAPI_Point2D(aSketchLine2.attribute("EndPoint"))
    aStartPoint2.setValue(-10., 10.)
    aEndPoint2.setValue(10., 10.)
    allFeatures.append(aSketchLine2)
    # Line3
    aSketchLine3 = theSketch.addFeature("SketchLine")
    aStartPoint3 = geomDataAPI_Point2D(aSketchLine3.attribute("StartPoint"))
    aEndPoint3   = geomDataAPI_Point2D(aSketchLine3.attribute("EndPoint"))
    aStartPoint3.setValue(10., 10.)
    aEndPoint3.setValue(10., -10.)
    allFeatures.append(aSketchLine3)
    # Coincidence1
    aCoincidence1 = theSketch.addFeature("SketchConstraintCoincidence")
    aCoincidence1.refattr("ConstraintEntityA").setAttr(aEndPoint1)
    aCoincidence1.refattr("ConstraintEntityB").setAttr(aStartPoint2)
    # Coincidence2
    aCoincidence2 = theSketch.addFeature("SketchConstraintCoincidence")
    aCoincidence2.refattr("ConstraintEntityA").setAttr(aEndPoint2)
    aCoincidence2.refattr("ConstraintEntityB").setAttr(aStartPoint3)

    theSketch.execute()
    return allFeatures


def createSketch2(theSketch):
    global aStartPoint1
    # Initialize sketch by line and arc with coincident boundary
    allFeatures = []
    # Line
    aSketchLine = theSketch.addFeature("SketchLine")
    aStartPoint1 = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
    aEndPoint1   = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
    aStartPoint1.setValue(10., 10.)
    aEndPoint1.setValue(30., 5.)
    allFeatures.append(aSketchLine)
    # Arc
    aSketchArc = theSketch.addFeature("SketchArc")
    aStartPoint2 = geomDataAPI_Point2D(aSketchArc.attribute("ArcStartPoint"))
    aEndPoint2   = geomDataAPI_Point2D(aSketchArc.attribute("ArcEndPoint"))
    aCenterPoint = geomDataAPI_Point2D(aSketchArc.attribute("ArcCenter"))
    aCenterPoint.setValue(20., 10.)
    aStartPoint2.setValue(10., 10.)
    aEndPoint2.setValue(20., 0.)
    allFeatures.append(aSketchArc)
    # Coincidence
    aCoincidence = theSketch.addFeature("SketchConstraintCoincidence")
    aCoincidence.refattr("ConstraintEntityA").setAttr(aStartPoint1)
    aCoincidence.refattr("ConstraintEntityB").setAttr(aStartPoint2)

    theSketch.execute()
    return allFeatures

def checkFillet(theObjects, theRadius):
    # Verify the arc and lines are connected smoothly
    print "Check Fillet"
    aLine = []
    anArc = []
    aSize = len(theObjects)
    for feat in theObjects:
        assert(feat is not None)
        if (feat.getKind() == "SketchLine"):
            aLine.append(feat)
        elif (feat.getKind() == "SketchArc"):
            anArc.append(feat)
    aFilletArc = anArc[-1]
    assert(aFilletArc is not None)
    anArc.pop()

    anArcPoints = []
    aPoint = geomDataAPI_Point2D(aFilletArc.attribute("ArcStartPoint"))
    print "ArcStartPoint " + repr(aPoint.x()) + " " + repr(aPoint.y())
    anArcPoints.append((aPoint.x(), aPoint.y()))
    aPoint = geomDataAPI_Point2D(aFilletArc.attribute("ArcEndPoint"))
    print "ArcEndPoint " + repr(aPoint.x()) + " " + repr(aPoint.y())
    anArcPoints.append((aPoint.x(), aPoint.y()))
    aPoint = geomDataAPI_Point2D(aFilletArc.attribute("ArcCenter"))
    print "ArcCenter " + repr(aPoint.x()) + " " + repr(aPoint.y())
    aCenterX = aPoint.x()
    aCenterY = aPoint.y()
    aFilletRadius = math.hypot(anArcPoints[0][0]-aCenterX, anArcPoints[0][1]-aCenterY)

    for line in aLine:
        aStartPoint = geomDataAPI_Point2D(line.attribute("StartPoint"))
        aEndPoint = geomDataAPI_Point2D(line.attribute("EndPoint"))

        aLinePoints = []
        aLinePoints.append((aStartPoint.x(), aStartPoint.y()))
        print "aLineStartPoint " + repr(aStartPoint.x()) + " " + repr(aStartPoint.y())
        aLinePoints.append((aEndPoint.x(), aEndPoint.y()))
        print "aLineEndPoint " + repr(aEndPoint.x()) + " " + repr(aEndPoint.y())

        aLineDirX = aEndPoint.x() - aStartPoint.x()
        aLineDirY = aEndPoint.y() - aStartPoint.y()

        for arcPt in anArcPoints:
            for linePt in aLinePoints:
                if (math.hypot(linePt[0]-arcPt[0], linePt[1]-arcPt[1]) < 1.e-10):
                    aDirX = linePt[0] - aCenterX
                    aDirY = linePt[1] - aCenterY
                    assert(math.fabs(math.hypot(aDirX, aDirY) - theRadius) < 1.e-7)
                    aDot = aDirX * aLineDirX + aDirY * aLineDirY

                    break;

    if (aSize == 3):
        for arc in anArc:
            aStartPoint = geomDataAPI_Point2D(arc.attribute("ArcStartPoint"))
            aEndPoint = geomDataAPI_Point2D(arc.attribute("ArcEndPoint"))
            aCenterPoint = geomDataAPI_Point2D(arc.attribute("ArcCenter"))

            aBaseArcPoints = []
            aBaseArcPoints.append((aStartPoint.x(), aStartPoint.y()))
            print "anArcStartPoint " + repr(aStartPoint.x()) + " " + repr(aStartPoint.y())
            aBaseArcPoints.append((aEndPoint.x(), aEndPoint.y()))
            print "anArcEndPoint " + repr(aEndPoint.x()) + " " + repr(aEndPoint.y())
            print "anArcCenter " + repr(aCenterPoint.x()) + " " + repr(aCenterPoint.y())

            aRadius = math.hypot(aStartPoint.x()-aCenterPoint.x(), aStartPoint.y()-aCenterPoint.y())
            aDist = math.hypot(aCenterPoint.x() - aCenterX, aCenterPoint.y() - aCenterY)
            assert math.fabs(aFilletRadius + aRadius - aDist) < 1.e-7 or math.fabs(math.fabs(aFilletRadius - aRadius) - aDist) < 1.e-7, \
                "Fillet radius = {0}, Base arc radius = {1}, distance between centers = {2}".format(aFilletRadius, aRadius, aDist)


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
# Initialize sketch by two lines
#=========================================================================
aSession.startOperation()
aFeaturesList = createSketch1(aSketchFeature)
aSession.finishOperation()
aSketchSubFeatures = []
for aSubIndex in range(0, aSketchFeature.numberOfSubs()):
    aSketchSubFeatures.append(aSketchFeature.subFeature(aSubIndex))
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
aRefAttrA = aFillet.data().refattrlist("ConstraintEntityA");
aRefAttrA.append(aEndPoint1)
aRefAttrA.append(aEndPoint2)
aRadius = aFillet.real("ConstraintValue")
aRadius.setValue(FILLET_RADIUS1)
aFillet.execute()
aResObjects = []
for aSubIndex in range(0, aSketchFeature.numberOfSubs()):
    aSubFeature = aSketchFeature.subFeature(aSubIndex)
    if aSubFeature not in aSketchSubFeatures:
        if aSubFeature.getKind() == "SketchLine":
            aResObjects.insert(0, aSubFeature)
        elif aSubFeature.getKind() == "SketchArc":
            aResObjects.append(aSubFeature)
#=========================================================================
# Verify the objects of fillet are created
#=========================================================================
assert(aResObjects)
checkFillet(aResObjects, FILLET_RADIUS1)
#=========================================================================
# Change Fillet radius
#=========================================================================
aRadius.setValue(FILLET_RADIUS2)
aSession.finishOperation()
checkFillet(aResObjects, FILLET_RADIUS2)

#=========================================================================
# Create another sketch
#=========================================================================
aSession.startOperation()
aSketchCommonFeature = aDocument.addFeature("Sketch")
aSketchFeature = featureToCompositeFeature(aSketchCommonFeature)
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 1, 0)
aSession.finishOperation()
#=========================================================================
# Initialize sketch by line and arc
#=========================================================================
aSession.startOperation()
aFeaturesList = createSketch2(aSketchFeature)
aSession.finishOperation()
aSketchSubFeatures = []
for aSubIndex in range(0, aSketchFeature.numberOfSubs()):
    aSketchSubFeatures.append(aSketchFeature.subFeature(aSubIndex))
#=========================================================================
# Create the Fillet
#=========================================================================
aSession.startOperation()
aFillet = aSketchFeature.addFeature("SketchConstraintFillet")
aRefAttrA = aFillet.data().refattrlist("ConstraintEntityA");
aRefAttrA.append(aStartPoint1)
aRadius = aFillet.real("ConstraintValue")
aRadius.setValue(FILLET_RADIUS1)
aFillet.execute()
aResObjects = []
for aSubIndex in range(0, aSketchFeature.numberOfSubs()):
    aSubFeature = aSketchFeature.subFeature(aSubIndex)
    if aSubFeature not in aSketchSubFeatures:
        if aSubFeature.getKind() == "SketchLine":
            aResObjects.insert(0, aSubFeature)
        elif aSubFeature.getKind() == "SketchArc":
            aResObjects.append(aSubFeature)
#=========================================================================
# Verify the objects of fillet are created
#=========================================================================
assert(aResObjects)
checkFillet(aResObjects, FILLET_RADIUS1)
#=========================================================================
# Change Fillet radius
#=========================================================================
aRadius.setValue(FILLET_RADIUS2)
aSession.finishOperation()
checkFillet(aResObjects, FILLET_RADIUS2)
#=========================================================================
# End of test
#=========================================================================
