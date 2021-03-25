# Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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

from ModelAPI import *
from GeomDataAPI import *
import ModelHighAPI
import math
from salome.shaper.model.sketcher import tools

TOLERANCE = 1.e-7

def assertPoint(thePoint, theCoords, theTolerance = TOLERANCE):
    """ Verifies coordinates of the point
    """
    aPoint = tools.toList(thePoint)
    assert((aPoint[0]-theCoords[0])**2 + (aPoint[1]-theCoords[1])**2 < theTolerance**2), "Wrong '{}' point {}, expected {}".format(thePoint.id(), aPoint, theCoords)


def assertLine(theLine, theStart, theEnd, theTolerance = TOLERANCE):
    """ Verifies coordinates of line extremities
    """
    aLine = tools.toSketchFeature(theLine)

    aStartPnt = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
    aEndPnt = geomDataAPI_Point2D(aLine.attribute("EndPoint"))
    if len(theStart):
        assertPoint(aStartPnt, theStart, theTolerance)
    if len(theEnd):
        assertPoint(aEndPnt, theEnd, theTolerance)


def assertCircle(theCircle, theCenter, theRadius, theTolerance = TOLERANCE):
    """ Verifies attributes of circle
    """
    aCircle = tools.toSketchFeature(theCircle)

    aCenter = geomDataAPI_Point2D(aCircle.attribute("circle_center"))
    if len(theCenter):
        assertPoint(aCenter, theCenter, theTolerance)

    aRadius = aCircle.real("circle_radius")
    assert aRadius.value() == theRadius, "Wrong circle radius {}, expected {}".format(aRadius.value(), theRadius)


def assertArc(theArc, theCenter, theStart, theEnd, theTolerance = TOLERANCE):
    """ Verifies coordinates of arc points and the consistency of the arc.
        Some of points may be empty lists.
    """
    anArc = tools.toSketchFeature(theArc)

    aCenterPnt = geomDataAPI_Point2D(anArc.attribute("center_point"))
    aStartPnt = geomDataAPI_Point2D(anArc.attribute("start_point"))
    aEndPnt = geomDataAPI_Point2D(anArc.attribute("end_point"))
    if len(theCenter):
        assertPoint(aCenterPnt, theCenter, theTolerance)
    if len(theStart):
        assertPoint(aStartPnt, theStart, theTolerance)
    if len(theEnd):
        assertPoint(aEndPnt, theEnd, theTolerance)

    assertArcValidity(anArc)


def assertArcValidity(theArc):
    """ Tests whether the arc is correctly defined
    """
    anArc = tools.toSketchFeature(theArc)

    aCenterPnt = geomDataAPI_Point2D(anArc.attribute("center_point"))
    aStartPnt = geomDataAPI_Point2D(anArc.attribute("start_point"))
    aEndPnt = geomDataAPI_Point2D(anArc.attribute("end_point"))
    aRadius = anArc.real("radius")
    aDistCS = tools.distancePointPoint(aCenterPnt, aStartPnt)
    aDistCE = tools.distancePointPoint(aCenterPnt, aEndPnt)
    assert math.fabs(aDistCS - aDistCE) < TOLERANCE, "Wrong arc: center-start distance {}, center-end distance {}".format(aDistCS, aDistCE)
    assert math.fabs(aRadius.value() - aDistCS) < TOLERANCE, "Wrong arc: radius is {}, expected {}".format(aRadius.value(), aDistCS)


def checkSketch(theSketch, theDOF = -1):
    """ Tests the sketch is valid and DoF is equal to the given
    """
    assert(theSketch.feature().error() == ""), "Sketch failed: {}".format(theSketch.feature().error())
    assert(theSketch.solverError().value() == ""), "Sketch solver failed: {}".format(theSketch.solverError().value())
    if theDOF != -1:
        aDOF = tools.dof(theSketch)
        assert(aDOF == theDOF), "Sketch DoF {} is wrong. Expected {}".format(aDOF, theDOF)


def checkSketchErrorDegenerated(theSketch):
    """ Verify the sketch reports error about degenerated geometry
    """
    errorValue = theSketch.solverError().value()
    assert(errorValue != "")
    assert(errorValue.find("degenerated") >= 0)


