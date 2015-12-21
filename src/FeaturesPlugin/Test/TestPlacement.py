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

aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
assert (len(aPartFeature.results()) == 1)
aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()

#=========================================================================
# Create a sketch to extrude
#=========================================================================
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aLineA1 = aSketchFeature.addFeature("SketchLine")
aLineB1 = aSketchFeature.addFeature("SketchLine")
aLineC1 = aSketchFeature.addFeature("SketchLine")
aLineD1 = aSketchFeature.addFeature("SketchLine")
aLineA1StartPoint = geomDataAPI_Point2D(aLineA1.attribute("StartPoint"))
aLineA1EndPoint = geomDataAPI_Point2D(aLineA1.attribute("EndPoint"))
aLineB1StartPoint = geomDataAPI_Point2D(aLineB1.attribute("StartPoint"))
aLineB1EndPoint = geomDataAPI_Point2D(aLineB1.attribute("EndPoint"))
aLineC1StartPoint = geomDataAPI_Point2D(aLineC1.attribute("StartPoint"))
aLineC1EndPoint = geomDataAPI_Point2D(aLineC1.attribute("EndPoint"))
aLineD1StartPoint = geomDataAPI_Point2D(aLineD1.attribute("StartPoint"))
aLineD1EndPoint = geomDataAPI_Point2D(aLineD1.attribute("EndPoint"))
aLineA1StartPoint.setValue(-100, 0)
aLineA1EndPoint.setValue(-50, 0)
aLineB1StartPoint.setValue(-50, 0)
aLineB1EndPoint.setValue(-50, 50)
aLineC1StartPoint.setValue(-50, 50)
aLineC1EndPoint.setValue(-100, 50)
aLineD1StartPoint.setValue(-100, 50)
aLineD1EndPoint.setValue(-100, 0)
aLineA2 = aSketchFeature.addFeature("SketchLine")
aLineB2 = aSketchFeature.addFeature("SketchLine")
aLineC2 = aSketchFeature.addFeature("SketchLine")
aLineD2 = aSketchFeature.addFeature("SketchLine")
aLineA2StartPoint = geomDataAPI_Point2D(aLineA2.attribute("StartPoint"))
aLineA2EndPoint = geomDataAPI_Point2D(aLineA2.attribute("EndPoint"))
aLineB2StartPoint = geomDataAPI_Point2D(aLineB2.attribute("StartPoint"))
aLineB2EndPoint = geomDataAPI_Point2D(aLineB2.attribute("EndPoint"))
aLineC2StartPoint = geomDataAPI_Point2D(aLineC2.attribute("StartPoint"))
aLineC2EndPoint = geomDataAPI_Point2D(aLineC2.attribute("EndPoint"))
aLineD2StartPoint = geomDataAPI_Point2D(aLineD2.attribute("StartPoint"))
aLineD2EndPoint = geomDataAPI_Point2D(aLineD2.attribute("EndPoint"))
aLineA2StartPoint.setValue(100, 0)
aLineA2EndPoint.setValue(50, 0)
aLineB2StartPoint.setValue(50, 0)
aLineB2EndPoint.setValue(50, 50)
aLineC2StartPoint.setValue(50, 50)
aLineC2EndPoint.setValue(100, 50)
aLineD2StartPoint.setValue(100, 50)
aLineD2EndPoint.setValue(100, 0)
aSession.finishOperation()

#=========================================================================
# Make extrusion
#=========================================================================
aSketchResult = aSketchFeature.firstResult()
aSketchEdges = modelAPI_ResultConstruction(aSketchResult).shape()
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin")).pnt()
dirX = geomDataAPI_Dir(aSketchFeature.attribute("DirX")).dir()
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm")).dir()
aSketchFaces = ShapeList()
GeomAlgoAPI_SketchBuilder.createFaces(origin, dirX, norm, aSketchEdges, aSketchFaces)
assert (len(aSketchFaces) > 0)
assert (aSketchFaces[0] is not None)
assert (aSketchFaces[1] is not None)

aSession.startOperation()
anExtrusionFt = aPart.addFeature("Extrusion")
assert (anExtrusionFt.getKind() == "Extrusion")
anExtrusionFt.selectionList("base").append(aSketchResult, aSketchFaces[0])
anExtrusionFt.selectionList("base").append(aSketchResult, aSketchFaces[1])
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
# Test placement by face - face
#=========================================================================
aSession.startOperation()
aPlacementFt = aPart.addFeature("Placement")
aPlacementFt.selectionList("placement_objects_list").append(anExtrusionResult, anExtrusionResult.shape())
aPlacementFt.selection("placement_start_shape").selectSubShape("face", "Extrusion_1_1/LateralFace_1")
aPlacementFt.selection("placement_end_shape").selectSubShape("face", "Extrusion_1_2/LateralFace_3")
aPlacementFt.boolean("placement_reverse_direction").setValue(False)
aPlacementFt.boolean("placement_centering").setValue(True)
aPlacementFt.execute()
aSession.finishOperation()

# Check placement results
assert (len(aPlacementFt.results()) > 0)
aPlacementResult = modelAPI_ResultBody(aPlacementFt.firstResult())
assert (aPlacementResult is not None)
aSession.undo()

