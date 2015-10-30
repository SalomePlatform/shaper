"""Sketch circle feature interface."""

from GeomDataAPI import geomDataAPI_Point2D
from model.errors import WrongNumberOfArguments
from model.roots import Interface

class Arc(Interface):
    """Interface to a sketch arc feature."""
    def __init__(self, feature, *args):
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "SketchArc")

        self._center = geomDataAPI_Point2D(
            self._feature.data().attribute("ArcCenter")
            )
        self._start_point = geomDataAPI_Point2D(
            self._feature.data().attribute("ArcStartPoint")
            )
        self._end_point = geomDataAPI_Point2D(
            self._feature.data().attribute("ArcEndPoint")
            )
        if len(args) == 6:
            self.__createByCoordinates(*args)
        elif len(args) == 3:
            self.__createByPoints(*args)
        else:
            raise WrongNumberOfArguments(
                "Arc takes 3 or 6 arguments (%s given)" % len(args)
                )

    def centerData(self):
        """Return the center point data."""
        return self._center

    def startPointData(self):
        """Return the start point data."""
        return self._start_point

    def endPointData(self):
        """Return the end point data."""
        return self._end_point

    def result(self):
        """Return the arc circular line attribute."""
        return self._feature.lastResult()

    ########
    #
    # Private methods
    #
    ########

    def __createByCoordinates(self, center_x, center_y,
                              start_x, start_y,
                              end_x, end_y):
        """Create an arc by point coordinates."""
        self._center.setValue(center_x, center_y)
        self._start_point.setValue(start_x, start_y)
        self._end_point.setValue(end_x, end_y)
        self._feature.execute()

    def __createByPoints(self, center, start, end):
        """Create an arc with point objects."""
        self._center.setValue(center.x(), center.y())
        self._start_point.setValue(start.x(), start.y())
        self._end_point.setValue(end.x(), end.y())
        self._feature.execute()
