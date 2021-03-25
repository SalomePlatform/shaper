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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-100, 0, -50, 0)
SketchLine_2 = Sketch_1.addLine(-50, 0, -50, 50)
SketchLine_3 = Sketch_1.addLine(-50, 50, -100, 50)
SketchLine_4 = Sketch_1.addLine(-100, 50, -100, 0)
SketchLine_5 = Sketch_1.addLine(100, 0, 50, 0)
SketchLine_6 = Sketch_1.addLine(50, 0, 50, 50)
SketchLine_7 = Sketch_1.addLine(50, 50, 100, 50)
SketchLine_8 = Sketch_1.addLine(100, 50, 100, 0)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f"), model.selection("FACE", "Sketch_1/Face-SketchLine_8r-SketchLine_7r-SketchLine_6r-SketchLine_5r")], model.selection(), 50, 0)
Placement_1 = model.addPlacement(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4][Extrusion_1_1/From_Face]"), model.selection("VERTEX", "[Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_7][Extrusion_1_2/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_2/To_Face]"), False, True)
model.do()
model.end()
