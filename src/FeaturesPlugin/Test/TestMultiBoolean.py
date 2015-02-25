"""
      TestExtrusion.py
      Unit test of FeaturesPlugin_Boolean class: many Boolean operations performance
      
      class FeaturesPlugin_Extrusion : public ModelAPI_Feature
        static const std::string MY_EXTRUSION_ID("Extrusion");
        static const std::string MY_FACE_ID("extrusion_face");
        static const std::string MY_SIZE_ID("extrusion_size");
        static const std::string MY_REVERSE_ID("extrusion_reverse");
          
        data()->addAttribute(FeaturesPlugin_Extrusion::FACE_ID(), ModelAPI_AttributeSelection::type());
        data()->addAttribute(FeaturesPlugin_Extrusion::SIZE_ID(), ModelAPI_AttributeDouble::type());
        data()->addAttribute(FeaturesPlugin_Extrusion::REVERSE_ID(), ModelAPI_AttributeBoolean::type());
"""

# Number rows and columns of cylinders that cuts the big box. Number of Boolena operations is N*N
N = 5

#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

__updated__ = "2015-02-25"

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
# Create a list of sketches with one circle inside of each to extrude
#=========================================================================
step = 99. / N
radius = 95. / N / 2.

aSession.startOperation()
aSketchFeatures = []
for i in xrange(0, N):
  for j in xrange(0, N):
    # Create circle
    aSketchFeature = modelAPI_CompositeFeature(aPart.addFeature("Sketch"))
    origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
    origin.setValue(0, 0, 0)
    dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
    dirx.setValue(1, 0, 0)
    diry = geomDataAPI_Dir(aSketchFeature.attribute("DirY"))
    diry.setValue(0, 1, 0)
    norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
    norm.setValue(0, 0, 1)
    aSketchCircle = aSketchFeature.addFeature("SketchCircle")
    anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
    aCircleRadius = aSketchCircle.real("CircleRadius")
    anCircleCentr.setValue(0.5 + step * (0.5 + i), 0.5 + step * (0.5 + j))
    aCircleRadius.setValue(radius)
    aSketchFeatures.append(aSketchFeature)

aSession.finishOperation()

#=========================================================================
# Make extrusions on circles
#=========================================================================
# Build shape from sketcher results

# Create extrusions
aSession.startOperation()

anExtrusions = []
for i in xrange(0, N * N):
  aSketchResult = aSketchFeatures[i].firstResult()
  aSketchEdges = modelAPI_ResultConstruction(aSketchResult).shape()
  origin = geomDataAPI_Point(aSketchFeatures[i].attribute("Origin")).pnt()
  dirX = geomDataAPI_Dir(aSketchFeatures[i].attribute("DirX")).dir()
  dirY = geomDataAPI_Dir(aSketchFeatures[i].attribute("DirY")).dir()
  norm = geomDataAPI_Dir(aSketchFeatures[i].attribute("Norm")).dir()
  aSketchFaces = ShapeList()
  GeomAlgoAPI_SketchBuilder.createFaces(
      origin, dirX, dirY, norm, aSketchEdges, aSketchFaces)

  anExtrusionFt = aPart.addFeature("Extrusion")
  assert (anExtrusionFt.getKind() == "Extrusion")
  anExtrusionFt.selection("extrusion_face").setValue(
      aSketchResult, aSketchFaces[0])
  anExtrusionFt.real("extrusion_size").setValue(10)
  anExtrusionFt.boolean("extrusion_reverse").setValue(False)
  anExtrusions.append(anExtrusionFt)

aSession.finishOperation()

#=========================================================================
# Make rectangle sketch: base for the box, size 100x100
#=========================================================================
aSession.startOperation()
aQuadrangleSketchFeature = modelAPI_CompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aQuadrangleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aQuadrangleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
diry = geomDataAPI_Dir(aQuadrangleSketchFeature.attribute("DirY"))
diry.setValue(0, 1, 0)
norm = geomDataAPI_Dir(aQuadrangleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSketchLineA = aQuadrangleSketchFeature.addFeature("SketchLine")
aSketchLineB = aQuadrangleSketchFeature.addFeature("SketchLine")
aSketchLineC = aQuadrangleSketchFeature.addFeature("SketchLine")
aSketchLineD = aQuadrangleSketchFeature.addFeature("SketchLine")
aLineAStartPoint = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(aSketchLineA.attribute("EndPoint"))
aLineBStartPoint = geomDataAPI_Point2D(aSketchLineB.attribute("StartPoint"))
aLineBEndPoint = geomDataAPI_Point2D(aSketchLineB.attribute("EndPoint"))
aLineCStartPoint = geomDataAPI_Point2D(aSketchLineC.attribute("StartPoint"))
aLineCEndPoint = geomDataAPI_Point2D(aSketchLineC.attribute("EndPoint"))
aLineDStartPoint = geomDataAPI_Point2D(aSketchLineD.attribute("StartPoint"))
aLineDEndPoint = geomDataAPI_Point2D(aSketchLineD.attribute("EndPoint"))
aLineAStartPoint.setValue(0., 0.)
aLineAEndPoint.setValue(0., 100.)
aLineBStartPoint.setValue(0., 100.)
aLineBEndPoint.setValue(100., 100.)
aLineCStartPoint.setValue(100., 100.)
aLineCEndPoint.setValue(100., 0.)
aLineDStartPoint.setValue(100., 0.)
aLineDEndPoint.setValue(0., 0.)

aSession.finishOperation()
aSession.startOperation()

#=========================================================================
# Build a big box extrusion
#=========================================================================
aSketchResult = aQuadrangleSketchFeature.firstResult()
aSketchEdges = modelAPI_ResultConstruction(aSketchResult).shape()
origin = geomDataAPI_Point(aQuadrangleSketchFeature.attribute("Origin")).pnt()
dirX = geomDataAPI_Dir(aQuadrangleSketchFeature.attribute("DirX")).dir()
dirY = geomDataAPI_Dir(aQuadrangleSketchFeature.attribute("DirY")).dir()
norm = geomDataAPI_Dir(aQuadrangleSketchFeature.attribute("Norm")).dir()
aSketchFaces = ShapeList()
GeomAlgoAPI_SketchBuilder.createFaces(
    origin, dirX, dirY, norm, aSketchEdges, aSketchFaces)
# Create extrusion on them
aBox = aPart.addFeature("Extrusion")
aBox.selection("extrusion_face").setValue(
    aSketchResult, aSketchFaces[0])
aBox.real("extrusion_size").setValue(10)
aBox.boolean("extrusion_reverse").setValue(False)

aSession.finishOperation()


#=========================================================================
# Create a boolean cut of cylinders from the box: 
# result of Boolean is the first argument of the next Boolean
#=========================================================================
aCurrentResult = aBox.firstResult()
aSession.startOperation()
for i in xrange(0, N * N):
  aBooleanFt = aPart.addFeature("Boolean")
  aBooleanFt.reference("main_object").setValue(modelAPI_ResultBody(aCurrentResult))
  aBooleanFt.reference("tool_object").setValue(modelAPI_ResultBody(anExtrusions[i].firstResult()))
  kBooleanTypeCut = 0
  aBooleanFt.integer("bool_type").setValue(kBooleanTypeCut)
  aBooleanFt.execute()
  aCurrentResult = aBooleanFt.firstResult()
aSession.finishOperation()

#=========================================================================
# End of test
#=========================================================================
