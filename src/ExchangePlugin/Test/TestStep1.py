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

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "LGX", '3.0')
model.addParameter(Part_1_doc, "LGY", '2.825')
model.addParameter(Part_1_doc, "HTR", '1.2')

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(0, 0, 1.165, 0)

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchPoint_1.result())

### Create SketchProjection
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.result())

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(1.165, 0, 1.165, 0.5)
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.startPoint())
Sketch_1.setVertical(SketchLine_3.result())

### Create SketchLine
SketchLine_4 = Sketch_1.addLine(1.165, 0.5, 1.665, 0.5)
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
Sketch_1.setHorizontal(SketchLine_4.result())

### Create SketchLine
SketchLine_5 = Sketch_1.addLine(1.665, 0.5, 1.665, 0)
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
Sketch_1.setVertical(SketchLine_5.result())

### Create SketchLine
SketchLine_6 = Sketch_1.addLine(1.665, 0, 2.825, 0)
Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
Sketch_1.setHorizontal(SketchLine_6.result())

### Create SketchLine
SketchLine_7 = Sketch_1.addLine(2.825, 0, 2.825, 1.2)
Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
Sketch_1.setVertical(SketchLine_7.result())

### Create SketchLine
SketchLine_8 = Sketch_1.addLine(2.825, 1.2, 0, 1.2)
Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())

### Create SketchProjection
SketchProjection_3 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_9 = SketchProjection_3.createdFeature()
Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.result())
Sketch_1.setHorizontal(SketchLine_8.result())

### Create SketchLine
SketchLine_10 = Sketch_1.addLine(0, 1.2, 0, 0)
Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_10.startPoint())
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_10.endPoint())
Sketch_1.setLength(SketchLine_8.result(), "LGY")
Sketch_1.setLength(SketchLine_10.result(), "HTR")
Sketch_1.setLength(SketchLine_3.result(), "HTR/1.2*0.5")
Sketch_1.setLength(SketchLine_4.result(), "LGY/2.825*0.5")
Sketch_1.setLength(SketchLine_1.result(), "LGY/2.825*1.165")
Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_2.result())
model.do()

### Create Face
Face_1 = model.addFace(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")])

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))

### Create SketchLine
SketchLine_11 = Sketch_2.addLine(0, 0, 0.9999999999999999, 0)

### Create SketchProjection
SketchProjection_4 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_10_EndVertex"), False)
SketchPoint_2 = SketchProjection_4.createdFeature()
Sketch_2.setCoincident(SketchLine_11.startPoint(), SketchPoint_2.result())

### Create SketchProjection
SketchProjection_5 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_12 = SketchProjection_5.createdFeature()
Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.result())

### Create SketchLine
SketchLine_13 = Sketch_2.addLine(0.9999999999999999, 0, 1, 0.95)
Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_13.startPoint())
Sketch_2.setVertical(SketchLine_13.result())

### Create SketchLine
SketchLine_14 = Sketch_2.addLine(1, 0.95, 2.25, 0.95)
Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
Sketch_2.setHorizontal(SketchLine_14.result())

### Create SketchLine
SketchLine_15 = Sketch_2.addLine(2.25, 0.95, 2.25, 0)
Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
Sketch_2.setCoincident(SketchLine_15.endPoint(), SketchLine_12.result())

### Create SketchLine
SketchLine_16 = Sketch_2.addLine(2.25, 0, 3, 0)
Sketch_2.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
Sketch_2.setCoincident(SketchLine_16.endPoint(), SketchLine_12.result())

### Create SketchLine
SketchLine_17 = Sketch_2.addLine(3, 0, 3, 1.2)
Sketch_2.setCoincident(SketchLine_16.endPoint(), SketchLine_17.startPoint())

### Create SketchLine
SketchLine_18 = Sketch_2.addLine(3, 1.2, 0, 1.2)
Sketch_2.setCoincident(SketchLine_17.endPoint(), SketchLine_18.startPoint())

### Create SketchProjection
SketchProjection_6 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_10_StartVertex"), False)
SketchPoint_3 = SketchProjection_6.createdFeature()
Sketch_2.setCoincident(SketchLine_18.endPoint(), SketchPoint_3.result())
Sketch_2.setHorizontal(SketchLine_18.result())
Sketch_2.setVertical(SketchLine_17.result())
Sketch_2.setVertical(SketchLine_15.result())
Sketch_2.setLength(SketchLine_11.result(), "LGX/3.")
Sketch_2.setLength(SketchLine_14.result(), "LGX/3.*1.25")
Sketch_2.setLength(SketchLine_18.result(), "LGX")
Sketch_2.setLength(SketchLine_13.result(), "HTR/1.2*0.95")

### Create SketchLine
SketchLine_19 = Sketch_2.addLine(0, 1.2, 0, 0)
Sketch_2.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
Sketch_2.setCoincident(SketchLine_11.startPoint(), SketchLine_19.endPoint())
model.do()

