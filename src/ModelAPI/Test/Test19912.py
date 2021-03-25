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
SketchCircle_1 = Sketch_1.addCircle(50, 50, 25)
model.do()

Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection(), 10, 0)

Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/To_Face"))
SketchCircle_2 = Sketch_2.addCircle(40, 42, 5)
SketchCircle_3 = Sketch_2.addCircle(60, 60, 6)
model.do()

Face_1 = model.addFace(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")])
Partition_1_objects = [model.selection("SOLID", "Extrusion_1_1"),
                       model.selection("FACE", "Face_1_1"),
                       model.selection("FACE", "Face_1_2")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects, keepSubResults = True)
Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Face_1_1/Face_1_1")])
model.end()

assert(len(Group_1.results()) == 1)

# remove Face_1_1 from the partition, so the group selection must become invalid
model.begin()
Partition_1.setBase([model.selection("SOLID", "Extrusion_1_1"), model.selection("FACE", "Face_1_2")])
model.end()

assert(len(Group_1.results()) == 0)
