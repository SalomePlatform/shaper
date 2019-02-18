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

from salome.shaper import model

from ModelAPI import *
from GeomDataAPI import *
from ModelGeomAlgo import ModelGeomAlgo_Point2D
from salome.shaper import geom
import math

TOLERANCE = 1.e-7

SketchLineId = 'SketchLine'
SketchArcId = 'SketchArc'
SketchCircleId = 'SketchCircle'
SketchConstraintCoincidenceId = 'SketchConstraintCoincidence'

aSession = ModelAPI_Session.get()
model.begin()
partSet = model.moduleDocument()
Part = model.addPart(partSet)
Part_doc = Part.document()

# Test1:begin split on circle with coincident point and intersection line : smaller part
Sketch = model.addSketch(Part_doc, model.defaultPlane("XOY"))
SketchCircle = Sketch.addCircle(50, 50, 20)
SketchLine = Sketch.addLine(30, 75, 75, 30)
SketchLine_intersecting = Sketch.addLine(10, 55, 90, 55)
Sketch_features = featureToCompositeFeature(Sketch.feature())
assert (Sketch_features.numberOfSubs() == 3)

Circle_Points = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchCircle.feature(), FeatureList([SketchLine_intersecting.feature()]))
assert(len(Circle_Points) == 2)

ModelAPI.removeFeaturesAndReferences(FeatureSet([SketchLine_intersecting.feature()]))

Sketch_features = featureToCompositeFeature(Sketch.feature())
assert (Sketch_features.numberOfSubs() == 2)

assert (len(Circle_Points) == 2)
if Circle_Points[0].x() < Circle_Points[1].x():
  GeomPoint = Circle_Points[1]
else:
  GeomPoint = Circle_Points[0]

#check number of features before trim
Sketch_feature = featureToCompositeFeature(Sketch.feature())
idList_before = []
for index in range(Sketch_feature.numberOfSubs()):
  idList_before.append(Sketch_feature.subFeature(index).getKind())

assert(idList_before.count(SketchCircleId) == 1)
assert(idList_before.count(SketchArcId) == 0)
assert(idList_before.count(SketchLineId) == 1)
assert(idList_before.count(SketchConstraintCoincidenceId) == 0)

#perform trim
SketchTrim = Sketch.addTrim(SketchCircle, Sketch.to2D(GeomPoint))
SketchTrim.execute()
model.do()

#check number of features after trim
SketchFeatures = featureToCompositeFeature(Sketch.feature())
idList_after = []
for SubIndex in range(SketchFeatures.numberOfSubs()):
    SubFeature = SketchFeatures.subFeature(SubIndex)
    idList_after.append(SubFeature.getKind())
    if SubFeature.getKind() == SketchArcId:
      SketchArc = SubFeature

assert(idList_after.count(SketchCircleId) == 0)
assert(idList_after.count(SketchArcId) == 1)
assert(idList_after.count(SketchLineId) == 1)
assert(idList_after.count(SketchConstraintCoincidenceId) == 2)


#check arc position intersections of created arc to an additional line
SketchLine_intersecting_1 = Sketch.addLine(0, 0, 50, 50)
SketchLine_intersecting_2 = Sketch.addLine(50, 50, 100, 100)

Intersection_Points_1 = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchArc, FeatureList([SketchLine_intersecting_1.feature()]))
Intersection_Points_2 = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchArc, FeatureList([SketchLine_intersecting_2.feature()]))

assert(len(Intersection_Points_1) == 1)
assert(len(Intersection_Points_2) == 0)

#add point for check
SketchPoint = Sketch.addPoint(GeomPoint.x(), GeomPoint.y())
Intersection_Points_3 = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchArc, FeatureList([SketchPoint.feature()]))
assert(len(Intersection_Points_3) == 0)

model.end()

assert(model.checkPythonDump())
