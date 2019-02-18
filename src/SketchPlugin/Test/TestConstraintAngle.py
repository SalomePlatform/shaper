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
    TestConstraintAngle.py
    Unit test of SketchPlugin_ConstraintAngle class

    SketchPlugin_ConstraintAngle
        static const std::string MY_CONSTRAINT_ANGLE_ID("SketchConstraintAngle");
        data()->addAttribute(SketchPlugin_Constraint::VALUE(),    ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());


"""
from GeomDataAPI import *
from ModelAPI import *
import math
from salome.shaper import model

#=========================================================================
# Auxiliary functions
#=========================================================================

def angle(theLine1, theLine2):
    # subroutine to angle between two lines
    aStartPoint1 = geomDataAPI_Point2D(theLine1.attribute("StartPoint"))
    aEndPoint1   = geomDataAPI_Point2D(theLine1.attribute("EndPoint"))
    aStartPoint2 = geomDataAPI_Point2D(theLine2.attribute("StartPoint"))
    aEndPoint2   = geomDataAPI_Point2D(theLine2.attribute("EndPoint"))

    aDirX1 = aEndPoint1.x() - aStartPoint1.x()
    aDirY1 = aEndPoint1.y() - aStartPoint1.y()
    aLen1 = math.hypot(aDirX1, aDirY1)
    aDirX2 = aEndPoint2.x() - aStartPoint2.x()
    aDirY2 = aEndPoint2.y() - aStartPoint2.y()
    aLen2 = math.hypot(aDirX2, aDirY2)

    aDot = aDirX1 * aDirX2 + aDirY1 * aDirY2

    anAngle = math.acos(aDot / aLen1 / aLen2)
    return round(anAngle * 180. / math.pi, 6)


#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2015-09-18"

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
aStartPoint = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aEndPoint = geomDataAPI_Point2D(aSketchLineA.attribute("EndPoint"))
aStartPoint.setValue(-10., 25.)
aEndPoint.setValue(100., 25.)

aSketchLineB = aSketchFeature.addFeature("SketchLine")
aStartPoint = geomDataAPI_Point2D(aSketchLineB.attribute("StartPoint"))
aEndPoint = geomDataAPI_Point2D(aSketchLineB.attribute("EndPoint"))
aStartPoint.setValue(-10., 15.)
aEndPoint.setValue(80., 50.)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 8)
#=========================================================================
# Make a constraint to keep the angle
#=========================================================================
ANGLE_DEGREE = 30.
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintAngle")
aConstraint.integer("AngleType").setValue(ANGLE_DIRECT)
anAngleVal = aConstraint.real("AngleValue")
refattrA = aConstraint.refattr("ConstraintEntityA")
refattrB = aConstraint.refattr("ConstraintEntityB")
assert (not anAngleVal.isInitialized())
assert (not refattrA.isInitialized())
assert (not refattrB.isInitialized())
refattrA.setObject(aSketchLineA.firstResult())
refattrB.setObject(aSketchLineB.firstResult())
anAngleVal.setValue(ANGLE_DEGREE)
aConstraint.execute()
aSession.finishOperation()
aSession.startOperation()
aFlyoutPoint = geomDataAPI_Point2D(aConstraint.attribute("ConstraintFlyoutValuePnt"))
aFlyoutPoint.setValue(50.0, 100.0)
aSession.finishOperation()
aSession.abortOperation()
assert (anAngleVal.isInitialized())
assert (refattrA.isInitialized())
assert (refattrB.isInitialized())
assert (angle(aSketchLineA, aSketchLineB) == ANGLE_DEGREE)
assert (model.dof(aSketchFeature) == 7)
#=========================================================================
# Move line, check that angle is constant
#=========================================================================
aSession.startOperation()
aStartPoint = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aStartPoint.setValue(0., -30.)
aConstraint.execute()
aSession.finishOperation()
assert (angle(aSketchLineA, aSketchLineB) == ANGLE_DEGREE)
assert (model.dof(aSketchFeature) == 7)
#=========================================================================
# Change angle value and check the lines are moved
#=========================================================================
NEW_ANGLE_DEGREE = 60.
aSession.startOperation()
anAngleVal.setValue(NEW_ANGLE_DEGREE)
aConstraint.execute()
aSession.finishOperation()
assert (angle(aSketchLineA, aSketchLineB) == NEW_ANGLE_DEGREE)
assert (model.dof(aSketchFeature) == 7)
#=========================================================================
# Change angle type
#=========================================================================
aSession.startOperation()
aConstraint.integer("AngleType").setValue(ANGLE_COMPLEMENTARY)
aSession.finishOperation()
assert (angle(aSketchLineA, aSketchLineB) == NEW_ANGLE_DEGREE)
aSession.startOperation()
aConstraint.integer("AngleType").setValue(ANGLE_BACKWARD)
aSession.finishOperation()
assert (angle(aSketchLineA, aSketchLineB) == NEW_ANGLE_DEGREE)
assert (model.dof(aSketchFeature) == 7)
#=========================================================================
# Remove constraint, move line's point to check the constraint is not applied
#=========================================================================
aSession.startOperation()
aDocument.removeFeature(aConstraint)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 8)
aSession.startOperation()
aStartPoint.setValue(-30., 0.)
aSession.finishOperation()
assert (angle(aSketchLineA, aSketchLineB) != NEW_ANGLE_DEGREE)
assert (model.dof(aSketchFeature) == 8)
#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
