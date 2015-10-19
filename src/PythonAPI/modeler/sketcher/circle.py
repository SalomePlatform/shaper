"""Sketch circle feature interface."""

from GeomDataAPI import geomDataAPI_Point2D

def addCircle(sketch, x, y, r):
    feature = sketch.addFeature("SketchCircle")
    return CircleInterface(feature, x, y, r)

class CircleInterface():
    def __init__(self, circle_feature, x, y, r):
        self._feature = circle_feature
        self._center = geomDataAPI_Point2D(
            self._feature.data().attribute("CircleCenter")
            )
        self._radius = self._feature.data().real("CircleRadius")
        self._center.setValue(x, y)
        self._radius.setValue(r)
        self._feature.execute()

    def centerData (self):
        return self._center
    
    def radiusData (self):
        return self._radius

    def result (self):
        return self._feature.lastResult()   # Returns the circular line attribute