### Create Face
Face_2 = model.addFace(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_2")])

### Create Sketch
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Face_1_1"))

### Create SketchLine
SketchLine_20 = Sketch_3.addLine(1.325, 0, 0, 0)
Sketch_3.setHorizontal(SketchLine_20.result())

### Create SketchProjection
SketchProjection_7 = Sketch_3.addProjection(model.selection("VERTEX", "Face_1_1/Modified_Vertex&Sketch_1/SketchLine_10_EndVertex&Sketch_1/SketchLine_1_StartVertex"), False)
SketchPoint_4 = SketchProjection_7.createdFeature()
Sketch_3.setCoincident(SketchLine_20.endPoint(), SketchPoint_4.result())

### Create SketchLine
SketchLine_21 = Sketch_3.addLine(0, 0, 0, 1.2)
Sketch_3.setVertical(SketchLine_21.result())

### Create SketchLine
SketchLine_22 = Sketch_3.addLine(0, 1.2, 1.325, 1.2)
Sketch_3.setHorizontal(SketchLine_22.result())
Sketch_3.setLength(SketchLine_22.result(), "LGY/2.825*1.325")

### Create SketchLine
SketchLine_23 = Sketch_3.addLine(1.325, 1.2, 1.325, 0)
Sketch_3.setCoincident(SketchLine_23.endPoint(), SketchLine_20.startPoint())
Sketch_3.setCoincident(SketchLine_20.endPoint(), SketchLine_21.startPoint())
Sketch_3.setCoincident(SketchLine_21.endPoint(), SketchLine_22.startPoint())
Sketch_3.setCoincident(SketchLine_22.endPoint(), SketchLine_23.startPoint())
Sketch_3.setVertical(SketchLine_23.result())

### Create SketchProjection
SketchProjection_8 = Sketch_3.addProjection(model.selection("EDGE", "Face_1_1/Modified_Edge&Sketch_1/SketchLine_8"), False)
SketchLine_24 = SketchProjection_8.createdFeature()
Sketch_3.setCoincident(SketchLine_22.endPoint(), SketchLine_24.result())
model.do()

### Create Face
Face_3 = model.addFace(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_3")])

### Create Translation
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "all-in-Face_3")], axis = model.selection("EDGE", "PartSet/OX"), distance = "LGX", keepSubResults = True)

### Create Plane
Plane_4 = model.addPlane(Part_1_doc, model.selection("VERTEX", "Sketch_2/SketchLine_17_EndVertex"), model.selection("VERTEX", "Translation_1_1_1/MV:Translated&Sketch_3/SketchLine_22_EndVertex&Sketch_3/SketchLine_23_StartVertex"), model.selection("VERTEX", "Face_1_1/Modified_Vertex&Sketch_1/SketchLine_8_StartVertex&Sketch_1/SketchLine_7_EndVertex"))

### Create Sketch
Sketch_4 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))

### Create SketchLine
SketchLine_25 = Sketch_4.addLine(0, 0, 3, 0)

### Create SketchProjection
SketchProjection_9 = Sketch_4.addProjection(model.selection("VERTEX", "Sketch_3/SketchLine_21_EndVertex"), False)
SketchPoint_5 = SketchProjection_9.createdFeature()
Sketch_4.setCoincident(SketchLine_25.startPoint(), SketchPoint_5.result())

### Create SketchProjection
SketchProjection_10 = Sketch_4.addProjection(model.selection("VERTEX", "Sketch_2/SketchLine_17_EndVertex"), False)
SketchPoint_6 = SketchProjection_10.createdFeature()
Sketch_4.setCoincident(SketchLine_25.endPoint(), SketchPoint_6.result())

### Create SketchLine
SketchLine_26 = Sketch_4.addLine(3, 0, 3, 1.325)
Sketch_4.setCoincident(SketchLine_25.endPoint(), SketchLine_26.startPoint())

### Create SketchProjection
SketchProjection_11 = Sketch_4.addProjection(model.selection("VERTEX", "Translation_1_1_1/MV:Translated&Sketch_3/SketchLine_22_EndVertex&Sketch_3/SketchLine_23_StartVertex"), False)
SketchPoint_7 = SketchProjection_11.createdFeature()
Sketch_4.setCoincident(SketchLine_26.endPoint(), SketchPoint_7.result())

### Create SketchLine
SketchLine_27 = Sketch_4.addLine(3, 1.325, 0, 2.825)
Sketch_4.setCoincident(SketchLine_26.endPoint(), SketchLine_27.startPoint())

### Create SketchProjection
SketchProjection_12 = Sketch_4.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_7_EndVertex"), False)
SketchPoint_8 = SketchProjection_12.createdFeature()
Sketch_4.setCoincident(SketchLine_27.endPoint(), SketchPoint_8.result())

### Create SketchLine
SketchLine_28 = Sketch_4.addLine(0, 2.825, 0, 0)
Sketch_4.setCoincident(SketchLine_27.endPoint(), SketchLine_28.startPoint())
Sketch_4.setCoincident(SketchLine_25.startPoint(), SketchLine_28.endPoint())
model.do()

