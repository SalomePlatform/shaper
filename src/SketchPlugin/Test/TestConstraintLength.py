"""
    TestConstraintLength.py
    Unit test of SketchPlugin_ConstraintLength class
        
    SketchPlugin_ConstraintLength
        static const std::string MY_CONSTRAINT_LENGTH_ID("SketchConstraintLength");
        data()->addAttribute(SketchPlugin_Constraint::VALUE(),    ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());

"""
from GeomDataAPI import *
from ModelAPI import *
#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2014-10-28"

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
#=========================================================================
# Creation of a sketch
#=========================================================================
aSession.startOperation()
aSketchCommonFeature = aDocument.addFeature("Sketch")
aSketchFeature = featureToCompositeFeature(aSketchCommonFeature)
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
diry = geomDataAPI_Dir(aSketchFeature.attribute("DirY"))
diry.setValue(0, 1, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSession.finishOperation()
#=========================================================================
# Create a line with length 100
#=========================================================================
aSession.startOperation()
aSketchLineA = aSketchFeature.addFeature("SketchLine")
aLineAStartPoint = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(aSketchLineA.attribute("EndPoint"))
aLineAStartPoint.setValue(0., 25.)
aLineAEndPoint.setValue(100., 25.)
aSession.finishOperation()
#=========================================================================
# Make a constraint to keep the length
#=========================================================================
aSession.startOperation()
aLengthConstraint = aSketchFeature.addFeature("SketchConstraintLength")
refattrA = aLengthConstraint.refattr("ConstraintEntityA")
aLength = aLengthConstraint.real("ConstraintValue")
assert (not refattrA.isInitialized())
assert (not aLength.isInitialized())

aResult = aSketchLineA.firstResult()
assert (aResult is not None)
refattrA.setObject(modelAPI_ResultConstruction(aResult))
# aLength.setValue(100.)
aLengthConstraint.execute()
aSession.finishOperation()
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
aSession.startOperation()
aLineAStartPoint.setValue(aLineAStartPoint.x() + deltaX,
                          aLineAStartPoint.y())
aSession.finishOperation()
assert (aLineAStartPoint.y() == 25)
assert (aLineAEndPoint.y() == 25)
# length of the line is the same
assert (aLineAEndPoint.x() - aLineAStartPoint.x() == 100)
#=========================================================================
# Change the length value of the constraint
#=========================================================================
aSession.startOperation()
aLength.setValue(140.)
aLengthConstraint.execute()
aSession.finishOperation()
assert (aLineAEndPoint.x() - aLineAStartPoint.x() == 140)
#=========================================================================
# TODO: improve test
# 1. remove constraint, move line's start point to
#    check that constraint are not applied
#=========================================================================
#=========================================================================
# End of test
#=========================================================================
