#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

__updated__ = "2014-12-16"

aSession = ModelAPI_Session.get()
# Create a part for extrusions & boolean
aSession.startOperation()
aPartFeature = aSession.moduleDocument().addFeature("Part")
aSession.finishOperation()
aPart = aSession.activeDocument()

#=========================================================================
# Create a sketch with circle to extrude
#=========================================================================
aSession.startOperation()
aCircleSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aCircleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aCircleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aCircleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)

aSketchCircle = aCircleSketchFeature.addFeature("SketchCircle")
aCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
aCircleRadius = aSketchCircle.real("CircleRadius")
aCircleCentr.setValue(0, 0)
aCircleRadius.setValue(50)

aSketchLine = aCircleSketchFeature.addFeature("SketchLine")
aLineStartPoint = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
aLineEndPoint = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
aLineStartPoint.setValue(0, -50)
aLineEndPoint.setValue(0, 50)
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
    aCircleSketchResult, None)
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
# Test extrusion cut between bounding planes
#=========================================================================
# Create from plane
aSession.startOperation()
aFromPlaneFeature = aPart.addFeature("Plane")
aFromPlaneFeature.string("CreationMethod").setValue("PlaneByGeneralEquation")
aFromPlaneFeature.real("A").setValue(0.)
aFromPlaneFeature.real("B").setValue(0.)
aFromPlaneFeature.real("C").setValue(1.)
aFromPlaneFeature.real("D").setValue(-25.)
aSession.finishOperation()

# Create to plane
aSession.startOperation()
aToPlaneFeature = aPart.addFeature("Plane")
aToPlaneFeature.string("CreationMethod").setValue("PlaneByGeneralEquation")
aToPlaneFeature.real("A").setValue(0.)
aToPlaneFeature.real("B").setValue(0.)
aToPlaneFeature.real("C").setValue(1.)
aToPlaneFeature.real("D").setValue(-60.)
aSession.finishOperation()

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
aCircleSketchFeature.selection("External").selectSubShape("face", "Extrusion_1/TopFace_1")
aSketchCircle = aCircleSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
aCircleRadius = aSketchCircle.real("CircleRadius")
anCircleCentr.setValue(0, 0)
aCircleRadius.setValue(10)
aSession.finishOperation()
aSession.startOperation()
anExtrusionCutFt.string("CreationMethod").setValue("ByPlanesAndOffsets")
anExtrusionCutFt.real("to_size").setValue(0)
anExtrusionCutFt.real("from_size").setValue(0)
aToResult = aToPlaneFeature.firstResult()
aToShape = modelAPI_ResultConstruction(aToResult).shape()
anExtrusionCutFt.selection("to_object").setValue(aToResult, aToShape)
anExtrusionCutFt.real("to_offset").setValue(0)
aFromResult = aFromPlaneFeature.firstResult()
aFromShape = modelAPI_ResultConstruction(aFromResult).shape()
anExtrusionCutFt.selection("from_object").setValue(aFromResult, aFromShape)
anExtrusionCutFt.real("from_offset").setValue(0)
anExtrusionCutFt.selectionList("boolean_objects").append(modelAPI_ResultCompSolid(anExtrusionResult).subResult(1), None)
anExtrusionCutFt.execute()
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
aSession.undo()

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
aLineStartPoint.setValue(-50., 50.)
aLineEndPoint.setValue(-50., -50.)
aSession.finishOperation()

# Build shape from sketcher results
aLineSketchResult = aLineSketchFeature.firstResult()
aLineSketchShape = modelAPI_ResultConstruction(aLineSketchResult).shape()
aShapeExplorer = GeomAPI_ShapeExplorer(aLineSketchShape, GeomAPI_Shape.EDGE)
aLineEdge = aShapeExplorer.current()

#=========================================================================
# Make revolution fuse
#=========================================================================
aSession.startOperation()
anRevolutionFuseFt = featureToCompositeFeature(aPart.addFeature("RevolutionFuse"))
assert (anRevolutionFuseFt.getKind() == "RevolutionFuse")
# selection type FACE=4
aSession.startOperation()
aCircleSketchFeature = featureToCompositeFeature(anRevolutionFuseFt.addFeature("Sketch"))
origin = geomDataAPI_Point(aCircleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 50)
dirx = geomDataAPI_Dir(aCircleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aCircleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aCircleSketchFeature.selection("External").selectSubShape("face", "Extrusion_1/TopFace_1")
aSketchCircle = aCircleSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
aCircleRadius = aSketchCircle.real("CircleRadius")
anCircleCentr.setValue(0, 0)
aCircleRadius.setValue(10)
aSession.finishOperation()
aSession.startOperation()
anRevolutionFuseFt.selection("axis_object").setValue(aLineSketchResult, aLineEdge)
anRevolutionFuseFt.string("CreationMethod").setValue("ByPlanesAndOffsets")
anRevolutionFuseFt.real("from_angle").setValue(0) #TODO: remove
anRevolutionFuseFt.real("to_angle").setValue(0) #TODO: remove
anRevolutionFuseFt.selection("to_object").setValue(aToResult, None)
anRevolutionFuseFt.real("to_offset").setValue(0)
anRevolutionFuseFt.selection("from_object").setValue(None, None)
anRevolutionFuseFt.real("from_offset").setValue(0)
anRevolutionFuseFt.selectionList("boolean_objects").append(modelAPI_ResultCompSolid(anExtrusionResult).subResult(1), None)
anRevolutionFuseFt.execute()
aSession.finishOperation()
aSession.finishOperation()

#=========================================================================
# Test results
#=========================================================================
aFactory = ModelAPI_Session.get().validators()
assert (aFactory.validate(anRevolutionFuseFt))
assert (len(anRevolutionFuseFt.results()) > 0)
aCurrentResult = modelAPI_ResultBody(anRevolutionFuseFt.firstResult())
assert (aCurrentResult is not None)

