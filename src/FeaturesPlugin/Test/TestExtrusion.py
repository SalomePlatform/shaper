"""
      TestExtrusion.py
      Unit test of FeaturesPlugin_Extrusion class
      
      class FeaturesPlugin_Extrusion : public ModelAPI_Feature
        static const std::string MY_EXTRUSION_ID("Extrusion");
        static const std::string MY_FACE_ID("extrusion_face");
        static const std::string MY_SIZE_ID("extrusion_size");
        static const std::string MY_REVERSE_ID("extrusion_reverse");
          
        data()->addAttribute(FeaturesPlugin_Extrusion::FACE_ID(), ModelAPI_AttributeSelection::type());
        data()->addAttribute(FeaturesPlugin_Extrusion::SIZE_ID(), ModelAPI_AttributeDouble::type());
        data()->addAttribute(FeaturesPlugin_Extrusion::REVERSE_ID(), ModelAPI_AttributeBoolean::type());
"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *

__updated__ = "2014-11-21"

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
aSketchFeature = modelAPI_CompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
diry = geomDataAPI_Dir(aSketchFeature.attribute("DirY"))
diry.setValue(0, 1, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
# Create circle
aSketchCircle = aSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
aCircleRadius = aSketchCircle.real("CircleRadius")
anCircleCentr.setValue(50., 50)
aCircleRadius.setValue(20.)
aSession.finishOperation()
#=========================================================================
# Make extrusion on circle
#=========================================================================
# Build shape from sketcher results
aSketchResult = aSketchFeature.firstResult()
aSketchEdges = modelAPI_ResultConstruction(aSketchResult).shape()
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin")).pnt()
dirX = geomDataAPI_Dir(aSketchFeature.attribute("DirX")).dir()
dirY = geomDataAPI_Dir(aSketchFeature.attribute("DirY")).dir()
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm")).dir()
aSketchFaces = ShapeList()
GeomAlgoAPI_SketchBuilder.createFaces(
    origin, dirX, dirY, norm, aSketchEdges, aSketchFaces)
assert (len(aSketchFaces) > 0)
assert (aSketchFaces[0] is not None)
# Create extrusion
aSession.startOperation()
anExtrusionFt = aPart.addFeature("Extrusion")
assert (anExtrusionFt.getKind() == "Extrusion")
anExtrusionFt.selection("extrusion_face").setValue(
    aSketchResult, aSketchFaces[0])
anExtrusionFt.real("extrusion_size").setValue(50)
anExtrusionFt.boolean("extrusion_reverse").setValue(False)
anExtrusionFt.execute()
aSession.finishOperation()
assert (anExtrusionFt.real("extrusion_size").value() == 50.0)
assert (anExtrusionFt.boolean("extrusion_reverse").value() == False)

# Check extrusion results
assert (len(anExtrusionFt.results()) > 0)
anExtrusionResult = modelAPI_ResultBody(anExtrusionFt.firstResult())
assert (anExtrusionResult is not None)
#=========================================================================
# End of test
#=========================================================================
