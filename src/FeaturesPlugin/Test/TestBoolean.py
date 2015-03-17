"""
      TestBoolean.py
      Unit test of FeaturesPlugin_Boolean class
      
      class FeaturesPlugin_Boolean
        static const std::string MY_ID("Boolean");
        static const std::string MY_OBJECT_ID("main_object");
        static const std::string MY_TOOL_ID("tool_object");
        static const std::string MY_TYPE_ID("bool_type");
        
        data()->addAttribute(FeaturesPlugin_Boolean::OBJECT_ID(), ModelAPI_AttributeReference::type());
        data()->addAttribute(FeaturesPlugin_Boolean::TOOL_ID(), ModelAPI_AttributeReference::type());
        data()->addAttribute(FeaturesPlugin_Boolean::TYPE_ID(), ModelAPI_AttributeInteger::type());
"""
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
aPart = aSession.activeDocument()
aSession.finishOperation()
#=========================================================================
# Create a sketch with circle to extrude
#=========================================================================
aSession.startOperation()
aCircleSketchFeature = modelAPI_CompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aCircleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aCircleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
diry = geomDataAPI_Dir(aCircleSketchFeature.attribute("DirY"))
diry.setValue(0, 1, 0)
norm = geomDataAPI_Dir(aCircleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSketchCircle = aCircleSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
aCircleRadius = aSketchCircle.real("CircleRadius")
anCircleCentr.setValue(10., 10.)
aCircleRadius.setValue(50.)
aSession.finishOperation()
#=========================================================================
# Create a sketch with triangle to extrude
#=========================================================================
aSession.startOperation()
aTriangleSketchFeature = modelAPI_CompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aTriangleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aTriangleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
diry = geomDataAPI_Dir(aTriangleSketchFeature.attribute("DirY"))
diry.setValue(0, 1, 0)
norm = geomDataAPI_Dir(aTriangleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSketchLineA = aTriangleSketchFeature.addFeature("SketchLine")
aSketchLineB = aTriangleSketchFeature.addFeature("SketchLine")
aSketchLineC = aTriangleSketchFeature.addFeature("SketchLine")
aLineAStartPoint = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(aSketchLineA.attribute("EndPoint"))
aLineBStartPoint = geomDataAPI_Point2D(aSketchLineB.attribute("StartPoint"))
aLineBEndPoint = geomDataAPI_Point2D(aSketchLineB.attribute("EndPoint"))
aLineCStartPoint = geomDataAPI_Point2D(aSketchLineC.attribute("StartPoint"))
aLineCEndPoint = geomDataAPI_Point2D(aSketchLineC.attribute("EndPoint"))
aLineAStartPoint.setValue(25., 25.)
aLineAEndPoint.setValue(100., 25.)
aLineBStartPoint.setValue(100., 25.)
aLineBEndPoint.setValue(60., 75.)
aLineCStartPoint.setValue(60., 75.)
aLineCEndPoint.setValue(25., 25.)
aSession.finishOperation()
#=========================================================================
# Make extrusion on circle (cylinder) and triangle (prism)
#=========================================================================
# Build shape from sketcher results
aSession.startOperation()
extrudedObjects = []
for eachSketchFeature in [aCircleSketchFeature, aTriangleSketchFeature]:
    # Build sketch faces
    aSketchResult = eachSketchFeature.firstResult()
    aSketchEdges = modelAPI_ResultConstruction(aSketchResult).shape()
    origin = geomDataAPI_Point(eachSketchFeature.attribute("Origin")).pnt()
    dirX = geomDataAPI_Dir(eachSketchFeature.attribute("DirX")).dir()
    dirY = geomDataAPI_Dir(eachSketchFeature.attribute("DirY")).dir()
    norm = geomDataAPI_Dir(eachSketchFeature.attribute("Norm")).dir()
    aSketchFaces = ShapeList()
    GeomAlgoAPI_SketchBuilder.createFaces(
        origin, dirX, dirY, norm, aSketchEdges, aSketchFaces)
    # Create extrusion on them
    anExtrusionFt = aPart.addFeature("Extrusion")
    # selection type FACE=4
    anExtrusionFt.selectionList("base").setSelectionType(4)
    anExtrusionFt.selectionList("base").append(
        aSketchResult, aSketchFaces[0])
    anExtrusionFt.real("size").setValue(50)
    anExtrusionFt.boolean("reverse").setValue(False)
    anExtrusionFt.execute()
    extrudedObjects.append(modelAPI_ResultBody(anExtrusionFt.firstResult()))
aSession.finishOperation()
#=========================================================================
# Create a pacman as boolean cut of the prism from the cylinder
#=========================================================================
aSession.startOperation()
aBooleanFt = aPart.addFeature("Boolean")
aBooleanFt.reference("main_object").setValue(extrudedObjects[0])
aBooleanFt.reference("tool_object").setValue(extrudedObjects[1])
kBooleanTypeCut = 0
aBooleanFt.integer("bool_type").setValue(kBooleanTypeCut)
aBooleanFt.execute()
aSession.finishOperation()

assert (len(aBooleanFt.results()) > 0)
aBooleanResult = modelAPI_ResultBody(aBooleanFt.firstResult())
assert (aBooleanResult is not None)
#=========================================================================
# End of test
#=========================================================================
