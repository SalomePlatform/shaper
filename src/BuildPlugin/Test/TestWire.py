# Initialization of the test
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

# Get document
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

# Create a part
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()

# Create a sketch
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
anOrigin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
anOrigin.setValue(0, 0, 0)
aDirX = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
aDirX.setValue(1, 0, 0)
aNorm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
aNorm.setValue(0, 0, 1)

# Create lines
aSketchLineFeature = aSketchFeature.addFeature("SketchLine")
aSketchLineStartPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("StartPoint"))
aSketchLineEndPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("EndPoint"))
aSketchLineStartPoint.setValue(0, 0)
aSketchLineEndPoint.setValue(0, 50)
aSketchLineFeature = aSketchFeature.addFeature("SketchLine")
aSketchLineStartPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("StartPoint"))
aSketchLineEndPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("EndPoint"))
aSketchLineStartPoint.setValue(0, 50)
aSketchLineEndPoint.setValue(50, 50)
aSketchLineFeature = aSketchFeature.addFeature("SketchLine")
aSketchLineStartPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("StartPoint"))
aSketchLineEndPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("EndPoint"))
aSketchLineStartPoint.setValue(50, 50)
aSketchLineEndPoint.setValue(50, 0)
aSketchLineFeature = aSketchFeature.addFeature("SketchLine")
aSketchLineStartPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("StartPoint"))
aSketchLineEndPoint = geomDataAPI_Point2D(aSketchLineFeature.attribute("EndPoint"))
aSketchLineStartPoint.setValue(50, 0)
aSketchLineEndPoint.setValue(0, 0)
aSession.finishOperation()
aSketchResult = aSketchFeature.firstResult()
aSketchShape = aSketchResult.shape()

# Create wire
aSession.startOperation()
aWireFeature = aPart.addFeature("Wire")
aBaseObjectsList = aWireFeature.selectionList("base_objects")
aShapeExplorer = GeomAPI_ShapeExplorer(aSketchShape, GeomAPI_Shape.EDGE)
if aShapeExplorer.more():
    aBaseObjectsList.append(aSketchResult, aShapeExplorer.current())
aWireFeature.customAction("add_contour")
aSession.finishOperation()

# Test results
assert (len(aWireFeature.results()) > 0)

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-68.61063464837, 232.418524871355, -488.85077186964, 232.418524871355)
SketchLine_2 = Sketch_1.addLine(-488.85077186964, 232.418524871355, -488.85077186964, -153.516295025729)
SketchLine_3 = Sketch_1.addLine(-488.85077186964, -153.516295025729, -68.61063464837, -153.516295025729)
SketchLine_4 = Sketch_1.addLine(-68.61063464837, -153.516295025729, -68.61063464837, 232.418524871355)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/Edge-SketchLine_1")])
Wire_1.addContour()
model.end()

assert(model.checkPythonDump())
