"""Sketch circle feature interface."""

from GeomDataAPI import geomDataAPI_Point2D
from model.roots import Interface

class Circle(Interface):
    """Interface for circle feature data manipulation."""
    def __init__(self, feature, x, y, radius):
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "SketchCircle")
        
        self._center = geomDataAPI_Point2D(
            self._feature.data().attribute("CircleCenter")
            )
        self._radius = self._feature.data().real("CircleRadius")
        self.setCenter(x, y)
        self.setRadius(radius)
        self.execute()

    def setCenter(self, x, y):
        """Set the center of the circle."""
        self._center.setValue(x, y)
        
    def setRadius(self, radius):
        """Set the radius of the circle."""
        self._radius.setValue(radius)
        
    def centerData(self):
        """Return center data."""
        return self._center

    def radiusData(self):
        """Return radius data."""
        return self._radius

    def result(self):
        """Return the cicular line attribute."""
        return self._feature.lastResult()
