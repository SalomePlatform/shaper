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
    TestConstraintCoincidence.py
    Unit test of SketchPlugin_ConstraintCoincidence class

    SketchPlugin_Constraint
        static const std::string MY_CONSTRAINT_VALUE("ConstraintValue");
        static const std::string MY_FLYOUT_VALUE_PNT("ConstraintFlyoutValuePnt");
        static const std::string MY_ENTITY_A("ConstraintEntityA");
        static const std::string MY_ENTITY_B("ConstraintEntityB");
        static const std::string MY_ENTITY_C("ConstraintEntityC");
        static const std::string MY_ENTITY_D("ConstraintEntityD");

    SketchPlugin_ConstraintCoincidence
        static const std::string MY_CONSTRAINT_COINCIDENCE_ID("SketchConstraintCoincidence");
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
TOLERANCE = 1.e-7


#=========================================================================
# Auxiliary functions
#=========================================================================
def checkPointOnLine(point, line):
    aStart = geomDataAPI_Point2D(line.attribute("StartPoint"))
    aEnd   = geomDataAPI_Point2D(line.attribute("EndPoint"))
    aDirX = aEnd.x() - aStart.x()
    aDirY = aEnd.y() - aStart.y()
    aVecX = point.x() - aStart.x()
    aVecY = point.y() - aStart.y()
    assert (math.fabs(aDirX * aVecY - aDirY * aVecX) <= TOLERANCE)

def checkPointOnCircle(point, circle):
    aCenter = geomDataAPI_Point2D(circle.attribute("circle_center"))
    aRadius = circle.real("circle_radius").value()
    aDist = math.hypot(point.x() - aCenter.x(), point.y() - aCenter.y())
    assert (math.fabs(aDist - aRadius) <= TOLERANCE)

def checkPointOnArc(point, arc):
    aStart  = geomDataAPI_Point2D(arc.attribute("start_point"))
    aCenter = geomDataAPI_Point2D(arc.attribute("center_point"))
    aRadius = math.hypot(aStart.x() - aCenter.x(), aStart.y() - aCenter.y())
    aDist = math.hypot(point.x() - aCenter.x(), point.y() - aCenter.y())
    assert (math.fabs(aDist - aRadius) <= TOLERANCE)


#=========================================================================
# Start of test
#=========================================================================
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
# add an origin
aSession.startOperation()
aFeature = aDocument.addFeature("Point")
geomDataAPI_Point(aFeature.attribute("point3d")).setValue(0., 0., 0.)
aFeature.string("creation_method").setValue("by_xyz")
anOriginName = aFeature.name()
aSession.finishOperation()
#=========================================================================
# Creation of a sketch
#=========================================================================
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aDocument.addFeature("Sketch"))
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSession.finishOperation()
#=========================================================================
# Create a line and an arc
#=========================================================================
aSession.startOperation()
aSketchArc = aSketchFeature.addFeature("SketchArc")
anArcCentr = geomDataAPI_Point2D(aSketchArc.attribute("center_point"))
anArcStartPoint = geomDataAPI_Point2D(aSketchArc.attribute("start_point"))
anArcEndPoint = geomDataAPI_Point2D(aSketchArc.attribute("end_point"))
anArcCentr.setValue(10., 10.)
anArcStartPoint.setValue(0., 50.)
anArcEndPoint.setValue(50., 0.)
aSketchLine = aSketchFeature.addFeature("SketchLine")
aLineStartPoint = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
aLineEndPoint = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
# Lets initialize line start at circle's end:
aLineStartPoint.setValue(50., 0.)
aLineEndPoint.setValue(100., 25.)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 9)
#=========================================================================
# Link arc's end and line's start points with concidence constraint
#=========================================================================
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintCoincidence")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setAttr(anArcEndPoint)
reflistB.setAttr(aLineStartPoint)
aConstraint.execute()
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 7)
#=========================================================================
# Check values and move one constrainted object
#=========================================================================
assert (anArcEndPoint.x() == aLineStartPoint.x())
assert (anArcEndPoint.y() == aLineStartPoint.y())
deltaX = deltaY = 40.
#  move line
aSession.startOperation()
aLineStartPoint.setValue(aLineStartPoint.x() + deltaX,
                         aLineStartPoint.y() + deltaY)
