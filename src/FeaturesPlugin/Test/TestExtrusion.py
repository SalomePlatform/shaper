"""
      TestExtrusion.py
      Unit test of FeaturesPlugin_Extrusion class
      
      class FeaturesPlugin_Extrusion : public ModelAPI_Feature
        static const std::string MY_EXTRUSION_ID("Extrusion");
        static const std::string LIST_ID("base");
        static const std::string METHOD_ATTR("CreationMethod");
        static const std::string MY_TO_SIZE_ID("to_size");
        static const std::string MY_FROM_SIZE_ID("from_size");
        static const std::string MY_TO_OBJECT_ID("to_object");
        static const std::string MY_TO_OFFSET_ID("to_offset");
        static const std::string MY_FROM_OBJECT_ID("from_object");
        static const std::string MY_FROM_OFFSET_ID("from_offset");

        data()->addAttribute(LIST_ID(), ModelAPI_AttributeSelection::typeId());
        data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());
        data()->addAttribute(TO_SIZE_ID(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(FROM_SIZE_ID(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(TO_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
        data()->addAttribute(TO_OFFSET_ID(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(FROM_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
        data()->addAttribute(FROM_OFFSET_ID(), ModelAPI_AttributeSelection::typeId());
"""
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
anCircleCentr.setValue(50., 50)
aCircleRadius.setValue(20.)
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
# Test extrusion between bounding planes
#=========================================================================
# Create from plane
aSession.startOperation()
aFromPlaneFeature = aPart.addFeature("Plane")
aFromPlaneFeature.string("CreationMethod").setValue("PlaneByGeneralEquation")
aFromPlaneFeature.real("A").setValue(0.)
aFromPlaneFeature.real("B").setValue(0.)
aFromPlaneFeature.real("C").setValue(1.)
aFromPlaneFeature.real("D").setValue(30.)
aSession.finishOperation()

# Create to plane
aSession.startOperation()
aToPlaneFeature = aPart.addFeature("Plane")
aToPlaneFeature.string("CreationMethod").setValue("PlaneByGeneralEquation")
aToPlaneFeature.real("A").setValue(0.)
aToPlaneFeature.real("B").setValue(0.)
aToPlaneFeature.real("C").setValue(1.)
aToPlaneFeature.real("D").setValue(-30.)
aSession.finishOperation()

# Create extrusion
aSession.startOperation()
anExtrusionFt = aPart.addFeature("Extrusion")
assert (anExtrusionFt.getKind() == "Extrusion")
# selection type FACE=4
anExtrusionFt.selectionList("base").append(
    aCircleSketchResult, aCircleSketchFaces[0])
anExtrusionFt.string("CreationMethod").setValue("ByPlanesAndOffsets")
anExtrusionFt.real("to_size").setValue(0) #TODO: remove
anExtrusionFt.real("from_size").setValue(0) #TODO: remove
aToResult = aToPlaneFeature.firstResult()
aToShape = modelAPI_ResultConstruction(aToResult).shape()
anExtrusionFt.selection("to_object").setValue(aToResult, aToShape)
anExtrusionFt.real("to_offset").setValue(10)
aFromResult = aFromPlaneFeature.firstResult()
aFromShape = modelAPI_ResultConstruction(aFromResult).shape()
anExtrusionFt.selection("from_object").setValue(aFromResult, aFromShape)
anExtrusionFt.real("from_offset").setValue(10)
anExtrusionFt.execute()
aSession.finishOperation()

# Check extrusion results
assert (len(anExtrusionFt.results()) > 0)
anExtrusionResult = modelAPI_ResultBody(anExtrusionFt.firstResult())
assert (anExtrusionResult is not None)

# Check extrusion volume
aRefVolume = 100530.96491487337
aResVolume = GeomAlgoAPI_ShapeProps_volume(anExtrusionResult.shape())
assert (math.fabs(aResVolume - aRefVolume) < 10 ** -5)

