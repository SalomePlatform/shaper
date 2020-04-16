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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchEllipticArc_1 = Sketch_1.addEllipticArc(-10.32877055751729, 9.639944794047965, 15.50521682336113, 20.50671907502198, 14.00087220087917, 9.066138556306997, -21.07385236240306, -7.836309501397333, False)
[SketchPoint_1, SketchPoint_2, SketchPoint_3, SketchPoint_4, SketchPoint_5, SketchPoint_6, SketchPoint_7, SketchLine_1, SketchLine_2] = SketchEllipticArc_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
SketchLine_3 = Sketch_1.addLine(14.00087220087917, 9.066138556306999, -27.08576797296286, 16.56662668932342)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchEllipticArc_1.startPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchEllipticArc_1.result())
SketchLine_4 = Sketch_1.addLine(-21.07385236240307, -7.836309501397318, 6.541391110246827, 27.42439482327214)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchEllipticArc_1.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchEllipticArc_1.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 50, 0)
FilterFace = model.filters(Part_1_doc, [model.addFilter(name = "OnGeometry", args = [model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchEllipticArc_1")])])
model.end()

Reference = {
    model.selection("FACE", "Extrusion_1_1_1/From_Face"): False,
    model.selection("FACE", "Extrusion_1_1_1/To_Face"): False,
    model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchEllipticArc_1"): True,
    model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_4"): False,
    model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_3"): False,
    model.selection("FACE", "Extrusion_1_1_2/To_Face"): False,
    model.selection("FACE", "Extrusion_1_1_2/From_Face"): False,
    model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchEllipticArc_1"): True,
    model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_3"): False,
    model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_4"): False,
    model.selection("FACE", "Extrusion_1_1_3/To_Face"): False,
    model.selection("FACE", "Extrusion_1_1_3/From_Face"): False,
    model.selection("FACE", "Extrusion_1_1_3/Generated_Face&Sketch_1/SketchLine_4"): False,
    model.selection("FACE", "Extrusion_1_1_3/Generated_Face&Sketch_1/SketchLine_3"): False,
    model.selection("FACE", "Extrusion_1_1_3/Generated_Face&Sketch_1/SketchEllipticArc_1"): True
}

model.checkFilter(Part_1_doc, model, FilterFace, Reference)
