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
      Test1806.py
      Unit test for testing the expression evaluation even no parameters are created

"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *

__updated__ = "2016-11-23"

aSession = ModelAPI_Session.get()
aPartSet = aSession.moduleDocument()

#=========================================================================
# Create a part
#=========================================================================
aSession.startOperation()
aPartFeature = aPartSet.addFeature("Part")
aSession.finishOperation()

aPart = aSession.activeDocument()
#=========================================================================
# Create a sketch with a line and length
#=========================================================================
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
aXOYPlane = objectToResult(aPartSet.objectByName("Construction", "XOY"))
aSketchFeature.selection("External").setValue(aXOYPlane, None)
# Create lines
aLine = aSketchFeature.addFeature("SketchLine")
aStart = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
aStart.setValue(0, 0)
anEnd = geomDataAPI_Point2D(aLine.attribute("EndPoint"))
anEnd.setValue(200, 0)

aDistance = aSketchFeature.addFeature("SketchConstraintDistance")
aDistance.refattr("ConstraintEntityA").setAttr(aStart)
aDistance.refattr("ConstraintEntityB").setAttr(anEnd)
aDistance.real("ConstraintValue").setText("25/2")

aSession.finishOperation()

assert(aDistance.real("ConstraintValue").value() == 12.5)
#=========================================================================
# End of test
#=========================================================================
from salome.shaper import model
assert(model.checkPythonDump())