aLineEndPoint.setValue(aLineEndPoint.x() + deltaX,
                       aLineEndPoint.y() + deltaY)
aSession.finishOperation()
# check that arc's points are moved also
assert (anArcEndPoint.x() == aLineStartPoint.x())
assert (anArcEndPoint.y() == aLineStartPoint.y())
assert (model.dof(aSketchFeature) == 7)
#=========================================================================
# Remove coincidence and move the line
#=========================================================================
aSession.startOperation()
aDocument.removeFeature(aConstraint)
aSession.finishOperation()
aSession.startOperation()
aLineStartPoint.setValue(70., 0.)
aSession.finishOperation()
assert (anArcEndPoint.x() != aLineStartPoint.x() or anArcEndPoint.y() != aLineStartPoint.y())
assert (model.dof(aSketchFeature) == 9)

#=========================================================================
# Add constraint point-on-line
#=========================================================================
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintCoincidence")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setAttr(anArcStartPoint)
reflistB.setObject(aSketchLine.lastResult())
aConstraint.execute()
aSession.finishOperation()
checkPointOnLine(anArcStartPoint, aSketchLine)
assert (model.dof(aSketchFeature) == 8)
#=========================================================================
# Add constraint point-on-circle
#=========================================================================
aSession.startOperation()
# create circle with center coincident with origin
aSketchCircle = aSketchFeature.addFeature("SketchCircle")
aCircleCenter = geomDataAPI_Point2D(aSketchCircle.attribute("circle_center"))
aCircleRadius = aSketchCircle.real("circle_radius")
aCircleCenter.setValue(10., 10.)
aCircleRadius.setValue(25.)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 11)
# create origin
aSession.startOperation()
anOrigRes = modelAPI_Result(aDocument.objectByName("Construction", anOriginName))
assert (anOrigRes)
anOrigShape = anOrigRes.shape()
assert (anOrigShape)
anOrigin = aSketchFeature.addFeature("SketchPoint")
anOriginCoord = geomDataAPI_Point2D(anOrigin.attribute("PointCoordinates"))
anOriginCoord.setValue(0., 0.)
anOrigin.selection("External").setValue(anOrigRes, anOrigShape)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 11)
# coincidence between center of circle and the origin
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintCoincidence")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setAttr(aCircleCenter)
reflistB.setObject(anOrigin.lastResult())
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 9)
# point-on-circle
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintCoincidence")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setObject(aSketchCircle.lastResult())
reflistB.setAttr(aLineEndPoint)
aConstraint.execute()
aSession.finishOperation()
checkPointOnCircle(aLineEndPoint, aSketchCircle)
assert (model.dof(aSketchFeature) == 8)
#=========================================================================
# Add constraint point-on-arc
#=========================================================================
aSession.startOperation("constraint point-on-arc")
aConstraint = aSketchFeature.addFeature("SketchConstraintCoincidence")
reflistA = aConstraint.refattr("ConstraintEntityA")
reflistB = aConstraint.refattr("ConstraintEntityB")
reflistA.setAttr(aCircleCenter)
reflistB.setObject(aSketchArc.lastResult())
aConstraint.execute()
aSession.finishOperation()
checkPointOnArc(aCircleCenter, aSketchArc)
# check center of circle is still in origin
assert (aCircleCenter.x() == 0. and aCircleCenter.y() == 0.)
assert (model.dof(aSketchFeature) == 7)

