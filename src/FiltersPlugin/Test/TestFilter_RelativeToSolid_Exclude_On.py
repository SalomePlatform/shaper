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
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchArc_1 = Sketch_1.addArc(0, 0, 20, 0, 0, 20, False)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.result(), SketchArc_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.result(), SketchArc_1.endPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.center(), SketchAPI_Line(SketchLine_2).startPoint())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 20)
SketchLine_3 = Sketch_1.addLine(0, 32.83768482493814, 29.63380351263586, 0)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchLine_2.result())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_1.result())
SketchLine_4 = Sketch_1.addLine(29.63380351263586, 0, 62.471488337574, 29.63380351263586)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(62.471488337574, 29.63380351263586, 32.83768482493814, 62.471488337574)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(32.83768482493814, 62.471488337574, 0, 32.83768482493814)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchLine_6.endPoint())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_3.result(), SketchLine_4.result())
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_3.result(), SketchLine_5.result())
SketchConstraintParallel_2 = Sketch_1.setParallel(SketchLine_4.result(), SketchLine_6.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchArc_1.center(), SketchLine_3.result(), 22, True)
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_3.result(), SketchLine_4.result())
SketchLine_7 = Sketch_1.addLine(0, 20, 0, 0)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_1).startPoint(), SketchLine_7.endPoint())
SketchLine_8 = Sketch_1.addLine(0, 0, 20, 0)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_1).startPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_8.endPoint())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_3f-SketchLine_4f-SketchLine_5f-SketchLine_6f"), model.selection("FACE", "Sketch_1/Face-SketchArc_1_2f-SketchLine_7f-SketchLine_8f")], model.selection(), 30, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/To_Face"))
SketchCircle_1 = Sketch_2.addCircle(28.24714778717828, 31.38901581881253, 10)
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/To_Face]"), False)
SketchLine_9 = SketchProjection_3.createdFeature()
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchCircle_1.center(), SketchLine_9.result(), 20, True)
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/To_Face]"), False)
SketchLine_10 = SketchProjection_4.createdFeature()
SketchConstraintDistance_3 = Sketch_2.setDistance(SketchCircle_1.center(), SketchLine_10.result(), 20, True)
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchCircle_1.results()[1], 10)
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchCircle_1_2r_wire")], model.selection(), 10, 0)
Extrusion_3 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_1_2r")], model.selection(), 0, 10)
Extrusion_4 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_1_2r")], model.selection(), -10, 20)
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1")], 50, 10, 10)
Filters = model.filters(Part_1_doc, [model.addFilter(name = "RelativeToSolid", exclude = True, args = [model.selection("SOLID", "Extrusion_1_1"), "on"])])
model.end()

