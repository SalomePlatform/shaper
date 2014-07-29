"""
    TestConstraintLength.py
    Unit test of SketchPlugin_ConstraintLength class
        
    SketchPlugin_ConstraintLength
        static const std::string MY_CONSTRAINT_LENGTH_ID("SketchConstraintLength");
        data()->addAttribute(SketchPlugin_Constraint::VALUE(),    ModelAPI_AttributeDouble::type());
        data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::type());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());

"""
from GeomDataAPI import *
from ModelAPI import *
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
# Create a line with length 100
#=========================================================================
aDocument.startOperation()
aSketchReflist = aSketchFeatureData.reflist("Features")
aSketchLineA = aDocument.addFeature("SketchLine")
aSketchReflist.append(aSketchLineA)
aLineAStartPoint = geomDataAPI_Point2D(
    aSketchLineA.data().attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(aSketchLineA.data().attribute("EndPoint"))
aLineAStartPoint.setValue(0., 25.)
aLineAEndPoint.setValue(100., 25.)
aDocument.finishOperation()
#=========================================================================
# Make a constraint to keep the length
#=========================================================================
aDocument.startOperation()
aCoincidenceConstraint = aDocument.addFeature("SketchConstraintLength")
aSketchReflist.append(aCoincidenceConstraint)
aConstraintData = aCoincidenceConstraint.data()
aLength = aConstraintData.real("ConstraintValue")
refattrA = aConstraintData.refattr("ConstraintEntityA")
assert (not aLength.isInitialized())
assert (not refattrA.isInitialized())
# aLength.setValue(100.)
# refattrA.setObject(aSketchLineA)
aResult = aSketchLineA.firstResult()
assert (aResult is not None)
refattrA.setObject(modelAPI_ResultConstruction(aResult))
aDocument.finishOperation()
assert (aLength.isInitialized())
assert (refattrA.isInitialized())
#=========================================================================
# Check values and move one constrainted object
#=========================================================================
deltaX = 40.
# move start point, check that end point are moved also
assert (aLineAStartPoint.x() == 0)
assert (aLineAStartPoint.y() == 25)
assert (aLineAEndPoint.x() == 100)
assert (aLineAEndPoint.y() == 25)
aDocument.startOperation()
aLineAStartPoint.setValue(aLineAStartPoint.x() + deltaX,
                          aLineAStartPoint.y())
aDocument.finishOperation()
assert (aLineAStartPoint.y() == 25)
assert (aLineAEndPoint.y() == 25)
# length of the line is the same
assert (aLineAEndPoint.x() - aLineAStartPoint.x() == 100)
#=========================================================================
# TODO: improve test
# 1. remove constraint, move line's start point to
#    check that constraint are not applied
#=========================================================================
#=========================================================================
# End of test
#=========================================================================
