"""
    TestConstraintRadius.py
    Unit test of SketchPlugin_ConstraintRadius class
    
    SketchPlugin_Constraint
        static const std::string MY_CONSTRAINT_VALUE("ConstraintValue");
        static const std::string MY_FLYOUT_VALUE_PNT("ConstraintFlyoutValuePnt");
        static const std::string MY_ENTITY_A("ConstraintEntityA");
        static const std::string MY_ENTITY_B("ConstraintEntityB");
        static const std::string MY_ENTITY_C("ConstraintEntityC");
        static const std::string MY_ENTITY_D("ConstraintEntityD");
        
    SketchPlugin_ConstraintRadius
        static const std::string MY_CONSTRAINT_RADIUS_ID("SketchConstraintRadius");
        data()->addAttribute(SketchPlugin_Constraint::VALUE(),    ModelAPI_AttributeDouble::type());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
        data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::type());

"""
from GeomDataAPI import *
from ModelAPI import *
import math
#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2014-07-29"

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
# Creation of an arc and a circle
#=========================================================================
aDocument.startOperation()
aSketchReflist = aSketchFeatureData.reflist("Features")
aSketchArc = aDocument.addFeature("SketchArc")
aSketchReflist.append(aSketchArc)
aSketchArcData = aSketchArc.data()
anArcCentr = geomDataAPI_Point2D(aSketchArcData.attribute("ArcCenter"))
anArcCentr.setValue(10., 10.)
anArcStartPoint = geomDataAPI_Point2D(
    aSketchArcData.attribute("ArcStartPoint"))
anArcStartPoint.setValue(0., 50.)
anArcEndPoint = geomDataAPI_Point2D(aSketchArcData.attribute("ArcEndPoint"))
anArcEndPoint.setValue(50., 0.)
aDocument.finishOperation()
# Circle
aDocument.startOperation()
aSketchCircle = aDocument.addFeature("SketchCircle")
aSketchReflist.append(aSketchCircle)
aSketchCircleData = aSketchCircle.data()
anCircleCentr = geomDataAPI_Point2D(
    aSketchCircleData.attribute("CircleCenter"))
aCircleRadius = aSketchCircleData.real("CircleRadius")
anCircleCentr.setValue(-25., -25)
aCircleRadius.setValue(25.)
aDocument.finishOperation()
#=========================================================================
# Make a constraint to keep the radius of the arc
#=========================================================================
aDocument.startOperation()
aConstraint = aDocument.addFeature("SketchConstraintRadius")
aSketchReflist.append(aConstraint)
aConstraintData = aConstraint.data()
aRadius = aConstraintData.real("ConstraintValue")
aRefObject = aConstraintData.refattr("ConstraintEntityA")
aResult = aSketchArc.firstResult()
assert (aResult is not None)
aRefObject.setObject(modelAPI_ResultConstruction(aResult))
aDocument.finishOperation()
assert (aRadius.isInitialized())
assert (aRefObject.isInitialized())
#=========================================================================
# Make a constraint to keep the radius of the circle
#=========================================================================
aDocument.startOperation()
aConstraint = aDocument.addFeature("SketchConstraintRadius")
aSketchReflist.append(aConstraint)
aConstraintData = aConstraint.data()
aRadius = aConstraintData.real("ConstraintValue")
aRefObject = aConstraintData.refattr("ConstraintEntityA")
aResult = aSketchCircle.firstResult()
assert (aResult is not None)
aRefObject.setObject(modelAPI_ResultConstruction(aResult))
aDocument.finishOperation()
assert (aRadius.isInitialized())
assert (aRefObject.isInitialized())
#=========================================================================
# Perform some actions and checks:
# 1. Check that constraints does not changes values
# 2. Move one point of the arc
# 3. Check that second point is moved also
#=========================================================================
assert (anArcCentr.x() == 10.)
assert (anArcCentr.y() == 10.)
assert (anArcStartPoint.x() == 0.)
assert (anArcStartPoint.y() == 50.)
anArcPrevEndPointX = anArcEndPoint.x()
anArcPrevEndPointY = anArcEndPoint.y()
assert (anArcPrevEndPointX == 50.)
assert (anArcPrevEndPointY == 0.)
# Move one point of the arc
aDocument.startOperation()
anArcStartPoint.setValue(0., 60)
aDocument.finishOperation()
assert (anArcCentr.x() == 10.)
assert (anArcCentr.y() == 10.)
assert (anArcEndPoint.x() != anArcPrevEndPointX)
assert (anArcEndPoint.y() != anArcPrevEndPointY)
#=========================================================================
# 4. Move the centr or the point of the arc
# 5. Check radius is the same
#=========================================================================
assert (anCircleCentr.x() == -25)
assert (anCircleCentr.y() == -25)
assert (aCircleRadius.value() == 25)
aDocument.startOperation()
anCircleCentr.setValue(100., 100.)
aDocument.finishOperation()
assert (anCircleCentr.x() == 100)
assert (anCircleCentr.y() == 100)
assert (aCircleRadius.value() == 25)
#=========================================================================
# End of test
#=========================================================================
