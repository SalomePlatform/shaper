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

#assert(model.checkPythonDump())
