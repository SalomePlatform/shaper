# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

"""
    TestOffset.py
    Unit test of SketchPlugin_Offset class

    SketchPlugin_Offset
        static const std::string ID("SketchOffset");
        data()->addAttribute(EDGES_ID(), ModelAPI_AttributeRefList::typeId());
        data()->addAttribute(VALUE_ID(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(REVERSED_ID(), ModelAPI_AttributeBoolean::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefList::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefList::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_C(), ModelAPI_AttributeIntArray::typeId());
"""

from GeomDataAPI import *
from ModelAPI import *
import math
from salome.shaper import model

#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2020-06-30"

#=========================================================================
# Auxiliary functions
#=========================================================================
def normalize(theDir):
    aLen = math.hypot(theDir[0], theDir[1])
    if aLen < 1.e-10:
        aLen = 1.0
    return [theDir[0] / aLen, theDir[1] / aLen]

def checkOffset(theListInit, theListOff, theValue, theIsReversed):
    TOL = 6.e-5
    aListSize = theListInit.size()

    #aLineStartPoint = geomDataAPI_Point2D(theMirrorLine.attribute("StartPoint"))
    #aLineEndPoint = geomDataAPI_Point2D(theMirrorLine.attribute("EndPoint"))
    #aLineDir = [aLineEndPoint.x() - aLineStartPoint.x(), aLineEndPoint.y() - aLineStartPoint.y()]
    #aLineDir = normalize(aLineDir)

    for ind in range(0, aListSize):
        aFeatureB = ModelAPI_Feature.feature(theListInit.object(ind))
        aFeatureC = ModelAPI_Feature.feature(theListOff.object(ind))
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
            #aDot = aLineDir[0] * aDir[0] + aLineDir[1] * aDir[1]
            #assert math.fabs(aDot) < TOL, "aDot = {0}".format(aDot)
            #aDir[0] = aLineEndPoint.x() - 0.5 * (aPointB.x() + aPointC.x())
            #aDir[1] = aLineEndPoint.y() - 0.5 * (aPointB.y() + aPointC.y())
            #aCross = aLineDir[0] * aDir[1] - aLineDir[1] * aDir[0]
            #assert math.fabs(aCross) < TOL, "aCross = {0}".format(aCross)


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
anArcStartPoint.setValue(50., 0.)
anArcEndPoint = geomDataAPI_Point2D(aSketchArc1.attribute("end_point"))
anArcEndPoint.setValue(0., 50.)
aSession.finishOperation()
# Line 1
aSession.startOperation()
aSketchLine1 = aSketchFeature.addFeature("SketchLine")
aLine1StartPoint = geomDataAPI_Point2D(aSketchLine1.attribute("StartPoint"))
aLine1EndPoint = geomDataAPI_Point2D(aSketchLine1.attribute("EndPoint"))
aLine1StartPoint.setValue(0., 50.)
aLine1EndPoint.setValue(-20., 0.)
aSession.finishOperation()
# Line 2
aSession.startOperation()
aSketchLine2 = aSketchFeature.addFeature("SketchLine")
aLine2StartPoint = geomDataAPI_Point2D(aSketchLine2.attribute("StartPoint"))
aLine2EndPoint = geomDataAPI_Point2D(aSketchLine2.attribute("EndPoint"))
aLine2StartPoint.setValue(50., 0.)
aLine2EndPoint.setValue(-20., 0.)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 13)
#=========================================================================
# Link arc points and lines points by the coincidence constraint
#=========================================================================
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintCoincidence")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setAttr(anArcEndPoint)
reflistB.setAttr(aLine1StartPoint)
aConstraint.execute()
aSession.finishOperation()
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintCoincidence")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setAttr(anArcStartPoint)
reflistB.setAttr(aLine2StartPoint)
aConstraint.execute()
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 9)
#=========================================================================
# Make offset for objects created above
#=========================================================================
VALUE = 13
IS_REVERSED = False
aSession.startOperation()
anOffset = aSketchFeature.addFeature("SketchOffset")
aRefListInitial = anOffset.reflist("segments")
aRefListInitial.append(aSketchLine1.lastResult())
aRefListInitial.append(aSketchArc1.lastResult())
aRefListInitial.append(aSketchLine2.lastResult())
anOffset.real("offset_value").setValue(VALUE)
anOffset.boolean("reversed").setValue(IS_REVERSED)
anOffset.execute()
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 9)
#=========================================================================
# Verify all offset objects
#=========================================================================
aRefListA = anOffset.reflist("ConstraintEntityA")
aRefListB = anOffset.reflist("ConstraintEntityB")
assert (aRefListA.size() == 3)
assert (aRefListB.size() == 6)
#checkOffset(aRefListA, aRefListB, VALUE, IS_REVERSED)
assert (model.dof(aSketchFeature) == 9)

#=========================================================================
# Remove object from offset
#=========================================================================
aSession.startOperation()
aRefListInitial.remove(aSketchLine2.lastResult())
aSession.finishOperation()
assert (aRefListA.size() == 2)
assert (aRefListB.size() == 4)
#checkOffset(aRefListA, aRefListB, VALUE, IS_REVERSED)
assert (model.dof(aSketchFeature) == 9)

#=========================================================================
# Clear list of mirrored features
#=========================================================================
aSession.startOperation()
aRefListInitial.clear()
assert (aRefListA.size() == 0)
assert (aRefListB.size() == 0)
# add arc once again
aRefListInitial.append(aSketchArc1.lastResult())
aSession.finishOperation()
assert (aRefListA.size() == 1)
assert (aRefListB.size() == 1)
#checkOffset(aRefListA, aRefListB, VALUE, IS_REVERSED)
assert (model.dof(aSketchFeature) == 9)

#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
