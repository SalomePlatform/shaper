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
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(20, 23, -19, 23)
SketchLine_2 = Sketch_1.addLine(-19, 23, -19, -21)
SketchLine_3 = Sketch_1.addLine(-19, -21, 20, -21)
SketchLine_4 = Sketch_1.addLine(20, -21, 20, 23)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection(), 30, 0)
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), 0, 20, [model.selection("SOLID", "Extrusion_1_1")])
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/To_Face"))
SketchLine_5 = Sketch_2.addLine(34, 36, -11, 36)
SketchLine_6 = Sketch_2.addLine(-11, 36, -11, -10)
SketchLine_7 = Sketch_2.addLine(-11, -10, 34, -10)
SketchLine_8 = Sketch_2.addLine(34, -10, 34, 36)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_8.result())
ExtrusionCut_1.setNestedSketch(Sketch_2)
# two filters with different faces each, the second is reversed
Filters = model.filters(Part_1_doc, [
  model.addFilter(name = "OnPlaneSide", args = [model.selection("FACE", "ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7")]),
  model.addFilter(name = "OnPlaneSide", exclude = True, args = [model.selection("FACE", "ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6")]),
  model.addFilter(name = "OnPlaneSide", args = [model.selection("FACE", "ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face")])])
model.end()

# 5 edges are in result
Reference = {
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2]"): True,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_1_1/Modified_Face&Extrusion_1_1/To_Face]"): True,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6]"): True,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][ExtrusionCut_1_1/Modified_Face&Extrusion_1_1/To_Face]"): True,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Extrusion_1_1/To_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6]"): True,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3]"): False,
    model.selection("EDGE", "[Extrusion_1_1/From_Face][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Extrusion_1_1/To_Face][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Extrusion_1_1/To_Face][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Extrusion_1_1/To_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7]"): False,
    model.selection("EDGE", "([ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4])([ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7])([ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_7])"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6][ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/From_Face][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4]"): False,
    model.selection("EDGE", "[Extrusion_1_1/From_Face][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2]"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "([ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6])([ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4])([ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_6][ExtrusionCut_1_1/Modified_Face&ExtrusionCut_1_1/From_Face])"): False,
    model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_4]"): False,
}

model.checkFilter(Part_1_doc, model, Filters, Reference)
