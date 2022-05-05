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
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Left"), model.selection("FACE", "Box_1_1/Right"))
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("FACE", "Plane_1")], keepSubResults = True)
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1")], model.selection("EDGE", "PartSet/OX"), 20)
FilterFace = model.filters(Part_1_doc, [model.addFilter(name = "OnGeometry", exclude = True, args = [model.selection("FACE", "Partition_1_1_1/Modified_Face&Box_1_1/Top")])])
model.end()

Reference = {}
# Faces of the box (selected as a sub-shapes of the result to keep original surface).
# Note: the expected values have to be updated if ShapeExplorer will return another order of sub-shapes.
ResultBox_1 = Partition_1.result().resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultBox_1.shape(), GeomAPI_Shape.FACE)
Reference[model.selection(ResultBox_1, exp.current())] = True;  exp.next()
Reference[model.selection(ResultBox_1, exp.current())] = False; exp.next()
Reference[model.selection(ResultBox_1, exp.current())] = True;  exp.next()
Reference[model.selection(ResultBox_1, exp.current())] = True;  exp.next()
Reference[model.selection(ResultBox_1, exp.current())] = True;  exp.next()
Reference[model.selection(ResultBox_1, exp.current())] = True;  exp.next()
Reference[model.selection(ResultBox_1, exp.current())] = True;  exp.next()
Reference[model.selection(ResultBox_1, exp.current())] = False; exp.next()
Reference[model.selection(ResultBox_1, exp.current())] = True;  exp.next()
Reference[model.selection(ResultBox_1, exp.current())] = True;  exp.next()
Reference[model.selection(ResultBox_1, exp.current())] = True;  exp.next()
Reference[model.selection(ResultBox_1, exp.current())] = True;  exp.next()
assert(not exp.more())
# Faces of the cylinder
ResultCylinder_1 = Translation_1.result().resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultCylinder_1.shape(), GeomAPI_Shape.FACE)
Reference[model.selection(ResultCylinder_1, exp.current())] = True;  exp.next()
Reference[model.selection(ResultCylinder_1, exp.current())] = False; exp.next()
Reference[model.selection(ResultCylinder_1, exp.current())] = True;  exp.next()
assert(not exp.more())
# Edges of the original box
exp = GeomAPI_ShapeExplorer(ResultBox_1.shape(), GeomAPI_Shape.EDGE)
while exp.more():
  Reference[model.selection(ResultBox_1, exp.current())] = True
  exp.next()
# Edges of the cylinder
exp = GeomAPI_ShapeExplorer(ResultCylinder_1.shape(), GeomAPI_Shape.EDGE)
while exp.more():
  Reference[model.selection(ResultCylinder_1, exp.current())] = True
  exp.next()
# Vertices of the original box
exp = GeomAPI_ShapeExplorer(ResultBox_1.shape(), GeomAPI_Shape.VERTEX)
while exp.more():
  Reference[model.selection(ResultBox_1, exp.current())] = True
  exp.next()
# Vertices of the cylinder
exp = GeomAPI_ShapeExplorer(ResultCylinder_1.shape(), GeomAPI_Shape.VERTEX)
while exp.more():
  Reference[model.selection(ResultCylinder_1, exp.current())] = True
  exp.next()

model.checkFilter(Part_1_doc, model, FilterFace, Reference)
