"""
    TestArcBehavior.py

    It tests behavior of the arc during the changing of parameters to avoid
    regressions in this complicated part.

"""

#=========================================================================
# of the test
#=========================================================================
from GeomDataAPI import *
from ModelAPI import *
import math

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
#=========================================================================
# Creation of a sketch
#=========================================================================
aSession.startOperation()
#aSketchFeature = aDocument.addFeature("Sketch")
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
# Test creation of arc and movements (of mouse coursor) of end point near to the start point:
# edge must be the minimal length.
#=========================================================================
aSession.startOperation()
aSketchArc = aSketchFeature.addFeature("SketchArc")
anArcCentr = geomDataAPI_Point2D(aSketchArc.attribute("center_point"))
anArcCentr.setValue(0., 0.)
anArcStartPoint = geomDataAPI_Point2D(aSketchArc.attribute("start_point"))
anArcStartPoint.setValue(0., 10.)
anArcEndPoint = geomDataAPI_Point2D(aSketchArc.attribute("end_point"))
anArcEndPoint.setValue(1., 10.)
aSession.finishOperation()
# the arc must be small, not near to the whole circle
assert shapeToEdge(aSketchArc.lastResult().shape()).length() < 2.
# move to the half-circle
aSession.startOperation()
anArcEndPoint.setValue(10., 0.)
aSession.finishOperation()
assert shapeToEdge(aSketchArc.lastResult().shape()).length() < 16.
aSession.startOperation()
anArcEndPoint.setValue(0., -10.)
aSession.finishOperation()
assert shapeToEdge(aSketchArc.lastResult().shape()).length() > 31.
assert shapeToEdge(aSketchArc.lastResult().shape()).length() < 32.
aSession.startOperation()
anArcEndPoint.setValue(-10., 0.)
aSession.finishOperation()
assert shapeToEdge(aSketchArc.lastResult().shape()).length() > 47.
assert shapeToEdge(aSketchArc.lastResult().shape()).length() < 48.
aSession.startOperation()
anArcEndPoint.setValue(-1., 10.)
aSession.finishOperation()
assert shapeToEdge(aSketchArc.lastResult().shape()).length() > 60.
# move to the initial point at the right side: the edge must reset to the initial one
aSession.startOperation()
anArcEndPoint.setValue(1., 10.)
aSession.finishOperation()
assert shapeToEdge(aSketchArc.lastResult().shape()).length() < 2.
# move to the counterclockwise direction
aSession.startOperation()
anArcEndPoint.setValue(-1., 10.)
aSession.finishOperation()
assert shapeToEdge(aSketchArc.lastResult().shape()).length() < 2.
aSession.startOperation()
anArcEndPoint.setValue(-10., 0.)
aSession.finishOperation()
assert shapeToEdge(aSketchArc.lastResult().shape()).length() < 16.
aSession.startOperation()
anArcEndPoint.setValue(0., -10.)
aSession.finishOperation()
assert shapeToEdge(aSketchArc.lastResult().shape()).length() > 31.
assert shapeToEdge(aSketchArc.lastResult().shape()).length() < 32.
aSession.startOperation()
anArcEndPoint.setValue(10., 0.)
aSession.finishOperation()
assert shapeToEdge(aSketchArc.lastResult().shape()).length() > 47.
assert shapeToEdge(aSketchArc.lastResult().shape()).length() < 48.
aSession.startOperation()
anArcEndPoint.setValue(1., 10.)
aSession.finishOperation()
assert shapeToEdge(aSketchArc.lastResult().shape()).length() > 60.
# check from the scratch that from initial state to counterclockwise position also works
aSession.startOperation()
aSketchArc = aSketchFeature.addFeature("SketchArc")
anArcCentr = geomDataAPI_Point2D(aSketchArc.attribute("center_point"))
anArcCentr.setValue(0., 0.)
anArcStartPoint = geomDataAPI_Point2D(aSketchArc.attribute("start_point"))
anArcStartPoint.setValue(0., 10.)
anArcEndPoint = geomDataAPI_Point2D(aSketchArc.attribute("end_point"))
anArcEndPoint.setValue(-1., 10.)
aSession.finishOperation()
# the arc must be small, not near to the whole circle
assert shapeToEdge(aSketchArc.lastResult().shape()).length() < 2.
#=========================================================================
# Test that movement of the center of arc does not change the orientation of the arc
#=========================================================================
aSession.startOperation()
aSketchArc = aSketchFeature.addFeature("SketchArc")
anArcCentr = geomDataAPI_Point2D(aSketchArc.attribute("center_point"))
anArcCentr.setValue(0., 0.)
anArcStartPoint = geomDataAPI_Point2D(aSketchArc.attribute("start_point"))
anArcStartPoint.setValue(0., 10.)
anArcEndPoint = geomDataAPI_Point2D(aSketchArc.attribute("end_point"))
anArcEndPoint.setValue(10, 0.)
aSession.finishOperation()
anInversed = aSketchArc.boolean("reversed").value()
for aCenterCoords in range(1, 20):
  aSession.startOperation()
  anArcCentr.setValue(aCenterCoords, aCenterCoords/2) # move center
  aSession.finishOperation()
  assert aSketchArc.boolean("reversed").value() == anInversed
for aCenterCoords in range(20, -20, -1):
  aSession.startOperation()
  anArcCentr.setValue(aCenterCoords, aCenterCoords/2) # move center
  aSession.finishOperation()
  assert aSketchArc.boolean("reversed").value() == anInversed
for aCenterCoords in range(-20, 20):
  aSession.startOperation()
  anArcCentr.setValue(aCenterCoords, aCenterCoords/2) # move center
  aSession.finishOperation()
  assert aSketchArc.boolean("reversed").value() == anInversed
#=========================================================================
# Test that movement of start point of arc does not change central point
#=========================================================================
TOL = 1.e-5
x = anArcCentr.x()
y = anArcCentr.y()
sx = anArcStartPoint.x()
sy = anArcStartPoint.y()
for aDelta in range(0, 20):
  aSession.startOperation()
  anArcStartPoint.setValue(sx, sy+aDelta) # move start point
  aSession.finishOperation()
  assert math.fabs(anArcCentr.x() - x) < TOL
  assert math.fabs(anArcCentr.y() - y) < TOL
for aDelta in range(20, -1, -1):
  aSession.startOperation()
  anArcStartPoint.setValue(sx, sy+aDelta) # move start point
  aSession.finishOperation()
  assert math.fabs(anArcCentr.x() - x) < TOL
  assert math.fabs(anArcCentr.y() - y) < TOL
#=========================================================================
# Test that movement of end point of arc does not change central point
#=========================================================================
x = anArcCentr.x()
y = anArcCentr.y()
sx = anArcEndPoint.x()
sy = anArcEndPoint.y()
for aDelta in range(0, 20):
  aSession.startOperation()
  anArcEndPoint.setValue(sx+aDelta, sy) # move end point
  aSession.finishOperation()
  assert math.fabs(anArcCentr.x() - x) < TOL
  assert math.fabs(anArcCentr.y() - y) < TOL
for aDelta in range(20, -1, -1):
  aSession.startOperation()
  anArcEndPoint.setValue(sx+aDelta, sy) # move end point
  aSession.finishOperation()
  assert math.fabs(anArcCentr.x() - x) < TOL
  assert math.fabs(anArcCentr.y() - y) < TOL
