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

"""
    TestConstraintDistanceHorizontal.py
    Unit test of SketchPlugin_ConstraintDistanceHorizontal class

    SketchPlugin_ConstraintDistanceHorizontal
        static const std::string MY_CONSTRAINT_DISTANCE_ID("SketchConstraintDistance");
        data()->addAttribute(SketchPlugin_Constraint::VALUE(),    ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
"""

from GeomDataAPI import *
from ModelAPI import *
import math
from salome.shaper import model

#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2017-05-10"


def horizontalDistance(point1, point2):
    """
    subroutine to calculate signed distance between two points
    """
    return point2.x() - point1.x()

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
# Create two movable and one fixed point
#=========================================================================
aSession.startOperation()
aPoint1 = aSketchFeature.addFeature("SketchPoint")
aPoint1Coords = geomDataAPI_Point2D(aPoint1.attribute("PointCoordinates"))
aPoint1Coords.setValue(50., 50.)
aSession.finishOperation()
aSession.startOperation()
aPoint2 = aSketchFeature.addFeature("SketchPoint")
aPoint2Coords = geomDataAPI_Point2D(aPoint2.attribute("PointCoordinates"))
aPoint2Coords.setValue(70., 70.)
aSession.finishOperation()
aSession.startOperation()
anOriginResult = modelAPI_Result(aDocument.objectByName("Construction", "Origin"))
anOriginShape = anOriginResult.shape()
anExtPoint = aSketchFeature.addFeature("SketchPoint")
anExtCoords = geomDataAPI_Point2D(anExtPoint.attribute("PointCoordinates"))
anExtCoords.setValue(0., 0.)
anExtPoint.selection("External").setValue(anOriginResult, anOriginShape)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 4)

#=========================================================================
# Create a constraint to keep horizontal distance between movable points
#=========================================================================
DISTANCE1 = 25.
aSession.startOperation()
aHDist1 = aSketchFeature.addFeature("SketchConstraintDistanceHorizontal")
aDistance = aHDist1.real("ConstraintValue")
refattrA = aHDist1.refattr("ConstraintEntityA")
refattrB = aHDist1.refattr("ConstraintEntityB")
assert (not aDistance.isInitialized())
assert (not refattrA.isInitialized())
assert (not refattrB.isInitialized())
refattrA.setObject(aPoint1.lastResult())
refattrB.setObject(aPoint2.lastResult())
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 3)
# set flyout point then abort operation, after that check the Distance is correct
aSession.startOperation()
aFlyoutPoint = geomDataAPI_Point2D(aHDist1.attribute("ConstraintFlyoutValuePnt"))
aFlyoutPoint.setValue(50.0, 100.0)
aSession.abortOperation()
assert (refattrA.isInitialized())
assert (refattrB.isInitialized())
assert (aDistance.isInitialized())
aSession.startOperation()
aDistance.setValue(DISTANCE1)
aSession.finishOperation()
assert math.fabs(horizontalDistance(aPoint1Coords, aPoint2Coords) - DISTANCE1) < 1.e-5, "Distance values are different: {0} != {1}".format(horizontalDistance(aPoint1Coords, aPoint2Coords), DISTANCE1)
assert (model.dof(aSketchFeature) == 3)
#=========================================================================
# Change a distance value
#=========================================================================
d = DISTANCE1 + 20.
dStep = -5.
while d >= -30.:
    aSession.startOperation()
    DISTANCE1 = d
    aDistance.setValue(DISTANCE1)
    aSession.finishOperation()
    if DISTANCE1 == 0:
        assert(aHDist1.error() != "")
    else:
        assert math.fabs(horizontalDistance(aPoint1Coords, aPoint2Coords) - DISTANCE1) < 1.e-5, "Distance values are different: {0} != {1}".format(horizontalDistance(aPoint1Coords, aPoint2Coords), DISTANCE1)
    d += dStep
assert (model.dof(aSketchFeature) == 3)