### Create Face
Face_4 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_4/Face-SketchLine_25r-SketchLine_26f-SketchLine_27f-SketchLine_28f")])

### Create Sketch
Sketch_5 = model.addSketch(Part_1_doc, model.selection("FACE", "Face_4_1"))

### Create SketchLine
SketchLine_29 = Sketch_5.addLine(1.5, 0, 1.5, 1.6)

### Create SketchProjection
SketchProjection_13 = Sketch_5.addProjection(model.selection("EDGE", "Face_4_1/Modified_Edge&Sketch_4/SketchLine_25"), False)
SketchLine_30 = SketchProjection_13.createdFeature()
Sketch_5.setCoincident(SketchLine_29.startPoint(), SketchLine_30.result())
Sketch_5.setVertical(SketchLine_29.result())
Sketch_5.setLength(SketchLine_29.result(), "LGY/2.825*1.6")
Sketch_5.setHorizontalDistance(SketchAPI_Line(SketchLine_30).startPoint(), SketchLine_29.startPoint(), "LGX*0.5")
model.do()

### Create Partition
Partition_1_objects = [model.selection("FACE", "Face_1_1"),
                       model.selection("FACE", "Face_2_1"),
                       model.selection("COMPOUND", "Translation_1_1"),
                       model.selection("FACE", "Face_4_1")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects, keepSubResults = True)

### Create Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Partition_1_1_4")], model.selection(), "HTR", 0, "Faces|Wires")

### Create Recover
Recover_1 = model.addRecover(Part_1_doc, Extrusion_1, [Partition_1.result()], True)

### Create Extrusion
Extrusion_2_objects = [model.selection("VERTEX", "Recover_1_1_3/Modified_Vertex&Sketch_3/SketchLine_20_StartVertex&Sketch_3/SketchLine_23_EndVertex"),
                       model.selection("VERTEX", "[Recover_1_1_2/Modified_Edge&Sketch_2/SketchLine_16]e[Recover_1_1_2/Modified_Edge&Sketch_3/SketchLine_21&Sketch_2/SketchLine_17]e"),
                       model.selection("VERTEX", "[Recover_1_1_1/Modified_Edge&Sketch_1/SketchLine_1]e[Recover_1_1_1/Modified_Edge&Sketch_2/SketchLine_19&Sketch_1/SketchLine_10]e"),
                       model.selection("VERTEX", "Recover_1_1_1/Modified_Vertex&Sketch_1/SketchLine_7_StartVertex&Sketch_1/SketchLine_6_EndVertex")]
Extrusion_2 = model.addExtrusion(Part_1_doc, Extrusion_2_objects, model.selection("EDGE", "PartSet/OZ"), 0, "HTR")

### Create Partition
Partition_2_objects = [model.selection("COMPOUND", "all-in-Extrusion_1"),
                       model.selection("COMPOUND", "all-in-Recover_1"),
                       model.selection("COMPOUND", "all-in-Extrusion_2")]
Partition_2 = model.addPartition(Part_1_doc, Partition_2_objects, keepSubResults = True)
Partition_2.result().setName("Structure")
Partition_2.result().subResult(0).setName("Hangar")
Partition_2.result().subResult(0).setColor(170, 0, 0)
Partition_2.result().subResult(1).setName("Mur_1")
Partition_2.result().subResult(2).setName("Mur_2")
Partition_2.result().subResult(3).setName("Mur_3")
Partition_2.result().subResult(4).setName("Poteau_1")
Partition_2.result().subResult(5).setName("Poteau_2")
Partition_2.result().subResult(6).setName("Poteau_3")
Partition_2.result().subResult(7).setName("Poteau_4")

file_name = model.getTmpFileName('teststep1', '.stp')

### Export to STEP format
Export_1 = model.exportToFile(Part_1_doc, file_name,
                              [model.selection("COMPOUND", "Structure")])

### Import from step
Import_1 = model.addImportSTEP(Part_1_doc, file_name, True, True, True)

model.end()

# Check the imported names and colors correspond to the exported ones
res = Import_1.result()
assert(file_name.find(res.name()[:-3]) > 0) # the higher level compound name equals to the file name
assert(res.subResult(0).name() == "Hangar")
assert(res.subResult(1).name() == "Mur_1")
assert(res.subResult(2).name() == "Mur_2")
assert(res.subResult(3).name() == "Mur_3")
assert(res.subResult(4).name() == "Poteau_1")
assert(res.subResult(5).name() == "Poteau_2")
assert(res.subResult(6).name() == "Poteau_3")
assert(res.subResult(7).name() == "Poteau_4")
# get color of "Hangar": 3 components in the array attribute
hangarBody = model.modelAPI_ResultBody(Import_1.feature().firstResult()).subResult(0)
assert(hangarBody.data().intArray("Color").size() == 3)
assert(hangarBody.data().intArray("Color").value(0) == 170)
assert(hangarBody.data().intArray("Color").value(1) == 0)
assert(hangarBody.data().intArray("Color").value(2) == 0)
