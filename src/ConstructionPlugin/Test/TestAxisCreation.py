#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *
import math
from ModelAPI import *

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

# Create a part
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
assert (len(aPartFeature.results()) == 1)
aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()

#=========================================================================
# Create a first point
#=========================================================================
aSession.startOperation()
aPointFeature = aPart.addFeature("Point")
aPointFeatureData = aPointFeature.data()
assert(aPointFeatureData is not None)
aPointFeatureData.real("x").setValue(0.)
aPointFeatureData.real("y").setValue(0.)
aPointFeatureData.real("z").setValue(0.)
aPointFeature.execute()
aSession.finishOperation()
aPoint1Result = aPointFeature.firstResult();

#=========================================================================
# Create a second point
#=========================================================================
aSession.startOperation()
aPointFeature = aPart.addFeature("Point")
aPointFeatureData = aPointFeature.data()
assert(aPointFeatureData is not None)
aPointFeatureData.real("x").setValue(0.)
aPointFeatureData.real("y").setValue(0.)
aPointFeatureData.real("z").setValue(100.)
aPointFeature.execute()
aSession.finishOperation()
aPoint2Result = aPointFeature.firstResult();

#=========================================================================
# Create axis by two points
#=========================================================================
aSession.startOperation()
anAxisFeature = aPart.addFeature("Axis")
anAxisFeatureData = anAxisFeature.data()
assert(anAxisFeatureData is not None)
anAxisFeatureData.string("CreationMethod").setValue("AxisByPointsCase")
anAxisFeatureData.selection("FirstPoint").setValue(aPoint1Result, aPoint1Result.shape())
anAxisFeatureData.selection("SecondPoint").setValue(aPoint2Result, aPoint2Result.shape())
anAxisFeature.execute()
aSession.finishOperation()

assert (len(anAxisFeature.results()) > 0)
anAxisResult = modelAPI_ResultConstruction(anAxisFeature.firstResult())
assert (anAxisResult is not None)

#=========================================================================
# Create a sketch circle to extrude
#=========================================================================
aSession.startOperation()
aCircleSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aCircleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aCircleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aCircleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
# Create circle
aSketchCircle = aCircleSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
aCircleRadius = aSketchCircle.real("CircleRadius")
anCircleCentr.setValue(0, 0)
aCircleRadius.setValue(50)
aSession.finishOperation()

#=========================================================================
# Make extrusion on circle
#=========================================================================
# Build shape from sketcher results
aCircleSketchResult = aCircleSketchFeature.firstResult()
aCircleSketchEdges = modelAPI_ResultConstruction(aCircleSketchResult).shape()
origin = geomDataAPI_Point(aCircleSketchFeature.attribute("Origin")).pnt()
dirX = geomDataAPI_Dir(aCircleSketchFeature.attribute("DirX")).dir()
norm = geomDataAPI_Dir(aCircleSketchFeature.attribute("Norm")).dir()
aCircleSketchFaces = ShapeList()
GeomAlgoAPI_SketchBuilder.createFaces(origin, dirX, norm, aCircleSketchEdges, aCircleSketchFaces)
assert (len(aCircleSketchFaces) > 0)
assert (aCircleSketchFaces[0] is not None)

# Create extrusion
aSession.startOperation()
anExtrusionFt = aPart.addFeature("Extrusion")
assert (anExtrusionFt.getKind() == "Extrusion")
# selection type FACE=4
anExtrusionFt.selectionList("base").append(aCircleSketchResult, aCircleSketchFaces[0])
anExtrusionFt.string("CreationMethod").setValue("BySizes")
anExtrusionFt.real("to_size").setValue(100)
anExtrusionFt.real("from_size").setValue(0)
anExtrusionFt.real("to_offset").setValue(0) #TODO: remove
anExtrusionFt.real("from_offset").setValue(0) #TODO: remove
anExtrusionFt.execute()
aSession.finishOperation()
assert (anExtrusionFt.real("to_size").value() == 100.0)

# Check extrusion results
assert (len(anExtrusionFt.results()) > 0)
anExtrusionResult = modelAPI_ResultBody(anExtrusionFt.firstResult())
assert (anExtrusionResult is not None)

#=========================================================================
# Create axis by cylinder
#=========================================================================
aSession.startOperation()
anAxisFeature = aPart.addFeature("Axis")
anAxisFeatureData = anAxisFeature.data()
assert(anAxisFeatureData is not None)
anAxisFeatureData.string("CreationMethod").setValue("AxisByCylindricalFaceCase")
anAxisFeatureData.selection("CylindricalFace").selectSubShape("face", "Extrusion_1_1/Generated_Face_1")
anAxisFeature.execute()
aSession.finishOperation()

assert (len(anAxisFeature.results()) > 0)
anAxisResult = modelAPI_ResultConstruction(anAxisFeature.firstResult())
assert (anAxisResult is not None)
