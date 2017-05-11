# Author: Artem Zhidkov
# Copyright (C) 2017-20xx CEA/DEN, EDF R&D

from ModelAPI import *
from GeomDataAPI import *
import ModelHighAPI
import math
from salome.shaper.model.sketcher import tools

TOLERANCE = 1.e-7

def assertPoint(thePoint, theCoords):
    """ Verifies coordinates of the point
    """
    aPoint = tools.toPoint(thePoint)
    assert aPoint.x() == theCoords[0] and aPoint.y() == theCoords[1], "Wrong '{}' point ({}, {}), expected ({}, {})".format(thePoint.id(), aPoint.x(), aPoint.y(), theCoords[0], theCoords[1])


def assertLine(theLine, theStart, theEnd):
    """ Verifies coordinates of line extremities
    """
    aLine = tools.toSketchFeature(theLine)

    aStartPnt = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
    aEndPnt = geomDataAPI_Point2D(aLine.attribute("EndPoint"))
    if len(theStart):
        assertPoint(aStartPnt, theStart)
    if len(theEnd):
        assertPoint(aEndPnt, theEnd)


def assertCircle(theCircle, theCenter, theRadius):
    """ Verifies attributes of circle
    """
    aCircle = tools.toSketchFeature(theCircle)

    aCenter = geomDataAPI_Point2D(aCircle.attribute("circle_center"))
    if len(theCenter):
        assertPoint(aCenter, theCenter)

    aRadius = aCircle.real("circle_radius")
    assert aRadius.value() == theRadius, "Wrong circle radius {}, expected {}".format(aRadius.value(), theRadius)


def assertArc(theArc, theCenter, theStart, theEnd):
    """ Verifies coordinates of arc points and the consistency of the arc.
        Some of points may be empty lists.
    """
    anArc = tools.toSketchFeature(theArc)

    aCenterPnt = geomDataAPI_Point2D(anArc.attribute("center_point"))
    aStartPnt = geomDataAPI_Point2D(anArc.attribute("start_point"))
    aEndPnt = geomDataAPI_Point2D(anArc.attribute("end_point"))
    if len(theCenter):
        assertPoint(aCenterPnt, theCenter)
    if len(theStart):
        assertPoint(aStartPnt, theStart)
    if len(theEnd):
        assertPoint(aEndPnt, theEnd)

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
    assert math.fabs(aRadius.value() - aDistCS) < TOLERANCE, "Wrong arc: radius is {0}, expected {1}".format(aRadius.value(), aDistCS)
