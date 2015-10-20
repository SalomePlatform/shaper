"""Sketch circle feature interface."""

from GeomDataAPI import geomDataAPI_Point2D

class Circle():
    """Interface for circle feature data manipulation."""

    def __init__(self, circle_feature, x, y, r):
        self._feature = circle_feature
        self._center = geomDataAPI_Point2D(
            self._feature.data().attribute("CircleCenter")
            )
        self._radius = self._feature.data().real("CircleRadius")
        self._center.setValue(x, y)
        self._radius.setValue(r)
        self._feature.execute()

    def centerData(self):
        """Return center data."""
        return self._center

    def radiusData(self):
        """Return radius data."""
        return self._radius

    def result(self):
        """Return the cicular line attribute."""
        return self._feature.lastResult()
