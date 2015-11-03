"""Sketch point feature interface."""

from GeomDataAPI import geomDataAPI_Point2D
from model.roots import Interface
from model.errors import FeatureInputInvalid

class Point(Interface):
    """Interface on point feature for data manipulation."""
    def __init__(self, feature, x, y):
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "SketchPoint")
        
        # Initialize attributes of the feature
        self._point_data = geomDataAPI_Point2D(
            self._feature.data().attribute("PointCoordinates")
            )
        self.setValue(x, y)
        self.execute()

    def setValue(self, x, y):
        """Set point coordinates."""
        self._point_data.setValue(x, y)

    def pointData (self):
        """Return the point data."""
        return self._point_data

    def result (self):
        """Return the feature result."""
        return self._point_feature.firstResult()
