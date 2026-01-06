# Copyright (C) 2025-2026  CEA, EDF
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
import os

data_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")

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

### Create Import
Import_1 = model.addImportSTEP(Part_1_doc, os.path.join(data_dir, "ColorRamp2.step"), False, False, False)

model.do()

aRes = Import_1.result()

### check colors
aBody = model.modelAPI_ResultBody(Import_1.feature().firstResult())

checkColor(aBody.subResult(0), [255, 0, 0])
checkColor(aBody.subResult(1), [255, 33, 0])
checkColor(aBody.subResult(2), [0, 255, 0])
checkColor(aBody.subResult(3), [0, 0, 255])
checkColor(aBody.subResult(4), [255, 153, 0])
checkColor(aBody.subResult(5), [153, 255, 0])
checkColor(aBody.subResult(6), [33, 255, 0])
checkColor(aBody.subResult(7), [0, 255, 33])
checkColor(aBody.subResult(8), [0, 255, 153])
checkColor(aBody.subResult(9), [0, 153, 255])
checkColor(aBody.subResult(10), [0, 33, 255])
checkColor(aBody.subResult(11), [33, 0, 255])
checkColor(aBody.subResult(12), [153, 0, 255])
checkColor(aBody.subResult(13), [255, 0, 153])
checkColor(aBody.subResult(14), [255, 0, 33])
checkColor(aBody.subResult(15), [0, 255, 255])

checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_2"), [255,0,0])
checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_3"), [255,0,0])

checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_4"), [255,33,0])
checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_5"), [255,33,0])

checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_6"), [255,153,0])
checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_7"), [255,153,0])

checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_8"), [153,255,0])
checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_9"), [153,255,0])

checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_10"), [33,255,0])
checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_11"), [33,255,0])

checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_12"), [0,255,0])
checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_13"), [0,255,0])

checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_14"), [0,255,33])
checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_15"), [0,255,33])

checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_16"), [0,255,153])
checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_17"), [0,255,153])

checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_18"), [0,153,255])
checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_19"), [0,153,255])

checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_20"), [0,33,255])
checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_21"), [0,33,255])

checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_22"), [0,0,255])
checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_23"), [0,0,255])

checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_24"), [33,0,255])
checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_25"), [33,0,255])

checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_26"), [153,0,255])
checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_27"), [153,0,255])

checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_28"), [255,0,153])
checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_29"), [255,0,153])

checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_30"), [255,0,33])
checkSubShapeColor(aRes, model.selection("FACE", "Pad/Shape_31"), [255,0,33])

col = aRes.getSubShapeColor(model.selection("FACE", "Pad/Shape_1"))
assert(col.size() == 0)
col = aRes.getSubShapeColor(model.selection("FACE", "Pad/Shape_32"))
assert(col.size() == 0)
col = aRes.getSubShapeColor(model.selection("FACE", "Pad/Shape_33"))
assert(col.size() == 0)
col = aRes.getSubShapeColor(model.selection("FACE", "Pad/Shape_34"))
assert(col.size() == 0)

model.end()

assert(model.checkPythonDump())
