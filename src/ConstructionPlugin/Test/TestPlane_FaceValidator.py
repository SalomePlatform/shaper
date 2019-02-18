# Copyright (C) 2018-2019  CEA/DEN, EDF R&D
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
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), 10)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("FACE", "Sphere_1_1/Face_1")])

Plane_1 = model.addPlane(Part_1_doc, model.selection("COMPOUND", (0, 0, 0)), 10, False)
assert(Plane_1.feature().error() != "")
Part_1_doc.removeFeature(Plane_1.feature())

Plane_1 = model.addPlane(Part_1_doc, model.selection("EDGE", "[Compound_1_1_1/Compound_1_1_1&Sphere_1_1/Face_1][weak_name_3]"), 10, False)
assert(Plane_1.feature().error() != "")
Part_1_doc.removeFeature(Plane_1.feature())

model.end()
