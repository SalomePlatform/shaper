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
aCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("circle_center"))
aCircleRadius = aSketchCircle.real("circle_radius")
aCircleCentr.setValue(0, 0)
aCircleRadius.setValue(50)

aSketchLine = aCircleSketchFeature.addFeature("SketchLine")
aLineStartPoint = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
aLineEndPoint = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
aLineStartPoint.setValue(0, -50)
aLineEndPoint.setValue(0, 50)
aSession.finishOperation()
#=========================================================================
# Create a sketch with triangle to extrude
#=========================================================================
aSession.startOperation()
aTriangleSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aTriangleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aTriangleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
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
    norm = geomDataAPI_Dir(eachSketchFeature.attribute("Norm")).dir()
    aSketchFaces = ShapeList()
    GeomAlgoAPI_SketchBuilder.createFaces(
        origin, dirX, norm, aSketchEdges, aSketchFaces)
    # Create extrusion on them
    anExtrusionFt = aPart.addFeature("Extrusion")
    anExtrusionFt.selectionList("base").append(
        aSketchResult, None)
    anExtrusionFt.string("CreationMethod").setValue("BySizes")
    anExtrusionFt.real("from_size").setValue(0)
    anExtrusionFt.real("to_size").setValue(50)
    anExtrusionFt.real("to_offset").setValue(0) #TODO: remove
    anExtrusionFt.real("from_offset").setValue(0) #TODO: remove
    anExtrusionFt.execute()
    extrudedObjects.append(modelAPI_ResultBody(anExtrusionFt.firstResult()))
aSession.finishOperation()
#=========================================================================
# Create a pacman as boolean cut of the prism from the cylinder
#=========================================================================
aSession.startOperation()
aBooleanFt = aPart.addFeature("Boolean")
aBooleanFt.selectionList("main_objects").append(modelAPI_ResultCompSolid(extrudedObjects[0]).subResult(1), None)
aBooleanFt.selectionList("tool_objects").append(extrudedObjects[1], None)
aBooleanType = 0
aBooleanFt.integer("bool_type").setValue(aBooleanType)
aBooleanFt.execute()
aSession.finishOperation()

assert (len(aBooleanFt.results()) > 0)
aBooleanResult = modelAPI_ResultBody(aBooleanFt.firstResult())
assert (aBooleanResult is not None)
aSession.undo()

#=========================================================================
# Fuse
#=========================================================================
aSession.startOperation()
aBooleanFt = aPart.addFeature("Boolean")
aBooleanFt.selectionList("main_objects").append(modelAPI_ResultCompSolid(extrudedObjects[0]).subResult(1), None)
aBooleanFt.selectionList("tool_objects").append(extrudedObjects[1], None)
aBooleanType = 1
aBooleanFt.integer("bool_type").setValue(aBooleanType)
aBooleanFt.execute()
aSession.finishOperation()

assert (len(aBooleanFt.results()) > 0)
aBooleanResult = modelAPI_ResultBody(aBooleanFt.firstResult())
assert (aBooleanResult is not None)
#=========================================================================
# End of test
#=========================================================================

from salome.shaper import model
assert(model.checkPythonDump())