#=========================================================================
# Test placement by face - edge
#=========================================================================
aSession.startOperation()
aPlacementFt = aPart.addFeature("Placement")
aPlacementFt.selectionList("placement_objects_list").append(anExtrusionResult, anExtrusionResult.shape())
aPlacementFt.selection("placement_start_shape").selectSubShape("face", "Extrusion_1_1/LateralFace_1")
aPlacementFt.selection("placement_end_shape").selectSubShape("edge", "Extrusion_1_2/ToFace_1&Extrusion_1_2/LateralFace_3")
aPlacementFt.boolean("placement_reverse_direction").setValue(False)
aPlacementFt.boolean("placement_centering").setValue(True)
aPlacementFt.execute()
aSession.finishOperation()

# Check placement results
assert (len(aPlacementFt.results()) > 0)
aPlacementResult = modelAPI_ResultBody(aPlacementFt.firstResult())
assert (aPlacementResult is not None)
aSession.undo()

#=========================================================================
# Test placement by face - vertex
#=========================================================================
aSession.startOperation()
aPlacementFt = aPart.addFeature("Placement")
aPlacementFt.selectionList("placement_objects_list").append(anExtrusionResult, anExtrusionResult.shape())
aPlacementFt.selection("placement_start_shape").selectSubShape("face", "Extrusion_1_1/LateralFace_1")
aPlacementFt.selection("placement_end_shape").selectSubShape("vertex", "Extrusion_1_2/ToFace_1&Extrusion_1_2/LateralFace_3&Extrusion_1_2/LateralFace_2")
aPlacementFt.boolean("placement_reverse_direction").setValue(False)
aPlacementFt.boolean("placement_centering").setValue(True)
aPlacementFt.execute()
aSession.finishOperation()

# Check placement results
assert (len(aPlacementFt.results()) > 0)
aPlacementResult = modelAPI_ResultBody(aPlacementFt.firstResult())
assert (aPlacementResult is not None)
aSession.undo()

#=========================================================================
# Test placement by edge - edge
#=========================================================================
aSession.startOperation()
aPlacementFt = aPart.addFeature("Placement")
aPlacementFt.selectionList("placement_objects_list").append(anExtrusionResult, anExtrusionResult.shape())
aPlacementFt.selection("placement_start_shape").selectSubShape("edge", "Extrusion_1_1/ToFace_1&Extrusion_1_1/LateralFace_1")
aPlacementFt.selection("placement_end_shape").selectSubShape("edge", "Extrusion_1_2/ToFace_1&Extrusion_1_2/LateralFace_3")
aPlacementFt.boolean("placement_reverse_direction").setValue(False)
aPlacementFt.boolean("placement_centering").setValue(True)
aPlacementFt.execute()
aSession.finishOperation()

# Check placement results
assert (len(aPlacementFt.results()) > 0)
aPlacementResult = modelAPI_ResultBody(aPlacementFt.firstResult())
assert (aPlacementResult is not None)
aSession.undo()

#=========================================================================
# Test placement by edge - vertex
#=========================================================================
aSession.startOperation()
aPlacementFt = aPart.addFeature("Placement")
aPlacementFt.selectionList("placement_objects_list").append(anExtrusionResult, anExtrusionResult.shape())
aPlacementFt.selection("placement_start_shape").selectSubShape("edge", "Extrusion_1_1/ToFace_1&Extrusion_1_1/LateralFace_1")
aPlacementFt.selection("placement_end_shape").selectSubShape("vertex", "Extrusion_1_2/ToFace_1&Extrusion_1_2/LateralFace_3&Extrusion_1_2/LateralFace_2")
aPlacementFt.boolean("placement_reverse_direction").setValue(False)
aPlacementFt.boolean("placement_centering").setValue(True)
aPlacementFt.execute()
aSession.finishOperation()

# Check placement results
assert (len(aPlacementFt.results()) > 0)
aPlacementResult = modelAPI_ResultBody(aPlacementFt.firstResult())
assert (aPlacementResult is not None)
aSession.undo()

#=========================================================================
# Test placement by vertex - vertex
#=========================================================================
aSession.startOperation()
aPlacementFt = aPart.addFeature("Placement")
aPlacementFt.selectionList("placement_objects_list").append(anExtrusionResult, anExtrusionResult.shape())
aPlacementFt.selection("placement_start_shape").selectSubShape("vertex", "Extrusion_1_1/LateralFace_4&Extrusion_1_1/FromFace_1&Extrusion_1_1/LateralFace_1")
aPlacementFt.selection("placement_end_shape").selectSubShape("vertex", "Extrusion_1_2/ToFace_1&Extrusion_1_2/LateralFace_3&Extrusion_1_2/LateralFace_2")
aPlacementFt.boolean("placement_reverse_direction").setValue(False)
aPlacementFt.boolean("placement_centering").setValue(True)
aPlacementFt.execute()
aSession.finishOperation()

# Check placement results
assert (len(aPlacementFt.results()) > 0)
aPlacementResult = modelAPI_ResultBody(aPlacementFt.firstResult())
assert (aPlacementResult is not None)
