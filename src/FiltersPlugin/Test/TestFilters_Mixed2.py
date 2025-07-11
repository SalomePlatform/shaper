# Copyright (C) 2014-2025  CEA, EDF
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
from ModelHighAPI import CHECK_NAMING

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "PartSet/OY"), 20, 3)
Filters = model.filters(Part_1_doc, [model.addFilter(name = "BelongsTo", args = [model.selection("SOLID", "LinearCopy_1_1_3")]),
                                     model.addFilter(name = "OnPlane", exclude = True, args = [model.selection("FACE", "LinearCopy_1_1_1/MF:Translated&Box_1_1/Top"),
                                                                                               model.selection("FACE", "LinearCopy_1_1_1/MF:Translated&Box_1_1/Front")
                                                                                               ])
                                     ])
model.end()

Box1 = LinearCopy_1.result().subResult(0).resultSubShapePair()[0]
Box2 = LinearCopy_1.result().subResult(1).resultSubShapePair()[0]
Box3 = LinearCopy_1.result().subResult(2).resultSubShapePair()[0]
emptyShape = GeomAPI_Shape()

Reference = {}
# First box reference data
Reference[model.selection(Box1, emptyShape)] = False
exp = GeomAPI_ShapeExplorer(Box1.shape(), GeomAPI_Shape.FACE)
while exp.more():
  Reference[model.selection(Box1, exp.current())] = False
  exp.next()
exp = GeomAPI_ShapeExplorer(Box1.shape(), GeomAPI_Shape.EDGE)
while exp.more():
  Reference[model.selection(Box1, exp.current())] = False
  exp.next()
exp = GeomAPI_ShapeExplorer(Box1.shape(), GeomAPI_Shape.VERTEX)
while exp.more():
  Reference[model.selection(Box1, exp.current())] = False
  exp.next()
# Second box reference data
Reference[model.selection(Box2, emptyShape)] = False
exp = GeomAPI_ShapeExplorer(Box2.shape(), GeomAPI_Shape.FACE)
while exp.more():
  Reference[model.selection(Box2, exp.current())] = False
  exp.next()
exp = GeomAPI_ShapeExplorer(Box2.shape(), GeomAPI_Shape.EDGE)
while exp.more():
  Reference[model.selection(Box2, exp.current())] = False
  exp.next()
exp = GeomAPI_ShapeExplorer(Box2.shape(), GeomAPI_Shape.VERTEX)
while exp.more():
  Reference[model.selection(Box2, exp.current())] = False
  exp.next()
# Third box reference data
Reference[model.selection(Box3, emptyShape)] = True
# faces
exp = GeomAPI_ShapeExplorer(Box3.shape(), GeomAPI_Shape.FACE)
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
assert(not exp.more())
# edges
exp = GeomAPI_ShapeExplorer(Box3.shape(), GeomAPI_Shape.EDGE)
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
assert(not exp.more())
# vertices
exp = GeomAPI_ShapeExplorer(Box3.shape(), GeomAPI_Shape.VERTEX)
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = True;  exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
Reference[model.selection(Box3, exp.current())] = False; exp.next()
assert(not exp.more())

model.checkFilter(Part_1_doc, model, Filters, Reference)

model.begin()
Group_1_objects = [model.selection("FACE", "LinearCopy_1_1_3/MF:Translated&Box_1_1/Back"), model.selection("FACE", "LinearCopy_1_1_3/MF:Translated&Box_1_1/Left"), model.selection("FACE", "LinearCopy_1_1_3/MF:Translated&Box_1_1/Right"), model.selection("FACE", "LinearCopy_1_1_3/MF:Translated&Box_1_1/Bottom"), Filters]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
model.end()
assert(model.checkPythonDump(CHECK_NAMING))
