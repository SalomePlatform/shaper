"""Point Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addPoint(part, *args):
    """Add an Point feature to the Part and return Point.

    .. function:: addPoint(part, x, y, z)

    Args:
        part (ModelAPI_Document): part document
        x (double): X coordinate for the point
        y (double): Y coordinate for the point
        z (double): Z coordinate for the point

    Returns:
        Point: point object
    """
    assert(args)
    feature = part.addFeature("Point")
    return Point(feature, *args)


class Point(Interface):
    """Interface class for Point feature.

    .. function:: Point(feature)

        Create interface for the feature without initialization.

    .. function:: Point(feature, x, y, z)

        Create interface for the feature and initialize the feature with arguments.
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

        self.execute()
        pass

    def setPoint(self, x, y, z):
        """Modify base attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._x, x)
        self._fillAttribute(self._y, y)
        self._fillAttribute(self._z, z)
        pass
