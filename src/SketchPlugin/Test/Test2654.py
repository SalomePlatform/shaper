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

"""
    Test2654.py
    Test case for issue #2654 "crash with a split"
"""

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "h", "3.5")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchLine_1 = Sketch_1.addLine(-5.833254622789047, -7.49820915323854, -11, -7.49820915323854)
SketchLine_2 = Sketch_1.addLine(-11, -7.49820915323854, -11, -9.998209153238539)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_1.setName("SketchConstraintCoincidence_4")
SketchLine_3 = Sketch_1.addLine(-11, -9.998209153238539, 11, -9.998209153238539)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_2.setName("SketchConstraintCoincidence_5")
SketchLine_4 = Sketch_1.addLine(11, -9.998209153238539, 11, -7.49820915323854)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_3.setName("SketchConstraintCoincidence_6")
SketchLine_5 = Sketch_1.addLine(11, -7.49820915323854, 5.833254622789045, -7.49820915323854)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_4.setName("SketchConstraintCoincidence_7")
SketchLine_6 = Sketch_1.addLine(-5.994422407538528, 7.37, -5.994422407538528, 12.00179084676146)
SketchLine_7 = Sketch_1.addLine(-5.994422407538528, 12.00179084676146, 6.005577592461473, 12.00179084676146)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_5.setName("SketchConstraintCoincidence_10")
SketchLine_8 = Sketch_1.addLine(6.005577592461473, 12.00179084676146, 6.005577592461473, 8.12179084676146)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_6.setName("SketchConstraintCoincidence_11")
SketchLine_9 = Sketch_1.addLine(6.005577592461473, 8.12179084676146, 1.564947006609954, 3.681160260909941)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_7.setName("SketchConstraintCoincidence_12")
SketchLine_10 = Sketch_1.addLine(3.684201429703701, 1.557773996884403, 7.696054479856212, 5.569627047036913)
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_8.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintHorizontal_3.setName("SketchConstraintHorizontal_4")
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_9.result(), SketchLine_10.result())
SketchArc_1 = Sketch_1.addArc(0, 0, 1.564947006609954, 3.681160260909941, 3.684201429703701, 1.557773996884403, False)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchArc_1.center())
SketchConstraintCoincidence_8.setName("SketchConstraintCoincidence_8")
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_9.setName("SketchConstraintCoincidence_16")
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_10.startPoint(), SketchArc_1.endPoint())
SketchConstraintCoincidence_10.setName("SketchConstraintCoincidence_17")
SketchArc_2 = Sketch_1.addArc(0, 0, -5.994422407538528, 7.37, -5.833254622789047, -7.49820915323854, False)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchArc_2.center())
SketchConstraintCoincidence_11.setName("SketchConstraintCoincidence_13")
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_12.setName("SketchConstraintCoincidence_18")
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_8.result(), 3.88)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_7.result(), 12)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_9.result(), 6.28)
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_11 = SketchProjection_2.createdFeature()
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 4)
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_2.results()[1], 9.5)
SketchConstraintCollinear_1 = Sketch_1.setCollinear(SketchLine_5.result(), SketchLine_1.result())
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_3.result(), 22)
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_4.result(), 2.5)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchLine_6.endPoint(), SketchLine_2.endPoint(), 22)
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_11.result(), SketchLine_10.result(), 45)
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchArc_1.startPoint(), SketchLine_10.result(), 3, True)
SketchLine_12 = Sketch_1.addLine(0, -9.998209153238539, 0, 0)
SketchLine_12.setAuxiliary(True)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_12.startPoint(), SketchLine_3.result())
SketchConstraintCoincidence_13.setName("SketchConstraintCoincidence_20")
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_12.endPoint())
SketchConstraintCoincidence_14.setName("SketchConstraintCoincidence_21")
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_12.result())
SketchConstraintMiddle_1 = Sketch_1.setMiddlePoint(SketchLine_3.result(), SketchLine_12.startPoint())
SketchArc_3 = Sketch_1.addArc(0, 0, 5.833254622789045, -7.49820915323854, 7.696054479856212, 5.569627047036913, False)
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchArc_3.endPoint())
SketchConstraintCoincidence_15.setName("SketchConstraintCoincidence_19")
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchArc_2.results()[1], SketchArc_3.results()[1])
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchArc_2.center(), SketchArc_3.center())
SketchConstraintCoincidence_16.setName("SketchConstraintCoincidence_22")
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchArc_3.startPoint())
SketchConstraintCoincidence_17.setName("SketchConstraintCoincidence_24")
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_2.endPoint())
SketchConstraintCoincidence_18.setName("SketchConstraintCoincidence_23")
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_11.result(), SketchArc_2.startPoint(), 7.37, True)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1f-SketchLine_2f-SketchLine_3f-SketchLine_4f-SketchLine_5f-SketchArc_3_2f-SketchLine_10r-SketchArc_1_2r-SketchLine_9r-SketchLine_8r-SketchLine_7r-SketchLine_6r-SketchArc_2_2f")], model.selection(), "h", 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_7"))
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_8][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_7][Extrusion_1_1/From_Face]"), False)
SketchPoint_2 = SketchProjection_3.createdFeature()
SketchProjection_4 = Sketch_2.addProjection(model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_7][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/From_Face]"), False)
SketchPoint_3 = SketchProjection_4.createdFeature()
SketchProjection_5 = Sketch_2.addProjection(model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_8][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_7][Extrusion_1_1/To_Face]"), False)
SketchPoint_4 = SketchProjection_5.createdFeature()
SketchLine_13 = Sketch_2.addLine(6.005577592461473, 0, -5.994422407538528, 0)
SketchConstraintCoincidence_19 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_13.startPoint())
SketchConstraintCoincidence_19.setName("SketchConstraintCoincidence_25")
SketchConstraintCoincidence_20 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_3).coordinates(), SketchLine_13.endPoint())
SketchConstraintCoincidence_20.setName("SketchConstraintCoincidence_26")
SketchLine_14 = Sketch_2.addLine(-5.994422407538528, 0, -5.994422407538528, -3.5)
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_21.setName("SketchConstraintCoincidence_27")
SketchProjection_6 = Sketch_2.addProjection(model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_7][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/To_Face]"), False)
SketchPoint_5 = SketchProjection_6.createdFeature()
SketchConstraintCoincidence_22 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchPoint_5.result())
SketchConstraintCoincidence_22.setName("SketchConstraintCoincidence_28")
SketchLine_15 = Sketch_2.addLine(-5.994422407538528, -3.5, -5.994422407538528, -10.5)
SketchConstraintCoincidence_23 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_23.setName("SketchConstraintCoincidence_29")
SketchLine_16 = Sketch_2.addLine(6.005577592461473, 0, 6.005577592461473, -3.5)
SketchConstraintCoincidence_24 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_16.startPoint())
SketchConstraintCoincidence_24.setName("SketchConstraintCoincidence_30")
SketchConstraintCoincidence_25 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_4).coordinates(), SketchLine_16.endPoint())
SketchConstraintCoincidence_25.setName("SketchConstraintCoincidence_31")
SketchLine_17 = Sketch_2.addLine(6.005577592461473, -3.5, 6.005577592461473, -10.5)
SketchConstraintCoincidence_26 = Sketch_2.setCoincident(SketchLine_16.endPoint(), SketchLine_17.startPoint())
SketchConstraintCoincidence_26.setName("SketchConstraintCoincidence_32")
SketchArc_4 = Sketch_2.addArc(0.005577592461472826, -10.5, -5.994422407538528, -10.5, 6.005577592461473, -10.5, False)
SketchArc_4.setName("SketchArc_5")
SketchArc_4.result().setName("SketchArc_5")
SketchArc_4.results()[1].setName("SketchArc_5_2")
SketchConstraintCoincidence_27 = Sketch_2.setCoincident(SketchLine_15.endPoint(), SketchArc_4.startPoint())
SketchConstraintCoincidence_27.setName("SketchConstraintCoincidence_35")
SketchConstraintTangent_1 = Sketch_2.setTangent(SketchLine_15.result(), SketchArc_4.results()[1])
SketchConstraintTangent_1.setName("SketchConstraintTangent_2")
SketchConstraintCoincidence_28 = Sketch_2.setCoincident(SketchLine_17.result(), SketchArc_4.endPoint())
SketchConstraintCoincidence_28.setName("SketchConstraintCoincidence_36")
SketchConstraintCoincidence_29 = Sketch_2.setCoincident(SketchArc_4.endPoint(), SketchLine_17.endPoint())
SketchConstraintCoincidence_29.setName("SketchConstraintCoincidence_33")
SketchConstraintParallel_2 = Sketch_2.setParallel(SketchLine_17.result(), SketchLine_15.result())
SketchConstraintVertical_6 = Sketch_2.setVertical(SketchLine_17.result())
SketchConstraintLength_6 = Sketch_2.setLength(SketchLine_17.result(), 7)
SketchConstraintEqual_2 = Sketch_2.setEqual(SketchLine_17.result(), SketchLine_15.result())
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_13f-SketchLine_14f-SketchLine_15f-SketchArc_5_2f-SketchLine_17r-SketchLine_16r")], model.selection(), "h", 0)
model.do()


from salome.shaper import geom

# Try to splie the line with point-line coincidence on the line boundary
Split_1 = Sketch_2.addSplit(SketchLine_17, geom.Pnt2d(6.005577592461473, -7))
model.do()

# Check the split leads to sketch failure
assert(Split_1.feature().error() != "")

# Remove the split feature
Part_1_doc.removeFeature(Split_1.feature())
model.do()
model.checkSketch(Sketch_2, 0)
model.end()

assert(model.checkPythonDump())
