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

# Create a part for extrusion
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

# Create circles
aSketchCircle = aSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("circle_center"))
aCircleRadius = aSketchCircle.real("circle_radius")
anCircleCentr.setValue(-25, 0)
aCircleRadius.setValue(50)
aSketchCircle = aSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("circle_center"))
aCircleRadius = aSketchCircle.real("circle_radius")
anCircleCentr.setValue(25, 0)
aCircleRadius.setValue(50)
aSession.finishOperation()
aSketchResult = aSketchFeature.firstResult()

#=========================================================================
# Make extrusion on sketch
#=========================================================================
# Create extrusion
aSession.startOperation()
anExtrusionFeature = aPart.addFeature("Extrusion")
anExtrusionFeature.selectionList("base").append(aSketchResult, None)
anExtrusionFeature.string("CreationMethod").setValue("BySizes")
anExtrusionFeature.real("to_size").setValue(50)
anExtrusionFeature.real("from_size").setValue(0)
anExtrusionFeature.real("to_offset").setValue(0) #TODO: remove
anExtrusionFeature.real("from_offset").setValue(0) #TODO: remove
anExtrusionFeature.execute()
aSession.finishOperation()
anExtrusionResult = modelAPI_ResultCompSolid(modelAPI_ResultBody(anExtrusionFeature.firstResult()))

#=========================================================================
# Make union on extrusion
#=========================================================================
aSession.startOperation()
aUnionFeature = aPart.addFeature("Union")
aUnionFeature.selectionList("base_objects").append(anExtrusionResult.subResult(0), None);
aUnionFeature.selectionList("base_objects").append(anExtrusionResult.subResult(1), None);
aUnionFeature.selectionList("base_objects").append(anExtrusionResult.subResult(2), None);
aSession.finishOperation()
assert (len(aUnionFeature.results()) > 0)
anUnionResult = modelAPI_ResultCompSolid(modelAPI_ResultBody(aUnionFeature.firstResult()))
assert (anUnionResult.numberOfSubs() == 0)

from salome.shaper import model
assert(model.checkPythonDump())
