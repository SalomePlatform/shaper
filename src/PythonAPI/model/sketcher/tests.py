## Copyright (C) 2017  CEA/DEN, EDF R&D
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

from ModelAPI import *
from GeomDataAPI import *
import ModelHighAPI
import math
from salome.shaper.model.sketcher import tools

TOLERANCE = 1.e-7

def assertPoint(thePoint, theCoords):
    """ Verifies coordinates of the point
    """
    aPoint = tools.toList(thePoint)
    assert aPoint[0] == theCoords[0] and aPoint[1] == theCoords[1], "Wrong '{}' point {}, expected {}".format(thePoint.id(), aPoint, theCoords)


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