Reference = {
    # Faces of solid 1
    model.selection("FACE", "Extrusion_1_1/To_Face"): False,
    model.selection("FACE", "Extrusion_1_1/From_Face"): False,
    model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3"): False,
    model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4"): False,
    model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_5"): False,
    model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6"): False,
    # Faces of solid 2
    model.selection("FACE", "Extrusion_1_2/To_Face"): True,
    model.selection("FACE", "Extrusion_1_2/From_Face"): True,
    model.selection("FACE", "Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_7"): True,
    model.selection("FACE", "Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_8"): True,
    model.selection("FACE", "Extrusion_1_2/Generated_Face&Sketch_1/SketchArc_1_2"): True,
    # Faces of solid 3
    model.selection("FACE", "Extrusion_2_1/To_Face"): True,
    model.selection("FACE", "Extrusion_2_1/From_Face"): False,
    model.selection("FACE", "Extrusion_2_1/Generated_Face&Sketch_2/SketchCircle_1_2"): True,
    # Faces of solid 4
    model.selection("FACE", "Extrusion_3_1/To_Face"): False,
    model.selection("FACE", "Extrusion_3_1/From_Face"): True,
    model.selection("FACE", "Extrusion_3_1/Generated_Face&Sketch_2/SketchCircle_1_2"): True,
    # Faces of solid 5
    model.selection("FACE", "Extrusion_4_1/To_Face"): True,
    model.selection("FACE", "Extrusion_4_1/From_Face"): True,
    model.selection("FACE", "Extrusion_4_1/Generated_Face&Sketch_2/SketchCircle_1_2"): True,
    # Faces of solid 6
    model.selection("FACE", "Translation_1_1/MF:Translated&Cylinder_1_1/Face_1"): True,
    model.selection("FACE", "Translation_1_1/MF:Translated&Cylinder_1_1/Face_2"): True,
    model.selection("FACE", "Translation_1_1/MF:Translated&Cylinder_1_1/Face_3"): True,

    # Edges of solid 1
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/To_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/To_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4][Extrusion_1_1/To_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_5]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_5][Extrusion_1_1/To_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_5][Extrusion_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_5][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6]"): False,
    # Edges of solid 2
    model.selection("EDGE", "[Extrusion_1_2/Generated_Face&Sketch_1/SketchArc_1_2][Extrusion_1_2/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_7][Extrusion_1_2/From_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_2/Generated_Face&Sketch_1/SketchArc_1_2][Extrusion_1_2/From_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_8][Extrusion_1_2/From_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_7][Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_8]"): True,
    model.selection("EDGE", "[Extrusion_1_2/Generated_Face&Sketch_1/SketchArc_1_2][Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_8]"): True,
    model.selection("EDGE", "[Extrusion_1_2/Generated_Face&Sketch_1/SketchArc_1_2][Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_7]"): True,
    model.selection("EDGE", "[Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_7][Extrusion_1_2/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_8][Extrusion_1_2/To_Face]"): True,
    # Edges of solid 3
    model.selection("EDGE", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchCircle_1_2][Extrusion_2_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchCircle_1_2][Extrusion_2_1/From_Face]"): False,
    model.selection("EDGE", "([Extrusion_2_1/Generated_Face&Sketch_2/SketchCircle_1_2][Extrusion_2_1/From_Face])([Extrusion_2_1/Generated_Face&Sketch_2/SketchCircle_1_2][Extrusion_2_1/To_Face])"): True,
    # Edges of solid 4
    model.selection("EDGE", "[Extrusion_3_1/Generated_Face&Sketch_2/SketchCircle_1_2][Extrusion_3_1/To_Face]"): False,
    model.selection("EDGE", "[Extrusion_3_1/Generated_Face&Sketch_2/SketchCircle_1_2][Extrusion_3_1/From_Face]"): True,
    model.selection("EDGE", "([Extrusion_3_1/Generated_Face&Sketch_2/SketchCircle_1_2][Extrusion_3_1/From_Face])([Extrusion_3_1/Generated_Face&Sketch_2/SketchCircle_1_2][Extrusion_3_1/To_Face])"): True,
    # Edges of solid 5
    model.selection("EDGE", "[Extrusion_4_1/Generated_Face&Sketch_2/SketchCircle_1_2][Extrusion_4_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_4_1/Generated_Face&Sketch_2/SketchCircle_1_2][Extrusion_4_1/From_Face]"): True,
    model.selection("EDGE", "([Extrusion_4_1/Generated_Face&Sketch_2/SketchCircle_1_2][Extrusion_4_1/From_Face])([Extrusion_4_1/Generated_Face&Sketch_2/SketchCircle_1_2][Extrusion_4_1/To_Face])"): True,
    # Edges of solid 6
    model.selection("EDGE", "[Translation_1_1/MF:Translated&Cylinder_1_1/Face_1][Translation_1_1/MF:Translated&Cylinder_1_1/Face_2]"): True,
    model.selection("EDGE", "[Translation_1_1/MF:Translated&Cylinder_1_1/Face_1][Translation_1_1/MF:Translated&Cylinder_1_1/Face_3]"): True,
    model.selection("EDGE", "([Translation_1_1/MF:Translated&Cylinder_1_1/Face_1][Translation_1_1/MF:Translated&Cylinder_1_1/Face_2])([Translation_1_1/MF:Translated&Cylinder_1_1/Face_1][Translation_1_1/MF:Translated&Cylinder_1_1/Face_3])"): True,

    # Vertices of solid 1
    model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_5][Extrusion_1_1/To_Face]"): False,
    model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4][Extrusion_1_1/To_Face]"): False,
    model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_5][Extrusion_1_1/From_Face]"): False,
    model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4][Extrusion_1_1/From_Face]"): False,
    model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_5][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/To_Face]"): False,
    model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/To_Face]"): False,
    model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_5][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/From_Face]"): False,
    model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/From_Face]"): False,
    # Vertices of solid 2
    model.selection("VERTEX", "[Extrusion_1_2/Generated_Face&Sketch_1/SketchArc_1_2][Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_8][Extrusion_1_2/To_Face]"): True,
    model.selection("VERTEX", "[Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_7][Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_8][Extrusion_1_2/To_Face]"): True,
    model.selection("VERTEX", "[Extrusion_1_2/Generated_Face&Sketch_1/SketchArc_1_2][Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_8][Extrusion_1_2/From_Face]"): True,
    model.selection("VERTEX", "[Extrusion_1_2/Generated_Face&Sketch_1/SketchArc_1_2][Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_7][Extrusion_1_2/To_Face]"): True,
    model.selection("VERTEX", "[Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_7][Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_8][Extrusion_1_2/From_Face]"): True,
    model.selection("VERTEX", "[Extrusion_1_2/Generated_Face&Sketch_1/SketchArc_1_2][Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_7][Extrusion_1_2/From_Face]"): True,
    # Vertices of solid 3
    model.selection("VERTEX", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchCircle_1_2][Extrusion_2_1/To_Face]"): True,
    model.selection("VERTEX", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchCircle_1_2][Extrusion_2_1/From_Face]"): False,
    # Vertices of solid 4
    model.selection("VERTEX", "[Extrusion_3_1/Generated_Face&Sketch_2/SketchCircle_1_2][Extrusion_3_1/To_Face]"): False,
    model.selection("VERTEX", "[Extrusion_3_1/Generated_Face&Sketch_2/SketchCircle_1_2][Extrusion_3_1/From_Face]"): True,
    # Vertices of solid 5
    model.selection("VERTEX", "[Extrusion_4_1/Generated_Face&Sketch_2/SketchCircle_1_2][Extrusion_4_1/To_Face]"): True,
    model.selection("VERTEX", "[Extrusion_4_1/Generated_Face&Sketch_2/SketchCircle_1_2][Extrusion_4_1/From_Face]"): True,
    # Vertices of solid 6
    model.selection("VERTEX", "[Translation_1_1/MF:Translated&Cylinder_1_1/Face_1][Translation_1_1/MF:Translated&Cylinder_1_1/Face_2]"): True,
    model.selection("VERTEX", "[Translation_1_1/MF:Translated&Cylinder_1_1/Face_1][Translation_1_1/MF:Translated&Cylinder_1_1/Face_3]"): True,
}
model.checkFilter(Part_1_doc, model, Filters, Reference)