def compareSketches(theReference, theSketch, TOLERANCE = 1.e-5):
    """ Compare sketches for the sequence of features
    """
    errors = ""

    # compare sketches degree of freedom
    if tools.dof(theReference) != tools.dof(theSketch):
        errors += "\nError in DoF (actual = {}, expected = {})".format(tools.dof(theSketch), tools.dof(theReference))

    # compare sketch solver error
    if theReference.solverError().value() != theSketch.solverError().value():
        errors += "\nError in solver message (actual = '{}', expected = '{}')".format(theSketch.solverError().value(), theReference.solverError().value())

    aRefSketch = featureToCompositeFeature(theReference.feature())
    anActualSketch = featureToCompositeFeature(theSketch.feature())

    # compare number of subs
    aRefNbSubs = aRefSketch.numberOfSubs()
    anActualNbSubs = anActualSketch.numberOfSubs()
    if aRefNbSubs != anActualNbSubs:
        errors += "\nError in number of sub-features (actual = {}, expected = {})".format(anActualNbSubs, aRefNbSubs)

    for index in range(min(aRefNbSubs, anActualNbSubs)):
        aRefFeature = aRefSketch.subFeature(index)
        aFeature = anActualSketch.subFeature(index)
        # compare types of subs
        if aFeature.getKind() != aRefFeature.getKind():
            errors += "\nWrong sketch feature (actual = '{}', expected = '{}')".format(aFeature.name(), aRefFeature.name())
            continue
        # compare attributes
        aRefAttrs = aRefFeature.data().attributes("")
        anAttrs = aFeature.data().attributes("")
        for ref, attr in zip(aRefAttrs, anAttrs):
            if ref.attributeType() != attr.attributeType():
                errors += "\nWrong sequence of attributes (feature = '{}', reference = '{}')".format(aFeature.name(), aRefFeature.name())
            elif not attr.isInitialized() or not ref.isInitialized():
                if attr.isInitialized() != ref.isInitialized():
                    errors += "\nAttribute '{}' initialization is different (feature = '{}', reference = '{}'): actual = {}, expected = {}".format(attr.id(), aFeature.name(), aRefFeature.name(), attr.isInitialized(), ref.isInitialized())
            elif ref.attributeType() == GeomDataAPI_Point2D.typeId():
                aRefPoint = geomDataAPI_Point2D(ref)
                aPoint = geomDataAPI_Point2D(attr)
                if tools.distancePointPoint(aPoint, aRefPoint) > TOLERANCE:
                    errors += "\nWrong coordinates '{}' (feature = '{}', reference = '{}'): actual = ({}, {}), expected = ({}, {})".format(attr.id(), aFeature.name(), aRefFeature.name(), aPoint.x(), aPoint.y(), aRefPoint.x(), aRefPoint.y())
            elif ref.attributeType() == GeomDataAPI_Point2DArray.typeId():
                aRefPoints = geomDataAPI_Point2DArray(ref)
                aPoints = geomDataAPI_Point2DArray(attr)
                for pInd in range(aRefPoints.size()):
                    aRefPoint = aRefPoints.pnt(pInd)
                    aPoint = aPoints.pnt(pInd)
                    if tools.distancePointPoint(aPoint, aRefPoint) > TOLERANCE:
                        errors += "\nWrong coordinates '{}', index {} (feature = '{}', reference = '{}'): actual = ({}, {}), expected = ({}, {})".format(attr.id(), pInd, aFeature.name(), aRefFeature.name(), aPoint.x(), aPoint.y(), aRefPoint.x(), aRefPoint.y())
            elif ref.attributeType() == ModelAPI_AttributeBoolean.typeId():
                aRefVal = modelAPI_AttributeBoolean(ref).value()
                aVal = modelAPI_AttributeBoolean(attr).value()
                if aVal != aRefVal:
                    errors += "\nWrong boolean value '{}' (feature = '{}', reference = '{}'): actual = {}, expected = {}".format(attr.id(), aFeature.name(), aRefFeature.name(), aVal, aRefVal)
            elif ref.attributeType() == ModelAPI_AttributeDouble.typeId():
                aRefVal = modelAPI_AttributeDouble(ref).value()
                aVal = modelAPI_AttributeDouble(attr).value()
                if math.fabs(aVal - aRefVal) > TOLERANCE:
                    errors += "\nWrong real value '{}' (feature = '{}', reference = '{}'): actual = {}, expected = {}".format(attr.id(), aFeature.name(), aRefFeature.name(), aVal, aRefVal)
            elif ref.attributeType() == ModelAPI_AttributeDoubleArray.typeId():
                aRefArray = modelAPI_AttributeDoubleArray(ref)
                anArray = modelAPI_AttributeDoubleArray(attr)
                for vInd in range(aRefArray.size()):
                    aRefVal = aRefArray.value(vInd)
                    aVal = anArray.value(vInd)
                    if math.fabs(aVal - aRefVal) > TOLERANCE:
                        errors += "\nWrong real value '{}' index {} (feature = '{}', reference = '{}'): actual = {}, expected = {}".format(attr.id(), vInd, aFeature.name(), aRefFeature.name(), aVal, aRefVal)
            elif ref.attributeType() == ModelAPI_AttributeInteger.typeId():
                aRefVal = modelAPI_AttributeInteger(ref).value()
                aVal = modelAPI_AttributeInteger(attr).value()
                if aVal != aRefVal:
                    errors += "\nWrong integer value '{}' (feature = '{}', reference = '{}'): actual = {}, expected = {}".format(attr.id(), aFeature.name(), aRefFeature.name(), aVal, aRefVal)
            elif ref.attributeType() == ModelAPI_AttributeIntArray.typeId():
                aRefArray = modelAPI_AttributeIntArray(ref)
                anArray = modelAPI_AttributeIntArray(attr)
                for vInd in range(aRefArray.size()):
                    aRefVal = aRefArray.value(vInd)
                    aVal = anArray.value(vInd)
                    if aVal != aRefVal:
                        errors += "\nWrong integer value '{}' index {} (feature = '{}', reference = '{}'): actual = {}, expected = {}".format(attr.id(), vInd, aFeature.name(), aRefFeature.name(), aVal, aRefVal)

    return errors
