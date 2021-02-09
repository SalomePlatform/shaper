# Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(0, 0, 30.79356776198042)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
model.do()

Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchCircle_1_2f_wire")], model.selection(), 100, 100)

Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchCircle_2 = Sketch_2.addCircle(0, 0, 25.19116787687743)
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchPoint_2.result(), SketchCircle_2.center())
model.do()

Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2f")], model.selection(), 100, 0)
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], model.selection("EDGE", "PartSet/OX"), 90)

Intersection_1 = model.addIntersection(Part_1_doc, [model.selection("SOLID", "Rotation_1_1"), model.selection("SOLID", "Extrusion_2_1")])

Sketch_3 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_3.addLine(7.903534676574237, 7.903534676574236, -7.903534676574236, 7.903534676574236)
SketchLine_2 = Sketch_3.addLine(-7.903534676574236, 7.903534676574236, -7.903534676574236, -7.903534676574237)
SketchLine_3 = Sketch_3.addLine(-7.903534676574236, -7.903534676574237, 7.903534676574237, -7.903534676574237)
SketchLine_4 = Sketch_3.addLine(7.903534676574237, -7.903534676574237, 7.903534676574237, 7.903534676574236)
SketchConstraintCoincidence_3 = Sketch_3.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_4 = Sketch_3.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_5 = Sketch_3.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_6 = Sketch_3.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_3.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_3.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_3.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_3.setVertical(SketchLine_4.result())
SketchConstraintEqual_1 = Sketch_3.setEqual(SketchLine_4.result(), SketchLine_1.result())
SketchLine_5 = Sketch_3.addLine(-7.903534676574236, 7.903534676574236, 7.903534676574237, -7.903534676574237)
SketchLine_5.setAuxiliary(True)
SketchConstraintCoincidence_7 = Sketch_3.setCoincident(SketchLine_1.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_8 = Sketch_3.setCoincident(SketchLine_3.endPoint(), SketchLine_5.endPoint())
SketchProjection_3 = Sketch_3.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_3 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_9 = Sketch_3.setCoincident(SketchLine_5.result(), SketchAPI_Point(SketchPoint_3).coordinates())
SketchConstraintMiddle_1 = Sketch_3.setMiddlePoint(SketchAPI_Point(SketchPoint_3).coordinates(), SketchLine_5.result())
model.do()

Edge_1_objects = [model.selection("EDGE", "Sketch_3/SketchLine_2"), model.selection("EDGE", "Sketch_3/SketchLine_1"), model.selection("EDGE", "Sketch_3/SketchLine_4"), model.selection("EDGE", "Sketch_3/SketchLine_3")]
Edge_1 = model.addEdge(Part_1_doc, Edge_1_objects)

Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOZ"), model.selection("EDGE", "PartSet/OZ"), 135)
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "Plane_1"), model.selection("EDGE", "PartSet/OZ"), 90)

Partition_1_objects = [model.selection("EDGE", "Intersection_1_1"), model.selection("FACE", "Plane_2"), model.selection("FACE", "Plane_1")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)

Filling_1 = model.addFilling(Part_1_doc, [model.selection("EDGE", "Partition_1_1_2"), model.selection("EDGE", "Edge_1_2")])
Filling_2 = model.addFilling(Part_1_doc, [model.selection("EDGE", "Partition_1_1_3"), model.selection("EDGE", "Edge_1_1")])
Filling_3 = model.addFilling(Part_1_doc, [model.selection("EDGE", "Partition_1_1_4"), model.selection("EDGE", "Edge_1_4")])

Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Partition_1_1_1"), model.selection("EDGE", "Partition_1_1_5")])
Filling_4 = model.addFilling(Part_1_doc, [model.selection("EDGE", "Edge_1_3"), model.selection("WIRE", "Wire_1_1")])
model.do()

from GeomAPI import *

# check fillings
REF_DATA = [(Filling_1, 719.149788883378505488508380949),
            (Filling_2, 910.894041506332769131404347718),
            (Filling_3, 719.149788883378505488508380949),
            (Filling_4, 910.894530912501409147807862610)]
for ref in REF_DATA:
    model.testNbResults(ref[0], 1)
    model.testNbSubResults(ref[0], [0])
    model.testNbSubShapes(ref[0], GeomAPI_Shape.SOLID, [0])
    model.testNbSubShapes(ref[0], GeomAPI_Shape.FACE, [1])
    model.testNbSubShapes(ref[0], GeomAPI_Shape.EDGE, [4])
    model.testNbSubShapes(ref[0], GeomAPI_Shape.VERTEX, [8])
    model.testResultsAreas(ref[0], [ref[1]])

model.end()

assert(model.checkPythonDump())
