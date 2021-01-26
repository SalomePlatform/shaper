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

from salome.shaper import model
from salome.shaper import geom

model.begin()
partSet = model.moduleDocument()
part = model.addPart(partSet).document()

sketch = model.addSketch(part, model.defaultPlane("XOY"))

# rectangles from start and end points
rectangle_1 = sketch.addRectangle(0, 0, 50, 50)

startPoint = geom.Pnt2d(50, 50)
endPoint = geom.Pnt2d(100, 100)
rectangle_2 = sketch.addRectangle(startPoint, endPoint)

# rectangles from center and end points -> add a True argument
centerPoint = geom.Pnt2d(100, 100)
endPoint2 = geom.Pnt2d(120, 75)
rectangle_3 = sketch.addRectangle(centerPoint, endPoint2, True)
rectangle_4 = sketch.addRectangle(10, 5, 25, 75, True)

# rectangles from center and end points second version
centerPoint2 = geom.Pnt2d(200, 180)
endPoint3 = geom.Pnt2d(220, 175)
rectangle_5 = sketch.addRectangleCentered(centerPoint2, endPoint3)
rectangle_6 = sketch.addRectangleCentered(108, 59, 125, 175)

model.end()


assert(model.checkPythonDump())
