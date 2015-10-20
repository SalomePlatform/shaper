"""Sketch point feature interface."""

from GeomDataAPI import geomDataAPI_Point2D

class Point():
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
