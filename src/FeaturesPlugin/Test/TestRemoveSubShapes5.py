# Copyright (C) 2014-2023  CEA, EDF
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
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Bottom"), model.selection("FACE", "Box_1_1/Top"))
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchLine_1 = Sketch_1.addLine(20, 0, 0, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, -20)
SketchLine_3 = Sketch_1.addLine(0, -20, 20, -20)
SketchLine_4 = Sketch_1.addLine(20, -20, 20, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 20)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")])
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("FACE", "Face_1_1")], model.selection("EDGE", "PartSet/OZ"), 2, 2)
Partition_1_objects = [model.selection("SOLID", "Box_1_1"), model.selection("FACE", "LinearCopy_1_1_1"), model.selection("FACE", "LinearCopy_1_1_2")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
Remove_SubShapes_1 = model.addRemoveSubShapes(Part_1_doc, model.selection("COMPOUND", "Partition_1_1"))
Remove_SubShapes_1.setSubShapesToRemove([model.selection("COMPSOLID", "Partition_1_1_1/Partition_1_1_1")])
model.do()

# check error when compsolid is selected
assert(Remove_SubShapes_1.feature().error() != "")

# fix the problematic feature
Remove_SubShapes_1.setSubShapesToRemove([model.selection("SOLID", "Partition_1_1_1_1"), model.selection("SOLID", "Partition_1_1_1_2"), model.selection("SOLID", "Partition_1_1_1_3")])
model.do()
assert(Remove_SubShapes_1.feature().error() == "")

# check number of sub-shapes
model.testNbResults(Remove_SubShapes_1, 1)
model.testNbSubResults(Remove_SubShapes_1, [2])
model.testNbSubShapes(Remove_SubShapes_1, GeomAPI_Shape.COMPOUND, [1])
model.testNbSubShapes(Remove_SubShapes_1, GeomAPI_Shape.COMPSOLID, [0])
model.testNbSubShapes(Remove_SubShapes_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Remove_SubShapes_1, GeomAPI_Shape.FACE, [2])

model.end()

assert(model.checkPythonDump())