#=========================================================================
# Create two more lines and set multi-coincidence between their extremities
#=========================================================================
aSession.startOperation()
# line 2
aLine2 = aSketchFeature.addFeature("SketchLine")
aLine2StartPoint = geomDataAPI_Point2D(aLine2.attribute("StartPoint"))
aLine2EndPoint = geomDataAPI_Point2D(aLine2.attribute("EndPoint"))
aLine2StartPoint.setValue(50., 0.)
aLine2EndPoint.setValue(100., 0.)
# line 3
aLine3 = aSketchFeature.addFeature("SketchLine")
aLine3StartPoint = geomDataAPI_Point2D(aLine3.attribute("StartPoint"))
aLine3EndPoint = geomDataAPI_Point2D(aLine3.attribute("EndPoint"))
aLine3StartPoint.setValue(50., 0.)
aLine3EndPoint.setValue(0., 100.)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 15)
# coincidences between extremities of lines
aSession.startOperation()
aConstraint12 = aSketchFeature.addFeature("SketchConstraintCoincidence")
refAttrA = aConstraint12.refattr("ConstraintEntityA")
refAttrB = aConstraint12.refattr("ConstraintEntityB")
refAttrA.setAttr(aLineStartPoint)
refAttrB.setAttr(aLine2StartPoint)
aConstraint23 = aSketchFeature.addFeature("SketchConstraintCoincidence")
refAttrA = aConstraint23.refattr("ConstraintEntityA")
refAttrB = aConstraint23.refattr("ConstraintEntityB")
refAttrA.setAttr(aLine2StartPoint)
refAttrB.setAttr(aLine3StartPoint)
aConstraint31 = aSketchFeature.addFeature("SketchConstraintCoincidence")
refAttrA = aConstraint31.refattr("ConstraintEntityA")
refAttrB = aConstraint31.refattr("ConstraintEntityB")
refAttrA.setAttr(aLine3StartPoint)
refAttrB.setAttr(aLineStartPoint)
aSession.finishOperation()
# check the points have same coordinates
assert (aLineStartPoint.x() == aLine2StartPoint.x() and aLineStartPoint.y() == aLine2StartPoint.y())
assert (aLineStartPoint.x() == aLine3StartPoint.x() and aLineStartPoint.y() == aLine3StartPoint.y())
assert (model.dof(aSketchFeature) == 11)
#=========================================================================
# Move one line and check other have been updated too
#=========================================================================
aSession.startOperation()
aLine3StartPoint.setValue(aLine3StartPoint.x() + deltaX,
                          aLine3StartPoint.y() + deltaY)
aLine3EndPoint.setValue(aLine3EndPoint.x() + deltaX,
                        aLine3EndPoint.y() + deltaY)
aSession.finishOperation()
assert (aLineStartPoint.x() == aLine2StartPoint.x() and aLineStartPoint.y() == aLine2StartPoint.y())
assert (aLineStartPoint.x() == aLine3StartPoint.x() and aLineStartPoint.y() == aLine3StartPoint.y())
assert (model.dof(aSketchFeature) == 11)
#=========================================================================
# Fix a line and move another connected segment
#=========================================================================
coordX = aLineStartPoint.x()
coordY = aLineStartPoint.y()
aSession.startOperation()
aFixed = aSketchFeature.addFeature("SketchConstraintRigid")
refAttrA = aFixed.refattr("ConstraintEntityA")
refAttrA.setObject(aLine2.lastResult())
aSession.finishOperation()
# move another line
aSession.startOperation()
aLine3StartPoint.setValue(aLine3StartPoint.x() + deltaX,
                          aLine3StartPoint.y() + deltaY)
aLine3EndPoint.setValue(aLine3EndPoint.x() + deltaX,
                        aLine3EndPoint.y() + deltaY)
aSession.finishOperation()
assert (aLineStartPoint.x() == coordX and aLineStartPoint.y() == coordY)
assert (aLine2StartPoint.x() == coordX and aLine2StartPoint.y() == coordY)
assert (aLine3StartPoint.x() == coordX and aLine3StartPoint.y() == coordY)
assert (model.dof(aSketchFeature) == 7)
#=========================================================================
# Detach fixed line and move one of remaining
#=========================================================================
aSession.startOperation()
aDocument.removeFeature(aConstraint12)
aDocument.removeFeature(aConstraint23)
aSession.finishOperation()
# move line
deltaX = 1.
deltaY = 0.
aSession.startOperation()
aLineStartPoint.setValue(aLineStartPoint.x() + deltaX,
                          aLineStartPoint.y() + deltaY)
aLineEndPoint.setValue(aLineEndPoint.x() + deltaX,
                        aLineEndPoint.y() + deltaY)
aSession.finishOperation()
assert (aLineStartPoint.x() != aLine2StartPoint.x() or aLineStartPoint.y() != aLine2StartPoint.y())
assert (aLineStartPoint.x() == aLine3StartPoint.x() and aLineStartPoint.y() == aLine3StartPoint.y())
assert (model.dof(aSketchFeature) == 9)

#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
