"""
    UnitTestBox.py
    Unit Test of PrimitivesPlugin_Box class

class PrimitivesPlugin_Box : public ModelAPI_Feature
    static const std::string MY_BOX_ID("Box");
    static const std::string METHOD_ATTR("CreationMethod");
    static const std::string MY_POINT_FIRST("FirstPoint");
    static const std::string MY_POINT_SECOND("SecondPoint");
    static const std::string MY_DX("dx");
    static const std::string MY_DY("dy");
    static const std::string MY_DZ("dz");

    data()->addAttribute(METHOD(), ModelAPI_AttributeString::typeId());
    data()->addAttribute(POINT_FIRST(), ModelAPI_AttributeSelection::typeId());
    data()->addAttribute(POINT_SECOND(), ModelAPI_AttributeSelection::typeId());
    data()->addAttribute(DX(), ModelAPI_AttributeDouble::typeId());
    data()->addAttribute(DY(), ModelAPI_AttributeDouble::typeId());
    data()->addAttribute(DZ(), ModelAPI_AttributeDouble::typeId());

"""

#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *
import math

__updated__ = "2016-01-04"

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
# Create a part for creation of a box
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
assert (len(aPartFeature.results()) == 1)

aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()
#=========================================================================
# Creation of a Box by coordinates
#=========================================================================
aSession.startOperation()
aBoxBy3Dims = aPart.addFeature("Box")
assert (aBoxBy3Dims.getKind() == "Box")

aBoxBy3Dims.string("CreationMethod").setValue("BoxByDimensions")
aBoxBy3Dims.real("dx").setValue(1.6)
aBoxBy3Dims.real("dy").setValue(1.6)
aBoxBy3Dims.real("dz").setValue(1.6)
aBoxBy3Dims.execute()

# Check box results
assert (len(aBoxBy3Dims.results()) > 0)
aBoxResult = modelAPI_ResultBody(aBoxBy3Dims.firstResult())
assert (aBoxResult is not None)

# Check box volume
aRefVolume = 1.6 * 1.6 * 1.6
aResVolume = GeomAlgoAPI_ShapeTools_volume(aBoxResult.shape())
assert (math.fabs(aResVolume - aRefVolume) < 10 ** -5)


#Check the naming by selecting a face and making a plane out of it
aPlaneTop = aPart.addFeature("Plane")
assert(aPlaneTop.getKind() == "Plane")
aPlaneTop.string("creation_method").setValue("by_other_plane")
aSelectionAttr = aPlaneTop.selection("plane")
aSelectionAttr.selectSubShape("face", "Box_1_1/Top_1")
aPlaneTop.string("by_other_plane_option").setValue("by_distance_from_other")
aPlaneTop.real("distance").setValue(0.4)
aPlaneTop.execute()

#The face should be at 1.6, so the plane should be at 1.6 + 0.4 = 2.
aRefPlaneTopLocation = 2.

#location() is a method from GeomAPI_Face that returns a GeomAPI_Pnt, from which we can extract the z coordinate
aPlaneTopResult = aPlaneTop.firstResult()
aPlaneTopFace = GeomAPI_Face(aPlaneTopResult.shape())
aPlaneTestLocation = aPlaneTopFace.getPlane()
aPlaneLocation = aPlaneTestLocation.location().z()
assert(math.fabs(aPlaneLocation - aRefPlaneTopLocation) < 10 ** -5)

aSession.finishOperation()

#=========================================================================
# Creation of a Box by two points
#=========================================================================

aSession.startOperation()

#Create two points
aPoint1 = aPart.addFeature("Point")
aPoint2 = aPart.addFeature("Point")
assert(aPoint1.getKind() == "Point")
assert(aPoint2.getKind() == "Point")
aPoint1.string("creation_method").setValue("by_xyz")
aPoint1.real("x").setValue(2.0)
aPoint1.real("y").setValue(2.0)
aPoint1.real("z").setValue(2.0)
aPoint2.string("creation_method").setValue("by_xyz")
aPoint2.real("x").setValue(2.5)
aPoint2.real("y").setValue(2.5)
aPoint2.real("z").setValue(2.5)
aPoint1.execute()
aPoint2.execute()
aPoint1Result = aPoint1.firstResult()
aPoint2Result = aPoint2.firstResult()
aPoint1Vertex = aPoint1Result.shape()
aPoint2Vertex = aPoint2Result.shape()

aBoxBy2Pts = aPart.addFeature("Box")
assert (aBoxBy2Pts.getKind() == "Box")
aBoxBy2Pts.string("CreationMethod").setValue("BoxByTwoPoints")
aBoxBy2Pts.selection("FirstPoint").setValue(aPoint1Result, aPoint1Vertex)
aBoxBy2Pts.selection("SecondPoint").setValue(aPoint2Result, aPoint2Vertex)
aBoxBy2Pts.execute()

# Check box volume
aBoxResult2 = modelAPI_ResultBody(aBoxBy2Pts.firstResult())
aRefVolume2 = 0.5 * 0.5 * 0.5
aResVolume2 = GeomAlgoAPI_ShapeTools_volume(aBoxResult2.shape())
assert (math.fabs(aResVolume2 - aRefVolume2) < 10 ** -5)

#Check the naming by selecting a face and making a plane out of it
aPlaneRight = aPart.addFeature("Plane")
assert(aPlaneRight.getKind() == "Plane")
aPlaneRight.string("creation_method").setValue("by_other_plane")
aSelectionAttr = aPlaneRight.selection("plane")
aSelectionAttr.selectSubShape("face", "Box_2_1/Right_1")
aPlaneRight.string("by_other_plane_option").setValue("by_distance_from_other")
aPlaneRight.real("distance").setValue(0.5)
aPlaneRight.execute()

#The face should be at 2.5, so the plane should be at 2.5 + 0.5 = 3.
aRefPlaneRightLocation = 3.

#location() is a method from GeomAPI_Face that returns a GeomAPI_Pnt,
#from which we can extract the y coordinate
aPlaneRightResult = aPlaneRight.firstResult()
aPlaneRightFace = GeomAPI_Face(aPlaneRightResult.shape())
aPlaneTestLocation = aPlaneRightFace.getPlane()
aPlaneLocation = aPlaneTestLocation.location().y()
assert(math.fabs(aPlaneLocation - aRefPlaneRightLocation) < 10 ** -5)

aSession.finishOperation()

#=========================================================================
# End of test
#=========================================================================
