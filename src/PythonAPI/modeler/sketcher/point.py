"""Sketch point feature interface."""

from GeomDataAPI import geomDataAPI_Point2D

def addPoint(sketch, x, y):
    """Add a sketch point feature to the sketch.
    
    :return: Interface object on this feature
    :rtype: sketcher.Point
    """
    sketch_point_feature = sketch.addFeature("SketchPoint")
    return PointInterface(sketch_feature, x, y)


class PointInterface():
    """Interface on point feature for data manipulation."""
    def __init__(self, point_feature, x, y):
        self._point_feature = point_feature
        self._point_data = geomDataAPI_Point2D(
            self._point_feature.data().attribute("PointCoordinates")
            )
        self._point_data.setValue(x, y)
        self._point_feature.execute()

    def pointData (self):
        """Return the point data."""
        return self._point_data

    def result (self):
        """Return the feature result."""
        return self._point_feature.firstResult()
