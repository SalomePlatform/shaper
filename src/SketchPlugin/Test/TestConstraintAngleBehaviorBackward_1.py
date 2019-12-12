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

from GeomDataAPI import *
from ModelAPI import *
import math
from salome.shaper import model


#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2019-12-12"

ANGLE_DIRECT = 0
ANGLE_COMPLEMENTARY = 1
ANGLE_BACKWARD = 2

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
# Create two lines
#=========================================================================
aSession.startOperation()
aSketchLineA = aSketchFeature.addFeature("SketchLine")
aStartPointA = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aEndPointA = geomDataAPI_Point2D(aSketchLineA.attribute("EndPoint"))
aStartPointA.setValue(-100., 25.)
aEndPointA.setValue(100., -25.)

aSketchLineB = aSketchFeature.addFeature("SketchLine")
aStartPointB = geomDataAPI_Point2D(aSketchLineB.attribute("StartPoint"))
aEndPointB = geomDataAPI_Point2D(aSketchLineB.attribute("EndPoint"))
aStartPointB.setValue(-100., -25.)
aEndPointB.setValue(100., 25.)
aSession.finishOperation()
#=========================================================================
# Make a constraint to keep the angle
#=========================================================================
ANGLE_DEGREE = 330.
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintAngle")
anAngleType = aConstraint.integer("AngleType")
anAngleType.setValue(ANGLE_DIRECT)
geomDataAPI_Point2D(aConstraint.attribute("SelectedPointA")).setValue(aStartPointA.pnt())
geomDataAPI_Point2D(aConstraint.attribute("SelectedPointB")).setValue(aStartPointB.pnt())
aConstraint.refattr("ConstraintEntityA").setObject(aSketchLineA.firstResult())
aConstraint.refattr("ConstraintEntityB").setObject(aSketchLineB.firstResult())
anAngleVal = aConstraint.real("AngleValue")
anAngleVal.setValue(360. - ANGLE_DEGREE)
aSession.finishOperation()
#=========================================================================
# Change the type of the constraint
#=========================================================================
aSession.startOperation()
anAngleType.setValue(ANGLE_BACKWARD)
aSession.finishOperation()
#=========================================================================
# Move presentation of the angle and check the angle value
#=========================================================================
pointsA = [aStartPointA.pnt(), aEndPointA.pnt()]
pointsB = [aStartPointB.pnt(), aEndPointB.pnt()]
refs = [(pointsA[0], pointsB[1], 540. - ANGLE_DEGREE),
        (pointsA[1], pointsB[1], ANGLE_DEGREE),
        (pointsA[1], pointsB[0], 540. - ANGLE_DEGREE),
        (pointsA[0], pointsB[0], ANGLE_DEGREE)
       ]
aFlyoutPoint = geomDataAPI_Point2D(aConstraint.attribute("ConstraintFlyoutValuePnt"))
for ref in refs:
    aSession.startOperation()
    aFlyoutPoint.setValue(0.5 * (ref[0].x() + ref[1].x()), 0.5 * (ref[0].y() + ref[1].y()))
    aSession.finishOperation()
    assert(anAngleType.value() == ANGLE_BACKWARD)
    assert(anAngleVal.value() == ref[2])
    assert(aStartPointA.x() == pointsA[0].x() and aStartPointA.y() == pointsA[0].y())
    assert(aEndPointA.x() == pointsA[1].x() and aEndPointA.y() == pointsA[1].y())
    assert(aStartPointB.x() == pointsB[0].x() and aStartPointB.y() == pointsB[0].y())
    assert(aEndPointB.x() == pointsB[1].x() and aEndPointB.y() == pointsB[1].y())
#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
