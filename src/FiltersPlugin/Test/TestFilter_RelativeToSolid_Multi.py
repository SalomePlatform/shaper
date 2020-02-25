# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
SketchCircle_1 = Sketch_1.addCircle(18.85055999990443, 20.40038647424626, 12.41450630559296)
SketchCircle_2 = Sketch_1.addCircle(25.08744138385902, 26.81117176326632, 2.271058181559462)
SketchCircle_2.setName("SketchCircle_3")
SketchCircle_2.result().setName("SketchCircle_3")
SketchCircle_2.results()[1].setName("SketchCircle_3_2")
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_3 = Sketch_2.addCircle(30.95603034069762, 31.40670583726882, 10.95693005417035)
SketchCircle_3.setName("SketchCircle_4")
SketchCircle_3.result().setName("SketchCircle_4")
SketchCircle_3.results()[1].setName("SketchCircle_4_2")
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchCircle_1_2r-SketchCircle_3_2r_wire")], model.selection(), 50, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchCircle_4_2r_wire")], model.selection(), 40, -10)
Extrusion_3 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_3_2f")], model.selection(), 20, -30)
# Two filters to search all face inside of two solids
Filters = model.filters(Part_1_doc, [
  model.addFilter(name = "RelativeToSolid", args = [model.selection("SOLID", "Extrusion_1_1"), "in"]),
  model.addFilter(name = "RelativeToSolid", args = [model.selection("SOLID", "Extrusion_2_1"), "in"])])
model.end()

# 3 faces of internal cylinder is inside
Reference = {
    model.selection("FACE", "Extrusion_3_1/Generated_Face&Sketch_1/SketchCircle_3_2"): True,
    model.selection("FACE", "Extrusion_3_1/From_Face"): True,
    model.selection("FACE", "Extrusion_3_1/To_Face"): True,
    model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2"): False,
    model.selection("FACE", "Extrusion_1_1/From_Face"): False,
    model.selection("FACE", "Extrusion_1_1/To_Face"): False,
    model.selection("FACE", "Extrusion_2_1/Generated_Face&Sketch_2/SketchCircle_4_2"): False,
    model.selection("FACE", "Extrusion_2_1/From_Face"): False,
    model.selection("FACE", "Extrusion_2_1/To_Face"): False
}

model.checkFilter(Part_1_doc, model, Filters, Reference)
