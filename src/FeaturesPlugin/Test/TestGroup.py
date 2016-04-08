"""
      TestBoolean.py
      Unit test of FeaturesPlugin_Group class

      class FeaturesPlugin_Group
        static const std::string MY_GROUP_ID("Group");
        static const std::string MY_GROUP_LIST_ID("group_list");

        data()->addAttribute(FeaturesPlugin_Group::LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
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
aSession.finishOperation()
aPart = aSession.activeDocument()
#=========================================================================
# Create a sketch with triangle and extrude it
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
aLineAStartPoint.setValue(-100., 0.)
aLineAEndPoint.setValue(100., 0.)
aLineBStartPoint.setValue(100., 0.)
aLineBEndPoint.setValue(0., 173.2)
aLineCStartPoint.setValue(0., 173.2)
aLineCEndPoint.setValue(-100., 0.)
aSession.finishOperation()
# Build sketch faces
aSession.startOperation()
aSketchResult = aTriangleSketchFeature.firstResult()
aSketchEdges = modelAPI_ResultConstruction(aSketchResult).shape()
origin = geomDataAPI_Point(aTriangleSketchFeature.attribute("Origin")).pnt()
dirX = geomDataAPI_Dir(aTriangleSketchFeature.attribute("DirX")).dir()
norm = geomDataAPI_Dir(aTriangleSketchFeature.attribute("Norm")).dir()
aSketchFaces = ShapeList()
GeomAlgoAPI_SketchBuilder.createFaces(origin, dirX, norm, aSketchEdges, aSketchFaces)
# Create extrusion on them
anExtrusionFt = aPart.addFeature("Extrusion")
anExtrusionFt.selectionList("base").append(aSketchResult, aSketchFaces[0])
anExtrusionFt.string("CreationMethod").setValue("BySizes")
anExtrusionFt.real("to_size").setValue(50)
anExtrusionFt.real("from_size").setValue(50)
anExtrusionFt.real("to_offset").setValue(0) #TODO: remove
anExtrusionFt.real("from_offset").setValue(0) #TODO: remove
anExtrusionFt.execute()
aSession.finishOperation()
anExtrusionBody = modelAPI_ResultBody(anExtrusionFt.firstResult())
#=========================================================================
# Create group of vertex
#=========================================================================
aSession.startOperation()
aGroupFeature = aSession.activeDocument().addFeature("Group")
aSelectionListAttr = aGroupFeature.selectionList("group_list")
aSelectionListAttr.setSelectionType("vertex")
aSelectionListAttr.append("Extrusion_1_1/Generated_Face_3&Extrusion_1_1/Generated_Face_1&Extrusion_1_1/To_Face_1_1")
aSelectionListAttr.append("Extrusion_1_1/Generated_Face_3&Extrusion_1_1/Generated_Face_2&Extrusion_1_1/To_Face_1_1")
aSelectionListAttr.append("Extrusion_1_1/Generated_Face_2&Extrusion_1_1/Generated_Face_1&Extrusion_1_1/To_Face_1_1")
aSelectionListAttr.append("Extrusion_1_1/Generated_Face_3&Extrusion_1_1/Generated_Face_1&Extrusion_1_1/From_Face_1_1")
aSelectionListAttr.append("Extrusion_1_1/Generated_Face_3&Extrusion_1_1/Generated_Face_2&Extrusion_1_1/From_Face_1_1")
aSelectionListAttr.append("Extrusion_1_1/Generated_Face_2&Extrusion_1_1/Generated_Face_1&Extrusion_1_1/From_Face_1_1")
aSession.finishOperation()
#=========================================================================
# Check results
#=========================================================================
assert(aSelectionListAttr.size() == 6)
aGroupResult = aGroupFeature.firstResult()
assert(aGroupResult)
#=========================================================================
# Create group of edges
#=========================================================================
aSession.startOperation()
aGroupFeature = aSession.activeDocument().addFeature("Group")
aSelectionListAttr = aGroupFeature.selectionList("group_list")
aSelectionListAttr.setSelectionType("edge")
aSelectionListAttr.append("Extrusion_1_1/Generated_Face_2&Extrusion_1_1/Generated_Face_1")
aSelectionListAttr.append("Extrusion_1_1/Generated_Face_3&Extrusion_1_1/Generated_Face_2")
aSelectionListAttr.append("Extrusion_1_1/Generated_Face_3&Extrusion_1_1/Generated_Face_1")
aSelectionListAttr.append("Extrusion_1_1/Generated_Face_3&Extrusion_1_1/From_Face_1_1")
aSelectionListAttr.append("Extrusion_1_1/Generated_Face_1&Extrusion_1_1/To_Face_1_1")
aSelectionListAttr.append("Extrusion_1_1/Generated_Face_1&Extrusion_1_1/From_Face_1_1")
aSelectionListAttr.append("Extrusion_1_1/Generated_Face_2&Extrusion_1_1/To_Face_1_1")
aSelectionListAttr.append("Extrusion_1_1/Generated_Face_2&Extrusion_1_1/From_Face_1_1")
aSelectionListAttr.append("Extrusion_1_1/Generated_Face_3&Extrusion_1_1/To_Face_1_1")
aSession.finishOperation()
#=========================================================================
# Check results
#=========================================================================
assert(aSelectionListAttr.size() == 9)
aGroupResult = aGroupFeature.firstResult()
assert(aGroupResult)
#=========================================================================
# Create group of faces
#=========================================================================
aSession.startOperation()
aGroupFeature = aSession.activeDocument().addFeature("Group")
aSelectionListAttr = aGroupFeature.selectionList("group_list")
aSelectionListAttr.setSelectionType("face")
aShapeExplorer = GeomAPI_ShapeExplorer(anExtrusionBody.shape(), GeomAPI_Shape.FACE)
while aShapeExplorer.more():
  aSelectionListAttr.append(anExtrusionBody, aShapeExplorer.current())
  aShapeExplorer.next();
aSession.finishOperation()
#=========================================================================
# Check results
#=========================================================================
assert(aSelectionListAttr.size() == 5)
aGroupResult = aGroupFeature.firstResult()
assert(aGroupResult)
#=========================================================================
# Create group of solids
#=========================================================================
aSession.startOperation()
aGroupFeature = aSession.activeDocument().addFeature("Group")
aSelectionListAttr = aGroupFeature.selectionList("group_list")
aSelectionListAttr.setSelectionType("face")
aSelectionListAttr.append(anExtrusionBody, None)
aSession.finishOperation()
#=========================================================================
# Check results
#=========================================================================
assert(aSelectionListAttr.size() == 1)
aGroupResult = aGroupFeature.firstResult()
assert(aGroupResult)

#=========================================================================
# Create group of face
#=========================================================================
aSession.startOperation()
aGroupFeature = aSession.activeDocument().addFeature("Group")
aSelectionListAttr = aGroupFeature.selectionList("group_list")
aSelectionListAttr.setSelectionType("face")
aSelectionListAttr.append("Extrusion_1_1/To_Face_1_1")
aSession.finishOperation()
#=========================================================================
# Check results
#=========================================================================
assert(aSelectionListAttr.size() == 1)
aGroupResult = aGroupFeature.firstResult()
assert(aGroupResult)

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
anCircleCentr.setValue(0., 57.74)
aCircleRadius.setValue(50.)
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
assert(len(aCircleSketchFaces) > 0)
assert(aCircleSketchFaces[0] is not None)
# Create extrusion
aSession.startOperation()
anExtrusionFt = aPart.addFeature("Extrusion")
assert (anExtrusionFt.getKind() == "Extrusion")
# selection type FACE=4
anExtrusionFt.selectionList("base").append(aCircleSketchResult, aCircleSketchFaces[0])
anExtrusionFt.string("CreationMethod").setValue("BySizes")
anExtrusionFt.real("to_size").setValue(50)
anExtrusionFt.real("from_size").setValue(50)
anExtrusionFt.real("to_offset").setValue(0) #TODO: remove
anExtrusionFt.real("from_offset").setValue(0) #TODO: remove
anExtrusionFt.execute()
aSession.finishOperation()
aCylinderBody = modelAPI_ResultBody(anExtrusionFt.firstResult())

#=========================================================================
# Create a cut
#=========================================================================
aSession.startOperation()
aBooleanFt = aPart.addFeature("Boolean")
aBooleanFt.selectionList("main_objects").append(anExtrusionBody, None)
aBooleanFt.selectionList("tool_objects").append(aCylinderBody, None)
aBooleanTypeCut = 0
aBooleanFt.integer("bool_type").setValue(aBooleanTypeCut)
aBooleanFt.execute()
aSession.finishOperation()

#=========================================================================
# Move group feature
#=========================================================================
aSession.startOperation()
aPart.moveFeature(aGroupFeature, aBooleanFt)
aSession.finishOperation()

#=========================================================================
# Check results
#=========================================================================
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(aGroupFeature))
assert(aSelectionListAttr.size() == 1)
assert(len(aGroupFeature.results()) > 0)
aGroupResult = aGroupFeature.firstResult()
assert(aGroupResult)
#=========================================================================
# End of test
#=========================================================================
