# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
    TestConstraintLength.py
    Unit test of SketchPlugin_ConstraintLength class

    SketchPlugin_ConstraintLength
        static const std::string MY_CONSTRAINT_LENGTH_ID("SketchConstraintLength");
        data()->addAttribute(SketchPlugin_Constraint::VALUE(),    ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());

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
# Create a line with length 100
#=========================================================================
aSession.startOperation()
aSketchLineA = aSketchFeature.addFeature("SketchLine")
aLineAStartPoint = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(aSketchLineA.attribute("EndPoint"))
aLineAStartPoint.setValue(0., 25.)
aLineAEndPoint.setValue(100., 25.)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 4)
#=========================================================================
# Make a constraint to keep the length
#=========================================================================
aSession.startOperation()
aLengthConstraint = aSketchFeature.addFeature("SketchConstraintLength")
refattrA = aLengthConstraint.refattr("ConstraintEntityA")
aLength = aLengthConstraint.real("ConstraintValue")
assert (not refattrA.isInitialized())
assert (not aLength.isInitialized())

aResult = aSketchLineA.firstResult()
assert (aResult is not None)
refattrA.setObject(modelAPI_ResultConstruction(aResult))
# aLength.setValue(100.)
aLengthConstraint.execute()
aSession.finishOperation()
assert (aLength.isInitialized())
assert (refattrA.isInitialized())
assert (model.dof(aSketchFeature) == 3)
#=========================================================================
# Check values and move one constrainted object
#=========================================================================
deltaX = 40.
# move start point, check that end point are moved also
assert (aLineAStartPoint.x() == 0)
assert (aLineAStartPoint.y() == 25)
assert (aLineAEndPoint.x() == 100)
assert (aLineAEndPoint.y() == 25)
aSession.startOperation()
aLineAStartPoint.setValue(aLineAStartPoint.x() + deltaX,
                          aLineAStartPoint.y())
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 3)
assert (aLineAStartPoint.y() == 25)
assert (aLineAEndPoint.y() == 25)
# length of the line is the same
assert (math.fabs(aLineAEndPoint.x() - aLineAStartPoint.x() - 100) < 1.e-10)
#=========================================================================
# Change the length value of the constraint
#=========================================================================
aSession.startOperation()
aLength.setValue(140.)
aLengthConstraint.execute()
aSession.finishOperation()
assert (math.fabs(aLineAEndPoint.x() - aLineAStartPoint.x() - 140) < 1.e-10)
assert (model.dof(aSketchFeature) == 3)
#=========================================================================
# TODO: improve test
# 1. remove constraint, move line's start point to
#    check that constraint are not applied
#=========================================================================
#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
