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

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Box
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_1.result().setColor(random=True)
assert(Box_1.result().resultSubShapePair()[0].data().intArray("Color").isInitialized())

### Create next Box
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
aRed = 60
aGreen = 200
aBlue = 100
Box_2.result().setColor(aRed, aGreen, aBlue)

assert(Box_2.result().resultSubShapePair()[0].data().intArray("Color").isInitialized())
assert(Box_2.result().resultSubShapePair()[0].data().intArray("Color").value(0) == aRed)
assert(Box_2.result().resultSubShapePair()[0].data().intArray("Color").value(1) == aGreen)
assert(Box_2.result().resultSubShapePair()[0].data().intArray("Color").value(2) == aBlue)

model.end()

assert(model.checkPythonDump())
