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
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Left"), model.selection("FACE", "Box_1_1/Right"))
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Front"), model.selection("FACE", "Box_1_1/Back"))
Partition_1_objects = [model.selection("SOLID", "Box_1_1"), model.selection("FACE", "Plane_1"), model.selection("FACE", "Plane_2")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects, keepSubResults = True)
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Filters = model.filters(Part_1_doc, [model.addFilter(name = "TopoConnectedFaces", exclude = True, args = [model.selection("EDGE", "Partition_1_1_2/Generated_Edge&Plane_2/Plane_2&Box_1_1/Top"), True])])
model.end()

Reference = {}
# Faces of the box
ResultBox_0 = Partition_1.result().subResult(0).resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultBox_0.shape(), GeomAPI_Shape.FACE)
while exp.more():
  Reference[model.selection(ResultBox_0, exp.current())] = False
  exp.next()

ResultBox_1 = Partition_1.result().subResult(1).resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultBox_1.shape(), GeomAPI_Shape.FACE)
while exp.more():
  Reference[model.selection(ResultBox_1, exp.current())] = False
  exp.next()

ResultBox_2 = Partition_1.result().subResult(2).resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultBox_2.shape(), GeomAPI_Shape.FACE)
while exp.more():
  Reference[model.selection(ResultBox_2, exp.current())] = False
  exp.next()

ResultBox_3 = Partition_1.result().subResult(3).resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultBox_3.shape(), GeomAPI_Shape.FACE)
while exp.more():
  Reference[model.selection(ResultBox_3, exp.current())] = False
  exp.next()

# Faces of the cylinder
ResultCylinder_1 = Cylinder_1.result().resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultCylinder_1.shape(), GeomAPI_Shape.FACE)
while exp.more():
  Reference[model.selection(ResultCylinder_1, exp.current())] = True
  exp.next()

model.checkFilter(Part_1_doc, model, Filters, Reference)
