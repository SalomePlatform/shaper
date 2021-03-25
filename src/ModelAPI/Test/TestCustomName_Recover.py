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
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Cylinder_1.result().setName("cylinder")
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "cylinder/Face_2"), 10, False)
Symmetry_1 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "cylinder")], model.selection("FACE", "Plane_1"))
model.do()

# check the name of the Symmetry
SymmetryName = Symmetry_1.result().name()
assert(SymmetryName == Cylinder_1.result().name()), "Symmetry name '{}' != '{}'".format(SymmetryName, Cylinder_1.result().name())

# recover original cylinder
Recover_1 = model.addRecover(Part_1_doc, Symmetry_1, [Cylinder_1.result()])
model.do()

# check the name of the recovered result is not the cylinder name
assert(Recover_1.result().name() != SymmetryName), "Recovered feature SHOULD NOT have the same name as original feature '{}'".format(SymmetryName)

model.end()

assert(model.checkPythonDump())
