# Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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

from GeomAPI import *
from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(0, 0, 40)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 40)
model.do()
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f"), 10, False)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchCircle_1"), True)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchCircle_2 = Sketch_2.addCircle(0, 0, 50)
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_2).coordinates(), SketchCircle_2.center())
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchCircle_2.results()[1], 50)
model.do()
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "Plane_1"), 10, False)
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_2"))
SketchProjection_3 = Sketch_3.addProjection(model.selection("VERTEX", "Sketch_1/SketchCircle_1"), True)
SketchPoint_3 = SketchProjection_3.createdFeature()
SketchCircle_3 = Sketch_3.addCircle(0, 0, 53)
SketchConstraintCoincidence_3 = Sketch_3.setCoincident(SketchAPI_Point(SketchPoint_3).coordinates(), SketchCircle_3.center())
SketchConstraintRadius_3 = Sketch_3.setRadius(SketchCircle_3.results()[1], 53)
model.do()
Filling_1_objects = [model.selection("EDGE", "Sketch_1/SketchCircle_1_2"), model.selection("EDGE", "Sketch_2/SketchCircle_2_2"), model.selection("EDGE", "Sketch_3/SketchCircle_3_2")]
Filling_1 = model.addFilling(Part_1_doc, Filling_1_objects)
Sketch_4 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchProjection_4 = Sketch_4.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_4 = SketchProjection_4.createdFeature()
SketchProjection_5 = Sketch_4.addProjection(model.selection("EDGE", "PartSet/OX"), True)
SketchLine_1 = SketchProjection_5.createdFeature()
SketchMultiRotation_1 = Sketch_4.addRotation([SketchLine_1.result()], SketchAPI_Point(SketchPoint_4).coordinates(), 360, 12, True)
[SketchLine_1, SketchLine_2, SketchLine_3, SketchLine_4, SketchLine_5, SketchLine_6, SketchLine_7, SketchLine_8, SketchLine_9, SketchLine_10, SketchLine_11, SketchLine_12] = SketchMultiRotation_1.rotated()
model.do()
Edge_1 = model.addEdge(Part_1_doc, [model.selection("COMPOUND", "Sketch_4")], False)
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Edge_1")], model.selection(), model.selection("FACE", "Plane_2"), 0, model.selection(), 0)
Split_1 = model.addSplit(Part_1_doc, [model.selection("FACE", "Filling_1_1")], [model.selection("COMPOUND", "Extrusion_1_1")], keepSubResults = True)
FilterFace = model.filters(Part_1_doc, [model.addFilter(name = "OnGeometry", args = [model.selection("FACE", "Split_1_1_1")])])
model.end()

Reference = {}
ResultSplit = Split_1.result().resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultSplit.shape(), GeomAPI_Shape.FACE)
while exp.more():
    Reference[model.selection(ResultSplit, exp.current())] = True
    exp.next()
model.checkFilter(Part_1_doc, model, FilterFace, Reference)
