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
#Sketch_feature = featureToCompositeFeature(Sketch.feature())
#idList_before = []
#for index in range(Sketch_feature.numberOfSubs()):
#  idList_before.append(Sketch_feature.subFeature(index).getKind())

#assert(idList_before.count(SketchLineId) == 3)
#assert(idList_before.count(SketchConstraintCoincidenceId) == 2)

#perform trim
SketchTrim = Sketch.addTrim(SketchLine_intersecting, GeomPoint)
SketchTrim.execute()
model.do()

#check number of features after trim
#SketchFeatures = featureToCompositeFeature(Sketch.feature())
#idList_after = []
#for SubIndex in range(SketchFeatures.numberOfSubs()):
#    SubFeature = SketchFeatures.subFeature(SubIndex)
#    idList_after.append(SubFeature.getKind())

#assert(idList_after.count(SketchLineId) == 3)
#assert(idList_after.count(SketchConstraintCoincidenceId) == 2)

#check if line points are the same
#assert(SketchLine_1.startPoint().x() == 200)
#assert(SketchLine_1.startPoint().y() == 20)
#assert(SketchLine_1.endPoint().x() == 20)
#assert(SketchLine_1.endPoint().y() == 70)

#assert(SketchLine_3.startPoint().x() == -10)
#assert(SketchLine_3.startPoint().y() == 190)
#assert(SketchLine_3.endPoint().x() == 90)
#assert(SketchLine_3.endPoint().y() == 50)

model.end()

#assert(model.checkPythonDump())
