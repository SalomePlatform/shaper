## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

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
from salome.shaper import model

#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2015-03-17"

#=========================================================================
# Auxiliary functions
#=========================================================================
def normalize(theDir):
    aLen = math.hypot(theDir[0], theDir[1])
    if aLen < 1.e-10:
        aLen = 1.0
    return [theDir[0] / aLen, theDir[1] / aLen]

def checkMirror(theListInit, theListMirr, theMirrorLine):
    TOL = 6.e-5
    aListSize = theListInit.size()

    aLineStartPoint = geomDataAPI_Point2D(theMirrorLine.attribute("StartPoint"))
    aLineEndPoint = geomDataAPI_Point2D(theMirrorLine.attribute("EndPoint"))
    aLineDir = [aLineEndPoint.x() - aLineStartPoint.x(), aLineEndPoint.y() - aLineStartPoint.y()]
    aLineDir = normalize(aLineDir)

    for ind in range(0, aListSize):
        aFeatureB = ModelAPI_Feature.feature(theListInit.object(ind))
        aFeatureC = ModelAPI_Feature.feature(theListMirr.object(ind))
        assert(aFeatureB is not None)
        assert(aFeatureC is not None)
        assert(aFeatureB.getKind() == aFeatureC.getKind())

        anAttributes = []
        if (aFeatureB.getKind() == "SketchLine"):
            anAttributes = ['StartPoint', 'EndPoint']
        elif (aFeatureB.getKind() == "SketchArc"):
            anAttributes = ['center_point', 'start_point', 'end_point']

        for key in anAttributes:
            aPointB = geomDataAPI_Point2D(aFeatureB.attribute(key))
            aPointC = geomDataAPI_Point2D(aFeatureC.attribute(key))
            aDir = [aPointC.x() - aPointB.x(), aPointC.y() - aPointB.y()]
            aDir = normalize(aDir)
            aDot = aLineDir[0] * aDir[0] + aLineDir[1] * aDir[1]
            assert math.fabs(aDot) < TOL, "aDot = {0}".format(aDot)
            aDir[0] = aLineEndPoint.x() - 0.5 * (aPointB.x() + aPointC.x())
            aDir[1] = aLineEndPoint.y() - 0.5 * (aPointB.y() + aPointC.y())
            aCross = aLineDir[0] * aDir[1] - aLineDir[1] * aDir[0]
            assert math.fabs(aCross) < TOL, "aCross = {0}".format(aCross)


#=========================================================================
# Start of test
#=========================================================================
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
assert (model.dof(aSketchFeature) == 8)
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
assert (model.dof(aSketchFeature) == 12)
#=========================================================================
# Make mirror for objects created above
#=========================================================================
aSession.startOperation()
aMirror = aSketchFeature.addFeature("SketchConstraintMirror")
aRefObjectA = aMirror.refattr("ConstraintEntityA")
aRefObjectA.setObject(modelAPI_ResultConstruction(aMirrorLine.firstResult()))
aRefListInitial = aMirror.reflist("ConstraintMirrorList")
aRefListInitial.append(aSketchLine1.lastResult())
aRefListInitial.append(aSketchArc1.lastResult())
aRefListInitial.append(aSketchLine2.lastResult())
aMirror.execute()
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 12)
#=========================================================================
# Verify the simmetricity of all mirrored objects
#=========================================================================
aRefListB = aMirror.reflist("ConstraintEntityB")
aRefListC = aMirror.reflist("ConstraintEntityC")
assert (aRefListB.size() == 3)
assert (aRefListC.size() == 3)
checkMirror(aRefListB, aRefListC, aMirrorLine)
assert (model.dof(aSketchFeature) == 12)

#=========================================================================
# Remove object from mirror
#=========================================================================
aSession.startOperation()
aRefListInitial.remove(aSketchLine2.lastResult())
aSession.finishOperation()
assert (aRefListB.size() == 2)
assert (aRefListC.size() == 2)
checkMirror(aRefListB, aRefListC, aMirrorLine)
assert (model.dof(aSketchFeature) == 12)

#=========================================================================
# Clear list of mirrored features
#=========================================================================
aSession.startOperation()
aRefListInitial.clear()
assert (aRefListB.size() == 0)
assert (aRefListC.size() == 0)
# add arc once again
aRefListInitial.append(aSketchArc1.lastResult())
aSession.finishOperation()
assert (aRefListB.size() == 1)
assert (aRefListC.size() == 1)
checkMirror(aRefListB, aRefListC, aMirrorLine)
assert (model.dof(aSketchFeature) == 12)

#=========================================================================
# Create distance between original and mirrored entities (check the error appears)
#=========================================================================
aSketchErrorAttr = aSketchFeature.string("SolverError")
assert len(aSketchErrorAttr.value()) == 0, "Sketch failed with error: {}".format(aSketchErrorAttr.value())
aMirroredArc = model.lastSubFeature(aSketchFeature, "SketchArc")
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintDistance")
refAttrA = aConstraint.refattr("ConstraintEntityA")
refAttrB = aConstraint.refattr("ConstraintEntityB")
anArcStartPoint = geomDataAPI_Point2D(aSketchArc1.attribute("start_point"))
aMirroredArcStartPoint = geomDataAPI_Point2D(aMirroredArc.attribute("start_point"))
refAttrA.setAttr(anArcStartPoint)
refAttrB.setAttr(aMirroredArcStartPoint)
aConstraint.real("ConstraintValue").setValue(200.)
aSession.finishOperation()
print "Sketch error : {}".format(aSketchErrorAttr.value())
assert len(aSketchErrorAttr.value()) != 0, "ERROR: Sketch has not been failed as expected"
aSession.startOperation()
aDocument.removeFeature(aConstraint)
aSession.finishOperation()

#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
