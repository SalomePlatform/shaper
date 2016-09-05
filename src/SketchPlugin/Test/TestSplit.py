import model

from ModelAPI import *

SketchPointId = 'SketchPoint'
SketchLineId = 'SketchLine'
SketchArcId = 'SketchArc'
SketchConstraintCoincidenceId = 'SketchConstraintCoincidence'
SketchConstraintParallelId = 'SketchConstraintParallel'
SketchConstraintTangentId = 'SketchConstraintTangent'
SketchConstraintEqualId = 'SketchConstraintEqual'

# Test split on line with one point
model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 50, 100, 50)
SketchPoint_1 = Sketch_1.addPoint(50, 50)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_1.result()[0])
Sketch_1.addSplit(SketchLine_1, SketchPoint_1.coordinates(), SketchLine_1.endPoint())
model.do()

Sketch_1_feature = featureToCompositeFeature(Sketch_1.feature())
idList = []
for index in range(Sketch_1_feature.numberOfSubs()):
  idList.append(Sketch_1_feature.subFeature(index).getKind())

assert(idList.count(SketchLineId) == 2)
assert(idList.count(SketchPointId) == 1)
assert(idList.count(SketchConstraintCoincidenceId) == 2)
assert(idList.count(SketchConstraintParallelId) == 1)
# Test end

# Test split on line with two points
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_2_1 = Sketch_2.addLine(0, 50, 100, 50)
SketchPoint_2_1 = Sketch_2.addPoint(25, 50)
SketchPoint_2_2 = Sketch_2.addPoint(75, 50)
SketchConstraintCoincidence_2_1 = Sketch_2.setCoincident(SketchPoint_2_1.coordinates(), SketchLine_2_1.result()[0])
SketchConstraintCoincidence_2_2 = Sketch_2.setCoincident(SketchPoint_2_2.coordinates(), SketchLine_2_1.result()[0])
Sketch_2.addSplit(SketchLine_2_1, SketchPoint_2_1.coordinates(), SketchPoint_2_2.coordinates())
model.do()

Sketch_2_feature = featureToCompositeFeature(Sketch_2.feature())
idList = []
for index in range(Sketch_2_feature.numberOfSubs()):
  idList.append(Sketch_2_feature.subFeature(index).getKind())

assert(idList.count(SketchLineId) == 3)
assert(idList.count(SketchPointId) == 2)
assert(idList.count(SketchConstraintCoincidenceId) == 4)
assert(idList.count(SketchConstraintParallelId) == 2)
# Test end

# Test split on circle with two points
Sketch_3 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_3_1 = Sketch_3.addCircle(50, 50, 50)
SketchPoint_3_1 = Sketch_3.addPoint(50, 0)
SketchPoint_3_2 = Sketch_3.addPoint(50, 100)
SketchConstraintCoincidence_3_1 = Sketch_3.setCoincident(SketchPoint_3_1.coordinates(), SketchCircle_3_1.result()[1])
SketchConstraintCoincidence_3_2 = Sketch_3.setCoincident(SketchPoint_3_2.coordinates(), SketchCircle_3_1.result()[1])
Sketch_3.addSplit(SketchCircle_3_1, SketchPoint_3_1.coordinates(), SketchPoint_3_2.coordinates())
model.do()

Sketch_3_feature = featureToCompositeFeature(Sketch_3.feature())
idList = []
for index in range(Sketch_3_feature.numberOfSubs()):
  idList.append(Sketch_3_feature.subFeature(index).getKind())

assert(idList.count(SketchArcId) == 2)
assert(idList.count(SketchPointId) == 2)
assert(idList.count(SketchConstraintCoincidenceId) == 2)
assert(idList.count(SketchConstraintTangentId) == 1)
# Test end

# Test split on arc with one point
Sketch_4 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchArc_4_1 = Sketch_4.addArc(50, 50, 50, 0, 0, 50, False)
Sketch_4.setFixed(SketchArc_4_1.startPoint())
Sketch_4.setFixed(SketchArc_4_1.endPoint())
Sketch_4.setRadius(SketchArc_4_1, 50)
SketchPoint_4_1 = Sketch_4.addPoint(50, 100)
SketchConstraintCoincidence_4_1 = Sketch_4.setCoincident(SketchPoint_4_1.coordinates(), SketchArc_4_1.result()[1])
Sketch_4.addSplit(SketchArc_4_1, SketchPoint_4_1.coordinates(), SketchArc_4_1.endPoint())
model.do()

Sketch_4_feature = featureToCompositeFeature(Sketch_4.feature())
idList = []
for index in range(Sketch_4_feature.numberOfSubs()):
  idList.append(Sketch_4_feature.subFeature(index).getKind())

assert(idList.count(SketchArcId) == 2)
assert(idList.count(SketchPointId) == 1)
assert(idList.count(SketchConstraintCoincidenceId) == 2)
assert(idList.count(SketchConstraintEqualId) == 1)
assert(idList.count(SketchConstraintTangentId) == 1)
# Test end

# Test split on arc with two points
Sketch_5 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
#SketchArc_5_1 = Sketch_5.addArc(50, 50, 6.69872981078, 75, 93.30127018922, 75, True)
SketchArc_5_1 = Sketch_5.addArc(50, 50, 0, 50, 93.30127018922, 75, True)
Sketch_5.setFixed(SketchArc_5_1.startPoint())
Sketch_5.setFixed(SketchArc_5_1.endPoint())
Sketch_5.setRadius(SketchArc_5_1, 50)
SketchLine_5_1 = Sketch_5.addLine(25, 93.301270189222, 75, 93.301270189222)
SketchConstraintCoincidence_5_1 = Sketch_5.setCoincident(SketchLine_5_1.startPoint(), SketchArc_5_1.result()[1])
SketchConstraintCoincidence_5_2 = Sketch_5.setCoincident(SketchLine_5_1.endPoint(), SketchArc_5_1.result()[1])
SketchConstraintHorizontal_5_1 = Sketch_5.setHorizontal(SketchLine_5_1.result()[0])
SketchConstraintLength_5_1 = Sketch_5.setLength(SketchLine_5_1.result()[0], 50)
Sketch_5.addSplit(SketchArc_5_1, SketchLine_5_1.startPoint(), SketchLine_5_1.endPoint())
model.do()

Sketch_5_feature = featureToCompositeFeature(Sketch_5.feature())
idList = []
for index in range(Sketch_5_feature.numberOfSubs()):
  idList.append(Sketch_5_feature.subFeature(index).getKind())

assert(idList.count(SketchArcId) == 3)
assert(idList.count(SketchPointId) == 0)
assert(idList.count(SketchConstraintCoincidenceId) == 4)
assert(idList.count(SketchConstraintEqualId) == 2)
assert(idList.count(SketchConstraintTangentId) == 2)
# Test end

model.end()

#assert(model.checkPythonDump())
