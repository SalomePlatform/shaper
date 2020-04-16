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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Extrusion_1 = model.addExtrusion(Part_1_doc, [], model.selection(), 20, 0)
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(40, 30, 5, 30)
SketchLine_2 = Sketch_1.addLine(5, 30, 5, 3)
SketchLine_3 = Sketch_1.addLine(5, 3, 40, 3)
SketchLine_4 = Sketch_1.addLine(40, 3, 40, 30)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
Extrusion_1.setNestedSketch(Sketch_1)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/To_Face"))
SketchLine_5 = Sketch_2.addLine(18, 39, 29, 39)
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_5.result())
SketchLine_6 = Sketch_2.addLine(29, 39, 29, 21)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_6.result())
SketchLine_7 = Sketch_2.addLine(29, 21, 46, 21)
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_7.result())
SketchLine_8 = Sketch_2.addLine(46, 21, 46, 11)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_8.result())
SketchLine_9 = Sketch_2.addLine(46, 11, 29, 11)
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_9.result())
SketchLine_10 = Sketch_2.addLine(29, 11, 29, -3)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintVertical_5 = Sketch_2.setVertical(SketchLine_10.result())
SketchLine_11 = Sketch_2.addLine(29, -3, 18, -3)
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchLine_12 = Sketch_2.addLine(18, -3, 18, 11)
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintVertical_6 = Sketch_2.setVertical(SketchLine_12.result())
SketchLine_13 = Sketch_2.addLine(18, 11, 3, 11)
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchLine_14 = Sketch_2.addLine(3, 11, 3, 21)
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintVertical_7 = Sketch_2.setVertical(SketchLine_14.result())
SketchLine_15 = Sketch_2.addLine(3, 21, 18, 21)
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchConstraintHorizontal_6 = Sketch_2.setHorizontal(SketchLine_15.result())
SketchLine_16 = Sketch_2.addLine(18, 21, 18, 39)
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchLine_5.startPoint(), SketchLine_16.endPoint())
SketchConstraintVertical_8 = Sketch_2.setVertical(SketchLine_16.result())
SketchConstraintHorizontal_7 = Sketch_2.setHorizontal(SketchLine_13.result())
SketchConstraintHorizontal_8 = Sketch_2.setHorizontal(SketchLine_11.result())
SketchConstraintCoincidence_17 = Sketch_2.setCoincident(SketchLine_16.result(), SketchLine_12.endPoint())
SketchConstraintCoincidence_18 = Sketch_2.setCoincident(SketchLine_6.result(), SketchLine_10.startPoint())
SketchConstraintCoincidence_19 = Sketch_2.setCoincident(SketchLine_15.result(), SketchLine_7.startPoint())
SketchConstraintCoincidence_20 = Sketch_2.setCoincident(SketchLine_13.result(), SketchLine_10.startPoint())
model.do()
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_16r-SketchLine_15r-SketchLine_14r-SketchLine_13r-SketchLine_12r-SketchLine_11r-SketchLine_10r-SketchLine_9r-SketchLine_8r-SketchLine_7r-SketchLine_6r-SketchLine_5r")], model.selection(), 0, 10, [model.selection("SOLID", "Extrusion_1_1")])
# two smae filters with two planes each
Filters = model.filters(Part_1_doc, [
  model.addFilter(name = "OnPlane", args = [model.selection("FACE", "ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_16"), model.selection("FACE", "ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6")]),
  model.addFilter(name = "OnPlane", args = [model.selection("FACE", "ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_15"), model.selection("FACE", "ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_13")])])

model.end()

# 4 edges are in result: intersection of pairs of planes
Reference = {
    model.selection("EDGE", "[ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_16][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_15]"): True,
    model.selection("EDGE", "[ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7]"): True,
    model.selection("EDGE", "[ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_13][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_12]"): True,
    model.selection("EDGE", "[ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_9][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_10]"): True,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_12]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2][(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_13)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_12)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)2(Extrusion_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_15)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_10)2]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3][(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_13)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_12)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)2(Extrusion_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_15)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_10)2]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_12]"): False,
    model.selection("EDGE", "[(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_13)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_12)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)2(Extrusion_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_15)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_10)2][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_12]"): False,
    model.selection("EDGE", "([ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_13])([ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_15])([ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_13])([ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_15])"): False,
    model.selection("EDGE", "[(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_13)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_12)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)2(Extrusion_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_15)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_10)2][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_13]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_13]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_13]"): False,
    model.selection("EDGE", "[(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_16)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_15)(Extrusion_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_13)2][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_15]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_15]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_15]"): False,
    model.selection("EDGE", "[Extrusion_1_1/From_Face][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_16][ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2][(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_16)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_15)(Extrusion_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_13)2]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_16)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_15)(Extrusion_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_13)2]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_16]"): False,
    model.selection("EDGE", "[(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_16)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_15)(Extrusion_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_13)2][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_16]"): False,
    model.selection("EDGE", "[Extrusion_1_1/From_Face][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6][(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7)(Extrusion_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_16)2(ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_9)2]"): False,
    model.selection("EDGE", "([ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_10])([ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_12])([ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_10])([ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_12])"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_10]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_10][(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_9)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_10)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)2(Extrusion_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2)2(ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_12)2]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_10]"): False,
    model.selection("EDGE", "([ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_16])([ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6])([ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_16][ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face])([ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6])"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3][(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_9)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_10)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)2(Extrusion_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2)2(ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_12)2]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4][(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_9)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_10)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)2(Extrusion_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2)2(ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_12)2]"): False,
    model.selection("EDGE", "([ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7])([ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_9])([ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7])([ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_9])"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_9]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_9][(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_9)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_10)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)2(Extrusion_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2)2(ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_12)2]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_9]"): False,
    model.selection("EDGE", "[Extrusion_1_1/From_Face][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7]"): False,
    model.selection("EDGE", "[(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7)(Extrusion_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_16)2(ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_9)2][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7)(Extrusion_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_16)2(ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_9)2]"): False,
    model.selection("EDGE", "[(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7)(Extrusion_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_2)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_16)2(ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face)2(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)2(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_9)2][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4]"): False,
}

model.checkFilter(Part_1_doc, model, Filters, Reference)
