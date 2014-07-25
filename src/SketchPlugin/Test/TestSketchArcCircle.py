"""
    TestSketchArcCircle.py
    
    static const std::string MY_SKETCH_ARC_ID("SketchArc");
    static const std::string MY_CENTER_ID = "ArcCenter";
    static const std::string MY_START_ID = "ArcStartPoint";
    static const std::string MY_END_ID = "ArcEndPoint";
    data()->addAttribute(SketchPlugin_Arc::CENTER_ID(), GeomDataAPI_Point2D::type());
    data()->addAttribute(SketchPlugin_Arc::START_ID(),  GeomDataAPI_Point2D::type());
    data()->addAttribute(SketchPlugin_Arc::END_ID(),    GeomDataAPI_Point2D::type());
    
    static const std::string MY_CIRCLE_ID("SketchCircle");
    static const std::string MY_CIRCLE_CENTER_ID("CircleCenter");
    static const std::string MY_CIRCLE_RADIUS_ID("CircleRadius");
    data()->addAttribute(SketchPlugin_Circle::CENTER_ID(), GeomDataAPI_Point2D::type());
    data()->addAttribute(SketchPlugin_Circle::RADIUS_ID(), ModelAPI_AttributeDouble::type());
"""

#=========================================================================
# Initialization of the test
#=========================================================================
from GeomDataAPI import *
from ModelAPI import *

__updated__ = "2014-07-25"

