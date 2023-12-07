# Copyright (C) 2021-2024  CEA, EDF
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

from salome.shaper import model
from salome.shaper import geom

from SketchAPI import *

def checkRectangle(lines, center, corner, tolerance = 1.e-7):
  dx = corner.x() - center.x()
  dy = corner.y() - center.y()
  points = [geom.Pnt2d(center.x() - dx, center.y() - dy),
            geom.Pnt2d(center.x() - dx, center.y() + dy),
            geom.Pnt2d(center.x() + dx, center.y() + dy),
            geom.Pnt2d(center.x() + dx, center.y() - dy)]
  for i in range(0, 4):
    line = SketchAPI_Line(lines[i])
    sp = line.startPoint().pnt()
    sp_ref = points[i-1]
    assert(sp.distance(sp_ref) <= tolerance)
    ep = line.endPoint().pnt()
    ep_ref = points[i]
    assert(ep.distance(ep_ref) <= tolerance)

def areCounterDirectedAndOfEqualLength(theLineA, theLineB):
  tolerance = 1.e-5

  aStartA = theLineA.startPoint().pnt()
  aEndA   = theLineA.endPoint().pnt()
  aDirA_X = aEndA.x() - aStartA.x()
  aDirA_Y = aEndA.y() - aStartA.y()

  aStartB = theLineB.startPoint().pnt()
  aEndB   = theLineB.endPoint().pnt()
  aDirB_X = aEndB.x() - aStartB.x()
  aDirB_Y = aEndB.y() - aStartB.y()

  return abs(aDirA_X + aDirB_X) < tolerance and abs(aDirA_Y + aDirB_Y) < tolerance

def arePerpendicular(theLineA, theLineB):
  tolerance = 1.e-5

  aStartA = theLineA.startPoint().pnt()
  aEndA   = theLineA.endPoint().pnt()
  aDirA_X = aEndA.x() - aStartA.x()
  aDirA_Y = aEndA.y() - aStartA.y()
  aLengthA = theLineA.defaultResult().shape().edge().length()

  aStartB = theLineB.startPoint().pnt()
  aEndB   = theLineB.startPoint().pnt()
  aDirB_X = aEndB.x() - aStartB.x()
  aDirB_Y = aEndB.y() - aStartB.y()
  aLengthB = theLineB.defaultResult().shape().edge().length()

  if (aLengthA < tolerance or aLengthB < tolerance):
    return True

  return (aDirA_X * aDirB_X + aDirA_Y * aDirB_Y) / (aLengthA * aLengthB) < tolerance

def areConnected(theLineA, theLineB):
  aEndA   = theLineA.endPoint().pnt()
  aStartB = theLineB.startPoint().pnt()
  return aEndA.x() == aStartB.x() and aEndA.y() == aStartB.y()

def checkIfArbitraryAlignedRectangle(theEdgeLines):
  aLine0 = SketchAPI_Line(theEdgeLines[0])
  aLine1 = SketchAPI_Line(theEdgeLines[1])
  aLine2 = SketchAPI_Line(theEdgeLines[2])
  aLine3 = SketchAPI_Line(theEdgeLines[3])

  assert (areCounterDirectedAndOfEqualLength(aLine0, aLine2))
  assert (areCounterDirectedAndOfEqualLength(aLine1, aLine3))
  assert (arePerpendicular(aLine0, aLine1))
  assert (arePerpendicular(aLine2, aLine3))
  assert (areConnected(aLine0, aLine1))
  assert (areConnected(aLine1, aLine2))
  assert (areConnected(aLine2, aLine3))
  assert (areConnected(aLine3, aLine0))       



model.begin()
partSet = model.moduleDocument()
part = model.addPart(partSet).document()

centerPoint = geom.Pnt2d(50, 50)
endPoint = geom.Pnt2d(100, 100)

sketch = model.addSketch(part, model.defaultPlane("XOY"))
rectangle_1 = sketch.addRectangleCentered(centerPoint, endPoint)
lines_1 = rectangle_1.lines()
model.end()

checkRectangle(lines_1, centerPoint, endPoint)

model.begin()
projection_1 = sketch.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
point_1 = SketchAPI_Point(projection_1.createdFeature())

rectangle_2 = sketch.addRectangleCentered(point_1.coordinates(), endPoint)
lines_2 = rectangle_2.lines()
model.end()

checkRectangle(lines_2, geom.Pnt2d(0.0, 0.0), endPoint)

model.begin()
rectangle_3 = sketch.addRectangleCentered(SketchAPI_Line(lines_1[0]).startPoint(), SketchAPI_Line(lines_2[0]).endPoint())
lines_3 = rectangle_3.lines()
model.end()

checkRectangle(lines_3, SketchAPI_Line(lines_1[0]).startPoint().pnt(), SketchAPI_Line(lines_2[0]).endPoint().pnt())

# move center of rectangle
SHIFT = 1.0
center = SketchAPI_Line(lines_1[0]).startPoint().pnt()
for i in range(0, 5):
  center.setX(center.x() + SHIFT)
  center.setY(center.y() + SHIFT)
  model.begin()
  sketch.move(SketchAPI_Line(lines_1[0]).startPoint(), center)
  model.end()
  checkIfArbitraryAlignedRectangle(lines_3)

# move corner of rectangle
corner = SketchAPI_Line(lines_2[0]).endPoint().pnt()
for i in range(0, 5):
  corner.setX(corner.x() + SHIFT)
  corner.setY(corner.y() + SHIFT)
  model.begin()
  sketch.move(SketchAPI_Line(lines_2[0]).endPoint(), corner)
  model.end()
  checkIfArbitraryAlignedRectangle(lines_3)

assert(model.checkPythonDump())
