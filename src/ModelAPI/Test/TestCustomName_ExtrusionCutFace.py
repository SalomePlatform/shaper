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
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_1.result().setName("box")
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "box/Front"))
SketchCircle_1 = Sketch_1.addCircle(7, 6, 2)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchCircle_1_2")])
Face_1.result().setName("circle")
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "circle")], model.selection(), 0, 3, [model.selection("SOLID", "box")])
model.do()

assert(ExtrusionCut_1.result().name() == Box_1.result().name()), "ExtrusionCut name '{}' != '{}'".format(ExtrusionCut_1.result().name(), Box_1.result().name())

model.end()

assert(model.checkPythonDump())
