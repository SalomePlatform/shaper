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
from SketchAPI import *
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Box
Box_1 = model.addBox(Part_1_doc, 100, 50, 100)

### Create Point
Point_1 = model.addPoint(Part_1_doc, 100, 0, 0)

### Create Point
Point_2 = model.addPoint(Part_1_doc, 250, 50, 100)

### Create Box
Box_2 = model.addBox(Part_1_doc, model.selection("VERTEX", "all-in-Point_1"), model.selection("VERTEX", "all-in-Point_2"))

### Create Point
Point_3 = model.addPoint(Part_1_doc, 100, 50, 250)

### Create Box
Box_3 = model.addBox(Part_1_doc, model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Top]"), model.selection("VERTEX", "Point_3"))

### Create CompSolid
CompSolid_1_objects = [model.selection("SOLID", "Box_3_1"),
                       model.selection("SOLID", "Box_2_1"),
                       model.selection("SOLID", "Box_1_1")]
CompSolid_1 = model.addCompSolid(Part_1_doc, CompSolid_1_objects)

Filters = model.filters(Part_1_doc, [model.addFilter(name = "VolumeSize", args = [ "inf" , 500000.0 ])])

model.end()

Reference = {}
ResultCompSolid_1 = CompSolid_1.result().resultSubShapePair()[0]
ResultBox_1 = Box_1.result().resultSubShapePair()[0]
ResultBox_2 = Box_2.result().resultSubShapePair()[0]
ResultBox_3 = Box_3.result().resultSubShapePair()[0]

exp = GeomAPI_ShapeExplorer(ResultCompSolid_1.shape(), GeomAPI_Shape.SOLID)
while exp.more():
  Reference[model.selection(ResultCompSolid_1, exp.current())] = False
  exp.next()
model.checkFilter(Part_1_doc, model, Filters, Reference)

model.begin()
Filters = model.filters(Part_1_doc, [model.addFilter(name = "VolumeSize", args = [ "infEq" , 500000.0 ])])
model.end()

Reference = {}
exp = GeomAPI_ShapeExplorer(ResultBox_1.shape(), GeomAPI_Shape.SOLID)
Reference[model.selection(ResultBox_1, exp.current())] = True
exp = GeomAPI_ShapeExplorer(ResultBox_2.shape(), GeomAPI_Shape.SOLID)
Reference[model.selection(ResultBox_2, exp.current())] = False
exp = GeomAPI_ShapeExplorer(ResultBox_3.shape(), GeomAPI_Shape.SOLID)
Reference[model.selection(ResultBox_3, exp.current())] = False

model.checkFilter(Part_1_doc, model, Filters, Reference)

model.begin()
Filters = model.filters(Part_1_doc, [model.addFilter(name = "VolumeSize", args = [ "sup" , 500000.0 ])])
model.end()

Reference = {}
exp = GeomAPI_ShapeExplorer(ResultBox_1.shape(), GeomAPI_Shape.SOLID)
Reference[model.selection(ResultBox_1, exp.current())] = False
exp = GeomAPI_ShapeExplorer(ResultBox_2.shape(), GeomAPI_Shape.SOLID)
Reference[model.selection(ResultBox_2, exp.current())] = True
exp = GeomAPI_ShapeExplorer(ResultBox_3.shape(), GeomAPI_Shape.SOLID)
Reference[model.selection(ResultBox_3, exp.current())] = True

model.checkFilter(Part_1_doc, model, Filters, Reference)

model.begin()
Filters = model.filters(Part_1_doc, [model.addFilter(name = "VolumeSize", args = [ "inf" , 50.0 ])])
model.end()

Reference = {}
exp = GeomAPI_ShapeExplorer(ResultBox_1.shape(), GeomAPI_Shape.SOLID)
Reference[model.selection(ResultBox_1, exp.current())] = False
exp = GeomAPI_ShapeExplorer(ResultBox_2.shape(), GeomAPI_Shape.SOLID)
Reference[model.selection(ResultBox_2, exp.current())] = False
exp = GeomAPI_ShapeExplorer(ResultBox_3.shape(), GeomAPI_Shape.SOLID)
Reference[model.selection(ResultBox_3, exp.current())] = False

model.checkFilter(Part_1_doc, model, Filters, Reference)

model.begin()
Filters = model.filters(Part_1_doc, [model.addFilter(name = "VolumeSize", args = [ "supEq" , 500000.0 ])])
model.end()

Reference = {}
exp = GeomAPI_ShapeExplorer(ResultCompSolid_1.shape(), GeomAPI_Shape.SOLID)
while exp.more():
  Reference[model.selection(ResultCompSolid_1, exp.current())] = True
  exp.next()
model.checkFilter(Part_1_doc, model, Filters, Reference)

model.begin()
Filters = model.filters(Part_1_doc, [model.addFilter(name = "VolumeSize", args = [ "supEq" , 800000.0 ])])
model.end()

Reference = {}
exp = GeomAPI_ShapeExplorer(ResultCompSolid_1.shape(), GeomAPI_Shape.SOLID)
while exp.more():
  Reference[model.selection(ResultCompSolid_1, exp.current())] = False
  exp.next()

model.checkFilter(Part_1_doc, model, Filters, Reference)

model.begin()
Filters = model.filters(Part_1_doc, [model.addFilter(name = "VolumeSize", args = [ "isBetween" , 50.0 , 600000.0])])
model.end()
Reference = {}
exp = GeomAPI_ShapeExplorer(ResultBox_1.shape(), GeomAPI_Shape.SOLID)
Reference[model.selection(ResultBox_1, exp.current())] = True
exp = GeomAPI_ShapeExplorer(ResultBox_2.shape(), GeomAPI_Shape.SOLID)
Reference[model.selection(ResultBox_2, exp.current())] = False
exp = GeomAPI_ShapeExplorer(ResultBox_3.shape(), GeomAPI_Shape.SOLID)
Reference[model.selection(ResultBox_3, exp.current())] = False

model.checkFilter(Part_1_doc, model, Filters, Reference)

model.begin()
Filters = model.filters(Part_1_doc, [model.addFilter(name = "VolumeSize", args = [ "isStrictlyBetween" , 500.0 , 750000.0])])
model.end()
Reference = {}
exp = GeomAPI_ShapeExplorer(ResultBox_1.shape(), GeomAPI_Shape.SOLID)
Reference[model.selection(ResultBox_1, exp.current())] = True
exp = GeomAPI_ShapeExplorer(ResultBox_2.shape(), GeomAPI_Shape.SOLID)
Reference[model.selection(ResultBox_2, exp.current())] = False
exp = GeomAPI_ShapeExplorer(ResultBox_3.shape(), GeomAPI_Shape.SOLID)
Reference[model.selection(ResultBox_3, exp.current())] = False

model.checkFilter(Part_1_doc, model, Filters, Reference)
