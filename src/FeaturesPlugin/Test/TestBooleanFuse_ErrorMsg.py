# Copyright (C) 2018-2023  CEA, EDF
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
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1"), model.selection("FACE", "PartSet/XOZ")])

Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Partition_1_1_1")])
assert(Fuse_1.feature().error() != "")
Part_1_doc.removeFeature(Fuse_1.feature())

Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Partition_1_1_1"), model.selection("SOLID", "Partition_1_1_2")])
# after merging Union and Fuse features, fusing of solids in the same composolid should work (issue #3062)
assert(Fuse_1.feature().error() == "")
Part_1_doc.removeFeature(Fuse_1.feature())

Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Partition_1_1_1")], [model.selection("SOLID", "Partition_1_1_2")])
# after merging Union and Fuse features, fusing of solids in the same composolid should work (issue #3062)
assert(Fuse_1.feature().error() == "")
Part_1_doc.removeFeature(Fuse_1.feature())
model.end()


from ModelAPI import *
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

aSession.startOperation()
Fuse_1 = Part_1_doc.addFeature("Fuse")
Fuse_1.execute()
assert(Fuse_1.error() != "")
aSession.finishOperation()
