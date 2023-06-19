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
    TestHighload.py
"""
from GeomAPI import *
from GeomDataAPI import *
from ModelAPI import *
import math


#=========================================================================
# Useful subroutines
#=========================================================================
def distance(pointA, pointB):
    """
    subroutine to calculate distance between two points
    result of calculated distance is has 10**-5 precision
    """
    xdiff = math.pow((pointA.x() - pointB.x()), 2)
    ydiff = math.pow((pointA.y() - pointB.y()), 2)
    return round(math.sqrt(xdiff + ydiff), 5)


def createNAngle(theSketch, theN, theRadius, theEdgeLength=0):
    # Create an N-Angle at (0,0)
    rad = 2. * math.pi / theN
    points = []
    for a in range(theN):
        x = round(math.cos(rad * a), 10) * theRadius
        y = round(math.sin(rad * a), 10) * theRadius
        points.append((x, y))
    # Close the contour
    points.append(points[0])

    # Create lines iterating over (A,B), (B,C), ... (N,A)
    allStartPoints = []
    allEndPoints = []
    allLines = []
    for begin, end in zip(points[:-1], points[1:]):
        aSketchLine = theSketch.addFeature("SketchLine")
        aStartPoint = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
        anEndPoint = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
        aStartPoint.setValue(begin[0], begin[1])
        anEndPoint.setValue(end[0], end[1])
        aSketchLine.execute()
        allStartPoints.append(aStartPoint)
        allEndPoints.append(anEndPoint)
        allLines.append(aSketchLine)
    theSketch.execute()
    # Shift a start point to the end of list:
    # (Bb, Cb, Ab) zip (Ae, Be, Ce) --> ((Ae, Bb), (Cb, Be), (Ab, Ce))
    allStartPoints.append(allStartPoints.pop(0))
    # Make lines coincident:
    for pointA, pointB in zip(allStartPoints, allEndPoints):
        aCoincidence = theSketch.addFeature("SketchConstraintCoincidence")
        aCoincidence.refattr("ConstraintEntityA").setAttr(pointA)
        aCoincidence.refattr("ConstraintEntityB").setAttr(pointB)
    return allLines


def fixLineLength(theSketch, theLines, theEdgeLength=0):
    aFirstLineLength = theEdgeLength
    # Make give lines fixed length:
    for aLine in theLines:
        aLengthConstraint = aSketchFeature.addFeature("SketchConstraintLength")
        refattrA = aLengthConstraint.refattr("ConstraintEntityA")
        refattrA.setObject(modelAPI_ResultConstruction(aLine.firstResult()))
        if aFirstLineLength == 0:
            pointA = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
            pointB = geomDataAPI_Point2D(aLine.attribute("EndPoint"))
            aFirstLineLength = distance(pointA, pointB)
        aLengthConstraint.real("ConstraintValue").setValue(aFirstLineLength)


def moveTo(theLines, theDeltaX, theDeltaY):
    for aLine in theLines:
        aStart = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
        anEnd = geomDataAPI_Point2D(aLine.attribute("EndPoint"))
        aStart.setValue(aStart.x() + theDeltaX, aStart.y() + theDeltaY)
        anEnd.setValue(anEnd.x() + theDeltaX, anEnd.y() + theDeltaY)

#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2014-11-27"

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
# Create 4x4 polygons N = {5, 21}
#=========================================================================
aDOF = 0
deltaX = deltaY = 50.
n = 5
aSession.startOperation()
for i in range(4):
    for j in range(4):
        allNangleLines = createNAngle(aSketchFeature, n, 50)
        fixLineLength(aSketchFeature, allNangleLines)
        moveTo(allNangleLines, deltaX, deltaY)
        aDOF += n
        n += 1
        deltaX += 110.
    deltaY += 110.
    deltaX = 50.
aSession.finishOperation()


#=========================================================================
# End of test
#=========================================================================

from salome.shaper import model
assert(model.dof(aSketchFeature) == aDOF)
assert(model.checkPythonDump())
