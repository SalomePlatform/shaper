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
SketchCircle_1 = Sketch_1.addCircle(8, 23, 2.5)
SketchCircle_2 = Sketch_1.addCircle(14, 17, 2.5)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r"), model.selection("FACE", "Sketch_1/Face-SketchCircle_2_2f")], model.selection(), 10, 0)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_1_2")])
Fillet_1 = model.addFillet(Part_1_doc, [model.selection("EDGE", "[Compound_1_1_2/Modified_Face&Sketch_1/SketchCircle_2_2][Compound_1_1_2/Modified_Face&Extrusion_1_2/To_Face]")], 2, keepSubResults = True)
Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Fillet_1_1_2/GF:Fillet&Fillet_1_1_2/FilletSelected")])
model.end()

assert(model.checkPythonDump())
