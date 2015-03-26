"""
    TestConstraintMirror.py
    Unit test of SketchPlugin_ConstraintMirror class
        
    SketchPlugin_ConstraintMirror
        static const std::string MY_CONSTRAINT_MIRROR_ID("SketchConstraintMirror");
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefListAttr::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_C(), ModelAPI_AttributeRefListAttr::typeId());

"""
from GeomDataAPI import *
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
#=========================================================================
# Create mirror line
#=========================================================================
aSession.startOperation()
aMirrorLine = aSketchFeature.addFeature("SketchLine")
aLineStartPoint = geomDataAPI_Point2D(aMirrorLine.attribute("StartPoint"))
aLineEndPoint = geomDataAPI_Point2D(aMirrorLine.attribute("EndPoint"))
aLineStartPoint.setValue(100., 0.)
aLineEndPoint.setValue(100., 100.)
aSession.finishOperation()
#=========================================================================
# Make mirror for objects created above
#=========================================================================
aSession.startOperation()
aMirror = aSketchFeature.addFeature("SketchConstraintMirror")
aRefObjectA = aMirror.refattr("ConstraintEntityA")
aRefObjectA.setObject(modelAPI_ResultConstruction(aMirrorLine.firstResult()))
aRefListB = aMirror.reflist("ConstraintEntityB")
aRefListB.append(aSketchArc1)
aRefListB.append(aSketchLine1)
aRefListB.append(aSketchLine2)
aMirror.execute()
aSession.finishOperation()
#=========================================================================
# Verify the simmetricity of all mirrored objects
#=========================================================================
aRefListC = aMirror.reflist("ConstraintEntityC")
aListSize = aRefListB.size()
aLineDirX = aLineEndPoint.x() - aLineStartPoint.x()
aLineDirY = aLineEndPoint.y() - aLineStartPoint.y()

for ind in range(0, aListSize):
  aFeatureB = modelAPI_Feature(aRefListB.object(ind))
  aFeatureC = modelAPI_Feature(aRefListC.object(ind))
  assert(aFeatureB is not None)
  assert(aFeatureC is not None)
  assert(aFeatureB.getKind() == aFeatureC.getKind())
  anAttributes = {}
  print aFeatureB.getKind()
  if (aFeatureB.getKind() == "SketchLine"):
    anAttributes = {'StartPoint':'StartPoint', 'EndPoint':'EndPoint'}
  elif (aFeatureB.getKind() == "SketchArc"):
    anAttributes = {'ArcCenter':'ArcCenter', 'ArcStartPoint':'ArcEndPoint', 'ArcEndPoint':'ArcStartPoint'}

  for key in anAttributes:
    aPointB = geomDataAPI_Point2D(aFeatureB.attribute(key))
    aPointC = geomDataAPI_Point2D(aFeatureC.attribute(anAttributes[key]))
    aDirX = aPointC.x() - aPointB.x()
    aDirY = aPointC.y() - aPointB.y()
    aDot = aLineDirX * aDirX + aLineDirY * aDirY
    assert(math.fabs(aDot) < 1.e-10)
    aDirX = aLineEndPoint.x() - 0.5 * (aPointB.x() + aPointC.x())
    aDirY = aLineEndPoint.y() - 0.5 * (aPointB.y() + aPointC.y())
    aCross = aLineDirX * aDirY - aLineDirY * aDirX
    assert(math.fabs(aCross) < 1.e-10)
#=========================================================================
# End of test
#=========================================================================
