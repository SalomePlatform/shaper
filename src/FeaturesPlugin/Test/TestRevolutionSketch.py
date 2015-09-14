#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *
import math

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
assert (len(aPartFeature.results()) == 1)
aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()

#=========================================================================
# Create a sketch line to revol
#=========================================================================
aSession.startOperation()
aLineSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aLineSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aLineSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aLineSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSketchLine = aLineSketchFeature.addFeature("SketchLine")
aLineStartPoint = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
aLineEndPoint = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
aLineStartPoint.setValue(-20, -20)
aLineEndPoint.setValue(20, -20)
aSession.finishOperation()

# Build shape from sketcher results
aLineSketchResult = aLineSketchFeature.firstResult()
aLineSketchShape = modelAPI_ResultConstruction(aLineSketchResult).shape()
aShapeExplorer = GeomAPI_ShapeExplorer(aLineSketchShape, GeomAPI_Shape.EDGE)
aLineEdge = aShapeExplorer.current()

#=========================================================================
# Make revolution sketch
#=========================================================================
aSession.startOperation()
anRevolutionSketchFt = featureToCompositeFeature(aPart.addFeature("RevolutionSketch"))
assert (anRevolutionSketchFt.getKind() == "RevolutionSketch")
# selection type FACE=4
aSession.startOperation()
aCircleSketchFeature = featureToCompositeFeature(anRevolutionSketchFt.addFeature("Sketch"))
origin = geomDataAPI_Point(aCircleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aCircleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aCircleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSketchCircle = aCircleSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
aCircleRadius = aSketchCircle.real("CircleRadius")
anCircleCentr.setValue(0, 0)
aCircleRadius.setValue(10)
aSession.finishOperation()
aSession.startOperation()
anRevolutionSketchFt.selection("axis_object").setValue(aLineSketchResult, aLineEdge)
anRevolutionSketchFt.string("CreationMethod").setValue("ByAngles")
anRevolutionSketchFt.real("to_angle").setValue(50)
anRevolutionSketchFt.real("from_angle").setValue(50)
anRevolutionSketchFt.real("to_offset").setValue(0) #TODO: remove
anRevolutionSketchFt.real("from_offset").setValue(0) #TODO: remove
anRevolutionSketchFt.execute()
aSession.finishOperation()
aSession.finishOperation()

#=========================================================================
# Test results
#=========================================================================
aFactory = ModelAPI_Session.get().validators()
assert (aFactory.validate(anRevolutionSketchFt))
assert (len(anRevolutionSketchFt.results()) > 0)
aCurrentResult = modelAPI_ResultBody(anRevolutionSketchFt.firstResult())
assert (aCurrentResult is not None)

#=========================================================================
# Test revolution between bounding planes
#=========================================================================
# Create from plane
aSession.startOperation()
aSketchPlaneFeature = aPart.addFeature("Plane")
aSketchPlaneFeature.string("CreationMethod").setValue("PlaneByGeneralEquation")
aSketchPlaneFeature.real("A").setValue(0.)
aSketchPlaneFeature.real("B").setValue(0.)
aSketchPlaneFeature.real("C").setValue(1.)
aSketchPlaneFeature.real("D").setValue(15.)
aSession.finishOperation()
aFromResult = aSketchPlaneFeature.firstResult()
aFromShape = modelAPI_ResultConstruction(aFromResult).shape()

# Create to plane
aSession.startOperation()
aToPlaneFeature = aPart.addFeature("Plane")
aToPlaneFeature.string("CreationMethod").setValue("PlaneByGeneralEquation")
aToPlaneFeature.real("A").setValue(0.)
aToPlaneFeature.real("B").setValue(0.)
aToPlaneFeature.real("C").setValue(1.)
aToPlaneFeature.real("D").setValue(-15.)
aSession.finishOperation()
aToResult = aToPlaneFeature.firstResult()
aToShape = modelAPI_ResultConstruction(aToResult).shape()

#=========================================================================
# Make revolution sketch
#=========================================================================
aSession.startOperation()
anRevolutionSketchFt = featureToCompositeFeature(aPart.addFeature("RevolutionSketch"))
assert (anRevolutionSketchFt.getKind() == "RevolutionSketch")
# selection type FACE=4
aSession.startOperation()
aCircleSketchFeature = featureToCompositeFeature(anRevolutionSketchFt.addFeature("Sketch"))
origin = geomDataAPI_Point(aCircleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aCircleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aCircleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSketchCircle = aCircleSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
aCircleRadius = aSketchCircle.real("CircleRadius")
anCircleCentr.setValue(0, 0)
aCircleRadius.setValue(10)
aSession.finishOperation()
aSession.startOperation()
anRevolutionSketchFt.selection("axis_object").setValue(aLineSketchResult, aLineEdge)
anRevolutionSketchFt.string("CreationMethod").setValue("ByPlanesAndOffsets")
anRevolutionSketchFt.real("to_angle").setValue(0) #TODO: remove
anRevolutionSketchFt.real("from_angle").setValue(0) #TODO: remove
anRevolutionSketchFt.selection("to_object").setValue(aToResult, aToShape)
anRevolutionSketchFt.real("to_offset").setValue(0)
anRevolutionSketchFt.selection("from_object").setValue(aFromResult, aFromShape)
anRevolutionSketchFt.real("from_offset").setValue(0)
anRevolutionSketchFt.execute()
aSession.finishOperation()
aSession.finishOperation()

#=========================================================================
# Test results
#=========================================================================
aFactory = ModelAPI_Session.get().validators()
assert (aFactory.validate(anRevolutionSketchFt))
assert (len(anRevolutionSketchFt.results()) > 0)
aCurrentResult = modelAPI_ResultBody(anRevolutionSketchFt.firstResult())
assert (aCurrentResult is not None)
