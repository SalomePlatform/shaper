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
# Create a sketch line to revol
#=========================================================================
aSession.startOperation()
aLineSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aLineSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 50)
dirx = geomDataAPI_Dir(aLineSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aLineSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aLineSketchFeature.selection("External").selectSubShape("face", "Extrusion_1/TopFace_1")

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
# Make revolution cut
#=========================================================================
aSession.startOperation()
anRevolutionCutFt = featureToCompositeFeature(aPart.addFeature("RevolutionCut"))
assert (anRevolutionCutFt.getKind() == "RevolutionCut")
# selection type FACE=4
aSession.startOperation()
aCircleSketchFeature = featureToCompositeFeature(anRevolutionCutFt.addFeature("Sketch"))
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
anRevolutionCutFt.selectionList("base").append(aCircleSketchFeature.firstResult(), None)
anRevolutionCutFt.selection("axis_object").setValue(aLineSketchResult, aLineEdge)
anRevolutionCutFt.string("CreationMethod").setValue("ByAngles")
anRevolutionCutFt.real("to_angle").setValue(50)
anRevolutionCutFt.real("from_angle").setValue(50)
anRevolutionCutFt.real("to_offset").setValue(0) #TODO: remove
anRevolutionCutFt.real("from_offset").setValue(0) #TODO: remove
anRevolutionCutFt.selectionList("main_objects").append(anExtrusionResult, anExtrusionResult.shape())
aSession.finishOperation()
aSession.finishOperation()

#=========================================================================
# Test results
#=========================================================================
aFactory = ModelAPI_Session.get().validators()
assert (aFactory.validate(anRevolutionCutFt))
assert (len(anRevolutionCutFt.results()) > 0)
aCurrentResult = modelAPI_ResultBody(anRevolutionCutFt.firstResult())
assert (aCurrentResult is not None)