aPluginManager = ModelAPI_PluginManager.get()
aDocument = aPluginManager.rootDocument()
#=========================================================================
# Creation of a sketch
#=========================================================================
aDocument.startOperation()
aSketchFeature = aDocument.addFeature("Sketch")
aSketchFeatureData = aSketchFeature.data()
origin = geomDataAPI_Point(aSketchFeatureData.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeatureData.attribute("DirX"))
dirx.setValue(1, 0, 0)
diry = geomDataAPI_Dir(aSketchFeatureData.attribute("DirY"))
diry.setValue(0, 1, 0)
norm = geomDataAPI_Dir(aSketchFeatureData.attribute("Norm"))
norm.setValue(0, 0, 1)
aDocument.finishOperation()
#=========================================================================
# Creation of an arc
# 1. Test SketchPlugin_Arc attributes
# 2. 
#=========================================================================
aDocument.startOperation()
aSketchReflist = aSketchFeatureData.reflist("Features")
assert (not aSketchReflist.isInitialized())
assert (aSketchReflist.size() == 0)
assert (len(aSketchReflist.list()) == 0)
aSketchArc = aDocument.addFeature("SketchArc")
assert (aSketchArc.getKind() == "SketchArc")
aSketchReflist.append(aSketchArc)
aSketchArcData = aSketchArc.data()
anArcCentr = geomDataAPI_Point2D(aSketchArcData.attribute("ArcCenter"))
assert (anArcCentr.x() == 0)
assert (anArcCentr.y() == 0)
assert (not anArcCentr.isInitialized())
anArcCentr.setValue(10.,10.)
anArcStartPoint = geomDataAPI_Point2D(aSketchArcData.attribute("ArcStartPoint"))
assert (anArcStartPoint.x() == 0)
assert (anArcStartPoint.y() == 0)
assert (not anArcStartPoint.isInitialized())
anArcStartPoint.setValue(0., 50.)
anArcEndPoint = geomDataAPI_Point2D(aSketchArcData.attribute("ArcEndPoint"))
assert (anArcEndPoint.x() == 0)
assert (anArcEndPoint.y() == 0)
assert (not anArcEndPoint.isInitialized())
anArcEndPoint.setValue(50., 0.)
aDocument.finishOperation()
# check that values have been changed
aSketchReflist = aSketchFeatureData.reflist("Features")
assert (aSketchReflist.size() == 1)
assert (len(aSketchReflist.list()) == 1)
assert (anArcCentr.x() == 10.0)
assert (anArcCentr.y() == 10.0)
assert (anArcStartPoint.x() == 0.0)
assert (anArcStartPoint.y() == 50.0)
assert (anArcEndPoint.x() == 50.0)
assert (anArcEndPoint.y() == 0.0)
#===============================================================================
# Edit the arc:
# 1. Move whole arc
# 2. Change the start point 
#===============================================================================
aDocument.startOperation()
deltaX, deltaY = 5., 10.
anArcCentr.setValue(anArcCentr.x() + deltaX, anArcCentr.y() + deltaY)
anArcStartPoint.setValue(anArcStartPoint.x() + deltaX, anArcStartPoint.y() + deltaY)
anArcEndPoint.setValue(anArcEndPoint.x() + deltaX, anArcEndPoint.y() + deltaY)
aDocument.finishOperation()
assert (anArcCentr.x() == 15)
assert (anArcCentr.y() == 20)
assert (anArcStartPoint.x() == 5)
assert (anArcStartPoint.y() == 60)
assert (anArcEndPoint.x() == 55)
assert (anArcEndPoint.y() == 10)
# Change the start point
aDocument.startOperation()
anArcStartPoint.setValue(anArcStartPoint.x() + deltaX, anArcStartPoint.y())
aPrevEndPointX = anArcEndPoint.x()
aPrevEndPointY = anArcEndPoint.y()
aDocument.finishOperation()
assert (anArcCentr.x() == 15)
assert (anArcCentr.y() == 20)
assert (anArcStartPoint.x() == 10)
assert (anArcStartPoint.y() == 60)
assert (anArcEndPoint.x() != aPrevEndPointX)
assert (anArcEndPoint.y() != aPrevEndPointY)
#=========================================================================
# Check results of the Arc 
#=========================================================================
aResult = aSketchArc.firstResult()
aResultConstruction = modelAPI_ResultConstruction(aResult)
aShape = aResultConstruction.shape()
assert (aShape is not None)
assert (not aShape.isNull())
#===============================================================================
# Create a circle
# 1. Test SketchPlugin_Circle.h attributes
# 2. ModelAPI_AttributeDouble attribute
#===============================================================================
aDocument.startOperation()
aSketchReflist = aSketchFeatureData.reflist("Features")
# Arc is already added
assert (aSketchReflist.size() == 1)
assert (len(aSketchReflist.list()) == 1)
aSketchCircle = aDocument.addFeature("SketchCircle")
assert (aSketchCircle.getKind() == "SketchCircle")
aSketchReflist.append(aSketchCircle)
aSketchCircleData = aSketchCircle.data()
anCircleCentr = geomDataAPI_Point2D(aSketchCircleData.attribute("CircleCenter"))
assert (anCircleCentr.x() == 0)
assert (anCircleCentr.y() == 0)
assert (not anCircleCentr.isInitialized())
aCircleRadius = aSketchCircleData.real("CircleRadius");
assert (type(aCircleRadius) == ModelAPI_AttributeDouble)
# ModelAPI_AttributeDouble.type() is checked in ModelAPI_TestConstants
assert (aCircleRadius.attributeType() == ModelAPI_AttributeDouble.type())
assert (aCircleRadius.value() == 0)
anCircleCentr.setValue(-25., -25)
aCircleRadius.setValue(25.)
assert (anCircleCentr.x() == -25)
assert (anCircleCentr.y() == -25)
assert (aCircleRadius.value() == 25)
aDocument.finishOperation()
#===============================================================================
# Edit the Cricle
# 1. Check that changing the centr of a circle does not affects radius
# 2. and vise versa; also check that int is acceptable as well as a real
#===============================================================================
aDocument.startOperation()
anCircleCentr.setValue(10, 60)
aDocument.finishOperation()
assert (anCircleCentr.x() == 10)
assert (anCircleCentr.y() == 60)
assert (aCircleRadius.value() == 25)
aDocument.startOperation()
aCircleRadius.setValue(int(20))
aDocument.finishOperation()
assert (anCircleCentr.x() == 10)
assert (anCircleCentr.y() == 60)
assert (aCircleRadius.value() == 20)
#===============================================================================
# End of test
#===============================================================================
