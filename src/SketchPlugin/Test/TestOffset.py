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

def checkOffset(theListIn, theListOut, theOutToIn, theDist, isReversed, nbIn, nbOut):
    TOL = 6.e-5
    aNbIn  = theListIn.size()
    aNbOut = theListOut.size()

    #print("**checkOffset**")
    assert (theListIn.size() == nbIn)
    assert (theListOut.size() == nbOut)
    assert (theOutToIn.size() == nbOut)

    for ind in range(0, aNbOut):
        aFeatureOut = ModelAPI_Feature.feature(theListOut.object(ind))
        assert(aFeatureOut is not None)
        anInInd = theOutToIn.value(ind)
        if (anInInd == -1):
            assert(aFeatureOut.getKind() == "SketchArc")
        else:
            aFeatureIn = ModelAPI_Feature.feature(theListIn.object(anInInd))
            assert(aFeatureIn is not None)

            #print(aFeatureIn.getKind())
            if (aFeatureIn.getKind() == "SketchLine"):
                assert(aFeatureOut.getKind() == aFeatureIn.getKind())
                # Line and its offset are parallel
                aP1Out = geomDataAPI_Point2D(aFeatureOut.attribute('StartPoint'))
                aP2Out = geomDataAPI_Point2D(aFeatureOut.attribute('EndPoint'))
                aP1In  = geomDataAPI_Point2D(aFeatureIn.attribute('StartPoint'))
                aP2In  = geomDataAPI_Point2D(aFeatureIn.attribute('EndPoint'))
                aDirOut = [aP2Out.x() - aP1Out.x(), aP2Out.y() - aP1Out.y()]
                aDirIn  = [ aP2In.x() -  aP1In.x(),  aP2In.y() -  aP1In.y()]
                aCross = aDirOut[0] * aDirIn[1] - aDirOut[1] * aDirIn[0]
                assert math.fabs(aCross) < TOL, "aCross = {0}".format(aCross)
            elif (aFeatureIn.getKind() == "SketchArc"):
                assert(aFeatureOut.getKind() == aFeatureIn.getKind())
                # Arc and its offset have the same center
                aCPOut = geomDataAPI_Point2D(aFeatureOut.attribute('center_point'))
                aCPIn  = geomDataAPI_Point2D(aFeatureIn.attribute('center_point'))
                assert (math.fabs(aCPOut.x() - aCPIn.x()) < TOL)
                assert (math.fabs(aCPOut.y() - aCPIn.y()) < TOL)


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
anOffsetToBaseMap = anOffset.intArray("ConstraintEntityC")
checkOffset(aRefListA, aRefListB, anOffsetToBaseMap, VALUE, IS_REVERSED, 3, 6)
assert (model.dof(aSketchFeature) == 9)

#=========================================================================
# Remove object from offset
#=========================================================================
aSession.startOperation()
aRefListInitial.remove(aSketchLine2.lastResult())
aSession.finishOperation()
checkOffset(aRefListA, aRefListB, anOffsetToBaseMap, VALUE, IS_REVERSED, 2, 4)
assert (model.dof(aSketchFeature) == 9)

#=========================================================================
# Clear list of objects
#=========================================================================
aSession.startOperation()
aRefListInitial.clear()
#TODO: uncomment next line
#checkOffset(aRefListA, aRefListB, anOffsetToBaseMap, VALUE, IS_REVERSED, 0, 0)
# add arc once again
aRefListInitial.append(aSketchArc1.lastResult())
aSession.finishOperation()
checkOffset(aRefListA, aRefListB, anOffsetToBaseMap, VALUE, IS_REVERSED, 1, 1)
assert (model.dof(aSketchFeature) == 9)

#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
