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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 50, 100, 50)
SketchPoint_1 = Sketch_1.addPoint(25, 50)
SketchPoint_2 = Sketch_1.addPoint(75, 50)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_1.result()[0])
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_1.result()[0])
Sketch_1.addSplit(SketchLine_1, SketchPoint_1.coordinates(), SketchPoint_2.coordinates())
model.do()

Sketch_1_feature = featureToCompositeFeature(Sketch_1.feature())
idList = []
for index in range(Sketch_1_feature.numberOfSubs()):
  idList.append(Sketch_1_feature.subFeature(index).getKind())

assert(idList.count(SketchLineId) == 3)
assert(idList.count(SketchPointId) == 2)
assert(idList.count(SketchConstraintCoincidenceId) == 4)
assert(idList.count(SketchConstraintParallelId) == 2)
# Test end

# Test split on circle with two points
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(50, 50, 50)
SketchPoint_1 = Sketch_1.addPoint(50, 0)
SketchPoint_2 = Sketch_1.addPoint(50, 100)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchCircle_1.result()[0])
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchCircle_1.result()[0])
Sketch_1.addSplit(SketchCircle_1, SketchPoint_1.coordinates(), SketchPoint_2.coordinates())
model.do()

Sketch_1_feature = featureToCompositeFeature(Sketch_1.feature())
idList = []
for index in range(Sketch_1_feature.numberOfSubs()):
  idList.append(Sketch_1_feature.subFeature(index).getKind())

assert(idList.count(SketchArcId) == 2)
assert(idList.count(SketchPointId) == 2)
assert(idList.count(SketchConstraintCoincidenceId) == 2)
assert(idList.count(SketchConstraintTangentId) == 1)
# Test end

# Test split on arc with one point
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchArc_1 = Sketch_1.addArc(50, 50, 0, 50, 100, 50, True)
Sketch_1.setFixed(SketchArc_1.startPoint())
Sketch_1.setFixed(SketchArc_1.endPoint())
Sketch_1.setRadius(SketchArc_1, 50)
SketchPoint_1 = Sketch_1.addPoint(50, 100)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchArc_1.result()[0])
Sketch_1.addSplit(SketchArc_1, SketchPoint_1.coordinates(), SketchArc_1.endPoint())
model.do()

Sketch_1_feature = featureToCompositeFeature(Sketch_1.feature())
idList = []
for index in range(Sketch_1_feature.numberOfSubs()):
  idList.append(Sketch_1_feature.subFeature(index).getKind())

assert(idList.count(SketchArcId) == 2)
assert(idList.count(SketchPointId) == 1)
assert(idList.count(SketchConstraintCoincidenceId) == 2)
assert(idList.count(SketchConstraintEqualId) == 1)
assert(idList.count(SketchConstraintTangentId) == 1)
# Test end

# Test split on arc with two points
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchArc_1 = Sketch_1.addArc(50, 50, 0, 50, 100, 50, True)
Sketch_1.setFixed(SketchArc_1.startPoint())
Sketch_1.setFixed(SketchArc_1.endPoint())
Sketch_1.setRadius(SketchArc_1, 50)
SketchPoint_1 = Sketch_1.addPoint(25, 93.301270189222)
SketchPoint_2 = Sketch_1.addPoint(75, 93.301270189222)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchArc_1.result()[0])
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchArc_1.result()[0])
Sketch_1.addSplit(SketchArc_1, SketchPoint_1.coordinates(), SketchPoint_2.coordinates())
model.do()

Sketch_1_feature = featureToCompositeFeature(Sketch_1.feature())
idList = []
for index in range(Sketch_1_feature.numberOfSubs()):
  idList.append(Sketch_1_feature.subFeature(index).getKind())

assert(idList.count(SketchArcId) == 3)
assert(idList.count(SketchPointId) == 2)
assert(idList.count(SketchConstraintCoincidenceId) == 4)
assert(idList.count(SketchConstraintEqualId) == 2)
assert(idList.count(SketchConstraintTangentId) == 2)
# Test end

model.end()

assert(model.checkPythonDump())
