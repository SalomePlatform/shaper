# Copyright (C) 2021-2022  CEA/DEN, EDF R&D
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

def checkRectangleL(lines, valref, tolerance = 1.e-5):
  for i in range(0, 4):
    line = SketchAPI_Line(lines[i])
    #print (line.defaultResult().shape().edge().length())
    #print (valref[i%2])
    #print (abs(line.defaultResult().shape().edge().length()-valref[i%2]))
    assert(abs(line.defaultResult().shape().edge().length()-valref[i%2]) <= tolerance)

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
valref = [ \
400.86931 , 200.78509 , \
401.73886 , 201.57021 , \
402.60865 , 202.35537 , \
403.47866 , 203.14056 , \
404.34890 , 203.92580 ]
for i in range(0, 5):
  center.setX(center.x() + SHIFT)
  center.setY(center.y() + SHIFT)
  model.begin()
  sketch.move(SketchAPI_Line(lines_1[0]).startPoint(), center)
  model.end()
  checkRectangleL(lines_3, valref[2*i:])

# move corner of rectangle
corner = SketchAPI_Line(lines_2[0]).endPoint().pnt()
valref = [ \
403.11209 , 202.95437 , \
401.87551 , 201.98300 , \
400.63915 , 201.01169 , \
399.40301 , 200.04043 , \
398.16710 , 199.06922 ]
for i in range(0, 5):
  corner.setX(corner.x() + SHIFT)
  corner.setY(corner.y() + SHIFT)
  model.begin()
  sketch.move(SketchAPI_Line(lines_2[0]).endPoint(), corner)
  model.end()
  checkRectangleL(lines_3, valref[2*i:])

assert(model.checkPythonDump())
