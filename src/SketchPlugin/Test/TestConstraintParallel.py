"""
    TestConstraintParallel.py
    Unit test of SketchPlugin_ConstraintParallel class
        
    SketchPlugin_ConstraintParallel
        static const std::string MY_CONSTRAINT_PARALLEL_ID("SketchConstraintParallel");
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::type());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::type());
        data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::type());

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
# Create two lines which are not parallel
#=========================================================================
aDocument.startOperation()
aSketchReflist = aSketchFeatureData.reflist("Features")
# line A
aSketchLineA = aDocument.addFeature("SketchLine")
aSketchReflist.append(aSketchLineA)
aLineAStartPoint = geomDataAPI_Point2D(
    aSketchLineA.data().attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(
    aSketchLineA.data().attribute("EndPoint"))
aSketchLineB = aDocument.addFeature("SketchLine")
aLineAStartPoint.setValue(0., 25)
aLineAEndPoint.setValue(85., 25)
# line B
aSketchReflist.append(aSketchLineB)
aLineBStartPoint = geomDataAPI_Point2D(
    aSketchLineB.data().attribute("StartPoint"))
aLineBEndPoint = geomDataAPI_Point2D(
    aSketchLineB.data().attribute("EndPoint"))
aLineBStartPoint.setValue(0., 50)
aLineBEndPoint.setValue(80., 75)
aDocument.finishOperation()
#=========================================================================
# Make a constraint to keep the length of the line constant
# to parallel perpendicular constraint collapsing line to point
#=========================================================================
aDocument.startOperation()
aLengthConstraint = aDocument.addFeature("SketchConstraintLength")
aSketchReflist.append(aLengthConstraint)
aLengthConstraintData = aLengthConstraint.data()
refattrA = aLengthConstraintData.refattr("ConstraintEntityA")
aResultA = modelAPI_ResultConstruction(aSketchLineA.firstResult())
assert (aResultA is not None)
refattrA.setObject(aResultA)
aDocument.finishOperation()
# Coordinates of lines should not be changed after this constraint
assert (aLineAStartPoint.x() == 0)
assert (aLineAStartPoint.y() == 25)
assert (aLineAEndPoint.x() == 85)
assert (aLineAEndPoint.y() == 25)
assert (aLineBStartPoint.x() == 0)
assert (aLineBStartPoint.y() == 50)
assert (aLineBEndPoint.x() == 80)
assert (aLineBEndPoint.y() == 75)
#=========================================================================
# Link lines with parallel constraint
#=========================================================================
aDocument.startOperation()
aParallelConstraint = aDocument.addFeature("SketchConstraintParallel")
aSketchReflist.append(aParallelConstraint)
aConstraintData = aParallelConstraint.data()
refattrA = aConstraintData.refattr("ConstraintEntityA")
refattrB = aConstraintData.refattr("ConstraintEntityB")
# aResultA is already defined for the length constraint
aResultB = modelAPI_ResultConstruction(aSketchLineB.firstResult())
assert (aResultB is not None)
refattrA.setObject(aResultA)
refattrB.setObject(aResultB)
aDocument.finishOperation()
# print "Link lines with parallel constraint"
# print "assert (aLineAStartPoint.x() == %d)" % aLineAStartPoint.x()
# print "assert (aLineAStartPoint.y() == %d)" % aLineAStartPoint.y()
# print "assert (aLineAEndPoint.x() == %d)" % aLineAEndPoint.x()
# print "assert (aLineAEndPoint.y() == %d)" % aLineAEndPoint.y()
# print "assert (aLineBStartPoint.x() == %d)" % aLineBStartPoint.x()
# print "assert (aLineBStartPoint.y() == %d)" % aLineBStartPoint.y()
# print "assert (aLineBEndPoint.x() == %d)" % aLineBEndPoint.x()
# print "assert (aLineBEndPoint.y() == %d)" % aLineBEndPoint.y()
#=========================================================================
# Check values and move one constrainted object
#=========================================================================
deltaX = deltaY = 10.
# rotate line, check that reference's line points are moved also
aLineAStartPoint.setValue(aLineAStartPoint.x() + deltaX,
                          aLineAStartPoint.y() + deltaY)
aLineAEndPoint.setValue(aLineAEndPoint.x() - deltaX,
                        aLineAEndPoint.y() - deltaY)
# print "Rotate line, check that reference's line points are moved also"
# print "assert (aLineAStartPoint.x() == %d)" % aLineAStartPoint.x()
# print "assert (aLineAStartPoint.y() == %d)" % aLineAStartPoint.y()
# print "assert (aLineAEndPoint.x() == %d)" % aLineAEndPoint.x()
# print "assert (aLineAEndPoint.y() == %d)" % aLineAEndPoint.y()
# print "assert (aLineBStartPoint.x() == %d)" % aLineBStartPoint.x()
# print "assert (aLineBStartPoint.y() == %d)" % aLineBStartPoint.y()
# print "assert (aLineBEndPoint.x() == %d)" % aLineBEndPoint.x()
# print "assert (aLineBEndPoint.y() == %d)" % aLineBEndPoint.y()
#=========================================================================
# End of test
#=========================================================================
