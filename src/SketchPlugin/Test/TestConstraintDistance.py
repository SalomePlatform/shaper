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
    TestConstraintDistance.py
    Unit test of SketchPlugin_ConstraintDistance class

    SketchPlugin_Constraint
        static const std::string MY_CONSTRAINT_VALUE("ConstraintValue");
        static const std::string MY_FLYOUT_VALUE_PNT("ConstraintFlyoutValuePnt");
        static const std::string MY_ENTITY_A("ConstraintEntityA");
        static const std::string MY_ENTITY_B("ConstraintEntityB");
        static const std::string MY_ENTITY_C("ConstraintEntityC");
        static const std::string MY_ENTITY_D("ConstraintEntityD");

    SketchPlugin_ConstraintDistance
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

__updated__ = "2014-10-28"


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
# Create a point and a line
#=========================================================================
aSession.startOperation()
aSketchPoint = aSketchFeature.addFeature("SketchPoint")
aSketchPointCoords = geomDataAPI_Point2D(
    aSketchPoint.attribute("PointCoordinates"))
aSketchPointCoords.setValue(50., 50.)
aSketchLine = aSketchFeature.addFeature("SketchLine")
aLineAStartPoint = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
aLineAStartPoint.setValue(0., 25.)
aLineAEndPoint.setValue(100., 25.)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 6)
#=========================================================================
# Make a constraint to keep the distance
#=========================================================================
PT_PT_DIST = 25.
aDist = model.distancePointPoint(aSketchPointCoords, aLineAStartPoint);
assert (aDist != PT_PT_DIST)
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintDistance")
aDistance = aConstraint.real("ConstraintValue")
refattrA = aConstraint.refattr("ConstraintEntityA")
refattrB = aConstraint.refattr("ConstraintEntityB")
assert (not aDistance.isInitialized())
assert (not refattrA.isInitialized())
assert (not refattrB.isInitialized())
aLineResult = aSketchLine.firstResult()
assert (aLineResult is not None)
# the following line is necessary to check automatic calculation of a distance
aConstraint.execute()
refattrA.setAttr(aSketchPointCoords)
refattrB.setAttr(aLineAStartPoint)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 5)
# set flyout point then abort operation, after that check the Distance is correct
aSession.startOperation()
aFlyoutPoint = geomDataAPI_Point2D(aConstraint.attribute("ConstraintFlyoutValuePnt"))
aFlyoutPoint.setValue(50.0, 100.0)
aSession.abortOperation()
assert (refattrA.isInitialized())
assert (refattrB.isInitialized())
assert (aDistance.isInitialized())
assert math.fabs(aDistance.value() - aDist) < 1.e-4, "Distance values are different: {0} != {1}".format(aDistance.value(), aDist)
assert (model.dof(aSketchFeature) == 5)
#=========================================================================
# Change distance value
#=========================================================================
aSession.startOperation()
aDistance.setValue(PT_PT_DIST)
aSession.finishOperation()
assert (math.fabs(model.distancePointPoint(aSketchPointCoords, aLineAStartPoint) - PT_PT_DIST) < 1.e-10)
assert (model.dof(aSketchFeature) == 5)
#=========================================================================
# Move line, check that distance is constant
#=========================================================================
aSession.startOperation()
aLineAStartPoint.setValue(0., 40.)
aLineAEndPoint.setValue(100., 40.)
aSession.finishOperation()
assert (math.fabs(model.distancePointPoint(aSketchPointCoords, aLineAStartPoint) - PT_PT_DIST) < 1.e-10)
assert (model.dof(aSketchFeature) == 5)
#=========================================================================
# Remove constraint, check the points are unconstrained now
#=========================================================================
aSession.startOperation()
aDocument.removeFeature(aConstraint)
aSession.finishOperation()
aSession.startOperation()
aSketchPointCoords.setValue(0., 0.)
aSession.finishOperation()
assert (math.fabs(model.distancePointPoint(aSketchPointCoords, aLineAStartPoint) - PT_PT_DIST) > 1.e-10)
assert (model.dof(aSketchFeature) == 6)

#=========================================================================
# Add distance between point and line
#=========================================================================
PT_LINE_DIST = 50.
aDist = model.distancePointLine(aSketchPointCoords, aSketchLine)
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintDistance")
aDistance = aConstraint.real("ConstraintValue")
refattrA = aConstraint.refattr("ConstraintEntityA")
refattrB = aConstraint.refattr("ConstraintEntityB")
assert (not aDistance.isInitialized())
assert (not refattrA.isInitialized())
assert (not refattrB.isInitialized())
aLineResult = aSketchLine.firstResult()
assert (aLineResult is not None)
refattrA.setObject(aLineResult)
refattrB.setAttr(aSketchPointCoords)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 5)
# set flyout point then abort operation, after that check the Distance is correct
aSession.startOperation()
aFlyoutPoint = geomDataAPI_Point2D(aConstraint.attribute("ConstraintFlyoutValuePnt"))
aFlyoutPoint.setValue(50.0, 100.0)
aSession.abortOperation()
assert (refattrA.isInitialized())
assert (refattrB.isInitialized())
assert (aDistance.isInitialized())
assert math.fabs(aDistance.value() - aDist) < 1.e-4, "Distance values are different: {0} != {1}".format(aDistance.value(), aDist)
assert (model.dof(aSketchFeature) == 5)

#=========================================================================
# Swap point and line in the distance
#=========================================================================
aSession.startOperation()
refattrA.setAttr(aSketchPointCoords)
refattrB.setObject(aLineResult)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 5)
# set flyout point then abort operation, after that check the Distance is correct
aSession.startOperation()
aFlyoutPoint = geomDataAPI_Point2D(aConstraint.attribute("ConstraintFlyoutValuePnt"))
aFlyoutPoint.setValue(100.0, 100.0)
aSession.abortOperation()
assert (refattrA.isInitialized())
assert (refattrB.isInitialized())
assert (aDistance.isInitialized())
assert math.fabs(aDistance.value() - aDist) < 1.e-4, "Distance values are different: {0} != {1}".format(aDistance.value(), aDist)
assert (model.dof(aSketchFeature) == 5)
#=========================================================================
# Change distance value
#=========================================================================
aSession.startOperation()
aDistance.setValue(PT_LINE_DIST)
aSession.finishOperation()
assert (math.fabs(model.distancePointLine(aSketchPointCoords, aSketchLine) - PT_LINE_DIST) < 1.e-10)
assert (model.dof(aSketchFeature) == 5)
#=========================================================================
# Set distance between line boundaries
#=========================================================================
aSession.startOperation()
refattrA.setAttr(aLineAStartPoint)
refattrB.setAttr(aLineAEndPoint)
aSession.finishOperation()
assert (math.fabs(model.distancePointPoint(aLineAStartPoint, aLineAEndPoint) - PT_LINE_DIST) < 1.e-10)
assert (model.dof(aSketchFeature) == 5)
#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
