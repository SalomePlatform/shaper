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
# Make extrusion sketch
#=========================================================================
aSession.startOperation()
anExtrusionSketchFt = featureToCompositeFeature(aPart.addFeature("ExtrusionSketch"))
assert (anExtrusionSketchFt.getKind() == "ExtrusionSketch")
# selection type FACE=4
aSession.startOperation()
aCircleSketchFeature = featureToCompositeFeature(anExtrusionSketchFt.addFeature("Sketch"))
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
anExtrusionSketchFt.string("CreationMethod").setValue("BySizes")
anExtrusionSketchFt.real("to_size").setValue(10)
anExtrusionSketchFt.real("from_size").setValue(10)
anExtrusionSketchFt.real("to_offset").setValue(0) #TODO: remove
anExtrusionSketchFt.real("from_offset").setValue(0) #TODO: remove
anExtrusionSketchFt.execute()
aSession.finishOperation()

#=========================================================================
# Test results
#=========================================================================
aFactory = ModelAPI_Session.get().validators()
assert (aFactory.validate(anExtrusionSketchFt))
assert (len(anExtrusionSketchFt.results()) > 0)
aCurrentResult = modelAPI_ResultBody(anExtrusionSketchFt.firstResult())
assert (aCurrentResult is not None)

#=========================================================================
# Test extrusion between bounding planes
#=========================================================================
# Create from plane
aSession.startOperation()
aSketchPlaneFeature = aPart.addFeature("Plane")
aSketchPlaneFeature.string("CreationMethod").setValue("PlaneByGeneralEquation")
aSketchPlaneFeature.real("A").setValue(0.)
aSketchPlaneFeature.real("B").setValue(0.)
aSketchPlaneFeature.real("C").setValue(1.)
aSketchPlaneFeature.real("D").setValue(30.)
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
aToPlaneFeature.real("D").setValue(-30.)
aSession.finishOperation()
aToResult = aToPlaneFeature.firstResult()
aToShape = modelAPI_ResultConstruction(aToResult).shape()

#=========================================================================
# Make extrusion sketch
#=========================================================================
aSession.startOperation()
anExtrusionSketchFt = featureToCompositeFeature(aPart.addFeature("ExtrusionSketch"))
assert (anExtrusionSketchFt.getKind() == "ExtrusionSketch")
aSession.startOperation()
aCircleSketchFeature = featureToCompositeFeature(anExtrusionSketchFt.addFeature("Sketch"))
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
anExtrusionSketchFt.string("CreationMethod").setValue("ByPlanesAndOffsets")
anExtrusionSketchFt.real("to_size").setValue(0) #TODO: remove
anExtrusionSketchFt.real("from_size").setValue(0) #TODO: remove
anExtrusionSketchFt.selection("to_object").setValue(aToResult, aToShape)
anExtrusionSketchFt.real("to_offset").setValue(0)
anExtrusionSketchFt.selection("from_object").setValue(aFromResult, aFromShape)
anExtrusionSketchFt.real("from_offset").setValue(0)
anExtrusionSketchFt.execute()
aSession.finishOperation()

#=========================================================================
# Test results
#=========================================================================
aFactory = ModelAPI_Session.get().validators()
assert (aFactory.validate(anExtrusionSketchFt))
assert (len(anExtrusionSketchFt.results()) > 0)
aCurrentResult = modelAPI_ResultBody(anExtrusionSketchFt.firstResult())
assert (aCurrentResult is not None)
