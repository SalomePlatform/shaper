from GeomDataAPI import geomDataAPI_Point2D
from model.roots import Interface
from model.errors import WrongNumberOfArguments

from .entity import Entity

class Line(Entity):
    """Interface for editing of a sketch line feature."""
    def __init__(self, feature, *args):
        Entity.__init__(self, feature)
        assert(self._feature.getKind() == "SketchLine")

        # Initialize attributes
        self._start_point = geomDataAPI_Point2D(
            self._feature.data().attribute("StartPoint")
            )
        self._end_point = geomDataAPI_Point2D(
            self._feature.data().attribute("EndPoint")
            )
        self._external = self._feature.selection("External")

        # If no arguments are given the attributes of the feature
        # are'nt initialized
        if args is None:
            return

        # Set attribute values and execute
        if len(args) == 4:
            self.__createByCoordinates(*args)
        elif len(args) == 2:
            self.__createByPoints(*args)
        elif len(args) == 1:
            self.__createByName(*args)
        else:
            raise WrongNumberOfArguments(
                "Arc takes 1, 2 or 4 arguments (%s given)" % len(args)
                )
        self.execute()

    def __createByCoordinates(self, x1, y1, x2, y2):
        self.setStartPoint(x1, y1)
        self.setEndPoint(x2, y2)

    def __createByPoints(self, p1, p2):
        self.setStartPoint(p1.x(), p1.y())
        self.setEndPoint(p2.x(), p2.y())

    def __createByName(self, name):
        self.setExternal(name)

    #######
    #
    # Set methods
    #
    #######

    def setStartPoint(self, x, y):
        """Set the start point of the line."""
        self._start_point.setValue(x, y)

    def setEndPoint(self, x, y):
        """Set the end point of the line."""
        self._end_point.setValue(x, y)

    # TODO : methods below will be removed.
    # Kept until all tests have been updated
    def startPoint(self):
        return self._start_point

    def endPoint(self):
        return self._end_point

    def setExternal(self, name):
        """Set external edge"""
        self._external.selectSubShape("EDGE", name)

    def result(self):
        return self._feature.firstResult()