#=========================================================================
# Create a constraint to keep horizontal distance between fixed and movable points
#=========================================================================
DISTANCE2 = 50.
aSession.startOperation()
aHDist2 = aSketchFeature.addFeature("SketchConstraintDistanceHorizontal")
aDistance = aHDist2.real("ConstraintValue")
refattrA = aHDist2.refattr("ConstraintEntityA")
refattrB = aHDist2.refattr("ConstraintEntityB")
assert (not aDistance.isInitialized())
assert (not refattrA.isInitialized())
assert (not refattrB.isInitialized())
refattrA.setObject(anExtPoint.lastResult())
refattrB.setAttr(aPoint1Coords)
aDistance.setValue(DISTANCE2)
aSession.finishOperation()
assert math.fabs(horizontalDistance(anExtCoords, aPoint1Coords) - DISTANCE2) < 1.e-5, "Distance values are different: {0} != {1}".format(horizontalDistance(anExtCoords, aPoint1Coords), DISTANCE2)
assert math.fabs(aPoint1Coords.x() - DISTANCE2) < 1.e-5, "Wrong point coordinates ({}, {}), expected x = {}".format(aPoint1Coords.x(), aPoint1Coords.y(), DISTANCE2)
assert (model.dof(aSketchFeature) == 2)
#=========================================================================
# Change a distance value (check previous constraint is applied too)
#=========================================================================
d = DISTANCE2
dStep = -5.
while d >= -50.:
    aSession.startOperation()
    DISTANCE2 = d
    aDistance.setValue(DISTANCE2)
    aSession.finishOperation()
    if DISTANCE2 == 0:
        assert(aHDist2.error() != "")
    else:
        assert math.fabs(horizontalDistance(anExtCoords, aPoint1Coords) - DISTANCE2) < 1.e-5, "Distance values are different: {0} != {1}".format(horizontalDistance(anExtCoords, aPoint1Coords), DISTANCE2)
        assert math.fabs(aPoint1Coords.x() - DISTANCE2) < 1.e-5, "Wrong point coordinates ({}, {}), expected x = {}".format(aPoint1Coords.x(), aPoint1Coords.y(), DISTANCE2)
        assert math.fabs(horizontalDistance(aPoint1Coords, aPoint2Coords) - DISTANCE1) < 1.e-5, "Distance values are different: {0} != {1}".format(horizontalDistance(aPoint1Coords, aPoint2Coords), DISTANCE1)
    d += dStep
assert (model.dof(aSketchFeature) == 2)

#=========================================================================
# Remove first distance
#=========================================================================
aStoredCoords = [aPoint2Coords.x(), aPoint2Coords.y()]
aSession.startOperation()
aDocument.removeFeature(aHDist1)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 3)
aSession.startOperation()
DISTANCE2 = 20.
aDistance.setValue(DISTANCE2)
aSession.finishOperation()
assert math.fabs(horizontalDistance(anExtCoords, aPoint1Coords) - DISTANCE2) < 1.e-5, "Distance values are different: {0} != {1}".format(horizontalDistance(anExtCoords, aPoint1Coords), DISTANCE2)
assert math.fabs(aPoint1Coords.x() - DISTANCE2) < 1.e-5, "Wrong point coordinates ({}, {}), expected x = {}".format(aPoint1Coords.x(), aPoint1Coords.y(), DISTANCE2)
assert aPoint2Coords.x() == aStoredCoords[0] and aPoint2Coords.y() == aStoredCoords[1]

#=========================================================================
# Create line and set horizontal distance between line boundaries
#=========================================================================
aSession.startOperation()
aLine = aSketchFeature.addFeature("SketchLine")
aStartPoint = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
aEndPoint = geomDataAPI_Point2D(aLine.attribute("EndPoint"))
aStartPoint.setValue(50., 0.)
aEndPoint.setValue(100., 20.)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 7)

DISTANCE3 = 50.
aSession.startOperation()
aHDist3 = aSketchFeature.addFeature("SketchConstraintDistanceHorizontal")
aDistance = aHDist3.real("ConstraintValue")
refattrA = aHDist3.refattr("ConstraintEntityA")
refattrB = aHDist3.refattr("ConstraintEntityB")
assert (not aDistance.isInitialized())
assert (not refattrA.isInitialized())
assert (not refattrB.isInitialized())
refattrA.setAttr(aStartPoint)
refattrB.setAttr(aEndPoint)
aDistance.setValue(DISTANCE3)
aSession.finishOperation()
assert math.fabs(horizontalDistance(aStartPoint, aEndPoint) - DISTANCE3) < 1.e-5, "Distance values are different: {0} != {1}".format(horizontalDistance(aStartPoint, aEndPoint), DISTANCE3)
assert (model.dof(aSketchFeature) == 6)
#=========================================================================
# Change a distance value
#=========================================================================
d = DISTANCE3
dStep = -5.
while d >= -50.:
    aSession.startOperation()
    DISTANCE3 = d
    aDistance.setValue(DISTANCE3)
    aSession.finishOperation()
    if DISTANCE3 == 0:
        assert(aHDist3.error() != "")
    else:
        assert math.fabs(horizontalDistance(aStartPoint, aEndPoint) - DISTANCE3) < 1.e-5, "Distance values are different: {0} != {1}".format(horizontalDistance(aStartPoint, aEndPoint), DISTANCE3)
    d += dStep
assert (model.dof(aSketchFeature) == 6)

#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
