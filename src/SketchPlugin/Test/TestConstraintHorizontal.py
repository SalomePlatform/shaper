# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
    TestConstraintHorizontal.py
    Unit test of SketchPlugin_ConstraintHorizontal class

    SketchPlugin_ConstraintHorizontal
        static const std::string MY_CONSTRAINT_HORIZONTAL_ID("SketchConstraintHorizontal");
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());

"""
from GeomDataAPI import *
from ModelAPI import *
from salome.shaper import model

#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2015-03-16"

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
# Create non-horizontal line
#=========================================================================
aSession.startOperation()
aSketchLine = aSketchFeature.addFeature("SketchLine")
aLineStartPoint = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
aLineEndPoint = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
aLineStartPoint.setValue(0., 15.)
aLineEndPoint.setValue(20., 25.)
aSession.finishOperation()
assert(model.dof(aSketchFeature) == 4)
#=========================================================================
# Assign horizontal constraint for a line
#=========================================================================
aSession.startOperation()
aHorizontalConstraint = aSketchFeature.addFeature("SketchConstraintHorizontal")
refattrA = aHorizontalConstraint.refattr("ConstraintEntityA")
aResult = modelAPI_ResultConstruction(aSketchLine.firstResult())
assert (aResult is not None)
refattrA.setObject(aResult)
aHorizontalConstraint.execute()
aSession.finishOperation()
assert(aLineStartPoint.y() == aLineEndPoint.y())
assert(model.dof(aSketchFeature) == 3)
#=========================================================================
# Move one of boundary points of a line
#=========================================================================
deltaX = deltaY = 10.
aSession.startOperation()
aLineStartPoint.setValue(aLineStartPoint.x() + deltaX,
                         aLineStartPoint.y() + deltaY)
aSession.finishOperation()
assert(aLineStartPoint.y() == aLineEndPoint.y())
assert(model.dof(aSketchFeature) == 3)
#=========================================================================
# Move other boundary point of a line
#=========================================================================
deltaX = -3.
deltaY = -10.
aSession.startOperation()
aLineEndPoint.setValue(aLineEndPoint.x() + deltaX,
                       aLineEndPoint.y() + deltaY)
aSession.finishOperation()
assert(aLineStartPoint.y() == aLineEndPoint.y())
assert(model.dof(aSketchFeature) == 3)
#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
