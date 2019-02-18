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
SketchLine_1 = Sketch.addLine(45, 68, 45, 73)
SketchLine_2 = Sketch.addLine(45, 73, 55, 73)
SketchLine_3 = Sketch.addLine(55, 73, 55, 68)

SketchConstraintCoincidence_1_1 = Sketch.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_1_2 = Sketch.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_1_3 = Sketch.setCoincident(SketchLine_1.startPoint(), SketchCircle.results()[1])
SketchConstraintCoincidence_1_4 = Sketch.setCoincident(SketchLine_3.endPoint(), SketchCircle.results()[1])

SketchLine_4 = Sketch.addLine(68, 45, 73, 45)
SketchLine_5 = Sketch.addLine(73, 45, 73, 55)
SketchLine_6 = Sketch.addLine(73, 55, 68, 55)

SketchConstraintCoincidence_1_1 = Sketch.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_1_2 = Sketch.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_1_3 = Sketch.setCoincident(SketchLine_4.startPoint(), SketchCircle.results()[1])
SketchConstraintCoincidence_1_4 = Sketch.setCoincident(SketchLine_6.endPoint(), SketchCircle.results()[1])


Sketch_features = featureToCompositeFeature(Sketch.feature())
assert (Sketch_features.numberOfSubs() == 15)
#intersection points on circle to prepare a trim selection point
#first intersection point - vertical
SketchLine_intersecting = Sketch.addLine(48, 10, 48, 90)
Circle_Points = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchCircle.feature(), FeatureList([SketchLine_intersecting.feature()]))
assert(len(Circle_Points) == 2)
ModelAPI.removeFeaturesAndReferences(FeatureSet([SketchLine_intersecting.feature()]))
Sketch_features = featureToCompositeFeature(Sketch.feature())
assert (Sketch_features.numberOfSubs() == 15)

assert (len(Circle_Points) == 2)
GeomPoint = Circle_Points[1]
if Circle_Points[0].y() < Circle_Points[1].y():
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
assert(idList_before.count(SketchLineId) == 6)
assert(idList_before.count(SketchConstraintCoincidenceId) == 8)

#perform trim: vertical
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
assert(idList_after.count(SketchLineId) == 6)
assert(idList_after.count(SketchConstraintCoincidenceId) == 8)


#add point for check
SketchPoint = Sketch.addPoint(GeomPoint.x(), GeomPoint.y())
Intersection_Points_3 = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchArc, FeatureList([SketchPoint.feature()]))
assert(len(Intersection_Points_3) == 0)

#perform trim: horizontal
#second intersection point - horizontal
SketchLine_intersecting_h = Sketch.addLine(10, 48, 72, 48)
Circle_Points_h = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchArc, FeatureList([SketchLine_intersecting_h.feature()]))
assert(len(Circle_Points_h) == 2)
ModelAPI.removeFeaturesAndReferences(FeatureSet([SketchLine_intersecting_h.feature()]))
Sketch_features = featureToCompositeFeature(Sketch.feature())
assert (Sketch_features.numberOfSubs() == 16)

assert (len(Circle_Points_h) == 2)
GeomPoint = Circle_Points_h[1]
if Circle_Points_h[0].x() < Circle_Points_h[1].x():
  GeomPoint_h = Circle_Points_h[1]
else:
  GeomPoint_h = Circle_Points_h[0]


SketchTrim = Sketch.addTrim(SketchArc, Sketch.to2D(GeomPoint_h))
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
      anArcList.append(SubFeature)

assert(len(anArcList) == 2)

assert(idList_after.count(SketchCircleId) == 0)
assert(idList_after.count(SketchArcId) == 2)
assert(idList_after.count(SketchLineId) == 6)
assert(idList_after.count(SketchConstraintCoincidenceId) == 9)


#add point for check
SketchPoint = Sketch.addPoint(GeomPoint_h.x(), GeomPoint_h.y())
Intersection_Points_5 = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchPoint.feature(), anArcList)
assert(len(Intersection_Points_5) == 0)


model.end()

assert(model.checkPythonDump())
