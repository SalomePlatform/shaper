"""
      TestRotation.py
      Unit test of FeaturesPlugin_Rotation class

      class FeaturesPlugin_Movement : public ModelAPI_Feature
        static const std::string MY_ROTATION_ID("Rotation");
        static const std::string MY_OBJECTS_LIST_ID("main_objects");
        static const std::string MY_AXIS_OBJECT_ID("axis_object");
        static const std::string MY_ANGLE_ID("angle");

        data()->addAttribute(OBJECTS_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
        data()->addAttribute(AXIS_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
        data()->addAttribute(ANGLE_ID(), ModelAPI_AttributeDouble::typeId());
"""
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
# Create a part for movement
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
anCircleCentr.setValue(50, 50)
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
assert (anExtrusionFt.real("to_size").value() == 50)

# Check extrusion results
assert (len(anExtrusionFt.results()) > 0)
anExtrusionResult = modelAPI_ResultBody(anExtrusionFt.firstResult())
assert (anExtrusionResult is not None)

#=========================================================================
# Create a sketch line to rotation
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
aLineStartPoint.setValue(-100, -100)
aLineEndPoint.setValue(100, -100)
aSession.finishOperation()

# Build shape from sketcher results
aLineSketchResult = aLineSketchFeature.firstResult()
aLineSketchShape = modelAPI_ResultConstruction(aLineSketchResult).shape()
aShapeExplorer = GeomAPI_ShapeExplorer(aLineSketchShape, GeomAPI_Shape.EDGE)
aLineEdge = aShapeExplorer.current()

#=========================================================================
# Test rotation
#=========================================================================
aSession.startOperation()
aRotateFt = aPart.addFeature("Rotation")
assert (aRotateFt.getKind() == "Rotation")
aRotateFt.selectionList("main_objects").append(
    anExtrusionResult, anExtrusionResult.shape())
aRotateFt.selection("axis_object").setValue(aLineSketchResult, aLineEdge)
aRotateFt.real("angle").setValue(90)
aRotateFt.execute()
aSession.finishOperation()
assert (aRotateFt.real("angle").value() == 90)

# Check rotation results
aFactory = ModelAPI_Session.get().validators()
assert (aFactory.validate(aRotateFt))
assert (len(aRotateFt.results()) > 0)
aMoveResult = modelAPI_ResultBody(aRotateFt.firstResult())
assert (aMoveResult is not None)