#=========================================================================
# Test extrusion between bounding faces from other sketch result
#=========================================================================
aSession.startOperation()
aClampSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aClampSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aClampSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aClampSketchFeature.attribute("Norm"))
norm.setValue(0, 1, 0)
# Create clamp
aSketchLineA = aClampSketchFeature.addFeature("SketchLine")
aSketchLineB = aClampSketchFeature.addFeature("SketchLine")
aSketchLineC = aClampSketchFeature.addFeature("SketchLine")
aSketchLineD = aClampSketchFeature.addFeature("SketchLine")
aSketchLineE = aClampSketchFeature.addFeature("SketchLine")
aLineAStartPoint = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(aSketchLineA.attribute("EndPoint"))
aLineBStartPoint = geomDataAPI_Point2D(aSketchLineB.attribute("StartPoint"))
aLineBEndPoint = geomDataAPI_Point2D(aSketchLineB.attribute("EndPoint"))
aLineCStartPoint = geomDataAPI_Point2D(aSketchLineC.attribute("StartPoint"))
aLineCEndPoint = geomDataAPI_Point2D(aSketchLineC.attribute("EndPoint"))
aLineDStartPoint = geomDataAPI_Point2D(aSketchLineD.attribute("StartPoint"))
aLineDEndPoint = geomDataAPI_Point2D(aSketchLineD.attribute("EndPoint"))
aLineEStartPoint = geomDataAPI_Point2D(aSketchLineE.attribute("StartPoint"))
aLineEEndPoint = geomDataAPI_Point2D(aSketchLineE.attribute("EndPoint"))
aLineAStartPoint.setValue(0., -50.)
aLineAEndPoint.setValue(0., 50.)
aLineBStartPoint.setValue(0., 50.)
aLineBEndPoint.setValue(100., 50.)
aLineCStartPoint.setValue(100., 50.)
aLineCEndPoint.setValue(10., 0.)
aLineDStartPoint.setValue(10., 0.)
aLineDEndPoint.setValue(100., -50.)
aLineEStartPoint.setValue(100., -50.)
aLineEEndPoint.setValue(0., -50.)
aSession.finishOperation()

# Extrude clamp
aClampSketchResult = aClampSketchFeature.firstResult()
aClampSketchEdges = modelAPI_ResultConstruction(aClampSketchResult).shape()
origin = geomDataAPI_Point(aClampSketchFeature.attribute("Origin")).pnt()
dirX = geomDataAPI_Dir(aClampSketchFeature.attribute("DirX")).dir()
norm = geomDataAPI_Dir(aClampSketchFeature.attribute("Norm")).dir()
aClampSketchFaces = ShapeList()
GeomAlgoAPI_SketchBuilder.createFaces(
    origin, dirX, norm, aClampSketchEdges, aClampSketchFaces)
aSession.startOperation()
aClampExtrusionFt = aPart.addFeature("Extrusion")
assert (aClampExtrusionFt.getKind() == "Extrusion")
# selection type FACE=4
aClampExtrusionFt.selectionList("base").append(
    aClampSketchResult, aClampSketchFaces[0])
aClampExtrusionFt.string("CreationMethod").setValue("BySizes")
aClampExtrusionFt.real("to_size").setValue(70)
aClampExtrusionFt.real("from_size").setValue(0)
aClampExtrusionFt.real("to_offset").setValue(0) #TODO: remove
aClampExtrusionFt.real("from_offset").setValue(0) #TODO: remove
aClampExtrusionFt.execute()
aSession.finishOperation()

# Check extrusion results
assert (len(aClampExtrusionFt.results()) > 0)
anExtrusionResult = modelAPI_ResultBody(aClampExtrusionFt.firstResult())
assert (anExtrusionResult is not None)

# Extrude circle
aSession.startOperation()
anExtrusionFt = aPart.addFeature("Extrusion")
assert (anExtrusionFt.getKind() == "Extrusion")
# selection type FACE=4
anExtrusionFt.selectionList("base").append(
    aCircleSketchResult, aCircleSketchFaces[0])
anExtrusionFt.string("CreationMethod").setValue("ByPlanesAndOffsets")
anExtrusionFt.real("to_size").setValue(0) #TODO: remove
anExtrusionFt.real("from_size").setValue(0) #TODO: remove
anExtrusionFt.selection("to_object").selectSubShape("face", "Extrusion_3/LateralFace_2")
anExtrusionFt.real("to_offset").setValue(0)
anExtrusionFt.selection("from_object").selectSubShape("face", "Extrusion_3/LateralFace_1")
anExtrusionFt.real("from_offset").setValue(0)
anExtrusionFt.execute()
aSession.finishOperation()

# Check extrusion results
assert (len(anExtrusionFt.results()) > 0)
anExtrusionResult = modelAPI_ResultBody(anExtrusionFt.firstResult())
assert (anExtrusionResult is not None)

#=========================================================================
# End of test
#=========================================================================
