# Copyright (C) 2014-2026  CEA, EDF
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

def checkColor(theBody, theColExp):
  aColAttr = theBody.data().intArray("Color")
  assert(aColAttr.size() == 3)
  col = [aColAttr.value(0), aColAttr.value(1), aColAttr.value(2)]
  assert(col[0] == theColExp[0] and col[1] == theColExp[1] and col[2] == theColExp[2]), "Shape color is %d,%d,%d, while expected %d,%d,%d"%(col[0],col[1],col[2], theColExp[0],theColExp[1],theColExp[2])

def checkSubShapeColor(theRes, theSel, theColExp):
  col = theRes.getSubShapeColor(theSel)

  assert(col.size() == 3)
  assert(col[0] == theColExp[0] and col[1] == theColExp[1] and col[2] == theColExp[2]), "Face color is %d,%d,%d, while expected %d,%d,%d"%(col[0],col[1],col[2], theColExp[0],theColExp[1],theColExp[2])

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Colors
aRed1   = 85
aGreen1 = 170
aBlue1  = 0

aRed2   = 60
aGreen2 = 100
aBlue2  = 255

### Create Box
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_1.result().setColor(aRed1, aGreen1, aBlue1)
assert(Box_1.result().resultSubShapePair()[0].data().intArray("Color").isInitialized())

### Create Cylinder
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)

Cylinder_1.result().setSubShapeColor(model.selection("FACE", "Cylinder_1_1/Face_2"), aRed2, aGreen2, aBlue2)

### Create Compound
Compound_1 = model.addCompound(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Cylinder_1_1")])
Compound_1.result().subResult(0).setColor(aRed2, aGreen2, aBlue2)
Compound_1.result().setSubShapeColor(model.selection("FACE", "Compound_1_1_2/Modified_Face&Cylinder_1_1/Face_2"), aRed1, aGreen1, aBlue1)

model.do()

### check colors
aBoxBody = model.modelAPI_ResultBody(Box_1.feature().firstResult())
checkColor(aBoxBody, [aRed1, aGreen1, aBlue1])

checkSubShapeColor(Cylinder_1.result(), model.selection("FACE", "Cylinder_1_1/Face_2"), [aRed2, aGreen2, aBlue2])

aCompBody = model.modelAPI_ResultBody(Compound_1.feature().firstResult())
checkColor(aCompBody.subResult(0), [aRed2, aGreen2, aBlue2])

checkSubShapeColor(Compound_1.result(), model.selection("FACE", "Compound_1_1_2/Modified_Face&Cylinder_1_1/Face_2"), [aRed1, aGreen1, aBlue1])

model.end()

assert(model.checkPythonDump())
