#!/usr/bin/env python

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

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "EP_MIN", '0.001')
model.addParameter(Part_1_doc, "tube_r", '0.25')
model.addParameter(Part_1_doc, "tube_l", '1.0')
model.addParameter(Part_1_doc, "rugo_r_min", '0.005')
model.addParameter(Part_1_doc, "rugo_r_max", '0.1')

### Create Sphere
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), "rugo_r_max")
Sphere_1.setName("base")
Sphere_1.result().setName("Sphere_1_1")

### Create Rotation
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("COMPOUND", "all-in-base")], axis = model.selection("EDGE", "PartSet/OX"), angle = 90, keepSubResults = True)
Rotation_1.result().setName("Rotation_1_1")

### Create Plane
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), "- 0.6*rugo_r_max", False)
Plane_4.result().setName("plan_partition")

### Create Partition
Partition_1 = model.addPartition(Part_1_doc, [model.selection("COMPOUND", "all-in-Rotation_1"), model.selection("FACE", "plan_partition")], keepSubResults = True)

### Create Partition
Partition_2_objects = [model.selection("SOLID", "Partition_1_1_1"),
                       model.selection("FACE", "PartSet/YOZ"),
                       model.selection("FACE", "PartSet/XOZ")]
Partition_2 = model.addPartition(Part_1_doc, Partition_2_objects, keepSubResults = True)
Partition_2.setName("PARTITION")
Partition_2.result().setName("PARTITION")
Partition_2.result().subResult(0).setName("Partition_2_1_1")
Partition_2.result().subResult(1).setName("Partition_2_1_2")
Partition_2.result().subResult(2).setName("Partition_2_1_3")
Partition_2.result().subResult(3).setName("Partition_2_1_4")
Partition_2.result().subResult(4).setName("Partition_2_1_5")

### Create Translation
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "all-in-PARTITION")], vector = ["tube_l*0.5", 0, "tube_r+EP_MIN"], keepSubResults = True)
Translation_1.setName("RUGO")
Translation_1.result().setName("RUGO")
Translation_1.result().subResult(0).setName("Translation_1_1_1")
Translation_1.result().subResult(0).subResult(0).setName("Translation_1_1_1_1")
Translation_1.result().subResult(0).subResult(1).setName("Translation_1_1_1_2")
Translation_1.result().subResult(0).subResult(2).setName("Translation_1_1_1_3")
Translation_1.result().subResult(0).subResult(3).setName("Translation_1_1_1_4")
Translation_1.result().subResult(0).subResult(4).setName("Translation_1_1_1_5")

### Create Cylinder
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OX"), "tube_r", "tube_l")
Cylinder_1.setName("tube_sain")
Cylinder_1.result().setName("tube_sain")

### Create Cut
Cut_1 = model.addCut(Part_1_doc, [model.selection("COMPOUND", "all-in-tube_sain")], [model.selection("COMPOUND", "all-in-RUGO")], keepSubResults = True)
Cut_1.result().setTransparency(0.7)

### Create Extrusion
Extrusion_1_objects = [model.selection("EDGE", "Translation_1_1_1_5/ME:Translated&plan_partition/plan_partition&Sphere_1_1/Face_1&new_weak_name_1"),
                       model.selection("EDGE", "Translation_1_1_1_5/ME:Translated&plan_partition/plan_partition&Sphere_1_1/Face_1&new_weak_name_2"),
                       model.selection("EDGE", "Translation_1_1_1_5/ME:Translated&plan_partition/plan_partition&Sphere_1_1/Face_1&new_weak_name_4"),
                       model.selection("EDGE", "Translation_1_1_1_5/ME:Translated&plan_partition/plan_partition&Sphere_1_1/Face_1&new_weak_name_3")]
Extrusion_1 = model.addExtrusion(Part_1_doc, Extrusion_1_objects, model.selection("EDGE", "PartSet/OZ"), 0, "2.*tube_r")

