"""Sketch circle feature interface."""

from GeomDataAPI import geomDataAPI_Point2D
from model.errors import WrongNumberOfArguments
from model.roots import Interface

class Arc(Interface):
    """Interface to a sketch arc feature."""
    def __init__(self, feature, *args, **kwargs):
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
        self._inversed = self._feature.boolean("InversedArc")
        if len(args) == 6:
            self.__createByCoordinates(*args)
        elif len(args) == 3:
            self.__createByPoints(*args)
        else:
            raise WrongNumberOfArguments(
                "Arc takes 3 or 6 arguments (%s given)" % len(args)
                )
        if "inversed" in kwargs:
            self.setInversed(kwargs["inversed"])
        self.execute()


    ########
    #
    # Getters
    #
    ########


    def center(self):
        """Return the center point."""
        return self._center

    def startPoint(self):
        """Return the start point."""
        return self._start_point

    def endPoint(self):
        """Return the end point."""
        return self._end_point

    def result(self):
        """Return the arc circular line attribute."""
        return self._feature.lastResult()


    ########
    #
    # Set methods
    #
    ########

    def setCenter(self, x, y):
        """Set arc center."""
        self._center.setValue(x, y)

    def setStartPoint(self, x, y):
        """Set start point."""
        self._start_point.setValue(x, y)

    def setEndPoint(self, x, y):
        """Set end point value."""
        self._end_point.setValue(x, y)

    def setInversed(self, inversed):
        self._fillAttribute(self._inversed, inversed)

    ########
    #
    # Private methods
    #
    ########


    def __createByCoordinates(self, center_x, center_y,
                              start_x, start_y,
                              end_x, end_y):
        """Create an arc by point coordinates."""
        self.setCenter(center_x, center_y)
        self.setStartPoint(start_x, start_y)
        self.setEndPoint(end_x, end_y)

    def __createByPoints(self, center, start, end):
        """Create an arc with point objects."""
        self.setCenter(center.x(), center.y())
        self.setStartPoint(start.x(), start.y())
        self.setEndPoint(end.x(), end.y())
