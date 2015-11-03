"""Box macro-feature Interface
Author: Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model import Interface
from macros.box.feature import BoxFeature as MY


def addBox(part, *args):
    """Add Box feature to the part and return Box.

    Pass all args to Box __init__ function.
    """
    feature = part.addFeature(MY.ID())
    return Box(feature, *args)


class Box(Interface):
    """Executes the macro-feature Box.

    Box(feature) -> feature interface without initialization
    Extrusion(feature, dx, dy, dz) ->
        feature interface initialized from arguments:
        - dx, dy, dz -- box dimensions
    """

    def __init__(self, feature, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == MY.ID())

        self._width = self._feature.real(MY.WIDTH_ID())
        self._length = self._feature.real(MY.LENGTH_ID())
        self._height = self._feature.real(MY.HEIGHT_ID())

        assert(self._width)
        assert(self._length)
        assert(self._height)

        if not args:
            return

        assert(len(args) == 3)
        dx, dy, dz = args
        self.setWidth(dx)
        self.setLength(dy)
        self.setHeight(dz)

        self._execute()
        pass

    def setWidth(self, width):
        """B.setWidth(float) -- modify width attribute"""
        self._fillAttribute(self._width, width)
        pass

    def setLength(self, length):
        """B.setLength(float) -- modify length attribute"""
        self._fillAttribute(self._length, length)
        pass

    def setHeight(self, height):
        """B.setHeight(float) -- modify height attribute"""
        self._fillAttribute(self._height, height)
        pass
