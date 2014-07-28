"""
    TestConstraintCoincidence.py
    Unit test of SketchPlugin_ConstraintCoincidence class
    
    SketchPlugin_Constraint
        static const std::string MY_CONSTRAINT_VALUE("ConstraintValue");
        static const std::string MY_FLYOUT_VALUE_PNT("ConstraintFlyoutValuePnt");
        static const std::string MY_ENTITY_A("ConstraintEntityA");
        static const std::string MY_ENTITY_B("ConstraintEntityB");
        static const std::string MY_ENTITY_C("ConstraintEntityC");
        static const std::string MY_ENTITY_D("ConstraintEntityD");
        
    SketchPlugin_ConstraintCoincidence
        static const std::string MY_CONSTRAINT_COINCIDENCE_ID("SketchConstraintCoincidence");
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::type());

"""
from GeomDataAPI import *
from ModelAPI import *
#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2014-07-28"

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
# Create a line and an arc
#=========================================================================
aDocument.startOperation()
aSketchReflist = aSketchFeatureData.reflist("Features")
aSketchArc = aDocument.addFeature("SketchArc")
aSketchReflist.append(aSketchArc)
aSketchArcData = aSketchArc.data()
anArcCentr = geomDataAPI_Point2D(aSketchArcData.attribute("ArcCenter"))
anArcStartPoint = geomDataAPI_Point2D(
    aSketchArcData.attribute("ArcStartPoint"))
anArcEndPoint = geomDataAPI_Point2D(aSketchArcData.attribute("ArcEndPoint"))
aSketchFeature = aDocument.addFeature("SketchConstraintCoincidence")
anArcCentr.setValue(10., 10.)
anArcStartPoint.setValue(0., 50.)
anArcEndPoint.setValue(50., 0.)
aSketchLine = aDocument.addFeature("SketchLine")
aSketchReflist.append(aSketchLine)
aSketchLineData = aSketchLine.data()
aLineStartPoint = geomDataAPI_Point2D(aSketchLineData.attribute("StartPoint"))
aLineEndPoint = geomDataAPI_Point2D(aSketchLineData.attribute("EndPoint"))
# Lets initialize line start at circle's end:
aLineStartPoint.setValue(50., 0.)
aLineEndPoint.setValue(100., 25.)
aDocument.finishOperation()
#=========================================================================
# Link arc's end and line's start points with concidence constraint
#=========================================================================
aDocument.startOperation()
aConstraint = aDocument.addFeature("SketchConstraintCoincidence")
aSketchReflist.append(aConstraint)
aConstraintData = aConstraint.data()
reflistA = aConstraintData.refattr("ConstraintEntityA")
reflistB = aConstraintData.refattr("ConstraintEntityB")
reflistA.setAttr(anArcEndPoint)
reflistB.setAttr(aLineStartPoint)
aDocument.finishOperation()
#=========================================================================
# Check values and move one constrainted object
#=========================================================================
assert (anArcEndPoint.x() == 50)
assert (anArcEndPoint.y() == 0)
assert (aLineStartPoint.x() == 50)
assert (aLineStartPoint.y() == 0)
deltaX = deltaY = 40.
#  move line
aDocument.startOperation()
aLineStartPoint.setValue(aLineStartPoint.x() + deltaX,
                         aLineStartPoint.y() + deltaY)
aLineEndPoint.setValue(aLineEndPoint.x() + deltaX,
                       aLineEndPoint.y() + deltaY)
aDocument.finishOperation()
# check that arc's points are moved also
assert (anArcEndPoint.x() == aLineStartPoint.x())
assert (anArcEndPoint.y() == aLineStartPoint.y())
#=========================================================================
# TODO: improve test
# 1. remove constraint, move line to check that constraint are not applied
# 2. make a new constraint when the points are distanced from each other,
#    check that one from constrainted objects has moved
#=========================================================================
#=========================================================================
# End of test
#=========================================================================
