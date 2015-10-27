"""Point Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addPoint(part, *args):
    """Add an Point feature to the Part and return Point.

    Pass all args to Point __init__ function.
    """
    feature = part.addFeature("Point")
    return Point(feature, *args)


class Point(Interface):
    """Interface on an Point feature."""

    def __init__(self, feature, *args):
        """Initialize an Point feature with given parameters.

        Expected arguments for all modes:
        feature -- a Point feature

        Expected arguments for initializing the feature:
        x, y, z -- x, y, z coordinates for the point.
        """
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Point")

        self._x = self._feature.data().real("x")
        self._y = self._feature.data().real("y")
        self._z = self._feature.data().real("z")

        if not args:
            return

        self.setPoint(*args)
        pass

    def setPoint(self, x, y, z):
        """Modify base attribute of the feature.

        See __init__.
        """
        self._fill_attribute(self._x, x)
        self._fill_attribute(self._y, y)
        self._fill_attribute(self._z, z)
        pass
