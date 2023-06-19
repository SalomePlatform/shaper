# Copyright (C) 2014-2023  CEA, EDF
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
from salome.shaper import model

#=========================================================================
# Auxiliary functions
#=========================================================================
def createSketch(theSketch):
    # Initialize sketch by arc
    allFeatures = []
    # Create arc
    aSketchArc = theSketch.addFeature("SketchArc")
    aCenter     = geomDataAPI_Point2D(aSketchArc.attribute("center_point"))
    aStartPoint = geomDataAPI_Point2D(aSketchArc.attribute("start_point"))
    aEndPoint   = geomDataAPI_Point2D(aSketchArc.attribute("end_point"))
    aCenter.setValue(5., 5.)
    aStartPoint.setValue(10., 5.)
    aEndPoint.setValue(5., 10.)
    allFeatures.append(aSketchArc)
    theSketch.execute()
    return allFeatures

def createLine(theSketch):
    aSketchLine = theSketch.addFeature("SketchLine")
    aStartPoint = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
    aEndPoint   = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
    aStartPoint.setValue(7., 5.)
    aEndPoint.setValue(1., 3.)
    theSketch.execute()
    return aSketchLine

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
        if (anInd > theNbObjects-1):
            anInd = 0
            continue
        assert(feat.getKind() == next.getKind())

        anAttributes = []
        if (feat.getKind() == "SketchLine"):
            anAttributes.append('StartPoint')
            anAttributes.append('EndPoint')
        elif (feat.getKind() == "SketchArc"):
            anAttributes.append('center_point')
            anAttributes.append('start_point')
            anAttributes.append('end_point')

        for attr in anAttributes:
             aPoint1 = geomDataAPI_Point2D(feat.attribute(attr))
             aPoint2 = geomDataAPI_Point2D(next.attribute(attr))
             aDiffX = aPoint2.x() - aPoint1.x() - theDeltaX
             aDiffY = aPoint2.y() - aPoint1.y() - theDeltaY
             assert(aDiffX**2 + aDiffY**2 < 1.e-15)
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
assert(model.dof(aSketchFeature) == 5)
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
assert(model.dof(aSketchFeature) == 9)
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
assert(model.dof(aSketchFeature) == 9)
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
assert(model.dof(aSketchFeature) == 9)

#=========================================================================
# Create new feature and add it into the Rotation
#=========================================================================
aSession.startOperation()
aLine = createLine(aSketchFeature)
aSession.finishOperation()
aSession.startOperation()
aResult = modelAPI_ResultConstruction(aLine.lastResult())
assert(aResult is not None)
aTransList.append(aResult)
aSession.finishOperation()
checkTranslation(aTranslated, aNbCopies.value(), DELTA_X, DELTA_Y)
assert(model.dof(aSketchFeature) == 13)
#=========================================================================
# Move line and check the copies are moved too
#=========================================================================
aSession.startOperation()
aStartPoint = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
aStartPoint.setValue(12., 5.)
aSession.finishOperation()
checkTranslation(aTranslated, aNbCopies.value(), DELTA_X, DELTA_Y)
assert(model.dof(aSketchFeature) == 13)
#=========================================================================
# Change number of copies and verify Rotation
#=========================================================================
aSession.startOperation()
aNbCopies.setValue(2)
aSession.finishOperation()
checkTranslation(aTranslated, aNbCopies.value(), DELTA_X, DELTA_Y)
assert(model.dof(aSketchFeature) == 13)

#=========================================================================
# Remove a feature from the Rotation
#=========================================================================
aSession.startOperation()
aRemoveIt = aTransList.object(0)
aTransList.remove(aRemoveIt)
aSession.finishOperation()
checkTranslation(aTranslated, aNbCopies.value(), DELTA_X, DELTA_Y)
assert(model.dof(aSketchFeature) == 13)

#=========================================================================
# Clear the list of rotated features
#=========================================================================
aSession.startOperation()
aTransList.clear()
checkTranslation(aTranslated, 1, DELTA_X, DELTA_Y)
# Add line once again
aTransList.append(aResult)
aSession.finishOperation()
checkTranslation(aTranslated, aNbCopies.value(), DELTA_X, DELTA_Y)
assert(model.dof(aSketchFeature) == 13)
#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
