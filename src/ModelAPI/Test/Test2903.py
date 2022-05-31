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
from ModelAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), 10)
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("COMPOUND", "all-in-Box_1"), model.selection("COMPOUND", "all-in-Cylinder_1")], True)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("COMPOUND", "all-in-Fuse_1"), model.selection("COMPOUND", "all-in-Sphere_1")])
model.do()
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Fuse_1.feature()))
assert(aFactory.validate(Partition_1.feature()))
# Modify Fuse to add sphere thatwas used in Partition
Fuse_1.setMainObjects([model.selection("COMPOUND", "all-in-Box_1"), model.selection("COMPOUND", "all-in-Cylinder_1"), model.selection("COMPOUND", "all-in-Sphere_1")])
model.end()
# partition must become invalid because it refers to the same object as fuse
assert(aFactory.validate(Fuse_1.feature()))
assert(not aFactory.validate(Partition_1.feature()))
