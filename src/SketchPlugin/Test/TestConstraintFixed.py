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
    TestConstraintRigid.py
    Unit test of SketchPlugin_ConstraintRigid class

    SketchPlugin_Constraint
        static const std::string MY_CONSTRAINT_VALUE("ConstraintValue");
        static const std::string MY_FLYOUT_VALUE_PNT("ConstraintFlyoutValuePnt");
        static const std::string MY_ENTITY_A("ConstraintEntityA");
        static const std::string MY_ENTITY_B("ConstraintEntityB");
        static const std::string MY_ENTITY_C("ConstraintEntityC");
        static const std::string MY_ENTITY_D("ConstraintEntityD");

    SketchPlugin_ConstraintRigid
        static const std::string MY_CONSTRAINT_RIGID_ID("SketchConstraintRigid");
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());

"""
from GeomDataAPI import *
from ModelAPI import *
from GeomAPI import *
from salome.shaper import model

#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2014-11-21"

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
# Create a triangle ABC
#=========================================================================
aSession.startOperation()
aSketchLineA = aSketchFeature.addFeature("SketchLine")
aSketchLineB = aSketchFeature.addFeature("SketchLine")
aSketchLineC = aSketchFeature.addFeature("SketchLine")
aLineAStartPoint = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(aSketchLineA.attribute("EndPoint"))
aLineBStartPoint = geomDataAPI_Point2D(aSketchLineB.attribute("StartPoint"))
aLineBEndPoint = geomDataAPI_Point2D(aSketchLineB.attribute("EndPoint"))
aLineCStartPoint = geomDataAPI_Point2D(aSketchLineC.attribute("StartPoint"))
aLineCEndPoint = geomDataAPI_Point2D(aSketchLineC.attribute("EndPoint"))
aLineAStartPoint.setValue(25., 25.)
aLineAEndPoint.setValue(100., 25.)
aLineBStartPoint.setValue(100., 25.)
aLineBEndPoint.setValue(60., 75.)
aLineCStartPoint.setValue(60., 75.)
aLineCEndPoint.setValue(25., 25.)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 12)
# Store initial values of lines
kLineAStart = (aLineAStartPoint.x(), aLineAStartPoint.y())
kLineAEnd = (aLineAEndPoint.x(),   aLineAEndPoint.y())
kLineBStart = (aLineBStartPoint.x(), aLineBStartPoint.y())
kLineBEnd = (aLineBEndPoint.x(),   aLineBEndPoint.y())
kLineCStart = (aLineCStartPoint.x(), aLineCStartPoint.y())
kLineCEnd = (aLineCEndPoint.x(),   aLineCEndPoint.y())
#=========================================================================
# Link triange lines with concidence
#=========================================================================
concidenceLinks = list(zip([aLineBStartPoint, aLineCStartPoint, aLineAStartPoint],
                      [aLineAEndPoint, aLineBEndPoint, aLineCEndPoint]))
aSession.startOperation()
for eachLink in concidenceLinks:
    aConstraint = aSketchFeature.addFeature("SketchConstraintCoincidence")
    reflistA = aConstraint.refattr("ConstraintEntityA")
    reflistB = aConstraint.refattr("ConstraintEntityB")
    reflistA.setAttr(eachLink[0])
    reflistB.setAttr(eachLink[1])
    aConstraint.execute()
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 6)

# Check that constarints doesn't affected lines' values
assert (kLineAStart == (aLineAStartPoint.x(), aLineAStartPoint.y()))
assert (kLineAEnd == (aLineAEndPoint.x(),   aLineAEndPoint.y()))
assert (kLineBStart == (aLineBStartPoint.x(), aLineBStartPoint.y()))
assert (kLineBEnd == (aLineBEndPoint.x(),   aLineBEndPoint.y()))
assert (kLineCStart == (aLineCStartPoint.x(), aLineCStartPoint.y()))
assert (kLineCEnd == (aLineCEndPoint.x(),   aLineCEndPoint.y()))
#=========================================================================
# Make line A rigid
#=========================================================================
aSession.startOperation()
aRigidConstraint = aSketchFeature.addFeature("SketchConstraintRigid")
eachRefattr = aRigidConstraint.refattr("ConstraintEntityA")
lineResults = aSketchLineA.results()
for eachResult in lineResults:
    if eachResult.shape().isEdge():
        break
aResult = modelAPI_ResultConstruction(eachResult)
assert (aResult is not None)
eachRefattr.setObject(aResult)
# aRigidConstraint.execute()
aSession.finishOperation()

# Check that constarints doesn't affected lines' values
assert ((aLineAStartPoint.x(), aLineAStartPoint.y()) == (aLineCEndPoint.x(), aLineCEndPoint.y()))
assert ((aLineBStartPoint.x(), aLineBStartPoint.y()) == (aLineAEndPoint.x(), aLineAEndPoint.y()))
assert ((aLineCStartPoint.x(), aLineCStartPoint.y()) == (aLineBEndPoint.x(), aLineBEndPoint.y()))
assert (model.dof(aSketchFeature) == 2)

#=========================================================================
# Check that moving line A does not affect lines
#=========================================================================
aSession.startOperation()
aLineAEndPoint.setValue(90., 0.)
aSession.finishOperation()
# Check that constarint keep features' values
assert ((aLineAStartPoint.x(), aLineAStartPoint.y()) == (aLineCEndPoint.x(), aLineCEndPoint.y()))
assert ((aLineBStartPoint.x(), aLineBStartPoint.y()) == (aLineAEndPoint.x(), aLineAEndPoint.y()))
assert ((aLineCStartPoint.x(), aLineCStartPoint.y()) == (aLineBEndPoint.x(), aLineBEndPoint.y()))
assert (model.dof(aSketchFeature) == 2)

#=========================================================================
# Check that moving line B does not affect lines
#=========================================================================
aSession.startOperation()
aLineBEndPoint.setValue(90., 150.)
aSession.finishOperation()
# Check that constarint keep features' values
assert ((aLineAStartPoint.x(), aLineAStartPoint.y()) == (aLineCEndPoint.x(), aLineCEndPoint.y()))
assert ((aLineBStartPoint.x(), aLineBStartPoint.y()) == (aLineAEndPoint.x(), aLineAEndPoint.y()))
assert ((aLineCStartPoint.x(), aLineCStartPoint.y()) == (aLineBEndPoint.x(), aLineBEndPoint.y()))
assert (model.dof(aSketchFeature) == 2)

#=========================================================================
# Remove constraint, move line to check that it is not fixed
#=========================================================================
aSession.startOperation()
aDocument.removeFeature(aRigidConstraint)
aLineBEndPoint.setValue(90., 150.)
aSession.finishOperation()
assert ((aLineAStartPoint.x(), aLineAStartPoint.y()) == (aLineCEndPoint.x(), aLineCEndPoint.y()))
assert ((aLineBStartPoint.x(), aLineBStartPoint.y()) == (aLineAEndPoint.x(), aLineAEndPoint.y()))
assert ((aLineCStartPoint.x(), aLineCStartPoint.y()) == (aLineBEndPoint.x(), aLineBEndPoint.y()))
assert (model.dof(aSketchFeature) == 6)

#=========================================================================
# Create circle, fix it and check the circle is not moved
#=========================================================================
aCenter = [10., 10.]
aRadius = 5.
aSession.startOperation()
aCircle = aSketchFeature.addFeature("SketchCircle")
aCircleCenter = geomDataAPI_Point2D(aCircle.attribute("circle_center"))
aCircleRadius = aCircle.real("circle_radius")
aCircleCenter.setValue(aCenter[0], aCenter[1])
aCircleRadius.setValue(aRadius)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 9)
# fixed constraints
aSession.startOperation()
aRigidConstraint = aSketchFeature.addFeature("SketchConstraintRigid")
aRigidConstraint.refattr("ConstraintEntityA").setObject(aCircle.lastResult())
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 6)
# move center of circle
aSession.startOperation()
aCircleCenter.setValue(aCenter[0] + 1., aCenter[1] - 1.)
aSession.finishOperation()
assert (aCircleCenter.x() == aCenter[0] and aCircleCenter.y() == aCenter[1])
assert (aCircleRadius.value() == aRadius)
assert (model.dof(aSketchFeature) == 6)
# change radius of circle
aSession.startOperation()
aCircleRadius.setValue(aRadius + 3.)
aSession.finishOperation()
assert (aCircleCenter.x() == aCenter[0] and aCircleCenter.y() == aCenter[1])
assert (aCircleRadius.value() == aRadius)
assert (model.dof(aSketchFeature) == 6)

#=========================================================================
# Remove Fixed constraint and check the circle can be moved
#=========================================================================
aSession.startOperation()
aDocument.removeFeature(aRigidConstraint)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 9)
# move center of circle
aCenter = [aCenter[0] + 1., aCenter[1] - 1.]
aSession.startOperation()
aCircleCenter.setValue(aCenter[0], aCenter[1])
aSession.finishOperation()
assert (aCircleCenter.x() == aCenter[0] and aCircleCenter.y() == aCenter[1])
assert (aCircleRadius.value() == aRadius)
assert (model.dof(aSketchFeature) == 9)
# change radius of circle
aRadius = aRadius + 3.
aSession.startOperation()
aCircleRadius.setValue(aRadius)
aSession.finishOperation()
assert (aCircleCenter.x() == aCenter[0] and aCircleCenter.y() == aCenter[1])
assert (aCircleRadius.value() == aRadius)
assert (model.dof(aSketchFeature) == 9)

#=========================================================================
# Create arc, fix it and check it is not moved
#=========================================================================
aCenter = [10., 10.]
aStart = [5., 10.]
aEnd = [10., 15.]
aSession.startOperation()
anArc = aSketchFeature.addFeature("SketchArc")
anArcCenter = geomDataAPI_Point2D(anArc.attribute("center_point"))
anArcStart = geomDataAPI_Point2D(anArc.attribute("start_point"))
anArcEnd = geomDataAPI_Point2D(anArc.attribute("end_point"))
anArcCenter.setValue(aCenter[0], aCenter[1])
anArcStart.setValue(aStart[0], aStart[1])
anArcEnd.setValue(aEnd[0], aEnd[1])
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 14)
# fixed constraints
aSession.startOperation()
aRigidConstraint = aSketchFeature.addFeature("SketchConstraintRigid")
aRigidConstraint.refattr("ConstraintEntityA").setObject(anArc.lastResult())
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 9)
# move center of arc
aSession.startOperation()
anArcCenter.setValue(aCenter[0] + 1., aCenter[1] - 1.)
aSession.finishOperation()
assert (anArcCenter.x() == aCenter[0] and anArcCenter.y() == aCenter[1])
assert (anArcStart.x() == aStart[0] and anArcStart.y() == aStart[1])
assert (anArcEnd.x() == aEnd[0] and anArcEnd.y() == aEnd[1])
assert (model.dof(aSketchFeature) == 9)
# move start point of arc
aSession.startOperation()
anArcStart.setValue(aStart[0] + 1., aStart[1] - 1.)
aSession.finishOperation()
assert (anArcCenter.x() == aCenter[0] and anArcCenter.y() == aCenter[1])
assert (anArcStart.x() == aStart[0] and anArcStart.y() == aStart[1])
assert (anArcEnd.x() == aEnd[0] and anArcEnd.y() == aEnd[1])
assert (model.dof(aSketchFeature) == 9)
# move end point of arc
aSession.startOperation()
anArcEnd.setValue(aEnd[0] + 1., aEnd[1] - 1.)
aSession.finishOperation()
assert (anArcCenter.x() == aCenter[0] and anArcCenter.y() == aCenter[1])
assert (anArcStart.x() == aStart[0] and anArcStart.y() == aStart[1])
assert (anArcEnd.x() == aEnd[0] and anArcEnd.y() == aEnd[1])
assert (model.dof(aSketchFeature) == 9)

#=========================================================================
# Remove Fixed constraint and check the arc can be moved
#=========================================================================
aSession.startOperation()
aDocument.removeFeature(aRigidConstraint)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 14)
# move center of arc
aCenter = [anArcCenter.x(), anArcCenter.y()]
aSession.startOperation()
anArcCenter.setValue(aCenter[0] + 1., aCenter[1] - 1.)
aSession.finishOperation()
assert (anArcCenter.x() != aCenter[0] or anArcCenter.y() != aCenter[1])
assert (model.dof(aSketchFeature) == 14)
# move start point of arc
aStart = [anArcStart.x(), anArcStart.y()]
aSession.startOperation()
anArcStart.setValue(aStart[0] + 1., aStart[1] - 1.)
aSession.finishOperation()
assert (anArcStart.x() != aStart[0] or anArcStart.y() != aStart[1])
assert (model.dof(aSketchFeature) == 14)
# move end point of arc
aEnd = [anArcEnd.x(), anArcEnd.y()]
aSession.startOperation()
anArcEnd.setValue(aEnd[0] + 1., aEnd[1] - 1.)
aSession.finishOperation()
assert (anArcEnd.x() != aEnd[0] or anArcEnd.y() != aEnd[1])
assert (model.dof(aSketchFeature) == 14)

#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
