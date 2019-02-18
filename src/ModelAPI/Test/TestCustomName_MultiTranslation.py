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

# -*- coding: utf-8 -*-

from salome.shaper import model
from ModelAPI import *

NB_COPIES = 3

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Cylinder_1.result().setName("cylinder")
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "cylinder")], model.selection("EDGE", "([cylinder/Face_1][cylinder/Face_2])([cylinder/Face_1][cylinder/Face_3])"), 10, NB_COPIES)
model.do()

TransResult = LinearCopy_1.result()
TransResultName = TransResult.name()
assert(TransResultName == Cylinder_1.result().name()), "MultiTranslation name '{}' != '{}'".format(TransResultName, Cylinder_1.result().name())
# check sub-result names
assert(NB_COPIES == TransResult.numberOfSubs()), "Number of results {} is not equal to reference {}".format(TransResult.numberOfSubs(), NB_COPIES)
MultiTranslationName = LinearCopy_1.name() + "_1"
for i in range(0, NB_COPIES):
  refName = MultiTranslationName + '_' + str(i + 1)
  subResult = TransResult.subResult(i)
  assert(subResult.name() == refName), "MultiTranslation sub-result {} name '{}' != '{}'".format(i, subResult.name(), refName)

model.end()

assert(model.checkPythonDump())
