# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
SketchArcId = 'SketchArc'
SketchConstraintCoincidenceId = 'SketchConstraintCoincidence'
SketchConstraintEqualId = 'SketchConstraintEqual'

aSession = ModelAPI_Session.get()
model.begin()
partSet = model.moduleDocument()
Part = model.addPart(partSet)
Part_doc = Part.document()

# Test1:begin split on arc with coincident point and intersection line : smaller part
Sketch = model.addSketch(Part_doc, model.defaultPlane("XOY"))
SketchArc = Sketch.addArc(50, 50, 30, 50, 50, 70, False)
SketchLine_1 = Sketch.addLine(50, 20, 100, 20)
SketchLine_2 = Sketch.addLine(100, 20, 70, 50)
SketchLine_intersecting = Sketch.addLine(10, 40, 90, 40)

SketchConstraintCoincidence_1_1 = Sketch.setCoincident(SketchLine_1.startPoint(), SketchArc.results()[1])
SketchConstraintCoincidence_1_3 = Sketch.setCoincident(SketchLine_2.endPoint(), SketchArc.results()[1])
SketchConstraintCoincidence_1_2 = Sketch.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())

Sketch_features = featureToCompositeFeature(Sketch.feature())
assert (Sketch_features.numberOfSubs() == 7)

Arc_Points = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchArc.feature(), FeatureList([SketchLine_intersecting.feature()]))
assert(len(Arc_Points) == 2)

ModelAPI.removeFeaturesAndReferences(FeatureSet([SketchLine_intersecting.feature()]))

Sketch_features = featureToCompositeFeature(Sketch.feature())
assert (Sketch_features.numberOfSubs() == 6)

assert (len(Arc_Points) == 2)
if Arc_Points[0].x() < Arc_Points[1].x():
  GeomPoint = Arc_Points[0]
else:
  GeomPoint = Arc_Points[1]

#check number of features before trim
Sketch_feature = featureToCompositeFeature(Sketch.feature())
idList_before = []
for index in range(Sketch_feature.numberOfSubs()):
  idList_before.append(Sketch_feature.subFeature(index).getKind())

assert(idList_before.count(SketchArcId) == 1)
assert(idList_before.count(SketchLineId) == 2)
assert(idList_before.count(SketchConstraintCoincidenceId) == 3)

#perform trim
SketchTrim = Sketch.addTrim(SketchArc, Sketch.to2D(GeomPoint))
SketchTrim.execute()
model.do()

#check number of features after trim
SketchFeatures = featureToCompositeFeature(Sketch.feature())
idList_after = []
anArcList = FeatureList()
for SubIndex in range(SketchFeatures.numberOfSubs()):
    SubFeature = SketchFeatures.subFeature(SubIndex)
    idList_after.append(SubFeature.getKind())
    if SubFeature.getKind() == SketchArcId:
      SketchArc = SubFeature
      anArcList.append(SubFeature)

assert(len(anArcList) == 1)
assert(idList_after.count(SketchArcId) == 1)
assert(idList_after.count(SketchLineId) == 2)
assert(idList_after.count(SketchConstraintCoincidenceId) == 3)

#check arc position intersections of created arc to an additional line
SketchLine_intersecting_1 = Sketch.addLine(50, 50, 25, 25)
SketchLine_intersecting_2 = Sketch.addLine(50, 50, 75, 25)
SketchLine_intersecting_3 = Sketch.addLine(50, 50, 75, 75)
SketchLine_intersecting_4 = Sketch.addLine(50, 50, 25, 75)

Intersection_Points_1 = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchLine_intersecting_1.feature(), anArcList)
Intersection_Points_2 = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchLine_intersecting_2.feature(), anArcList)
Intersection_Points_3 = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchLine_intersecting_3.feature(), anArcList)
Intersection_Points_4 = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchLine_intersecting_4.feature(), anArcList)

assert(len(Intersection_Points_1) == 0)
assert(len(Intersection_Points_2) == 1)
assert(len(Intersection_Points_3) == 1)
assert(len(Intersection_Points_4) == 0)

#add point for check
SketchPoint = Sketch.addPoint(GeomPoint.x(), GeomPoint.y())
SketchPoint = Sketch.addPoint(GeomPoint.x(), GeomPoint.y())
Intersection_Points_5 = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchPoint.feature(), anArcList)
assert(len(Intersection_Points_5) == 0)

model.end()

assert(model.checkPythonDump())
