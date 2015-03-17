"""
      TestBoolean.py
      Unit test of FeaturesPlugin_Group class
      
      class FeaturesPlugin_Group
        static const std::string MY_GROUP_ID("Group");
        static const std::string MY_GROUP_LIST_ID("group_list");
        
        data()->addAttribute(FeaturesPlugin_Group::LIST_ID(), ModelAPI_AttributeSelectionList::type());
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
# Build sketch faces
aSession.startOperation()
aSketchResult = aTriangleSketchFeature.firstResult()
aSketchEdges = modelAPI_ResultConstruction(aSketchResult).shape()
origin = geomDataAPI_Point(aTriangleSketchFeature.attribute("Origin")).pnt()
dirX = geomDataAPI_Dir(aTriangleSketchFeature.attribute("DirX")).dir()
dirY = geomDataAPI_Dir(aTriangleSketchFeature.attribute("DirY")).dir()
norm = geomDataAPI_Dir(aTriangleSketchFeature.attribute("Norm")).dir()
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
aSession.finishOperation()
anExtrusionBody = modelAPI_ResultBody(anExtrusionFt.firstResult())
#=========================================================================
# Create group of edges
# TODO: After implementation of selection from batch script
# update this test to have proper reslts in the group feature
#=========================================================================
aSession.startOperation()
aGroupFeature = aSession.activeDocument().addFeature("Group")
aSelectionListAttr = aGroupFeature.selectionList("group_list")
topAbs_EdgeType = 6
aSelectionListAttr.setSelectionType(topAbs_EdgeType)
aSelectionListAttr.append("Extrusion_1/LateralFace_3|Extrusion_1/LateralFace_1")
aSession.finishOperation()
#=========================================================================
# Check results
#=========================================================================
#aGroupResult = aGroupFeature.firstResult()
#assert(aGroupResult)
#=========================================================================
# End of test
#=========================================================================
