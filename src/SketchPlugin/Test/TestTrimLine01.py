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

from ModelAPI import *
from GeomDataAPI import *
from ModelGeomAlgo import ModelGeomAlgo_Point2D
from salome.shaper import geom
import math

TOLERANCE = 1.e-7

SketchLineId = 'SketchLine'
SketchConstraintCoincidenceId = 'SketchConstraintCoincidence'

aSession = ModelAPI_Session.get()
model.begin()
partSet = model.moduleDocument()
Part = model.addPart(partSet)
Part_doc = Part.document()

# Test1:begin split on arc with coincident point and intersection line : smaller part
Sketch = model.addSketch(Part_doc, model.defaultPlane("XOY"))
# Test of inverted Arc
SketchLine_1 = Sketch.addLine(200, 20, 20, 70)
SketchLine_2 = Sketch.addLine(20, 70, 90, 150)
SketchLine_3 = Sketch.addLine(-10, 190, 90, 50)

SketchLine_intersecting = Sketch.addLine(-30, 85, 50, 85)

SketchConstraintCoincidence_1_1 = Sketch.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_1_2 = Sketch.setCoincident(SketchLine_1.results()[0], SketchLine_3.endPoint())

Sketch_features = featureToCompositeFeature(Sketch.feature())
assert (Sketch_features.numberOfSubs() == 6)

Intersection_Point = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchLine_2.feature(), FeatureList([SketchLine_intersecting.feature()]))
assert(len(Intersection_Point) == 1)

ModelAPI.removeFeaturesAndReferences(FeatureSet([SketchLine_intersecting.feature()]))

Sketch_features = featureToCompositeFeature(Sketch.feature())
assert (Sketch_features.numberOfSubs() == 5)

GeomPoint = Intersection_Point[0]

#check number of features before trim
Sketch_feature = featureToCompositeFeature(Sketch.feature())
idList_before = []
for index in range(Sketch_feature.numberOfSubs()):
  idList_before.append(Sketch_feature.subFeature(index).getKind())

assert(idList_before.count(SketchLineId) == 3)
assert(idList_before.count(SketchConstraintCoincidenceId) == 2)

#perform trim
SketchTrim = Sketch.addTrim(SketchLine_2, Sketch.to2D(GeomPoint))
SketchTrim.execute()
model.do()

#check number of features after trim
SketchFeatures = featureToCompositeFeature(Sketch.feature())
idList_after = []
for SubIndex in range(SketchFeatures.numberOfSubs()):
    SubFeature = SketchFeatures.subFeature(SubIndex)
    idList_after.append(SubFeature.getKind())

assert(idList_after.count(SketchLineId) == 3)
assert(idList_after.count(SketchConstraintCoincidenceId) == 2)

model.end()

assert(model.checkPythonDump())
