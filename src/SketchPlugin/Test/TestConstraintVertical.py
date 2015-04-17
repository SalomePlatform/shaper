"""
    TestConstraintVertical.py
    Unit test of SketchPlugin_ConstraintVertical class
        
    SketchPlugin_ConstraintVertical
        static const std::string MY_CONSTRAINT_VERTICAL_ID("SketchConstraintVertical");
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());

"""
from GeomDataAPI import *
from ModelAPI import *
#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2015-03-16"

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
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSession.finishOperation()
#=========================================================================
# Create non-vertical line
#=========================================================================
aSession.startOperation()
aSketchLine = aSketchFeature.addFeature("SketchLine")
aLineStartPoint = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
aLineEndPoint = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
aLineStartPoint.setValue(0., 15.)
aLineEndPoint.setValue(20., 25.)
aSession.finishOperation()
#=========================================================================
# Assign vertical constraint for a line
#=========================================================================
aSession.startOperation()
aVerticalConstraint = aSketchFeature.addFeature("SketchConstraintVertical")
refattrA = aVerticalConstraint.refattr("ConstraintEntityA")
aResult = modelAPI_ResultConstruction(aSketchLine.firstResult())
assert (aResult is not None)
refattrA.setObject(aResult)
aVerticalConstraint.execute()
aSession.finishOperation()
assert(aLineStartPoint.x() == aLineEndPoint.x())
#=========================================================================
# Move one of boundary points of a line
#=========================================================================
deltaX = deltaY = 10.
aSession.startOperation()
aLineStartPoint.setValue(aLineStartPoint.x() + deltaX,
                         aLineStartPoint.y() + deltaY)
aSession.finishOperation()
assert(aLineStartPoint.x() == aLineEndPoint.x())
#=========================================================================
# Move other boundary point of a line
#=========================================================================
deltaX = -3.
deltaY = -10.
aSession.startOperation()
aLineEndPoint.setValue(aLineEndPoint.x() + deltaX,
                       aLineEndPoint.y() + deltaY)
aSession.finishOperation()
assert(aLineStartPoint.x() == aLineEndPoint.x())
#=========================================================================
# End of test
#=========================================================================
