# Copyright (C) 2025-2026  CEA, EDF
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
    TestRectangleCenteredModeSwitch.py
    Unit test of SketchPlugin_Rectangle class (centered rectangle case)
    Test Tuleap issue 44019
"""
from GeomDataAPI import *
from ModelAPI import *
from salome.shaper import model

#=========================================================================
# Initialization of the test
#=========================================================================
__updated__ = "2025-01-24"

#=========================================================================
# Auxiliary functions
#=========================================================================
def isHorizontal(line):
    aStart = geomDataAPI_Point2D(line.attribute("StartPoint"))
    aEnd   = geomDataAPI_Point2D(line.attribute("EndPoint"))
    return aStart.y() == aEnd.y()

def isVertical(line):
    aStart = geomDataAPI_Point2D(line.attribute("StartPoint"))
    aEnd   = geomDataAPI_Point2D(line.attribute("EndPoint"))
    return aStart.x() == aEnd.x()


#=========================================================================
# Start of test
#=========================================================================
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
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
# Create a rectangle
#=========================================================================
aSession.startOperation()
aRectangle = aSketchFeature.addFeature("SketchRectangle")
# Start with RectangleTypeByCorners, emulating GUI behavior
aRectangle.string("RectangleType").setValue("RectangleTypeByCorners")
aStart = geomDataAPI_Point2D(aRectangle.attribute("RectStartPoint"))
# One SketchLine is created here, but not fully initialized,
# and this prevents Sketch validation then
aStart.setValue(10., 10.)
# Switch to RectangleTypeCentered
# With implemented fix, previously created line is removed here
aRectangle.string("RectangleType").setValue("RectangleTypeCentered")
aCenter = geomDataAPI_Point2D(aRectangle.attribute("RectCenterPoint"))
aCorner = geomDataAPI_Point2D(aRectangle.attribute("RectCornerPoint"))
aCenter.setValue(10., 10.)
aCorner.setValue(40., 30.)
#=========================================================================
# Check the rectangle feature is valid
#=========================================================================
aValidators = ModelAPI_Session.get().validators()
#
name = aRectangle.name()
error = aRectangle.error()
assert(error == ''), "The feature {0} is in error: {1}".format(name, error)
assert(aValidators.validate(aRectangle)), "The feature {0} is in error: {1}".format(name, error)
aSession.finishOperation()
#=========================================================================
# Check the sketch feature is valid
#=========================================================================
name = aSketchFeature.name()
error = aSketchFeature.error()
assert (aValidators.validate(aSketchFeature)), "The feature {0} is in error: {1}".format(name, error)
#=========================================================================
# Check all the sub-features of the sketch are valid
#=========================================================================
aNbSubs = aSketchFeature.numberOfSubs()
assert (aNbSubs >= 5) # The last feature on the sketch is center SketchPoint.
aNbLines = 0
for i in range (0, aNbSubs):
    aFeature = objectToFeature(aSketchFeature.subFeature(i))
    name = aFeature.name()
    error = aFeature.error()
    # Without fixing 44019 this script fails here for i = 0 with error:
    # "AssertionError: The feature SketchLine_1 is in error: Start point is not selected"
    assert(aValidators.validate(aFeature)), "The feature {0} is in error: {1}".format(name, error)

    if i < 5 and aFeature.getKind() == "SketchLine":
        assert (isHorizontal(aFeature) or isVertical(aFeature))
        aNbLines = aNbLines + 1
assert (aNbLines == 4)
assert (model.dof(aSketchFeature) == 5)
#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