### Create Recover
Recover_1 = model.addRecover(Part_1_doc, Extrusion_1, [Cut_1.result()])

### Create Common
Common_1 = model.addCommon(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_1"), model.selection("COMPOUND", "all-in-Recover_1")], keepSubResults = True)
Common_1.result().subResult(0).setName("Common_1_1_1")
Common_1.result().subResult(1).setName("Common_1_1_2")
Common_1.result().subResult(2).setName("Common_1_1_3")
Common_1.result().subResult(3).setName("Common_1_1_4")

### Create Recover
Recover_2 = model.addRecover(Part_1_doc, Common_1, [Recover_1.result()])

### Create Partition
Partition_3 = model.addPartition(Part_1_doc, [model.selection("COMPOUND", "all-in-Common_1"), model.selection("COMPOUND", "all-in-Recover_2")], keepSubResults = True)
Partition_3.result().setTransparency(0.53)
Partition_3.result().subResult(0).setName("Partition_3_1_1")
Partition_3.result().subResult(1).setName("Partition_3_1_2")

### Create Extrusion
Extrusion_2_objects = [model.selection("EDGE", "(Partition_3_1_1/Modified_Edge&plan_partition/plan_partition&Sphere_1_1/Face_1&__tube_sain/To_Edge)(Partition_3_1_1/Modified_Edge&plan_partition/plan_partition&Sphere_1_1/Face_1&tube_sain_4/To_Edge)(Common_1_1_4/Generated_Edge&____tube_sain/Face_1&plan_partition/plan_partition&Sphere_1_1/Face_1)2(Common_1_1_1/Generated_Edge&____tube_sain/Face_1&plan_partition/plan_partition&Sphere_1_1/Face_1)2(Partition_3_1_1/Modified_Edge&plan_partition/plan_partition&Sphere_1_1/Face_1&tube_sain_3/To_Edge)2(Partition_3_1_1/Modified_Edge&plan_partition/plan_partition&Sphere_1_1/Face_1&tube_sain_2/To_Edge)2"),
                       model.selection("EDGE", "[Partition_3_1_2/Modified_Face&plan_partition/plan_partition&Sphere_1_1/Face_1&new_weak_name_1][Partition_3_1_2/Modified_Face&plan_partition/plan_partition&Sphere_1_1/Face_1&new_weak_name_3]"),
                       model.selection("EDGE", "(Partition_3_1_1/Modified_Edge&plan_partition/plan_partition&Sphere_1_1/Face_1&tube_sain_3/To_Edge)(Partition_3_1_1/Modified_Edge&plan_partition/plan_partition&Sphere_1_1/Face_1&tube_sain_2/To_Edge)(Partition_3_1_1/Modified_Edge&PartSet/YOZ/YOZ&plan_partition/plan_partition&Sphere_1_1/Face_1)"),
                       model.selection("EDGE", "Partition_3_1_2/Modified_Edge&PartSet/YOZ/YOZ&plan_partition/plan_partition&Sphere_1_1/Face_1")]
Extrusion_2 = model.addExtrusion(Part_1_doc, Extrusion_2_objects, model.selection("EDGE", "PartSet/OX"), "tube_l", "tube_l")

### Create Recover
Recover_3 = model.addRecover(Part_1_doc, Extrusion_2, [Partition_3.result()], True)

### Create Common
Common_2 = model.addCommon(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_2"), model.selection("COMPOUND", "all-in-Recover_3")], keepSubResults = True)
Common_2.result().subResult(0).setName("Common_2_1_1")
Common_2.result().subResult(1).setName("Common_2_1_2")
Common_2.result().subResult(2).setName("Common_2_1_3")
Common_2.result().subResult(3).setName("Common_2_1_4")
Common_2.result().subResult(4).setName("Common_2_1_5")
Common_2.result().subResult(5).setName("Common_2_1_6")
Common_2.result().subResult(6).setName("Common_2_1_7")
Common_2.result().subResult(7).setName("Common_2_1_8")

### Create Recover
Recover_4 = model.addRecover(Part_1_doc, Common_2, [Recover_3.result()])

### Create Partition
Partition_4 = model.addPartition(Part_1_doc, [model.selection("COMPOUND", "all-in-Common_2"), model.selection("COMPOUND", "all-in-Recover_4")], keepSubResults = True)
Partition_4.result().setTransparency(0.67)
Partition_4.result().subResult(0).setName("Partition_4_1_1")
Partition_4.result().subResult(1).setName("Partition_4_1_2")
Partition_4.result().subResult(2).setName("Partition_4_1_3")
Partition_4.result().subResult(3).setName("Partition_4_1_4")
Partition_4.result().subResult(4).setName("Partition_4_1_5")

### Create Extrusion
Extrusion_3 = model.addExtrusion(Part_1_doc, [model.selection("EDGE", "(Partition_4_1_1/Modified_Edge&plan_partition/plan_partition&Sphere_1_1/Face_1&_tube_sain_3/To_Edge)(Partition_4_1_1/Modified_Edge&plan_partition/plan_partition&Sphere_1_1/Face_1&_tube_sain_4/To_Edge)(Common_2_1_6/Generated_Edge&_______tube_sain/Face_2&PartSet/YOZ/YOZ&Sphere_1_1/Face_1)3(Common_2_1_8/Generated_Edge&_______tube_sain/Face_2&PartSet/YOZ/YOZ&plan_partition/plan_partition&Sphere_1_1/Face_1)3(Common_2_1_4/Generated_Edge&_______tube_sain/Face_2&tube_sain_2/tube_sain_2)3(Common_2_1_2/Generated_Edge&_______tube_sain/Face_2&PartSet/YOZ/YOZ&Sphere_1_1/Face_1)3(Partition_4_1_1/Modified_Edge&_______tube_sain/Face_1&PartSet/YOZ/YOZ&plan_partition/plan_partition&Sphere_1_1/Face_1)3"), model.selection("EDGE", "(Partition_4_1_5/Modified_Edge&plan_partition/plan_partition&Sphere_1_1/Face_1&_tube_sain_4/To_Edge)(Partition_4_1_5/Modified_Edge&plan_partition/plan_partition&Sphere_1_1/Face_1&_tube_sain_3/To_Edge)")], model.selection("EDGE", "PartSet/OX"), "tube_l", 0)

### Create Recover
Recover_5 = model.addRecover(Part_1_doc, Extrusion_3, [Partition_4.result()], True)

### Create Common
Common_3 = model.addCommon(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_3"), model.selection("COMPOUND", "all-in-Recover_5")], keepSubResults = True)
Common_3.result().subResult(0).setName("Common_3_1_1")
Common_3.result().subResult(1).setName("Common_3_1_2")

### Create Recover
Recover_6 = model.addRecover(Part_1_doc, Common_3, [Recover_5.result()], True)

### Create Partition
Partition_5 = model.addPartition(Part_1_doc, [model.selection("COMPOUND", "all-in-Common_3"), model.selection("COMPOUND", "all-in-Recover_6")], keepSubResults = True)
Partition_5.result().setTransparency(0.72)
Partition_5.result().subResult(0).setName("Partition_5_1_1")
Partition_5.result().subResult(1).setName("Partition_5_1_2")
Partition_5.result().subResult(2).setName("Partition_5_1_3")
Partition_5.result().subResult(3).setName("Partition_5_1_4")
Partition_5.result().subResult(4).setName("Partition_5_1_5")
Partition_5.result().subResult(5).setName("Partition_5_1_6")

### Create Extrusion
Extrusion_4 = model.addExtrusion(Part_1_doc, [model.selection("EDGE", "(Recover_6_1_3/Modified_Edge&plan_partition/plan_partition&Sphere_1_1/Face_1&__tube_sain_2/To_Edge)(Recover_6_1_3/Modified_Edge&plan_partition/plan_partition&Sphere_1_1/Face_1&________tube_sain/To_Edge)(Recover_6_1_3/Modified_Edge&PartSet/XOZ/XOZ&plan_partition/plan_partition&Sphere_1_1/Face_1)"), model.selection("EDGE", "Recover_6_1_3/Modified_Edge&PartSet/XOZ/XOZ&plan_partition/plan_partition&Sphere_1_1/Face_1")], model.selection("EDGE", "PartSet/OX"), 0, "tube_l")

### Create Common
Common_4 = model.addCommon(Part_1_doc, [model.selection("COMPOUND", "all-in-Partition_5"), model.selection("COMPOUND", "all-in-Extrusion_4")], keepSubResults = True)
Common_4.result().subResult(0).setName("Common_4_1_1")
Common_4.result().subResult(1).setName("Common_4_1_2")

### Create Recover
Recover_7 = model.addRecover(Part_1_doc, Common_4, [Partition_5.result()], True)

### Create Partition
Partition_6 = model.addPartition(Part_1_doc, [model.selection("COMPOUND", "all-in-Common_4"), model.selection("COMPOUND", "all-in-Recover_7")], keepSubResults = True)
Partition_6.result().setTransparency(0.72)
Partition_6.result().subResult(0).setName("Partition_6_1_1")
Partition_6.result().subResult(1).setName("Partition_6_1_2")
Partition_6.result().subResult(2).setName("Partition_6_1_3")
Partition_6.result().subResult(3).setName("Partition_6_1_4")
Partition_6.result().subResult(4).setName("Partition_6_1_5")
Partition_6.result().subResult(5).setName("Partition_6_1_6")
Partition_6.result().subResult(6).setName("Partition_6_1_7")

### Create Plane
Plane_5 = model.addPlane(Part_1_doc, model.selection("VERTEX", "Recover_7_1_5/Modified_Vertex&_____________tube_sain/Face_3"), model.selection("VERTEX", "Recover_7_1_3/Modified_Vertex&_____________tube_sain/Face_3"), model.selection("VERTEX", "Recover_7_1_1/Modified_Vertex&_____________tube_sain/Face_2&new_weak_name_1"))

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_2"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(0, -0.08, 1, -0.08)
Sketch_1.setHorizontal(SketchLine_1.result())

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "Recover_7_1_1/Modified_Vertex&_____________tube_sain/Face_2&new_weak_name_2"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())

### Create SketchLine
SketchLine_2 = Sketch_1.addLine(1, -0.08, 1, 0.08)
Sketch_1.setVertical(SketchLine_2.result())

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(1, 0.08, 0, 0.08)
Sketch_1.setHorizontal(SketchLine_3.result())

### Create SketchProjection
SketchProjection_2 = Sketch_1.addProjection(model.selection("VERTEX", "Recover_7_1_5/Modified_Vertex&_____________tube_sain/Face_3"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchPoint_2.result())

### Create SketchLine
SketchLine_4 = Sketch_1.addLine(0, 0.08, 0, -0.08)
Sketch_1.setVertical(SketchLine_4.result())
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
model.do()

### Create Face
Face_1 = model.addFace(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")])

### Create Partition
Partition_7 = model.addPartition(Part_1_doc, [model.selection("COMPOUND", "all-in-Face_1"), model.selection("COMPOUND", "all-in-Partition_6")], keepSubResults = True)
Partition_7.setName("Tube_rugueux")
Partition_7.result().setName("Tube_rugueux")
Partition_7.result().setTransparency(0.7)
Partition_7.result().subResult(0).setName("Partition_7_1_1")
Partition_7.result().subResult(1).setName("Partition_7_1_2")
Partition_7.result().subResult(1).subResult(0).setName("Partition_7_1_2_1")
Partition_7.result().subResult(1).subResult(1).setName("Partition_7_1_2_2")
Partition_7.result().subResult(1).subResult(2).setName("Partition_7_1_2_3")
Partition_7.result().subResult(1).subResult(3).setName("Partition_7_1_2_4")
Partition_7.result().subResult(1).subResult(4).setName("Partition_7_1_2_5")
Partition_7.result().subResult(1).subResult(5).setName("Partition_7_1_2_6")
Partition_7.result().subResult(1).subResult(6).setName("Partition_7_1_2_7")
Partition_7.result().subResult(1).subResult(7).setName("Partition_7_1_2_8")
Partition_7.result().subResult(1).subResult(8).setName("Partition_7_1_2_9")
Partition_7.result().subResult(1).subResult(9).setName("Partition_7_1_2_10")
Partition_7.result().subResult(1).subResult(10).setName("Partition_7_1_2_11")

### Create Extrusion
Extrusion_5 = model.addExtrusion(Part_1_doc, [model.selection("EDGE", "(Partition_6_1_7/Modified_Edge&plan_partition/plan_partition&Sphere_1_1/Face_1&___________tube_sain/To_Edge)(Partition_7_1_2_8/Modified_Edge&Sketch_1/SketchLine_3)"), model.selection("EDGE", "(Partition_6_1_7/Modified_Edge&plan_partition/plan_partition&Sphere_1_1/Face_1&___________tube_sain/To_Edge)(Partition_7_1_2_9/Modified_Edge&Sketch_1/SketchLine_3)")], model.selection("EDGE", "PartSet/OY"), 0, "tube_r")
Extrusion_5.result().setName("Extrusion_5_1")
Extrusion_5.results()[1].setName("Extrusion_5_2")

### Create Recover
Recover_8 = model.addRecover(Part_1_doc, Extrusion_5, [Partition_7.result()], True)
Recover_8.result().subResult(1).setTransparency(0.67)

### Create Common
Common_5 = model.addCommon(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_5"), model.selection("COMPOUND", "all-in-Recover_8")], keepSubResults = True)

### Create Recover
Recover_9 = model.addRecover(Part_1_doc, Common_5, [Recover_8.result()], True)
Recover_9.setName("Recover_10")
Recover_9.result().setName("Recover_10_1")
Recover_9.result().subResult(0).setName("Recover_10_1_1")
Recover_9.result().subResult(0).setTransparency(0.72)
Recover_9.result().subResult(1).setName("Recover_10_1_2")
Recover_9.result().subResult(1).setTransparency(0.64)
Recover_9.result().subResult(1).subResult(0).setName("Recover_10_1_2_1")
Recover_9.result().subResult(1).subResult(0).setTransparency(0.64)
Recover_9.result().subResult(1).subResult(1).setName("Recover_10_1_2_2")
Recover_9.result().subResult(1).subResult(1).setTransparency(0.64)
Recover_9.result().subResult(1).subResult(2).setName("Recover_10_1_2_3")
Recover_9.result().subResult(1).subResult(2).setTransparency(0.64)
Recover_9.result().subResult(1).subResult(3).setName("Recover_10_1_2_4")
Recover_9.result().subResult(1).subResult(3).setTransparency(0.64)
Recover_9.result().subResult(1).subResult(4).setName("Recover_10_1_2_5")
Recover_9.result().subResult(1).subResult(4).setTransparency(0.64)
Recover_9.result().subResult(1).subResult(5).setName("Recover_10_1_2_6")
Recover_9.result().subResult(1).subResult(5).setTransparency(0.64)
Recover_9.result().subResult(1).subResult(6).setName("Recover_10_1_2_7")
Recover_9.result().subResult(1).subResult(6).setTransparency(0.64)
Recover_9.result().subResult(1).subResult(7).setName("Recover_10_1_2_8")
Recover_9.result().subResult(1).subResult(7).setTransparency(0.64)
Recover_9.result().subResult(1).subResult(8).setName("Recover_10_1_2_9")
Recover_9.result().subResult(1).subResult(8).setTransparency(0.64)
Recover_9.result().subResult(1).subResult(9).setName("Recover_10_1_2_10")
Recover_9.result().subResult(1).subResult(9).setTransparency(0.64)
Recover_9.result().subResult(1).subResult(10).setName("Recover_10_1_2_11")
Recover_9.result().subResult(1).subResult(10).setTransparency(0.64)

### Create Extrusion
Extrusion_6 = model.addExtrusion(Part_1_doc, [model.selection("EDGE", "(Recover_10_1_2_6/Modified_Edge&PartSet/YOZ/YOZ&Sphere_1_1/Face_1&_____________tube_sain/Face_1)(Recover_10_1_2_6/Modified_Edge&plan_partition/plan_partition&Sphere_1_1/Face_1&___tube_sain_2/To_Edge)(Recover_10_1_2_6/Modified_Edge&Sketch_1/SketchLine_1)"), model.selection("EDGE", "(Recover_10_1_2_7/Modified_Edge&plan_partition/plan_partition&Sphere_1_1/Face_1&___tube_sain_2/To_Edge)(Recover_10_1_2_7/Modified_Edge&_____________tube_sain/Face_1&PartSet/YOZ/YOZ&plan_partition/plan_partition&Sphere_1_1/Face_1)(Recover_10_1_2_7/Modified_Edge&Sketch_1/SketchLine_1)")], model.selection("EDGE", "PartSet/OY"), "tube_r", 0)

### Create Recover
Recover_10 = model.addRecover(Part_1_doc, Extrusion_6, [Recover_8.result()], True)
Recover_10.setName("Recover_9")
Recover_10.result().setName("Recover_9_1")
Recover_10.result().subResult(0).setName("Recover_9_1_1")
Recover_10.result().subResult(1).setName("Recover_9_1_2")
Recover_10.result().subResult(1).subResult(0).setName("Recover_9_1_2_1")
Recover_10.result().subResult(1).subResult(1).setName("Recover_9_1_2_2")
Recover_10.result().subResult(1).subResult(2).setName("Recover_9_1_2_3")
Recover_10.result().subResult(1).subResult(3).setName("Recover_9_1_2_4")
Recover_10.result().subResult(1).subResult(4).setName("Recover_9_1_2_5")
Recover_10.result().subResult(1).subResult(5).setName("Recover_9_1_2_6")
Recover_10.result().subResult(1).subResult(6).setName("Recover_9_1_2_7")
Recover_10.result().subResult(1).subResult(7).setName("Recover_9_1_2_8")
Recover_10.result().subResult(1).subResult(8).setName("Recover_9_1_2_9")
Recover_10.result().subResult(1).subResult(9).setName("Recover_9_1_2_10")
Recover_10.result().subResult(1).subResult(10).setName("Recover_9_1_2_11")

### Create Common
Common_6 = model.addCommon(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_6"), model.selection("COMPOUND", "all-in-Recover_9")], keepSubResults = True)

### Create Recover
Recover_11 = model.addRecover(Part_1_doc, Common_6, [Recover_10.result()], True)

model.do()

### Create Group
Group_1 = model.addGroup(Part_1_doc, "Edges", [model.selection("EDGE", "Recover_11_1_2_10/Modified_Edge&_____________tube_sain/Face_2&__tube_sain_2/tube_sain_2")])
Group_1.setName("ValidEdge")
Group_1.result().setName("ValidEdge")

model.end()

# check that resulting group selection is valid
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Group_1.feature()))
assert(Group_1.groupList().size() == 1)
for a in range(1):
  assert(Group_1.groupList().value(a).value().shapeTypeStr() == "EDGE")
  assert(len(Group_1.groupList().value(a).namingName()) > 0)
assert(model.checkPythonDump())
