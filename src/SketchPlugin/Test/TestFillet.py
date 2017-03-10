"""
    TestFillet.py
    Unit test of SketchPlugin_Fillet class

    SketchPlugin_Fillet
        static const std::string MY_CONSTRAINT_FILLET_ID("SketchFillet");
        data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttrList::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefList::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_C(), ModelAPI_AttributeRefAttrList::typeId());

"""
from GeomDataAPI import *
from ModelAPI import *
import math
from salome.shaper import model

#=========================================================================
# Auxiliary functions
#=========================================================================
TOLERANCE = 1.e-7

def createSketch1(theSketch):
    global aEndPoint1, aEndPoint2
    # Initialize sketch by three lines with coincident boundaries
    allFeatures = []

    aSession.startOperation()
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

    aSession.finishOperation()
    return allFeatures


def createSketch2(theSketch):
    global aStartPoint1
    # Initialize sketch by line and arc with coincident boundary
    allFeatures = []

    aSession.startOperation()
    # Line
    aSketchLine = theSketch.addFeature("SketchLine")
    aStartPoint1 = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
    aEndPoint1   = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
    aStartPoint1.setValue(10., 10.)
    aEndPoint1.setValue(30., 15.)
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

    aSession.finishOperation()
    return allFeatures

def checkSmoothness(theSketch):
    aPtPtCoincidences = getCoincidences(theSketch)
    for coinc in aPtPtCoincidences:
        aConnectedFeatures = connectedFeatures(coinc)
        assert(len(aConnectedFeatures) == 2)
        if aConnectedFeatures[0].getKind() == "SketchArc":
            if aConnectedFeatures[1].getKind() == "SketchArc":
                checkArcArcSmoothness(aConnectedFeatures[0], aConnectedFeatures[1])
            elif aConnectedFeatures[1].getKind() == "SketchLine":
                checkArcLineSmoothness(aConnectedFeatures[0], aConnectedFeatures[1])
        elif aConnectedFeatures[0].getKind() == "SketchLine" and aConnectedFeatures[1].getKind() == "SketchArc":
            checkArcLineSmoothness(aConnectedFeatures[1], aConnectedFeatures[0])

def checkArcLineSmoothness(theArc, theLine):
    aCenter = geomDataAPI_Point2D(theArc.attribute("ArcCenter"))
    aDistance = distancePointLine(aCenter, theLine)
    aRadius = arcRadius(theArc)
    assert(math.fabs(aRadius - aDistance) < TOLERANCE)

def checkArcArcSmoothness(theArc1, theArc2):
    aCenter1 = geomDataAPI_Point2D(theArc1.attribute("ArcCenter"))
    aCenter2 = geomDataAPI_Point2D(theArc2.attribute("ArcCenter"))
    aDistance = distancePointPoint(aCenter1, aCenter2)
    aRadius1 = arcRadius(theArc1)
    aRadius2 = arcRadius(theArc2)
    aRadSum = aRadius1 + aRadius2
    aRadDiff = math.fabs(aRadius1 - aRadius2)
    assert(math.fabs(aDistance - aRadSum) < TOLERANCE or math.fabs(aDistance - aRadDiff) < TOLERANCE)

def getCoincidences(theSketch):
    aCoincidences = []
    for anIndex in range(0, theSketch.numberOfSubs()):
        aSubFeature = theSketch.subFeature(anIndex)
        if aSubFeature.getKind == "SketchConstraintCoincidence":
            anEntityA = aSubFeature.refattr("ConstraintEntityA")
            anEntityB = aSubFeature.refattr("ConstraintEntityB")
            if not anEntityA.isObject() and not anEntityB.isObject():
                aCoincidences.append(aSubFeature)
    return aCoincidences

def connectedFeatures(theCoincidence):
    anEntityA = aSubFeature.refattr("ConstraintEntityA")
    anEntityB = aSubFeature.refattr("ConstraintEntityB")
    return [anEntityA.attr().owner(), anEntityB.attr().owner()]

def arcRadius(theArc):
    aCenter = geomDataAPI_Point2D(theArc.attribute("ArcCenter"))
    aStart = geomDataAPI_Point2D(theArc.attribute("ArcStartPoint"))
    return distancePointPoint(aCenter, aStart)

def distancePointPoint(thePoint1, thePoint2):
    return math.hypot(thePoint1.x() - thePoint2.x(), thePoint1.y() - thePoint2.y())

def distancePointLine(thePoint, theLine):
    aLineStart = geomDataAPI_Point2D(theLine.attribute("StartPoint"))
    aLineEnd = geomDataAPI_Point2D(theLine.attribute("EndPoint"))
    aLength = distancePointPoint(aLineStart, aLineEnd)

    aDir1x, aDir1y = aLineEnd.x() - aLineStart.x(), aLineEnd.y() - aLineStart.y()
    aDir2x, aDir2y = thePoint.x() - aLineStart.x(), thePoint.y() - aLineStart.y()
    aCross = aDir1x * aDir2y - aDir1y * aDir2x
    return math.fabs(aCross) / aLength


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
# Initialize sketch by three connected lines
#=========================================================================
createSketch1(aSketchFeature)
assert (model.dof(aSketchFeature) == 8)
#=========================================================================
# Create the Fillet
#=========================================================================
aSession.startOperation()
aFillet = aSketchFeature.addFeature("SketchFillet")
aFillet.refattr("fillet_point").setAttr(aEndPoint1);
aSession.finishOperation()
aSession.startOperation()
aFillet = aSketchFeature.addFeature("SketchFillet")
aFillet.refattr("fillet_point").setAttr(aEndPoint2);
aSession.finishOperation()
#=========================================================================
# Verify the objects of fillet are created
#=========================================================================
checkSmoothness(aSketchFeature)
assert model.dof(aSketchFeature) == 14, "PlaneGCS limitation: if you see this message, then maybe PlaneGCS has solved DoF for sketch with fillet correctly (expected DoF = 10, observed = {0}".format(model.dof(aSketchFeature))

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
createSketch2(aSketchFeature)
assert (model.dof(aSketchFeature) == 7)
#=========================================================================
# Create the Fillet
#=========================================================================
aSession.startOperation()
aFillet = aSketchFeature.addFeature("SketchFillet")
aFillet.refattr("fillet_point").setAttr(aStartPoint1)
aSession.finishOperation()
#=========================================================================
# Verify the objects of fillet are created
#=========================================================================
checkSmoothness(aSketchFeature)
assert model.dof(aSketchFeature) == 10, "PlaneGCS limitation: if you see this message, then maybe PlaneGCS has solved DoF for sketch with fillet correctly (expected DoF = 8, observed = {0}".format(model.dof(aSketchFeature))
#=========================================================================
# End of test
#=========================================================================

# TODO: Checking of Python dump has been disabled until the Fillet redesigned.
#assert(model.checkPythonDump())
