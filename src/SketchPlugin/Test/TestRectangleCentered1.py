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
    TestRectangle.py
    Unit test of SketchPlugin_Rectangle class (centered rectangle case)

"""
from GeomDataAPI import *
from ModelAPI import *
import math
from salome.shaper import model

#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2023-08-07"


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

def areCounterDirectedAndOfEqualLength(theLineA, theLineB):
    tolerance = 1.e-5

    aStartA = geomDataAPI_Point2D(theLineA.attribute("StartPoint"))
    aEndA   = geomDataAPI_Point2D(theLineA.attribute("EndPoint"))
    aDirA_X = aEndA.x() - aStartA.x()
    aDirA_Y = aEndA.y() - aStartA.y()

    aStartB = geomDataAPI_Point2D(theLineB.attribute("StartPoint"))
    aEndB   = geomDataAPI_Point2D(theLineB.attribute("EndPoint"))
    aDirB_X = aEndB.x() - aStartB.x()
    aDirB_Y = aEndB.y() - aStartB.y()

    return abs(aDirA_X + aDirB_X) < tolerance and abs(aDirA_Y + aDirB_Y) < tolerance

def arePerpendicular(theLineA, theLineB):
    tolerance = 1.e-5

    aStartA = geomDataAPI_Point2D(theLineA.attribute("StartPoint"))
    aEndA   = geomDataAPI_Point2D(theLineA.attribute("EndPoint"))
    aDirA_X = aEndA.x() - aStartA.x()
    aDirA_Y = aEndA.y() - aStartA.y()
    aLengthA = theLineA.lastResult().shape().edge().length()

    aStartB = geomDataAPI_Point2D(theLineB.attribute("StartPoint"))
    aEndB   = geomDataAPI_Point2D(theLineB.attribute("EndPoint"))
    aDirB_X = aEndB.x() - aStartB.x()
    aDirB_Y = aEndB.y() - aStartB.y()
    aLengthB = theLineB.lastResult().shape().edge().length()

    if (aLengthA < tolerance or aLengthB < tolerance):
        return True

    return (aDirA_X * aDirB_X + aDirA_Y * aDirB_Y) / (aLengthA * aLengthB) < tolerance

def areConnected(theLineA, theLineB): 
    aEndA = geomDataAPI_Point2D(theLineA.attribute("EndPoint"))
    aStartB = geomDataAPI_Point2D(theLineB.attribute("StartPoint"))
    return aEndA.x() == aStartB.x() and aEndA.y() == aStartB.y()


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
aRectangle.string("RectangleType").setValue("RectangleTypeCentered")
aCenter = geomDataAPI_Point2D(aRectangle.attribute("RectCenterPoint"))
aCorner = geomDataAPI_Point2D(aRectangle.attribute("RectCornerPoint"))
aCenter.setValue(10., 10.)
aCorner.setValue(40., 30.)
aSession.finishOperation()
#=========================================================================
# Check the lines of rectangle are parallel to the axes
#=========================================================================
aNbSubs = aSketchFeature.numberOfSubs()
assert (aNbSubs >= 5) # The first feature on the sketch is center SketchPoint.
aNbLines = 0
for i in range (1, 5):
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
# Check the opposites lines of rectangle are parallel, and neighboring
# ones are perpendicular
#=========================================================================
aLine0 = objectToFeature(aSketchFeature.subFeature(1))
assert (aLine0.getKind() == "SketchLine")

aLine1 = objectToFeature(aSketchFeature.subFeature(2))
assert (aLine1.getKind() == "SketchLine")

aLine2 = objectToFeature(aSketchFeature.subFeature(3))
assert (aLine2.getKind() == "SketchLine")

aLine3 = objectToFeature(aSketchFeature.subFeature(4))
assert (aLine3.getKind() == "SketchLine")

assert (areCounterDirectedAndOfEqualLength(aLine0, aLine2))
assert (areCounterDirectedAndOfEqualLength(aLine1, aLine3))
assert (arePerpendicular(aLine0, aLine1))
assert (arePerpendicular(aLine2, aLine3))

#=========================================================================
# Check the contour is closed
#=========================================================================
assert (areConnected(aLine0, aLine1))
assert (areConnected(aLine1, aLine2))
assert (areConnected(aLine2, aLine3))
assert (areConnected(aLine3, aLine0))

#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())
