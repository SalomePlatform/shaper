from salome.shaper import model

from ModelAPI import *
from GeomDataAPI import *
from salome.shaper import geom
import math

TOLERANCE = 1.e-7

SketchPointId = 'SketchPoint'
SketchLineId = 'SketchLine'
SketchArcId = 'SketchArc'
SketchCircleId = 'SketchCircle'
SketchConstraintCoincidenceId = 'SketchConstraintCoincidence'
SketchConstraintParallelId = 'SketchConstraintParallel'
SketchConstraintTangentId = 'SketchConstraintTangent'
SketchConstraintEqualId = 'SketchConstraintEqual'

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Test1:begin split on circle with coincident point and intersection line : smaller part
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1_1 = Sketch_1.addCircle(50, 50, 20)
SketchLine_1_1 = Sketch_1.addLine(50, 30, 100, 30)
SketchLine_1_2 = Sketch_1.addLine(60, 50, 100, 30)

SketchConstraintCoincidence_1_1 = Sketch_1.setCoincident(SketchLine_1_1.startPoint(), SketchCircle_1_1.results()[1])
SketchConstraintCoincidence_1_2 = Sketch_1.setCoincident(SketchLine_1_1.endPoint(), SketchLine_1_2.endPoint())
GeomPoint_1_1 = geom.Pnt2d(60, 35)

#check number of features before trim
Sketch_1_feature = featureToCompositeFeature(Sketch_1.feature())
idList_before_1 = []
for index in range(Sketch_1_feature.numberOfSubs()):
  idList_before_1.append(Sketch_1_feature.subFeature(index).getKind())
assert(idList_before_1.count(SketchCircleId) == 1)
assert(idList_before_1.count(SketchArcId) == 0)
assert(idList_before_1.count(SketchLineId) == 2)
assert(idList_before_1.count(SketchConstraintCoincidenceId) == 2)

#perform trim
SketchTrim_1_1 = Sketch_1.addTrim(SketchCircle_1_1, GeomPoint_1_1)
SketchTrim_1_1.execute()
model.do()

#check number of features after trim
SketchFeatures = featureToCompositeFeature(Sketch_1.feature())
idList_after_1 = []
for SubIndex in range(SketchFeatures.numberOfSubs()):
    SubFeature = SketchFeatures.subFeature(SubIndex)
    idList_after_1.append(SubFeature.getKind())
    if SubFeature.getKind() == SketchArcId:
      ArcFeature_1 = SubFeature


assert(idList_after_1.count(SketchCircleId) == 0)
assert(idList_after_1.count(SketchArcId) == 1)
assert(idList_after_1.count(SketchLineId) == 2)
assert(idList_after_1.count(SketchConstraintCoincidenceId) == 3)

#test created arc: it is not inversed, has coincidence to end line point
anInversed_1 = ArcFeature_1.boolean("InversedArc").value()
assert(anInversed_1 == False)
ArcPoint_1 = geomDataAPI_Point2D(ArcFeature_1.attribute("ArcEndPoint"))
LinePoint_1 = geomDataAPI_Point2D(SketchLine_1_1.startPoint())
aDistance_1 = math.hypot(LinePoint_1.x() - ArcPoint_1.x(), LinePoint_1.y() - ArcPoint_1.y())
#print "Distance " + repr(aDistance)
assert (math.fabs(aDistance_1) <= TOLERANCE)
# Test1:end


# Test2: split on circle with coincident point and intersection line : largest part
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
move_test_delta_y = 100
move_test_delta_x = 0
SketchCircle_2_1 = Sketch_2.addCircle(50, 50 + move_test_delta_y, 20)
SketchLine_2_1 = Sketch_2.addLine(50, 30 + move_test_delta_y, 100, 30 + move_test_delta_y)
SketchLine_2_2 = Sketch_2.addLine(60, 50 + move_test_delta_y, 100, 30 + move_test_delta_y)

SketchConstraintCoincidence_2_1 = Sketch_2.setCoincident(SketchLine_2_1.startPoint(), SketchCircle_2_1.results()[1])
SketchConstraintCoincidence_2_2 = Sketch_2.setCoincident(SketchLine_2_1.endPoint(), SketchLine_2_2.endPoint())
GeomPoint_2_1 = geom.Pnt2d(50, 75 + move_test_delta_y)

#check number of features before trim
Sketch_2_feature = featureToCompositeFeature(Sketch_2.feature())
idList_before_2 = []
for index in range(Sketch_2_feature.numberOfSubs()):
  idList_before_2.append(Sketch_2_feature.subFeature(index).getKind())
assert(idList_before_2.count(SketchCircleId) == 1)
assert(idList_before_2.count(SketchArcId) == 0)
assert(idList_before_2.count(SketchLineId) == 2)
assert(idList_before_2.count(SketchConstraintCoincidenceId) == 2)

#perform trim
SketchTrim_2_1 = Sketch_2.addTrim(SketchCircle_2_1, GeomPoint_2_1)
SketchTrim_2_1.execute()
model.do()

#check number of features after trim
SketchFeatures = featureToCompositeFeature(Sketch_2.feature())
idList_after_2 = []
for SubIndex in range(SketchFeatures.numberOfSubs()):
    SubFeature = SketchFeatures.subFeature(SubIndex)
    idList_after_2.append(SubFeature.getKind())
    if SubFeature.getKind() == SketchArcId:
      ArcFeature_2 = SubFeature


assert(idList_after_2.count(SketchCircleId) == 0)
assert(idList_after_2.count(SketchArcId) == 1)
assert(idList_after_2.count(SketchLineId) == 2)
assert(idList_after_2.count(SketchConstraintCoincidenceId) == 3)

#test created arc : it is not inversed, has coincidence to start line point
anInversed_2 = ArcFeature_2.boolean("InversedArc").value()
assert(anInversed_2 == False)
ArcPoint_2 = geomDataAPI_Point2D(ArcFeature_2.attribute("ArcStartPoint"))
LinePoint_2 = geomDataAPI_Point2D(SketchLine_2_1.startPoint())
aDistance_2 = math.hypot(LinePoint_2.x() - ArcPoint_2.x(), LinePoint_2.y() - ArcPoint_2.y())
#print "Distance " + repr(aDistance_2)
assert (math.fabs(aDistance_2) <= TOLERANCE)
# Test2:end

model.end()

#assert(model.checkPythonDump())
