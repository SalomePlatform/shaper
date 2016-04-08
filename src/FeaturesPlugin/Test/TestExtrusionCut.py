#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *
import math

__updated__ = "2014-12-16"

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
# Create a part for extrusion
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
assert (len(aPartFeature.results()) == 1)
# Another way is:
# aPart = aSession.activeDocument()
aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()
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
aCircleRadius.setValue(20)
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
GeomAlgoAPI_SketchBuilder.createFaces(
    origin, dirX, norm, aCircleSketchEdges, aCircleSketchFaces)
assert (len(aCircleSketchFaces) > 0)
assert (aCircleSketchFaces[0] is not None)
# Create extrusion
aSession.startOperation()
anExtrusionFt = aPart.addFeature("Extrusion")
assert (anExtrusionFt.getKind() == "Extrusion")
# selection type FACE=4
anExtrusionFt.selectionList("base").append(
    aCircleSketchResult, aCircleSketchFaces[0])
anExtrusionFt.string("CreationMethod").setValue("BySizes")
anExtrusionFt.real("to_size").setValue(50)
anExtrusionFt.real("from_size").setValue(0)
anExtrusionFt.real("to_offset").setValue(0) #TODO: remove
anExtrusionFt.real("from_offset").setValue(0) #TODO: remove
anExtrusionFt.execute()
aSession.finishOperation()
assert (anExtrusionFt.real("to_size").value() == 50.0)

# Check extrusion results
assert (len(anExtrusionFt.results()) > 0)
anExtrusionResult = modelAPI_ResultBody(anExtrusionFt.firstResult())
assert (anExtrusionResult is not None)

#=========================================================================
# Make extrusion cut
#=========================================================================
aSession.startOperation()
anExtrusionCutFt = featureToCompositeFeature(aPart.addFeature("ExtrusionCut"))
assert (anExtrusionCutFt.getKind() == "ExtrusionCut")
# selection type FACE=4
aSession.startOperation()
aCircleSketchFeature = featureToCompositeFeature(anExtrusionCutFt.addFeature("Sketch"))
origin = geomDataAPI_Point(aCircleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 50)
dirx = geomDataAPI_Dir(aCircleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aCircleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aCircleSketchFeature.selection("External").selectSubShape("face", "Extrusion_1/To_Face_1")
aSession.startOperation()
aSketchCircle = aCircleSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
aCircleRadius = aSketchCircle.real("CircleRadius")
anCircleCentr.setValue(0, 0)
aCircleRadius.setValue(10)
aSession.finishOperation()
aSession.finishOperation()
aSession.startOperation()
aCircleSketchFeature.execute() # execute for sketch should be called here, because it is not set as current feature, so it is disabled.
anExtrusionCutFt.selectionList("base").append(aCircleSketchFeature.firstResult(), None)
anExtrusionCutFt.string("CreationMethod").setValue("BySizes")
anExtrusionCutFt.real("to_size").setValue(10)
anExtrusionCutFt.real("from_size").setValue(10)
anExtrusionCutFt.real("to_offset").setValue(0) #TODO: remove
anExtrusionCutFt.real("from_offset").setValue(0) #TODO: remove
anExtrusionCutFt.selectionList("main_objects").append(anExtrusionResult, anExtrusionResult.shape())
aSession.finishOperation()
aSession.finishOperation()

#=========================================================================
# Test results
#=========================================================================
aFactory = ModelAPI_Session.get().validators()
assert (aFactory.validate(anExtrusionCutFt))
assert (len(anExtrusionCutFt.results()) > 0)
aCurrentResult = modelAPI_ResultBody(anExtrusionCutFt.firstResult())
assert (aCurrentResult is not None)
