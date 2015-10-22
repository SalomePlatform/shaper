"""Sketch circle feature interface."""

from GeomDataAPI import geomDataAPI_Point2D
from model.roots import Interface

class Circle(Interface):
    """Interface for circle feature data manipulation."""
    def __init__(self, feature, x, y, r):
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "SketchCircle")
        
        self._center = geomDataAPI_Point2D(
            self._feature.data().attribute("CircleCenter")
            )
        self._radius = self._feature.data().real("CircleRadius")
        self._center.setValue(x, y)
        self._radius.setValue(r)
        self.execute()

    def centerData(self):
        """Return center data."""
        return self._center

    def radiusData(self):
        """Return radius data."""
        return self._radius

    def result(self):
        """Return the cicular line attribute."""
        return self._feature.lastResult()
