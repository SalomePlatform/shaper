# Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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
from GeomAPI import *
import math

TOLERANCE = 1.e-7

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], model.selection("EDGE", "PartSet/OX"), 10)
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Translation_1_1")], True)
model.do()
model.end()

aShape = Fuse_1.result().resultSubShapePair()[0].shape()
assert(aShape.isSolid())
aBox = aShape.solid().getParallelepiped()
assert(aBox is not None)
assert(aBox.isAxesAligned())

CORNER = GeomAPI_Pnt(0, 0, 0)
WIDTH = 20
HEIGHT = 10
DEPTH = 10

aCorner = aBox.corner()
assert(aCorner.distance(CORNER) < TOLERANCE), "({}, {}, {}) != expected ({}, {}, {})".format(aCorner.x(), aCorner.y(), aCorner.z(), CORNER.x(), CORNER.y(), CORNER.z())
assert(math.fabs(aBox.width() - WIDTH) < TOLERANCE), "Width {} != {}".format(aBox.width(), WIDTH)
assert(math.fabs(aBox.depth() - DEPTH) < TOLERANCE), "Depth {} != {}".format(aBox.depth(), DEPTH)
assert(math.fabs(aBox.height() - HEIGHT) < TOLERANCE), "Height {} != {}".format(aBox.height(), HEIGHT)
