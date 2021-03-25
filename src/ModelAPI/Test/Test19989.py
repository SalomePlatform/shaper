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
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(12, 10, 8)
model.do()
Sketch_2 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchCircle_2 = Sketch_2.addCircle(24, 10, 8)
model.do()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "PartSet/Sketch_1/Face-SketchCircle_1_2r")], model.selection(), 10, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "PartSet/Sketch_2/Face-SketchCircle_2_2r")], model.selection(), 12, 0)
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_1"), model.selection("COMPOUND", "all-in-Extrusion_2")], keepSubResults = True)
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), [model.selection("COMPOUND", "all-in-Fuse_1")])
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Fuse_1_1/Modified_Face&Extrusion_1_1/To_Face"))
SketchCircle_3 = Sketch_3.addCircle(32, 10, 2)
ExtrusionCut_1.setNestedSketch(Sketch_3)
Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Extrusion_2_1/To_Face")])
model.end()

# cause the update in the history for the group
model.begin()
Part_1_doc.setCurrentFeature(Fuse_1.feature(), True)
model.do()
Part_1_doc.setCurrentFeature(Group_1.feature(), True)
model.end()

assert(ExtrusionCut_1.feature().firstResult().shape().isSubShape(Group_1.groupList().value(0).value()))

assert(model.checkPythonDump())
