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
Sketch = model.addSketch(Part_doc, model.defaultPlane("XOZ"))
# Test of inverted Arc
SketchLine_1 = Sketch.addLine(10, 80, 40, 10)
SketchLine_2 = Sketch.addLine(40, 10, 70, 80)
SketchLine_3 = Sketch.addLine(70, 80, 100, 10)
SketchLine_4 = Sketch.addLine(100, 10, 130, 80)

SketchLine_intersecting = Sketch.addLine(10, 50, 130, 50)

SketchConstraintCoincidence_1_1 = Sketch.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_1_2 = Sketch.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_1_3 = Sketch.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())

GeomPoint = geom.Pnt2d(40, 50)

#check number of features before trim
Sketch_feature = featureToCompositeFeature(Sketch.feature())
idList_before = []
for index in range(Sketch_feature.numberOfSubs()):
  idList_before.append(Sketch_feature.subFeature(index).getKind())

assert(idList_before.count(SketchLineId) == 5)
assert(idList_before.count(SketchConstraintCoincidenceId) == 3)

#perform trim
SketchTrim = Sketch.addTrim(SketchLine_intersecting, GeomPoint)
SketchTrim.execute()
model.do()

#check number of features after trim
SketchFeatures = featureToCompositeFeature(Sketch.feature())
idList_after = []

LinesList = FeatureList()
for SubIndex in range(SketchFeatures.numberOfSubs()):
    SubFeature = SketchFeatures.subFeature(SubIndex)
    idList_after.append(SubFeature.getKind())
    if SubFeature.getKind() == SketchLineId:
      LinesList.append(SubFeature)

assert(idList_after.count(SketchLineId) == 6)
assert(idList_after.count(SketchConstraintCoincidenceId) == 5)


#check lines intersection to the source line
SketchLine_intersecting_1 = Sketch.addLine(15, 55, 15, 40)
SketchLine_intersecting_2 = Sketch.addLine(40, 55, 40, 40)
SketchLine_intersecting_3 = Sketch.addLine(70, 55, 70, 40)
SketchLine_intersecting_4 = Sketch.addLine(100, 55, 100, 40)
SketchLine_intersecting_5 = Sketch.addLine(125, 55, 125, 40)

Intersection_Points_1 = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchLine_intersecting_1.feature(), LinesList)
Intersection_Points_2 = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchLine_intersecting_2.feature(), LinesList)
Intersection_Points_3 = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchLine_intersecting_3.feature(), LinesList)
Intersection_Points_4 = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchLine_intersecting_4.feature(), LinesList)
Intersection_Points_5 = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchLine_intersecting_5.feature(), LinesList)

assert(len(Intersection_Points_1) == 1)
assert(len(Intersection_Points_2) == 0)
assert(len(Intersection_Points_3) == 1)
assert(len(Intersection_Points_4) == 1)
assert(len(Intersection_Points_5) == 1)

#check if line points are the same

model.end()

assert(model.checkPythonDump())
