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

"""
    Test2273.py
    Test case for issue #2273 "Error when reading HDF and Python dump"
                              (multi-rotation center refers to a feature produced by itself)
"""

from salome.shaper import model
from SketchAPI import *

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-65.36884900412264, 10.74954405845571, -18.59380593895045, 62.75409504395774)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()


# Test 1. Check MultiRotation error

SketchMultiRotation_1 = Sketch_1.addRotation([SketchLine_1.result()], SketchAPI_Point(SketchPoint_1).coordinates(), 90, 3)
[SketchLine_2, SketchLine_3] = SketchMultiRotation_1.rotated()
model.do()
# check MultiRotation has NO error
assert(SketchMultiRotation_1.feature().error() == "")
# change center of MultiRotation to extremity of generated feature, check an error appears
SketchMultiRotation_1.center().setAttr(SketchAPI_Line(SketchLine_2).startPoint())
model.do()
assert(SketchMultiRotation_1.feature().error() != "")
# revert changes => no error
SketchMultiRotation_1.center().setAttr(SketchAPI_Point(SketchPoint_1).coordinates())
model.do()
assert(SketchMultiRotation_1.feature().error() == "")


# Test 2. Check MultiRotation error

SketchMultiTranslation_1 = Sketch_1.addTranslation([SketchLine_1.result()], SketchLine_1.startPoint(), SketchAPI_Line(SketchLine_2).endPoint(), 2)
[SketchLine_4] = SketchMultiTranslation_1.translated()
model.do()
# check MultiTranslation has NO error
assert(SketchMultiTranslation_1.feature().error() == "")
# change start point to extremity of generated feature, check an error appears
SketchMultiTranslation_1.startPoint().setAttr(SketchAPI_Line(SketchLine_4).endPoint())
model.do()
assert(SketchMultiTranslation_1.feature().error() != "")
# revert changes => no error
SketchMultiTranslation_1.startPoint().setAttr(SketchLine_1.startPoint())
model.do()
assert(SketchMultiTranslation_1.feature().error() == "")
# change end point to extremity of generated feature, check an error appears
SketchMultiTranslation_1.endPoint().setAttr(SketchAPI_Line(SketchLine_4).endPoint())
model.do()
assert(SketchMultiTranslation_1.feature().error() != "")
# revert changes => no error
SketchMultiTranslation_1.endPoint().setAttr(SketchAPI_Line(SketchLine_2).endPoint())
model.do()
assert(SketchMultiTranslation_1.feature().error() == "")


# Test 3. Check Mirror error

SketchConstraintMirror_1 = Sketch_1.addMirror(SketchLine_2.result(), [SketchLine_1.result(), SketchLine_3.result()])
[SketchLine_5, SketchLine_6] = SketchConstraintMirror_1.mirrored()
model.do()
# check Mirror has NO error
assert(SketchConstraintMirror_1.feature().error() == "")
# change mirror line to generated feature, check an error appears
SketchConstraintMirror_1.mirrorLine().setObject(SketchLine_5.feature().firstResult())
model.do()
assert(SketchConstraintMirror_1.feature().error() != "")
# revert changes => no error
SketchConstraintMirror_1.mirrorLine().setObject(SketchLine_2.feature().firstResult())
model.do()
assert(SketchConstraintMirror_1.feature().error() == "")

model.end()


assert(model.checkPythonDump())
