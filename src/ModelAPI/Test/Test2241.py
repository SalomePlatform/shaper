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

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "l", "10")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 60, 10, 60)
SketchLine_2 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OY"))
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_2.result())
SketchLine_3 = Sketch_1.addLine(10, 60, 10, 40)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(10, 40, 20, 40)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(20, 40, 20, 20)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(20, 20, 30, 20)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(30, 20, 30, 0)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OX"))
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.result())
SketchLine_9 = Sketch_1.addLine(30, 0, 81.26217508737108, 0)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_8.result())
SketchLine_10 = Sketch_1.addLine(0, 60, 0, 81.26217508737108)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_2.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_5.result())
SketchArc_1 = Sketch_1.addArc(4.151438510550382e-34, -3.089278765476956e-34, 81.26217508737108, 0, 0, 81.26217508737108, False)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchArc_1.center())
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_2.result(), SketchArc_1.endPoint())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchArc_1.endPoint())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_7.result())
SketchArc_2 = Sketch_1.addArc(4.151438510550382e-34, -3.089278765476956e-34, 76.26217508737108, 0, 0, 76.26217508737108, False)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchArc_2.center())
SketchConstraintCoincidence_16.setName("SketchConstraintCoincidence_23")
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_9.result(), SketchArc_2.startPoint())
SketchConstraintCoincidence_17.setName("SketchConstraintCoincidence_24")
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_10.result(), SketchArc_2.endPoint())
SketchConstraintCoincidence_18.setName("SketchConstraintCoincidence_25")
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchArc_1.endPoint(), SketchArc_2.endPoint(), 5, False)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_7.result(), "2*l")
SketchConstraintLength_1.setName("SketchConstraintLength_2")
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_6.result(), "l")
SketchConstraintLength_2.setName("SketchConstraintLength_3")
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_5.result(), SketchLine_7.result())
SketchConstraintEqual_1.setName("SketchConstraintEqual_2")
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_6.result(), SketchLine_4.result())
SketchConstraintEqual_2.setName("SketchConstraintEqual_3")
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchLine_5.result(), SketchLine_3.result())
SketchConstraintEqual_3.setName("SketchConstraintEqual_4")
SketchConstraintEqual_4 = Sketch_1.setEqual(SketchLine_4.result(), SketchLine_1.result())
SketchConstraintEqual_4.setName("SketchConstraintEqual_5")
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_2.addCircle(9.082839147404137, 71.32948234489588, 2.005056553640547)
SketchCircle_2 = Sketch_2.addCircle(20.11065019242717, 63.7102674410618, 2.268462537186828)
SketchCircle_3 = Sketch_2.addCircle(26.52683116407693, 51.67992811921848, 2.727171758268866)
SketchCircle_4 = Sketch_2.addCircle(38.35666483055617, 58.29661474623231, 3.007584830460834)
SketchCircle_5 = Sketch_2.addCircle(33.94554041254697, 44.0607132153844, 3.054011155390377)
SketchCircle_6 = Sketch_2.addCircle(48.58245325412298, 42.45666797247195, 2.346858526438435)
SketchCircle_7 = Sketch_2.addCircle(38.95818179664835, 31.42885692744893, 3.547307159201095)
SketchCircle_8 = Sketch_2.addCircle(63.82088306179116, 27.41874382016783, 4.536925074373651)
SketchCircle_9 = Sketch_2.addCircle(50.18649849703544, 28.4212720969881, 4.820482891521984)
SketchCircle_10 = Sketch_2.addCircle(49.58498153094326, 15.18789884296047, 2.552020354335177)
SketchCircle_11 = Sketch_2.addCircle(66.22695092615982, 10.77677442495125, 4.536925074373645)
SketchCircle_12 = Sketch_2.addCircle(50.98852111849166, 6.165144351577979, 3.547307159201085)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchLine_1r-SketchLine_3f-SketchLine_4f-SketchLine_5f-SketchLine_6f-SketchLine_7f-SketchLine_9f-SketchArc_2_2f-SketchLine_10r_wire")])
Face_2 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchLine_9f-SketchArc_1_2f-SketchLine_10r-SketchArc_2_2r_wire")])
Face_3_objects = [model.selection("EDGE", "Sketch_2/SketchCircle_1_2"), model.selection("EDGE", "Sketch_2/SketchCircle_2_2"), model.selection("EDGE", "Sketch_2/SketchCircle_3_2"), model.selection("EDGE", "Sketch_2/SketchCircle_4_2"), model.selection("EDGE", "Sketch_2/SketchCircle_5_2"), model.selection("EDGE", "Sketch_2/SketchCircle_6_2"), model.selection("EDGE", "Sketch_2/SketchCircle_7_2"), model.selection("EDGE", "Sketch_2/SketchCircle_9_2"), model.selection("EDGE", "Sketch_2/SketchCircle_8_2"), model.selection("EDGE", "Sketch_2/SketchCircle_10_2"), model.selection("EDGE", "Sketch_2/SketchCircle_12_2"), model.selection("EDGE", "Sketch_2/SketchCircle_11_2")]
Face_3 = model.addFace(Part_1_doc, Face_3_objects)
Face_4_objects = [model.selection("EDGE", "Sketch_2/SketchCircle_1_2"), model.selection("EDGE", "Sketch_2/SketchCircle_2_2"), model.selection("EDGE", "Sketch_2/SketchCircle_3_2"), model.selection("EDGE", "Sketch_2/SketchCircle_4_2"), model.selection("EDGE", "Sketch_2/SketchCircle_5_2"), model.selection("EDGE", "Sketch_2/SketchCircle_7_2"), model.selection("EDGE", "Sketch_2/SketchCircle_6_2"), model.selection("EDGE", "Sketch_2/SketchCircle_9_2"), model.selection("EDGE", "Sketch_2/SketchCircle_8_2"), model.selection("EDGE", "Sketch_2/SketchCircle_10_2"), model.selection("EDGE", "Sketch_2/SketchCircle_12_2"), model.selection("EDGE", "Sketch_2/SketchCircle_11_2"), model.selection("WIRE", "Sketch_1/Face-SketchLine_1r-SketchLine_3f-SketchLine_4f-SketchLine_5f-SketchLine_6f-SketchLine_7f-SketchLine_9f-SketchArc_2_2f-SketchLine_10r_wire")]
Face_4 = model.addFace(Part_1_doc, Face_4_objects)
Sketch_3 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchLine_11 = Sketch_3.addLine(40, 0, 30, 0)
SketchPoint_1 = Sketch_3.addPoint(model.selection("VERTEX", "Face_1_1/Modified_Vertex&Sketch_1/SketchLine_9_StartVertex&Sketch_1/SketchLine_7_EndVertex"))
SketchConstraintCoincidence_19 = Sketch_3.setCoincident(SketchLine_11.endPoint(), SketchPoint_1.result())
SketchConstraintCoincidence_19.setName("SketchConstraintCoincidence_19")
SketchLine_12 = Sketch_3.addLine(30, 0, 30, 10)
SketchLine_13 = Sketch_3.addLine(30, 10, 40, 10)
SketchLine_14 = Sketch_3.addLine(40, 10, 40, 0)
SketchConstraintCoincidence_20 = Sketch_3.setCoincident(SketchLine_14.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_20.setName("SketchConstraintCoincidence_20")
SketchConstraintCoincidence_21 = Sketch_3.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_21.setName("SketchConstraintCoincidence_21")
SketchConstraintCoincidence_22 = Sketch_3.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_22.setName("SketchConstraintCoincidence_22")
SketchConstraintCoincidence_23 = Sketch_3.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_23.setName("SketchConstraintCoincidence_26")
SketchConstraintHorizontal_4 = Sketch_3.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_4 = Sketch_3.setVertical(SketchLine_12.result())
SketchConstraintHorizontal_5 = Sketch_3.setHorizontal(SketchLine_13.result())
SketchConstraintVertical_5 = Sketch_3.setVertical(SketchLine_14.result())
SketchConstraintEqual_5 = Sketch_3.setEqual(SketchLine_14.result(), SketchLine_13.result())
SketchConstraintEqual_5.setName("SketchConstraintEqual_6")
SketchConstraintLength_3 = Sketch_3.setLength(SketchLine_13.result(), "l")
SketchConstraintLength_3.setName("SketchConstraintLength_4")
model.do()
Face_5 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_3/Face-SketchLine_14r-SketchLine_13r-SketchLine_12r-SketchLine_11r_wire")])
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("FACE", "Face_5_1")], model.selection("EDGE", "PartSet/OX"), "l", 4, model.selection("EDGE", "PartSet/OY"), "l", 2)
Recover_1 = model.addRecover(Part_1_doc, LinearCopy_1, [Face_5.result()])
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("FACE", "Recover_1_1")], model.selection("VERTEX", "Face_4_1/Modified_Vertex&Sketch_1/SketchLine_9_StartVertex&Sketch_1/SketchLine_7_EndVertex"), model.selection("VERTEX", "Face_4_1/Modified_Vertex&Sketch_1/SketchLine_6_StartVertex&Sketch_1/SketchLine_5_EndVertex"))
LinearCopy_2 = model.addMultiTranslation(Part_1_doc, [model.selection("FACE", "Translation_1_1")], model.selection("EDGE", "PartSet/OX"), "l", 4, model.selection("EDGE", "PartSet/OY"), "l", 2)
Recover_2 = model.addRecover(Part_1_doc, LinearCopy_1, [Face_5.result()])
Translation_2 = model.addTranslation(Part_1_doc, [model.selection("FACE", "Recover_2_1")], model.selection("VERTEX", "Face_4_1/Modified_Vertex&Sketch_1/SketchLine_9_StartVertex&Sketch_1/SketchLine_7_EndVertex"), model.selection("VERTEX", "Face_4_1/Modified_Vertex&Sketch_1/SketchLine_4_StartVertex&Sketch_1/SketchLine_3_EndVertex"))
LinearCopy_3 = model.addMultiTranslation(Part_1_doc, [model.selection("FACE", "Translation_2_1")], model.selection("EDGE", "PartSet/OX"), "l", 4)
Recover_3 = model.addRecover(Part_1_doc, LinearCopy_1, [Face_5.result()])
Translation_3 = model.addTranslation(Part_1_doc, [model.selection("FACE", "Recover_3_1")], model.selection("VERTEX", "Face_4_1/Modified_Vertex&Sketch_1/SketchLine_9_StartVertex&Sketch_1/SketchLine_7_EndVertex"), model.selection("VERTEX", "LinearCopy_3_1_1/MV:Translated&Sketch_3/SketchLine_12_EndVertex&Sketch_3/SketchLine_13_StartVertex"))
LinearCopy_4 = model.addMultiTranslation(Part_1_doc, [model.selection("FACE", "Translation_3_1")], model.selection("EDGE", "PartSet/OX"), "l", 3)
Recover_4 = model.addRecover(Part_1_doc, LinearCopy_1, [Face_5.result()])
Translation_4 = model.addTranslation(Part_1_doc, [model.selection("FACE", "Recover_4_1")], model.selection("VERTEX", "Face_4_1/Modified_Vertex&Sketch_1/SketchLine_9_StartVertex&Sketch_1/SketchLine_7_EndVertex"), model.selection("VERTEX", "Face_4_1/Modified_Vertex&Sketch_1/SketchLine_10_StartVertex&Sketch_1/SketchLine_1_StartVertex"))
LinearCopy_5 = model.addMultiTranslation(Part_1_doc, [model.selection("FACE", "Translation_4_1")], model.selection("EDGE", "PartSet/OX"), "l", 2)
Group_1_objects = [model.selection("FACE", "LinearCopy_4_1_2"), model.selection("FACE", "LinearCopy_1_1_8"), model.selection("FACE", "LinearCopy_1_1_4"), model.selection("FACE", "LinearCopy_1_1_3"), model.selection("FACE", "LinearCopy_1_1_6"), model.selection("FACE", "LinearCopy_1_1_2"), model.selection("FACE", "LinearCopy_1_1_7"), model.selection("FACE", "LinearCopy_2_1_3"), model.selection("FACE", "LinearCopy_3_1_4"), model.selection("FACE", "LinearCopy_2_1_8"), model.selection("FACE", "LinearCopy_2_1_4"), model.selection("FACE", "LinearCopy_2_1_7"), model.selection("FACE", "LinearCopy_4_1_3"), model.selection("FACE", "LinearCopy_1_1_1"), model.selection("FACE", "LinearCopy_5_1_2"), model.selection("FACE", "LinearCopy_4_1_1"), model.selection("FACE", "LinearCopy_5_1_1"), model.selection("FACE", "LinearCopy_3_1_3"), model.selection("FACE", "LinearCopy_3_1_2"), model.selection("FACE", "LinearCopy_3_1_1"), model.selection("FACE", "LinearCopy_2_1_6"), model.selection("FACE", "LinearCopy_2_1_2"), model.selection("FACE", "LinearCopy_2_1_5"), model.selection("FACE", "LinearCopy_2_1_1"), model.selection("FACE", "LinearCopy_1_1_5")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
Group_1.setName("Group_3")
Group_1.result().setName("assemblages")
Group_2_objects = [model.selection("FACE", "Face_2_1"), model.selection("FACE", "Face_3_7"), model.selection("FACE", "Face_3_5"), model.selection("FACE", "Face_3_3"), model.selection("FACE", "Face_3_4"), model.selection("FACE", "Face_3_1"), model.selection("FACE", "Face_3_2"), model.selection("FACE", "Face_3_6"), model.selection("FACE", "Face_3_11"), model.selection("FACE", "Face_3_10"), model.selection("FACE", "Face_3_8"), model.selection("FACE", "Face_3_9"), model.selection("FACE", "Face_3_12")]
Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
Group_2.setName("Group_1")
Group_2.result().setName("eau")
Group_2.result().setColor(0, 170, 255)
Group_3 = model.addGroup(Part_1_doc, [model.selection("FACE", "Face_4_1")])
Group_3.setName("Group_2")
Group_3.result().setName("acier")
Group_3.result().setColor(170, 85, 0)
Shell_1_objects = [model.selection("FACE", "LinearCopy_1_1_1"), model.selection("FACE", "LinearCopy_1_1_2"), model.selection("FACE", "LinearCopy_1_1_3"), model.selection("FACE", "LinearCopy_1_1_4"), model.selection("FACE", "LinearCopy_1_1_5"), model.selection("FACE", "LinearCopy_1_1_6"), model.selection("FACE", "LinearCopy_1_1_7"), model.selection("FACE", "LinearCopy_2_1_4"), model.selection("FACE", "LinearCopy_2_1_5"), model.selection("FACE", "LinearCopy_2_1_6"), model.selection("FACE", "LinearCopy_2_1_7"), model.selection("FACE", "LinearCopy_2_1_8"), model.selection("FACE", "LinearCopy_2_1_1"), model.selection("FACE", "LinearCopy_2_1_2"), model.selection("FACE", "LinearCopy_2_1_3"), model.selection("FACE", "LinearCopy_3_1_4"), model.selection("FACE", "LinearCopy_3_1_3"), model.selection("FACE", "LinearCopy_3_1_2"), model.selection("FACE", "LinearCopy_3_1_1"), model.selection("FACE", "LinearCopy_4_1_1"), model.selection("FACE", "LinearCopy_4_1_2"), model.selection("FACE", "LinearCopy_4_1_3"), model.selection("FACE", "LinearCopy_5_1_1"), model.selection("FACE", "LinearCopy_5_1_2"), model.selection("FACE", "LinearCopy_1_1_8")]
Shell_1 = model.addShell(Part_1_doc, Shell_1_objects)
Partition_1_objects = [model.selection("FACE", "Face_3_1"), model.selection("FACE", "Face_3_2"), model.selection("FACE", "Face_4_1"), model.selection("FACE", "Face_2_1"), model.selection("FACE", "Face_1_1"), model.selection("FACE", "Face_3_5"), model.selection("FACE", "Face_3_3"), model.selection("FACE", "Face_3_4"), model.selection("FACE", "Face_3_7"), model.selection("FACE", "Face_3_6"), model.selection("FACE", "Face_3_8"), model.selection("FACE", "Face_3_9"), model.selection("FACE", "Face_3_10"), model.selection("FACE", "Face_3_11"), model.selection("FACE", "Face_3_12"), model.selection("SHELL", "Shell_1_1")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
model.do()
model.end()

# move groups
model.begin()
Part_1_doc.moveFeature(Group_1.feature(), Partition_1.feature())
Part_1_doc.moveFeature(Group_2.feature(), Group_1.feature())
Part_1_doc.moveFeature(Group_3.feature(), Group_2.feature())
model.end()

# check that all groups are correct
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()

for Group in [Group_1, Group_2, Group_3]:
  assert(aFactory.validate(Group.feature()))
  assert(Group.groupList().size() != 0)
  for a in range(Group.groupList().size()):
    assert(Group.groupList().value(a).value().shapeTypeStr() == "FACE")
    assert(len(Group.groupList().value(a).namingName()) > 0)

# for the issue #2892 check fix
assert(model.checkPythonDump())
