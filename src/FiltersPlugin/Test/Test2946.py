# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(0, 0, 10)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 10)
SketchLine_1 = Sketch_1.addLine(19.31851652578136, 5.176380902050418, 38.63703305156273, 10.35276180410083)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_1.result())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_2.result(), SketchLine_1.result(), 15)
SketchConstraintMirror_1 = Sketch_1.addMirror(SketchLine_2.result(), [SketchLine_1.result()])
[SketchLine_3] = SketchConstraintMirror_1.mirrored()
SketchMultiRotation_1 = Sketch_1.addRotation([SketchLine_1.result(), SketchLine_3.result()], SketchAPI_Line(SketchLine_2).startPoint(), 360, 4, True)
[SketchLine_4, SketchLine_5, SketchLine_6, SketchLine_3, SketchLine_7, SketchLine_8, SketchLine_9] = SketchMultiRotation_1.rotated()
SketchArc_1 = Sketch_1.addArc(0, 0, 10.35276180410083, 38.63703305156273, -10.35276180410083, 38.63703305156273, False)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchArc_1.center())
SketchConstraintRadius_3 = Sketch_1.setRadius(SketchArc_1.results()[1], 40)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_7.result())
SketchArc_2 = Sketch_1.addArc(0, 0, -38.63703305156272, 10.35276180410084, -38.63703305156273, -10.35276180410081, False)
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchArc_1.results()[1], SketchArc_2.results()[1])
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchArc_1.center(), SketchArc_2.center())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_4.result())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_8.result())
SketchArc_3 = Sketch_1.addArc(0, 0, -10.35276180410082, -38.63703305156272, 10.35276180410081, -38.63703305156273, False)
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchArc_2.results()[1], SketchArc_3.results()[1])
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchArc_2.center(), SketchArc_3.center())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_5.result())
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchArc_3.startPoint(), SketchLine_9.result())
SketchArc_4 = Sketch_1.addArc(0, 0, 38.63703305156272, -10.35276180410083, 38.63703305156273, 10.35276180410083, False)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchArc_4.endPoint())
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchArc_3.results()[1], SketchArc_4.results()[1])
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchArc_3.center(), SketchArc_4.center())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchArc_4.startPoint(), SketchLine_3.result())
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_6.result())
SketchArc_5 = Sketch_1.addArc(0, 0, -5.176380902050417, 19.31851652578137, -19.31851652578136, 5.17638090205042, False)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchArc_5.center())
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_5.results()[1], 20)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_5.results()[1])
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchArc_5.startPoint(), SketchLine_4.result())
SketchArc_6 = Sketch_1.addArc(0, 0, -19.31851652578137, -5.176380902050416, -5.176380902050431, -19.31851652578136, False)
SketchConstraintEqual_4 = Sketch_1.setEqual(SketchArc_5.results()[1], SketchArc_6.results()[1])
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchArc_5.center(), SketchArc_6.center())
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchArc_5.endPoint(), SketchLine_8.result())
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchArc_6.startPoint(), SketchLine_5.result())
SketchArc_7 = Sketch_1.addArc(0, 0, 5.176380902050406, -19.31851652578137, 19.31851652578136, -5.176380902050432, False)
SketchConstraintEqual_5 = Sketch_1.setEqual(SketchArc_6.results()[1], SketchArc_7.results()[1])
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchArc_6.center(), SketchArc_7.center())
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchArc_7.startPoint(), SketchLine_6.result())
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchArc_6.endPoint(), SketchLine_9.result())
SketchArc_8 = Sketch_1.addArc(0, 0, 19.31851652578136, 5.176380902050419, 5.176380902050419, 19.31851652578136, False)
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchArc_8.endPoint(), SketchLine_7.result())
SketchConstraintEqual_6 = Sketch_1.setEqual(SketchArc_7.results()[1], SketchArc_8.results()[1])
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchArc_7.center(), SketchArc_8.center())
SketchConstraintCoincidence_26 = Sketch_1.setCoincident(SketchArc_7.endPoint(), SketchLine_3.result())
SketchConstraintCoincidence_27 = Sketch_1.setCoincident(SketchArc_8.startPoint(), SketchLine_1.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchArc_8_2f-SketchLine_7f-SketchArc_1_2f-SketchLine_4r-SketchArc_5_2f-SketchLine_8f-SketchArc_2_2f-SketchLine_5r-SketchArc_6_2f-SketchLine_9f-SketchArc_3_2f-SketchLine_6r-SketchArc_7_2f-SketchLine_3f-SketchArc_4_2f-SketchLine_1r-SketchCircle_1_2r")], model.selection(), 10, 0)
Filters = model.filters(Part_1_doc, [model.addFilter(name = "OnPlane", args = [model.selection("FACE", "Extrusion_1_1/To_Face")])])
model.end()

Reference = {
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_9][Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_3_2]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_9][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_6_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_9]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_2_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_5]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_5][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_6_2][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_5][Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_6_2]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_3_2][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_3_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_7_2]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "([Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1/From_Face])([Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1/To_Face])"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_7_2][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_7_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_4_2]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_5_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_8]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_5_2][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4][Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_5_2]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_8_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_7]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_2_2][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_8][Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_2_2]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_8_2][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_8_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_8][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_4_2][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_4_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_7][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_7][Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_1_2]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_1_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_1_2][Extrusion_1_1/From_Face]"): False,

    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_9][Extrusion_1_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_5][Extrusion_1_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_6_2][Extrusion_1_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_3_2][Extrusion_1_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_7_2][Extrusion_1_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_5_2][Extrusion_1_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_8_2][Extrusion_1_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_2_2][Extrusion_1_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_4_2][Extrusion_1_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_8][Extrusion_1_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_1_2][Extrusion_1_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_7][Extrusion_1_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4][Extrusion_1_1/To_Face]"): True,
}
model.checkFilter(Part_1_doc, model, Filters, Reference)
