# Copyright (C) 2019-2023  CEA/DEN, EDF R&D
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

from GeomAlgoAPI import *
from ModelAPI import *
from SketchAPI import *
from salome.shaper import model

import os
import math

# PartSet => Part

model.begin()
partSet = model.moduleDocument()
Point_1 = model.addPoint(partSet, 100, 100, 100)
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(0, 0, 30)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 30)
model.do()
Axis_1 = model.addAxis(partSet, model.selection("VERTEX", "Origin"), model.selection("VERTEX", "Point_2"))
model.end()

# store the reference data (without Origin, coordinate axes and planes)
features = partSet.allFeatures()
for i in range(0, 7):
    features.pop_front()
refData = []
for feat in features:
    res = []
    for r in feat.results():
        res.append(GeomAlgoAPI_ShapeTools.volume(r.shape()))
    refData.append( (feat.getKind(), res) )

filename = model.getTmpFileName('check_export', '.shaperpart')
model.removeTmpFile(filename)

# emport the document
model.begin()
model.exportPart(partSet, filename)
model.end()
assert os.path.exists(filename), "ERROR: Cannot export PartSet"

# close all documents
model.reset()

# import the document
model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.importPart(Part_1_doc, filename)
model.end()

model.removeTmpFile(filename)

# Test 1. No Part should be created
assert(partSet.size(ModelAPI_ResultPart.group()) == 1)

# Test 2. Compare results with the reference data
TOLERANCE = 1.e-7
features = Part_1_doc.allFeatures()
assert(len(features) == len(refData))
for feat, ref in zip(features, refData):
    assert(feat.getKind() == ref[0])
    for fv, rv in zip(feat.results(), ref[1]):
        assert(math.fabs(GeomAlgoAPI_ShapeTools.volume(fv.shape()) - rv) < TOLERANCE)

assert(model.checkPythonDump())
