# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "Origin"))
SketchCircle_1 = Sketch_1.addCircle(0, 0, 0.3529019640553777)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
SketchCircle_2 = Sketch_1.addCircle(0, 0, 0.4492388920251514)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchCircle_2.center())
model.do()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "PartSet/Sketch_1/Face-SketchCircle_2_2f-SketchCircle_1_2r")], model.selection(), 5, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))
SketchCircle_3 = Sketch_2.addCircle(-0.01730125245914903, 1.175069067120482, 0.2787321880802174)
SketchCircle_4 = Sketch_2.addCircle(-0.01730125245914903, 1.175069067120482, 0.3197841740766353)
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchCircle_3.center(), SketchCircle_4.center())
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_2_2f-SketchCircle_1_2r")], model.selection(), 0, 3)
Extrusion_3 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "PartSet/Sketch_1/Face-SketchCircle_1_2f")], model.selection(), model.selection("FACE", "Extrusion_1_1/To_Face"), 0, model.selection(), 0)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Extrusion_2_1")], [model.selection("SOLID", "Extrusion_3_1")])
Sketch_3 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))
SketchLine_1 = Sketch_3.addLine(-3.362323990279312, -0.5188129046480872, 0.8318943090987101, -0.5188129046480872)
SketchLine_2 = Sketch_3.addLine(0.8318943090987101, -0.5188129046480872, 0.8318943090987101, 7.155729512171993)
SketchLine_3 = Sketch_3.addLine(0.8318943090987101, 7.155729512171993, -3.362323990279312, 7.155729512171993)
SketchLine_4 = Sketch_3.addLine(-3.362323990279312, 7.155729512171993, -3.362323990279312, -0.5188129046480872)
SketchConstraintCoincidence_4 = Sketch_3.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_5 = Sketch_3.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_6 = Sketch_3.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_7 = Sketch_3.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_3.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_3.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_3.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_3.setVertical(SketchLine_4.result())
model.do()
Face_1_objects = [model.selection("EDGE", "Sketch_2/SketchLine_2"), model.selection("EDGE", "Sketch_2/SketchLine_3"), model.selection("EDGE", "Sketch_2/SketchLine_4"), model.selection("EDGE", "Sketch_2/SketchLine_1")]
Face_1 = model.addFace(Part_1_doc, Face_1_objects)
Extrusion_4 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchCircle_2_2f-SketchCircle_1_2r_wire")], model.selection(), model.selection("FACE", "Extrusion_2_1/From_Face"), 0, model.selection(), 0)
Cut_2 = model.addCut(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], [model.selection("SOLID", "Extrusion_4_1")])
Partition_1_objects = [model.selection("SOLID", "Cut_1_1"), model.selection("SOLID", "Cut_2_1"), model.selection("FACE", "Face_1_1")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
Remove_SubShapes_1_objects_1 = [model.selection("SOLID", "Partition_1_1_1_1"), model.selection("SOLID", "Partition_1_1_1_2"), model.selection("SOLID", "Partition_1_1_1_3"), model.selection("SOLID", "Partition_1_1_1_4")]
Remove_SubShapes_1 = model.addRemoveSubShapes(Part_1_doc, model.selection("COMPOUND", "Partition_1_1"))
Remove_SubShapes_1.setSubShapesToKeep(Remove_SubShapes_1_objects_1)
model.do()
model.end()

# check that remove sub-shapes contains correct selection
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Remove_SubShapes_1.feature()))
assert(Remove_SubShapes_1.subshapesToKeep().size() == 4)
assert(Remove_SubShapes_1.subshapesToKeep().value(0).namingName() == "Partition_1_1_1_1")
assert(Remove_SubShapes_1.subshapesToKeep().value(1).namingName() == "Partition_1_1_1_2")
assert(Remove_SubShapes_1.subshapesToKeep().value(2).namingName() == "Partition_1_1_1_3")
assert(Remove_SubShapes_1.subshapesToKeep().value(3).namingName() == "Partition_1_1_1_4")
assert(Remove_SubShapes_1.subshapesToRemove().size() == 1)
assert(Remove_SubShapes_1.subshapesToRemove().value(0).namingName() == "Partition_1_1_2")
