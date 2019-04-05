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
SketchCircle_1 = Sketch_1.addCircle(0, 0, 50)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_2.addLine(25, 25, 100, 25)
SketchLine_2 = Sketch_2.addLine(100, 25, 60, 75)
SketchLine_3 = Sketch_2.addLine(60, 75, 25, 25)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1"), model.selection("COMPOUND", "Sketch_2")], model.selection(), 100, 0)
Split_1 = model.addSplit(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], [model.selection("SOLID", "Extrusion_1_2")])
model.do()
model.end()
assert (len(Split_1.results()) > 0)
assert(model.checkPythonDump())
