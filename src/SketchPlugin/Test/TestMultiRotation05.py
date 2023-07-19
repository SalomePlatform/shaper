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

from GeomAlgoAPI import GeomAlgoAPI_ShapeTools
from SketchAPI import *
from math import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(10, 0, 30, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
model.do()

ROT_ANGLE = 20
ROT_COPIES = 15
SketchMultiRotation_1 = Sketch_1.addRotation([SketchLine_1.result()], SketchAPI_Point(SketchPoint_1).coordinates(), ROT_ANGLE, ROT_COPIES)
model.do()

# collect coordinates of centers as reference data
TOLERANCE = 1.e-7
REF_DATA = [[]]
aLines = SketchMultiRotation_1.rotated()
for lin in aLines:
    curShape = lin.feature().lastResult().shape()
    massCenter = GeomAlgoAPI_ShapeTools.centreOfMass(curShape)
    REF_DATA.append([massCenter.x(), massCenter.y()])
del REF_DATA[0]

FeatureMultiRotation = SketchMultiRotation_1.feature()

# assign "reversed" flag
FeatureMultiRotation.boolean("MultiRotationReversed").setValue(True)
model.do()
assert(SketchMultiRotation_1.angle().value() == ROT_ANGLE)

# check coordinates of center of mass of each line
aLines = SketchMultiRotation_1.rotated()
ind = 0
for lin in aLines:
    curShape = lin.feature().lastResult().shape()
    massCenter = GeomAlgoAPI_ShapeTools.centreOfMass(curShape)
    assert(fabs(massCenter.x() - REF_DATA[ind][0]) < TOLERANCE and fabs(massCenter.y() + REF_DATA[ind][1]) < TOLERANCE and massCenter.z() == 0), "({}, {}, {}) != ({}, {}, {})".format(massCenter.x(), massCenter.y(), massCenter.z(), REF_DATA[ind][0], -REF_DATA[ind][1], 0.0)
    ind += 1

# drop "reversed" flag
FeatureMultiRotation.boolean("MultiRotationReversed").setValue(False)
model.do()
assert(SketchMultiRotation_1.angle().value() == ROT_ANGLE)

# check coordinates of center of mass of each line
aLines = SketchMultiRotation_1.rotated()
ind = 0
for lin in aLines:
    curShape = lin.feature().lastResult().shape()
    massCenter = GeomAlgoAPI_ShapeTools.centreOfMass(curShape)
    assert(fabs(massCenter.x() - REF_DATA[ind][0]) < TOLERANCE and fabs(massCenter.y() - REF_DATA[ind][1]) < TOLERANCE and massCenter.z() == 0), "({}, {}, {}) != ({}, {}, {})".format(massCenter.x(), massCenter.y(), massCenter.z(), REF_DATA[ind][0], REF_DATA[ind][1], 0.0)
    ind += 1

# set angle value from single to full
SketchMultiRotation_1.feature().string("AngleType").setValue("FullAngle")
model.do()
assert(SketchMultiRotation_1.angle().value() == ROT_ANGLE*(ROT_COPIES - 1))

# check coordinates of center of mass of each line
aLines = SketchMultiRotation_1.rotated()
ind = 0
for lin in aLines:
    curShape = lin.feature().lastResult().shape()
    massCenter = GeomAlgoAPI_ShapeTools.centreOfMass(curShape)
    assert(fabs(massCenter.x() - REF_DATA[ind][0]) < TOLERANCE and fabs(massCenter.y() - REF_DATA[ind][1]) < TOLERANCE and massCenter.z() == 0), "({}, {}, {}) != ({}, {}, {})".format(massCenter.x(), massCenter.y(), massCenter.z(), REF_DATA[ind][0], REF_DATA[ind][1], 0.0)
    ind += 1

# assign "reversed" flag
FeatureMultiRotation.boolean("MultiRotationReversed").setValue(True)
model.do()
assert(SketchMultiRotation_1.angle().value() == ROT_ANGLE*(ROT_COPIES - 1))

# check coordinates of center of mass of each line
aLines = SketchMultiRotation_1.rotated()
ind = 0
for lin in aLines:
    curShape = lin.feature().lastResult().shape()
    massCenter = GeomAlgoAPI_ShapeTools.centreOfMass(curShape)
    assert(fabs(massCenter.x() - REF_DATA[ind][0]) < TOLERANCE and fabs(massCenter.y() + REF_DATA[ind][1]) < TOLERANCE and massCenter.z() == 0), "({}, {}, {}) != ({}, {}, {})".format(massCenter.x(), massCenter.y(), massCenter.z(), REF_DATA[ind][0], -REF_DATA[ind][1], 0.0)
    ind += 1

model.end()

assert(model.checkPythonDump())
