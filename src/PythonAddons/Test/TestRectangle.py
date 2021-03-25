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

from salome.shaper import model
from salome.shaper import geom

model.begin()
partSet = model.moduleDocument()
part = model.addPart(partSet).document()

sketch = model.addSketch(part, model.defaultPlane("XOY"))
rectangle_1 = sketch.addRectangle(0, 0, 50, 50)

startPoint = geom.Pnt2d(50, 50)
endPoint = geom.Pnt2d(100, 100)

rectangle_2 = sketch.addRectangle(startPoint, endPoint)
model.end()

assert(model.checkPythonDump())
