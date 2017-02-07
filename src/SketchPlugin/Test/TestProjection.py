"""
    TestProjection.py
    Unit test of SketchPlugin_Projection class

"""
from GeomDataAPI import *
from ModelAPI import *
import math
#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2016-05-16"


#=========================================================================
# Start of test
#=========================================================================
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
#=========================================================================
# Creation of a sketch
#=========================================================================
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aDocument.addFeature("Sketch"))
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSession.finishOperation()
#=========================================================================
# Create a line, circle and arc
#=========================================================================
aSession.startOperation()
aLine = aSketchFeature.addFeature("SketchLine")
aLineStart = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
aLineEnd = geomDataAPI_Point2D(aLine.attribute("EndPoint"))
aLineStart.setValue(10., 10.)
aLineEnd.setValue(40., 30.)

aCircle = aSketchFeature.addFeature("SketchCircle")
aCircleCenter = geomDataAPI_Point2D(aCircle.attribute("CircleCenter"))
aCircleRadius = aCircle.real("CircleRadius")
aCircleCenter.setValue(-25., -25)
aCircleRadius.setValue(25.)

anArc = aSketchFeature.addFeature("SketchArc")
anArcCenter = geomDataAPI_Point2D(anArc.attribute("ArcCenter"))
anArcStart = geomDataAPI_Point2D(anArc.attribute("ArcStartPoint"))
anArcEnd = geomDataAPI_Point2D(anArc.attribute("ArcEndPoint"))
anArcCenter.setValue(10., 10.)
anArcStart.setValue(50., 0.)
anArcEnd.setValue(0., 50.)
aSession.finishOperation()
#=========================================================================
# Create another sketch
#=========================================================================
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aDocument.addFeature("Sketch"))
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 10)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSession.finishOperation()
#=========================================================================
# Project all features onto the new sketch
#=========================================================================
aSession.startOperation()
aLineProjector = aSketchFeature.addFeature("SketchProjection")
aLineProjector.selection("ExternalFeature").selectSubShape("EDGE", "Sketch_1/Edge-SketchLine_1")
aLineProjector.execute()

aCircleProjector = aSketchFeature.addFeature("SketchProjection")
aCircleProjector.selection("ExternalFeature").selectSubShape("EDGE", "Sketch_1/Edge-SketchCircle_1_2")
aCircleProjector.execute()

anArcProjector = aSketchFeature.addFeature("SketchProjection")
anArcProjector.selection("ExternalFeature").selectSubShape("EDGE", "Sketch_1/Edge-SketchArc_1_2")
anArcProjector.execute()
aSession.finishOperation()
#=========================================================================
# Check projection coordinates
#=========================================================================
aProjLine = ModelAPI_Feature.feature(aLineProjector.refattr("ProjectedFeature").object())
assert(aProjLine)
aProjLineStart = geomDataAPI_Point2D(aProjLine.attribute("StartPoint"))
aProjLineEnd = geomDataAPI_Point2D(aProjLine.attribute("EndPoint"))
assert(math.fabs(aProjLineStart.x() - aLineStart.x()) < 1.e-10)
assert(math.fabs(aProjLineStart.y() - aLineStart.y()) < 1.e-10)
assert(math.fabs(aProjLineEnd.x() - aLineEnd.x()) < 1.e-10)
assert(math.fabs(aProjLineEnd.y() - aLineEnd.y()) < 1.e-10)

aProjCircle = ModelAPI_Feature.feature(aCircleProjector.refattr("ProjectedFeature").object())
assert(aProjCircle)
aProjCircleCenter = geomDataAPI_Point2D(aProjCircle.attribute("CircleCenter"))
aProjCircleRadius = aProjCircle.real("CircleRadius")
assert(math.fabs(aProjCircleCenter.x() - aCircleCenter.x()) < 1.e-10)
assert(math.fabs(aProjCircleCenter.y() - aCircleCenter.y()) < 1.e-10)
assert(math.fabs(aProjCircleRadius.value() - aCircleRadius.value()) < 1.e-10)

aProjArc = ModelAPI_Feature.feature(anArcProjector.refattr("ProjectedFeature").object())
aProjArcCenter = geomDataAPI_Point2D(aProjArc.attribute("ArcCenter"))
aProjArcStart = geomDataAPI_Point2D(aProjArc.attribute("ArcStartPoint"))
aProjArcEnd = geomDataAPI_Point2D(aProjArc.attribute("ArcEndPoint"))
assert(math.fabs(aProjArcCenter.x() - anArcCenter.x()) < 1.e-10)
assert(math.fabs(aProjArcCenter.y() - anArcCenter.y()) < 1.e-10)
assert(math.fabs(aProjArcStart.x() - anArcStart.x()) < 1.e-10)
assert(math.fabs(aProjArcStart.y() - anArcStart.y()) < 1.e-10)
assert(math.fabs(aProjArcEnd.x() - anArcEnd.x()) < 1.e-10)
assert(math.fabs(aProjArcEnd.y() - anArcEnd.y()) < 1.e-10)
#=========================================================================
# Move original feature and check the projection is agreed
#=========================================================================
aSession.startOperation()
aLineStart.setValue(20., 0.)
aSession.finishOperation()
assert(math.fabs(aProjLineStart.x() - aLineStart.x()) < 1.e-10)
assert(math.fabs(aProjLineStart.y() - aLineStart.y()) < 1.e-10)
assert(math.fabs(aProjLineEnd.x() - aLineEnd.x()) < 1.e-10)
assert(math.fabs(aProjLineEnd.y() - aLineEnd.y()) < 1.e-10)
#=========================================================================
# End of test
#=========================================================================

#from salome.shaper import model
#assert(model.checkPythonDump())
