# Copyright (C) 2014-2023  CEA, EDF
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
    TestRectangle1.py
    Unit test of SketchPlugin_Rectangle class

"""
from GeomDataAPI import *
from ModelAPI import *
from GeomAPI import *
import math
from salome.shaper import model

#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2016-02-05"


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

def isPerpendicular(line1, line2, tolerance):
    aStart1 = geomDataAPI_Point2D(line1.attribute("StartPoint"))
    aEnd1   = geomDataAPI_Point2D(line1.attribute("EndPoint"))
    aStart2 = geomDataAPI_Point2D(line2.attribute("StartPoint"))
    aEnd2   = geomDataAPI_Point2D(line2.attribute("EndPoint"))
    anAngle = abs(GeomAPI_Angle2d(aStart1.pnt(), aEnd1.pnt(), aStart2.pnt(), aEnd2.pnt()).angleDegree())
    #print("Angle = ", anAngle)
    return abs(anAngle-90) < tolerance # or abs(anAngle-270) < tolerance

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
aRectangle.string("RectangleType").setValue("RectangleTypeByCorners")
aStartCorner = geomDataAPI_Point2D(aRectangle.attribute("RectStartPoint"))
aEndCorner = geomDataAPI_Point2D(aRectangle.attribute("RectEndPoint"))
aStartCorner.setValue(10., 10.)
aEndCorner.setValue(40., 30.)
aSession.finishOperation()
#=========================================================================
# Check the lines of rectangle are parallel to the axes
#=========================================================================
aNbSubs = aSketchFeature.numberOfSubs()
aNbLines = 0
for i in range (0, aNbSubs):
    aFeature = objectToFeature(aSketchFeature.subFeature(i))
    if aFeature.getKind() == "SketchLine":
        aLastLine = aFeature
        assert (isHorizontal(aLastLine) or isVertical(aLastLine))
        aNbLines = aNbLines + 1
assert (aNbLines == 4)
assert (model.dof(aSketchFeature) == 5)
#=========================================================================
# Move one of lines
#=========================================================================
aSession.startOperation()
aLineEnd = geomDataAPI_Point2D(aLastLine.attribute("EndPoint"))
aLineEnd.setValue(41., 30.)
aSession.finishOperation()
#=========================================================================
# Check that we have still two pairs of lines with same length
# and that connected lines are perpendicular
#=========================================================================
aNbSubs = aSketchFeature.numberOfSubs()
aNbLines = 0
tolerance = 1.e-5
valref = [0, 0]
aPrevLine = None
for i in range (0, aNbSubs):
    aFeature = objectToFeature(aSketchFeature.subFeature(i))
    if aFeature.getKind() == "SketchLine":
        aLastLine = aFeature
        # Opposite lines must have same length
        edgeLen = aLastLine.lastResult().shape().edge().length()
        if (valref[i%2] == 0):
            valref[i%2] = edgeLen
        else:
            assert(abs(edgeLen-valref[i%2]) <= tolerance)
        # Connected lines must be perpendicular
        if aPrevLine != None:
            assert(isPerpendicular(aPrevLine, aLastLine, tolerance))
        aPrevLine = aLastLine
        aNbLines = aNbLines + 1
assert (aNbLines == 4)
assert (model.dof(aSketchFeature) == 5)
#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
