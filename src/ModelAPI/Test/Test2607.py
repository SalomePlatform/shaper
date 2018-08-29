## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10, 10)
Box_1 = model.addBox(Part_1_doc, 2, 20, 5)
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("COMPOUND", "all-in-Box_1")], model.selection("EDGE", "PartSet/OZ"), 90, 2)
AngularCopy_1.setName("Cross")
AngularCopy_1.result().setName("Cross")

LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("COMPOUND", "all-in-Cross")], model.selection("EDGE", "PartSet/OZ"), -5, 2)

model.end()

# check names of the Linear copy: only higher level result must inherit the result name
assert(LinearCopy_1.result().name() == "Cross")
assert(LinearCopy_1.result().subResult(0).name() == "LinearCopy_1_1_1")
assert(LinearCopy_1.result().subResult(0).subResult(0).name() == "LinearCopy_1_1_1_1")
assert(LinearCopy_1.result().subResult(1).subResult(0).name() == "LinearCopy_1_1_2_1")
assert(LinearCopy_1.result().subResult(1).subResult(0).subResult(0).name() == "LinearCopy_1_1_2_1_1")
assert(LinearCopy_1.result().subResult(1).subResult(0).subResult(0).subResult(0).name() == "LinearCopy_1_1_2_1_1_1")
