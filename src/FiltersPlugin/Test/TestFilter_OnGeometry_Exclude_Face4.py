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
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10, 45)
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Cylinder_1_1/Face_4"))
SketchArc_1 = Sketch_1.addArc(2.402631123626777, 4.538350751693263, 2.946623076146047, 2.768866376496988, 3.159520035183744, 6.227765257973941, False)
model.do()
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchArc_1_2")])
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("EDGE", "Edge_1_1")], model.selection("EDGE", "PartSet/OZ"), 0, 90)
FiltersCyl = model.filters(Part_1_doc, [model.addFilter(name = "OnGeometry", exclude = True, args = [model.selection("FACE", "Cylinder_1_1/Face_1")])])
FiltersRev = model.filters(Part_1_doc, [model.addFilter(name = "OnGeometry", exclude = True, args = [model.selection("FACE", "Revolution_1_1")])])
model.end()

Reference = {}
ResultRevolution = Revolution_1.result().resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultRevolution.shape(), GeomAPI_Shape.FACE)
Reference[model.selection(ResultRevolution, exp.current())] = True; exp.next()
assert(not exp.more())

model.checkFilter(Part_1_doc, model, FiltersCyl, Reference)

Reference = {}
ResultCylinder = Cylinder_1.result().resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultCylinder.shape(), GeomAPI_Shape.FACE)
while exp.more():
  Reference[model.selection(ResultCylinder, exp.current())] = True
  exp.next()

model.checkFilter(Part_1_doc, model, FiltersRev, Reference)
