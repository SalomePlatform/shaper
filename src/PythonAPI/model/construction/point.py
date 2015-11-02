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
    """Interface class for Point feature.

    Point(feature) -> feature interface without initialization
    Point(feature, x, y, z) ->
        feature interface initialized from arguments:
        - x, y, z -- coordinates for the point
    """

    def __init__(self, feature, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Point")

        self._x = self._feature.data().real("x")
        self._y = self._feature.data().real("y")
        self._z = self._feature.data().real("z")

        assert(self._x)
        assert(self._y)
        assert(self._z)

        if not args:
            return

        assert(len(args) == 3)
        self.setPoint(*args)

        self._execute()
        pass

    def setPoint(self, x, y, z):
        """Modify base attribute of the feature.

        See __init__.
        """
        self._fill_attribute(self._x, x)
        self._fill_attribute(self._y, y)
        self._fill_attribute(self._z, z)
        pass
