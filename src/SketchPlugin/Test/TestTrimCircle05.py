from salome.shaper import model

from ModelAPI import *
from GeomDataAPI import *
from ModelGeomAlgo import ModelGeomAlgo_Point2D
from salome.shaper import geom
import math

TOLERANCE = 1.e-7

SketchPointId = 'SketchPoint'
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
SketchPoint_1 = Sketch.addPoint(70, 50)
SketchPoint_2 = Sketch.addPoint(30, 50)
SketchPoint_3 = Sketch.addPoint(50, 70)
SketchPoint_4 = Sketch.addPoint(50, 30)

#coincidence point to circle
SketchConstraintCoincidence_1_1 = Sketch.setCoincident(SketchPoint_1.results()[0], SketchCircle.results()[1])
SketchConstraintCoincidence_1_2 = Sketch.setCoincident(SketchPoint_2.results()[0], SketchCircle.results()[1])

#coincidence circle to point
SketchConstraintCoincidence_1_3 = Sketch.setCoincident(SketchCircle.results()[1], SketchPoint_3.results()[0])
SketchConstraintCoincidence_1_4 = Sketch.setCoincident(SketchCircle.results()[1], SketchPoint_4.results()[0])

Sketch_features = featureToCompositeFeature(Sketch.feature())
assert (Sketch_features.numberOfSubs() == 9)
#intersection points on circle to prepare a trim selection point
SketchLine_intersecting = Sketch.addLine(10, 60, 90, 60)
Circle_Points = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchCircle.feature(), FeatureList([SketchLine_intersecting.feature()]))
assert(len(Circle_Points) == 2)
ModelAPI.removeFeaturesAndReferences(FeatureSet([SketchLine_intersecting.feature()]))
Sketch_features = featureToCompositeFeature(Sketch.feature())
assert (Sketch_features.numberOfSubs() == 9)

assert (len(Circle_Points) == 2)
if Circle_Points[0].x() < Circle_Points[1].x():
  GeomPoint = Circle_Points[0]
else:
  GeomPoint = Circle_Points[1]

#check number of features before trim
Sketch_feature = featureToCompositeFeature(Sketch.feature())
idList_before = []
for index in range(Sketch_feature.numberOfSubs()):
  idList_before.append(Sketch_feature.subFeature(index).getKind())

assert(idList_before.count(SketchCircleId) == 1)
assert(idList_before.count(SketchArcId) == 0)
assert(idList_before.count(SketchPointId) == 4)
assert(idList_before.count(SketchConstraintCoincidenceId) == 4)

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
assert(idList_after.count(SketchPointId) == 4)
assert(idList_after.count(SketchConstraintCoincidenceId) == 4)


#check arc position intersections of created arc to an additional line
SketchLine_intersecting_1 = Sketch.addLine(0, 0, 100, 100)
SketchLine_intersecting_2 = Sketch.addLine(0, 100, 100, 0)

Intersection_Points_1 = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchArc, FeatureList([SketchLine_intersecting_1.feature()]))
Intersection_Points_2 = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchArc, FeatureList([SketchLine_intersecting_2.feature()]))

assert(len(Intersection_Points_1) == 2)
assert(len(Intersection_Points_2) == 1)

#add point for check
SketchPoint = Sketch.addPoint(GeomPoint.x(), GeomPoint.y())
Intersection_Points_3 = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchArc, FeatureList([SketchPoint.feature()]))
assert(len(Intersection_Points_3) == 0)

model.end()

assert(model.checkPythonDump())
