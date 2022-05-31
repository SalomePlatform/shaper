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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 100, 100, 100)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], -50, -50, -50)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), model.selection("EDGE", "PartSet/OX"), 30)
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/YOZ"), model.selection("EDGE", "PartSet/OY"), 30)
Plane_6 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOZ"), model.selection("EDGE", "PartSet/OZ"), 30)
Partition_1_objects = [model.selection("SOLID", "Translation_1_1"), model.selection("FACE", "Plane_1"), model.selection("FACE", "Plane_2"), model.selection("FACE", "Plane_3")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects, keepSubResults = True)
Filters = model.filters(Part_1_doc, [model.addFilter(name = "OppositeToEdge", exclude = True, args = [model.selection("EDGE", "Partition_1_1_7/Generated_Edge&Plane_2/Plane_2&Plane_1/Plane_1")])])
model.end()

from GeomAPI import *

Reference = {}
# Faces are not applicable
ResultBox_1 = Partition_1.result().resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultBox_1.shape(), GeomAPI_Shape.FACE)
while exp.more():
  Reference[model.selection(ResultBox_1, exp.current())] = True
  exp.next()
# Vertices are not applicable too
exp = GeomAPI_ShapeExplorer(ResultBox_1.shape(), GeomAPI_Shape.VERTEX)
while exp.more():
  Reference[model.selection(ResultBox_1, exp.current())] = True
  exp.next()

# Edges of the partitioned box.
# Note: the expected values have to be updated if ShapeExplorer will return another order of sub-shapes.

# sub-result 0
SubResult = Partition_1.result().subResult(0).resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(SubResult.shape(), GeomAPI_Shape.EDGE)
# edges of face 1
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
# edges of face 2
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
# edges of face 3
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
# edges of face 4
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
# edges of face 5
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
# edges of face 6
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
assert(not exp.more())

# sub-result 1
SubResult = Partition_1.result().subResult(1).resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(SubResult.shape(), GeomAPI_Shape.EDGE)
# edges of face 1
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
# edges of face 2
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
# edges of face 3
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
# edges of face 4
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
# edges of face 5
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
# edges of face 6
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
assert(not exp.more())

# sub-result 2
SubResult = Partition_1.result().subResult(2).resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(SubResult.shape(), GeomAPI_Shape.EDGE)
while exp.more():
  Reference[model.selection(SubResult, exp.current())] = True
  exp.next()

# sub-result 3
SubResult = Partition_1.result().subResult(3).resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(SubResult.shape(), GeomAPI_Shape.EDGE)
while exp.more():
  Reference[model.selection(SubResult, exp.current())] = True
  exp.next()

# sub-result 4
SubResult = Partition_1.result().subResult(4).resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(SubResult.shape(), GeomAPI_Shape.EDGE)
# edges of face 1
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
# edges of face 2
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
# edges of face 3
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
# edges of face 4
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
# edges of face 5
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
# edges of face 6
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
assert(not exp.more())

# sub-result 5
SubResult = Partition_1.result().subResult(5).resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(SubResult.shape(), GeomAPI_Shape.EDGE)
while exp.more():
  Reference[model.selection(SubResult, exp.current())] = True
  exp.next()

# sub-result 6
SubResult = Partition_1.result().subResult(6).resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(SubResult.shape(), GeomAPI_Shape.EDGE)
# edges of face 1
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
# edges of face 2
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
# edges of face 3
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
# edges of face 4
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
# edges of face 5
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
# edges of face 6
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
Reference[model.selection(SubResult, exp.current())] = False; exp.next()
Reference[model.selection(SubResult, exp.current())] = True;  exp.next()
assert(not exp.more())

# sub-result 7
SubResult = Partition_1.result().subResult(7).resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(SubResult.shape(), GeomAPI_Shape.EDGE)
while exp.more():
  Reference[model.selection(SubResult, exp.current())] = True
  exp.next()

model.checkFilter(Part_1_doc, model, Filters, Reference)
