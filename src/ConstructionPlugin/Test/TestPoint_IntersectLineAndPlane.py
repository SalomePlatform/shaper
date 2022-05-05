# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
Test case for Construction Point feature as intersection of line and plane.
"""

from salome.shaper import model
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "Origin"))
SketchCircle_1 = Sketch_1.addCircle(0, 0, 60)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
model.do()
Sketch_2 = model.addSketch(partSet, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_2.addLine(60, 100, 0, 20)
SketchArc_1 = Sketch_2.addArc(0, 0, -65.89631323066888, 61.2998850129882, -90, 0, False)
model.do()

# point by sketch face and a line
Point_1 = model.addPoint(partSet, model.selection("EDGE", "Sketch_2/SketchLine_1"), model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f"))
model.do()
# check the point position
rightPosition = GeomAPI_Vertex(-15, 0, 0)
assert(rightPosition.isEqual(Point_1.results()[0].resultSubShapePair()[0].shape()))

# point by sketch face and an arc, intersection outside of the face, offset is defined
Point_2 = model.addPoint(partSet, model.selection("EDGE", "Sketch_2/SketchArc_1_2"), model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f"), 10, True)
# check the point position
rightPosition = GeomAPI_Vertex(-89.442719099991606, 0, -10)
assert(rightPosition.isEqual(Point_2.results()[0].resultSubShapePair()[0].shape()))


model.end()
assert(model.checkPythonDump())
