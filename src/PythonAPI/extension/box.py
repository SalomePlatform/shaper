"""Box macro-feature Interface
Author: Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model import Interface
from macros.box.feature import BoxFeature as MY


def addBox(container, *args):
    feature = container.addFeature(MY.ID())
    return Box(feature, *args)


class Box(Interface):
    """Executes the macro-feature Box."""

    def __init__(self, feature, *args):
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
        self._fill_attribute(self._width, width)
        pass

    def setLength(self, length):
        self._fill_attribute(self._length, length)
        pass

    def setHeight(self, height):
        self._fill_attribute(self._height, height)
        pass
