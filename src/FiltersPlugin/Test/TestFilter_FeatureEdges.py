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
from SketchAPI import *
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()

###===========Test with chamfer on face=========================================================

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Box
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)

### Create Chamfer
Chamfer_1 = model.addChamfer(Part_1_doc, [model.selection("FACE", "Box_1_1/Left")], True, 2, 2, keepSubResults = True)

model.do()
Filters = model.filters(Part_1_doc, [model.addFilter(name = "FeatureEdges",  args = [ 5.0 ])])
model.end()

Reference = {}
ResultChamfer_1_1 = Chamfer_1.result().resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultChamfer_1_1.shape(), GeomAPI_Shape.EDGE)
while exp.more():
  Reference[model.selection(ResultChamfer_1_1, exp.current())] = True
  exp.next()
model.checkFilter(Part_1_doc, model, Filters, Reference)

model.do()

###===========Test with fillet on solide===========================================================

### Create Part
Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()

### Create Box
Box_2 = model.addBox(Part_2_doc, 10, 10, 10)

### Create Fillet
Fillet_1_objects_4 = [model.selection("FACE", "Box_1_1/Left"),
                      model.selection("FACE", "Box_1_1/Front"),
                      model.selection("FACE", "Box_1_1/Top"),
                      model.selection("FACE", "Box_1_1/Right"),
                      model.selection("FACE", "Box_1_1/Bottom")]
Fillet_1 = model.addFillet(Part_2_doc, Fillet_1_objects_4, 2, keepSubResults = True)

model.end()

model.do()
Filters = model.filters(Part_2_doc, [model.addFilter(name = "FeatureEdges",  args = [ 5.0 ])])
model.end()


###===========Test with Chamfer by an angle of 43° and filters with angle 30 and 50 ===========
Reference = {}
ResultFillet_1_1 = Fillet_1.result().resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultFillet_1_1.shape(), GeomAPI_Shape.EDGE)
while exp.more():
  Reference[model.selection(ResultFillet_1_1, exp.current())] = False
  exp.next()
model.checkFilter(Part_2_doc, model, Filters, Reference)

### Create Part
Part_3 = model.addPart(partSet)
Part_3_doc = Part_3.document()

### Create Box
Box_3 = model.addBox(Part_3_doc, 10, 10, 10)

### Create Chamfer
Chamfer_1_objects = [model.selection("FACE", "Box_1_1/Left"),
                     model.selection("FACE", "Box_1_1/Front"),
                     model.selection("FACE", "Box_1_1/Top"),
                     model.selection("FACE", "Box_1_1/Right"),
                     model.selection("FACE", "Box_1_1/Bottom")]
Chamfer_2 = model.addChamfer(Part_3_doc, Chamfer_1_objects, False, 2, 43, keepSubResults = True)

model.end()

model.do()
Filters = model.filters(Part_3_doc, [model.addFilter(name = "FeatureEdges",  args = [ 30.0 ])])
model.end()

Reference = {}
ResultChamfer_2_1 = Chamfer_2.result().resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultChamfer_2_1.shape(), GeomAPI_Shape.EDGE)
while exp.more():
  Reference[model.selection(ResultChamfer_2_1, exp.current())] = True
  exp.next()
model.checkFilter(Part_3_doc, model, Filters, Reference)

model.do()
Filters = model.filters(Part_3_doc, [model.addFilter(name = "FeatureEdges",  args = [ 50.0 ])])
model.end()

Reference = {}
exp = GeomAPI_ShapeExplorer(ResultChamfer_2_1.shape(), GeomAPI_Shape.EDGE)
while exp.more():
  Reference[model.selection(ResultChamfer_2_1, exp.current())] = False
  exp.next()
model.checkFilter(Part_3_doc, model, Filters, Reference)
