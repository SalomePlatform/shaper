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
SketchConstraintMirrorId = 'SketchConstraintMirror'
SketchConstraintTangentId = 'SketchConstraintTangent'
SketchConstraintEqualId = 'SketchConstraintEqual'

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Test1:begin split on circle with coincident point and intersection line : smaller part
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchArc_1_1 = Sketch_1.addArc(50, 50, 55, 70, 30, 45, True)
SketchLine_1_1 = Sketch_1.addLine(50, 30, 100, 30)
SketchLine_1_2 = Sketch_1.addLine(60, 50, 100, 30)

SketchConstraintCoincidence_1_1 = Sketch_1.setCoincident(SketchLine_1_1.startPoint(), SketchArc_1_1.results()[1])
SketchConstraintCoincidence_1_2 = Sketch_1.setCoincident(SketchLine_1_1.endPoint(), SketchLine_1_2.endPoint())
GeomPoint_1_1 = geom.Pnt2d(60, 35)

#check number of features before trim
Sketch_1_feature = featureToCompositeFeature(Sketch_1.feature())
idList_before_1 = []
for index in range(Sketch_1_feature.numberOfSubs()):
  idList_before_1.append(Sketch_1_feature.subFeature(index).getKind())
assert(idList_before_1.count(SketchArcId) == 1)
assert(idList_before_1.count(SketchLineId) == 2)
assert(idList_before_1.count(SketchConstraintCoincidenceId) == 2)
assert(idList_before_1.count(SketchConstraintEqualId) == 0)

#perform trim
SketchTrim_1_1 = Sketch_1.addTrim(SketchArc_1_1, GeomPoint_1_1)
SketchTrim_1_1.execute()
model.do()

#check number of features after trim
SketchFeatures = featureToCompositeFeature(Sketch_1.feature())
idList_after_1 = []
for SubIndex in range(SketchFeatures.numberOfSubs()):
    SubFeature = SketchFeatures.subFeature(SubIndex)
    idList_after_1.append(SubFeature.getKind())

assert(idList_after_1.count(SketchArcId) == 2)
assert(idList_after_1.count(SketchLineId) == 2)
aCount = idList_after_1.count(SketchConstraintCoincidenceId)

assert(idList_after_1.count(SketchConstraintCoincidenceId) == 4)
assert(idList_after_1.count(SketchConstraintEqualId) == 1)
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


# Test3: constraints to circle
Sketch_3 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_3_1 = Sketch_3.addCircle(150, 50, 25)
SketchLine_3_1 = Sketch_3.addLine(160, 30, 200, 30)
SketchLine_3_2 = Sketch_3.addLine(160, 50, 200, 30)
SketchLine_3_3 = Sketch_3.addLine(240, 120, 280, 100)
SketchCircle_3_2 = Sketch_3.addCircle(150, 120, 20)
aSketchPoint_3 = Sketch_3.addPoint(145, 70)
SketchLine_3_4 = Sketch_3.addLine(120, 25, 120, 70)

SketchConstraintCoincidence_3_1 = Sketch_3.setCoincident(SketchLine_3_1.startPoint(), SketchCircle_3_1.results()[1])
SketchConstraintCoincidence_3_2 = Sketch_3.setCoincident(SketchLine_3_1.endPoint(), SketchLine_3_2.endPoint())
GeomPoint_3_1 = geom.Pnt2d(165, 40)

#Constraints
aConstraint_3_1 = Sketch_3.setDistance(SketchLine_3_1.endPoint(), SketchCircle_3_1.center(), 50)
aConstraint_3_2 = Sketch_3.setEqual(SketchCircle_3_1.results()[1], SketchCircle_3_2.results()[1])
aConstraint_3_3 = Sketch_3.setRadius(SketchCircle_3_1.results()[1], 25)
aConstraint_3_4 = Sketch_3.setCoincident(SketchCircle_3_1.results()[1], aSketchPoint_3.results()[0])
aConstraint_3_4 = Sketch_3.setTangent(SketchCircle_3_1.results()[1], SketchLine_3_4.results()[0])

MirrorObjects_3 = [SketchCircle_3_1.results()[1], SketchCircle_3_2.results()[1]]
aConstraint_3_5 = Sketch_3.addMirror(SketchLine_3_3.result(), MirrorObjects_3)

#check number of features before trim
Sketch_3_feature = featureToCompositeFeature(Sketch_3.feature())
idList_before_3 = []
for index in range(Sketch_3_feature.numberOfSubs()):
  idList_before_3.append(Sketch_3_feature.subFeature(index).getKind())

assert(idList_before_3.count(SketchCircleId) == 4)
assert(idList_before_3.count(SketchArcId) == 0)
assert(idList_before_3.count(SketchLineId) == 4)
assert(idList_before_3.count(SketchConstraintCoincidenceId) == 3)
assert(idList_before_3.count(SketchConstraintMirrorId) == 1)
assert(idList_before_3.count(SketchConstraintTangentId) == 1)
assert(idList_before_3.count(SketchConstraintEqualId) == 1)

#perform trim
SketchTrim_3_1 = Sketch_3.addTrim(SketchCircle_3_1, GeomPoint_3_1)
SketchTrim_3_1.execute()
model.do()

#check number of features after trim
SketchFeatures = featureToCompositeFeature(Sketch_3.feature())
idList_after_3 = []
for SubIndex in range(SketchFeatures.numberOfSubs()):
    SubFeature = SketchFeatures.subFeature(SubIndex)
    idList_after_3.append(SubFeature.getKind())


assert(idList_after_3.count(SketchCircleId) == 3)
assert(idList_after_3.count(SketchArcId) == 1)
assert(idList_after_3.count(SketchLineId) == 4)
assert(idList_after_3.count(SketchConstraintCoincidenceId) == 3)
assert(idList_after_3.count(SketchConstraintMirrorId) == 0)
assert(idList_after_3.count(SketchConstraintTangentId) == 1)
assert(idList_after_3.count(SketchConstraintEqualId) == 1)
# Test3:end


model.end()

#assert(model.checkPythonDump())
