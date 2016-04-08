"""
      Test1064.py
      Unit test for testing the Part sub-shapes naming, described in the issue 1064

"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

__updated__ = "2015-10-16"

aSession = ModelAPI_Session.get()

#=========================================================================
# Create a sketch triangle in PartSet
#=========================================================================
aPartSet = aSession.moduleDocument()
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPartSet.addFeature("Sketch"))
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
# Create lines
aLine1 = aSketchFeature.addFeature("SketchLine")
geomDataAPI_Point2D(aLine1.attribute("StartPoint")).setValue(0, 0)
geomDataAPI_Point2D(aLine1.attribute("EndPoint")).setValue(200, 0)
aLine2 = aSketchFeature.addFeature("SketchLine")
geomDataAPI_Point2D(aLine2.attribute("StartPoint")).setValue(200, 0)
geomDataAPI_Point2D(aLine2.attribute("EndPoint")).setValue(0, 200)
aLine3 = aSketchFeature.addFeature("SketchLine")
geomDataAPI_Point2D(aLine3.attribute("StartPoint")).setValue(0, 200)
geomDataAPI_Point2D(aLine3.attribute("EndPoint")).setValue(0, 0)
aSession.finishOperation()

#=========================================================================
# Create a part
#=========================================================================
aSession.startOperation()
aPartFeature = aPartSet.addFeature("Part")
aSession.finishOperation()
assert (len(aPartFeature.results()) == 1)
aPart = aSession.activeDocument()

#=========================================================================
# Make extrusion on triangle
#=========================================================================
aSketchResult = aSketchFeature.firstResult()
aSketchEdges = modelAPI_ResultConstruction(aSketchResult).shape()
aSketchFaces = ShapeList()
GeomAlgoAPI_SketchBuilder.createFaces(
    origin.pnt(), dirx.dir(), norm.dir(), aSketchEdges, aSketchFaces)
aSession.startOperation()
anExtrusionFt = aPart.addFeature("Extrusion")
anExtrusionFt.selectionList("base").append(aSketchResult, aSketchFaces[0])
anExtrusionFt.string("CreationMethod").setValue("BySizes")
anExtrusionFt.real("to_size").setValue(50)
anExtrusionFt.real("from_size").setValue(0)
aSession.finishOperation()


#=========================================================================
# Make a plane in PartSet on lateral face of the Extrusion
#=========================================================================
aSession.startOperation("Make Plane")
aSession.setActiveDocument(aPartSet)
aPlane = aPartSet.addFeature("Plane")
aPlane.string("CreationMethod").setValue("PlaneByFaceAndDistance")
aPlane.real("distance").setValue(0)
aPlane.selection("planeFace").selectSubShape("face", "Part_1/Extrusion_1_1/Generated_Face_3")
aSession.finishOperation()

#=========================================================================
# Update the sketch edges in order to update the plane on the lateral face automatically
#=========================================================================
aSession.startOperation("UpdateLine")
geomDataAPI_Point2D(aLine1.attribute("EndPoint")).setValue(400, 0)
geomDataAPI_Point2D(aLine2.attribute("StartPoint")).setValue(400, 0)
aSession.finishOperation()

#=========================================================================
# Check that the plane is also updated
#=========================================================================

assert(len(aPlane.results()) > 0)
aShape = aPlane.firstResult().shape()
aFace = GeomAPI_Face(aShape)
assert(aFace.isPlanar())
aPln = aFace.getPlane()
# Must be 0.4472135955, 0.894427191, 0.0
assert(aPln.direction().x() > 0.44)
assert(aPln.direction().x() < 0.45)
assert(aPln.direction().y() > 0.89)
assert(aPln.direction().y() < 0.90)
assert(aPln.direction().z() == 0.)

#=========================================================================
# End of test
#=========================================================================
