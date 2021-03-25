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

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(10, 10, 10, 100)
SketchLine_2 = Sketch_1.addLine(10, 100, 100, 10)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
model.do()
# make non-perpendicular line perpendicular
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_1.result(), SketchLine_2.result())
model.do()
# this moves the line
assert(SketchLine_1.startPoint().x() != 10)
model.end()
model.undo()
# undo must move the line exactly back
assert(SketchLine_1.startPoint().x() == 10)
