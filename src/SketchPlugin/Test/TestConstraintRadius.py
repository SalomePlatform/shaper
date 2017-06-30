## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
    TestConstraintRadius.py
    Unit test of SketchPlugin_ConstraintRadius class

    SketchPlugin_Constraint
        static const std::string MY_CONSTRAINT_VALUE("ConstraintValue");
        static const std::string MY_FLYOUT_VALUE_PNT("ConstraintFlyoutValuePnt");
        static const std::string MY_ENTITY_A("ConstraintEntityA");
        static const std::string MY_ENTITY_B("ConstraintEntityB");
        static const std::string MY_ENTITY_C("ConstraintEntityC");
        static const std::string MY_ENTITY_D("ConstraintEntityD");

    SketchPlugin_ConstraintRadius
        static const std::string MY_CONSTRAINT_RADIUS_ID("SketchConstraintRadius");
        data()->addAttribute(SketchPlugin_Constraint::VALUE(),    ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());

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
# Creation of an arc and a circle
#=========================================================================
aSession.startOperation()
aSketchArc = aSketchFeature.addFeature("SketchArc")
anArcCentr = geomDataAPI_Point2D(aSketchArc.attribute("center_point"))
anArcCentr.setValue(10., 10.)
anArcStartPoint = geomDataAPI_Point2D(aSketchArc.attribute("start_point"))
anArcStartPoint.setValue(0., 50.)
anArcEndPoint = geomDataAPI_Point2D(aSketchArc.attribute("end_point"))
anArcEndPoint.setValue(50., 0.)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 5)
# Test changing the arc start/end point
aSession.startOperation()
anArcStartPoint.setValue(anArcStartPoint.x(), 40.)
anArcStartPoint.setValue(0., 50.)
assert (math.hypot(anArcStartPoint.x() - 0., anArcStartPoint.y() - 50.) < 1.e-10)
aSession.finishOperation()
aSession.startOperation()
anArcEndPoint.setValue(40., anArcEndPoint.y())
anArcEndPoint.setValue(50., 0.)
assert (math.hypot(anArcEndPoint.x() - 50., anArcEndPoint.y() - 0.) < 1.e-10)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 5)
# Circle
aSession.startOperation()
aSketchCircle = aSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("circle_center"))
aCircleRadius = aSketchCircle.real("circle_radius")
anCircleCentr.setValue(-25., -25)
aCircleRadius.setValue(25.)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 8)
#=========================================================================
# Make a constraint to keep the radius of the arc
#=========================================================================
RADIUS = 40
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintRadius")
aRadius = aConstraint.real("ConstraintValue")
aRefObject = aConstraint.refattr("ConstraintEntityA")
aResult = aSketchArc.lastResult()
assert (aResult is not None)
aRefObject.setObject(modelAPI_ResultConstruction(aResult))
aRadius.setValue(RADIUS)
aSession.finishOperation()
assert (aRadius.isInitialized())
assert (aRefObject.isInitialized())
assert (model.dof(aSketchFeature) == 7)
#=========================================================================
# Make a constraint to keep the radius of the circle
#=========================================================================
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintRadius")
aRadius = aConstraint.real("ConstraintValue")
aRefObject = aConstraint.refattr("ConstraintEntityA")
aResult = aSketchCircle.lastResult()
assert (aResult is not None)
aRefObject.setObject(modelAPI_ResultConstruction(aResult))
aRadius.setValue(RADIUS)
aSession.finishOperation()
assert (aRadius.isInitialized())
assert (aRefObject.isInitialized())
assert (model.dof(aSketchFeature) == 6)
#=========================================================================
# Perform some actions and checks:
# 1. Check that constraints does not change values
# 2. Move one point of the arc
# 3. Check that second point is moved also
#=========================================================================
distCS = model.distancePointPoint(anArcCentr, anArcStartPoint)
distCE = model.distancePointPoint(anArcCentr, anArcEndPoint)
assert (math.fabs(distCS - RADIUS) < 1.e-10)
assert (math.fabs(distCE - RADIUS) < 1.e-10)
anArcPrevEndPointX = anArcEndPoint.x()
anArcPrevEndPointY = anArcEndPoint.y()
# Move one point of the arc
aSession.startOperation()
anArcStartPoint.setValue(0, 60)
aSession.finishOperation()
assert (anArcEndPoint.x() != anArcPrevEndPointX)
assert (anArcEndPoint.y() != anArcPrevEndPointY)
distCS = model.distancePointPoint(anArcCentr, anArcStartPoint)
distCE = model.distancePointPoint(anArcCentr, anArcEndPoint)
assert (math.fabs(distCS - RADIUS) < 1.e-10)
assert (math.fabs(distCE - RADIUS) < 1.e-10)
assert (model.dof(aSketchFeature) == 6)
#=========================================================================
# 4. Move the centr or the point of the arc
# 5. Check radius is the same
#=========================================================================
assert (anCircleCentr.x() == -25)
assert (anCircleCentr.y() == -25)
assert (aCircleRadius.value() == RADIUS)
aSession.startOperation()
anCircleCentr.setValue(100., 100.)
aSession.finishOperation()
assert (anCircleCentr.x() == 100)
assert (anCircleCentr.y() == 100)
assert (aCircleRadius.value() == RADIUS)
assert (model.dof(aSketchFeature) == 6)
#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
