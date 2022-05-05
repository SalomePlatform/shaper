# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
    TestConstraintParallel.py
    Unit test of SketchPlugin_ConstraintParallel class

    SketchPlugin_ConstraintParallel
        static const std::string MY_CONSTRAINT_PARALLEL_ID("SketchConstraintParallel");
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());

"""
from GeomDataAPI import *
from ModelAPI import *
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
# Create two lines which are not parallel
#=========================================================================
aSession.startOperation()
# line A
aSketchLineA = aSketchFeature.addFeature("SketchLine")
aLineAStartPoint = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(aSketchLineA.attribute("EndPoint"))
aSketchLineB = aSketchFeature.addFeature("SketchLine")
aLineAStartPoint.setValue(0., 25)
aLineAEndPoint.setValue(85., 25)
# line B
aLineBStartPoint = geomDataAPI_Point2D(aSketchLineB.attribute("StartPoint"))
aLineBEndPoint = geomDataAPI_Point2D(aSketchLineB.attribute("EndPoint"))
aLineBStartPoint.setValue(0., 50)
aLineBEndPoint.setValue(80., 75)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 8)
#=========================================================================
# Make a constraint to keep the length of the line constant
# to parallel perpendicular constraint collapsing line to point
#=========================================================================
aSession.startOperation()
for eachFeature in [aSketchLineA, aSketchLineB]:
    aLengthConstraint = aSketchFeature.addFeature("SketchConstraintLength")
    refattrA = aLengthConstraint.refattr("ConstraintEntityA")
    aResultA = modelAPI_ResultConstruction(eachFeature.firstResult())
    assert (aResultA is not None)
    refattrA.setObject(aResultA)
    aLengthConstraint.execute()
aSession.finishOperation()
# Coordinates of lines should not be changed after this constraint
assert (aLineAStartPoint.x() == 0)
assert (aLineAStartPoint.y() == 25)
assert (aLineAEndPoint.x() == 85)
assert (aLineAEndPoint.y() == 25)
assert (aLineBStartPoint.x() == 0)
assert (aLineBStartPoint.y() == 50)
assert (aLineBEndPoint.x() == 80)
assert (aLineBEndPoint.y() == 75)
assert (model.dof(aSketchFeature) == 6)
#=========================================================================
# Link lines with parallel constraint
#=========================================================================
aSession.startOperation()
aParallelConstraint = aSketchFeature.addFeature("SketchConstraintParallel")
refattrA = aParallelConstraint.refattr("ConstraintEntityA")
refattrB = aParallelConstraint.refattr("ConstraintEntityB")
# aResultA is already defined for the length constraint
aResultA = modelAPI_ResultConstruction(aSketchLineA.firstResult())
aResultB = modelAPI_ResultConstruction(aSketchLineB.firstResult())
assert (aResultA is not None)
assert (aResultB is not None)
refattrA.setObject(aResultA)
refattrB.setObject(aResultB)
aParallelConstraint.execute()
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 5)
#=========================================================================
# Check values and move one constrainted object
#=========================================================================
deltaX = deltaY = 10.
# rotate line, check that reference's line points are moved also
aLineBStartPointPrev = (aLineBStartPoint.x(), aLineBStartPoint.y())
aLineBEndPointPrev = (aLineBEndPoint.x(), aLineBEndPoint.y())
aSession.startOperation()
aLineAStartPoint.setValue(aLineAStartPoint.x() + deltaX,
                          aLineAStartPoint.y() + deltaY)
aLineAEndPoint.setValue(aLineAEndPoint.x() - deltaX,
                        aLineAEndPoint.y() - deltaY)
aSession.finishOperation()
aLineBStartPointNew = (aLineBStartPoint.x(), aLineBStartPoint.y())
aLineBEndPointNew = (aLineBEndPoint.x(), aLineBEndPoint.y())

assert (aLineBStartPointPrev != aLineBStartPointNew)
assert (aLineBEndPointPrev != aLineBEndPointNew)
assert (model.dof(aSketchFeature) == 5)
#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
