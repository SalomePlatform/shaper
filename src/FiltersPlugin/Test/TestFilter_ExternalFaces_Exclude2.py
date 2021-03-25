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
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Left"), model.selection("FACE", "Box_1_1/Right"))
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchLine_1 = Sketch_1.addLine(15, 2, 5, 2)
SketchLine_2 = Sketch_1.addLine(5, 2, 5, 8)
SketchLine_3 = Sketch_1.addLine(5, 8, 15, 8)
SketchLine_4 = Sketch_1.addLine(15, 8, 15, 2)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_2.result(), 5, True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_4.result(), 15, True)
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_1.result(), 2, True)
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_3.result(), 8, True)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")])
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("FACE", "Face_1_1")], keepSubResults = True)
Filters = model.filters(Part_1_doc, [model.addFilter(name = "ExternalFaces", exclude = True)])
model.end()

Reference = {}

ResultPartition = Partition_1.result().resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultPartition.shape(), GeomAPI_Shape.FACE)
Reference[model.selection(ResultPartition, exp.current())] = False; exp.next()
Reference[model.selection(ResultPartition, exp.current())] = False; exp.next()
Reference[model.selection(ResultPartition, exp.current())] = False; exp.next()
Reference[model.selection(ResultPartition, exp.current())] = False; exp.next()
Reference[model.selection(ResultPartition, exp.current())] = False; exp.next()
Reference[model.selection(ResultPartition, exp.current())] = False; exp.next()
Reference[model.selection(ResultPartition, exp.current())] = True;  exp.next()
Reference[model.selection(ResultPartition, exp.current())] = False; exp.next()
assert(not exp.more())

model.checkFilter(Part_1_doc, model, Filters, Reference)
