"""Axis Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addAxis(part, *args):
    """Add an Axis feature to the Part and return Axis.

    Pass all args to Axis __init__ function.
    """
    feature = part.addFeature("Axis")
    return Axis(feature, *args)


class Axis(Interface):
    """Interface on an Axis feature."""

    def __init__(self, feature, *args):
        """Initialize an Axis feature with given parameters.

        Expected arguments for all modes:
        feature -- a Axis feature

        For AxisByPointsCase mode (expect 2 arguments):
        p1 -- FirstPoint
        p2 -- SecondPoint

        For AxisByCylindricalFaceCase mode (expect 1 argument):
        face -- CylindricalFace
        """
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Axis")

        self._CreationMethod = self._feature.data().string("CreationMethod")
        self._FirstPoint = self._feature.data().selection("FirstPoint")
        self._SecondPoint = self._feature.data().selection("SecondPoint")
        self._CylindricalFace = self._feature.data().selection("CylindricalFace")

        assert(self._CreationMethod)
        assert(self._FirstPoint)
        assert(self._SecondPoint)
        assert(self._CylindricalFace)

        if not args:
            return

        assert(len(args) in (1, 2))
        if len(args) == 2:
            self.setPoints(*args)
        elif len(args) == 1:
            self.setCylindricalFace(*args)

        self._execute()
        pass

    def __clear(self):
        self._fill_attribute(self._CreationMethod, "AxisByPointsCase")
        self._fill_attribute(self._FirstPoint, None)
        self._fill_attribute(self._SecondPoint, None)
        self._fill_attribute(self._CylindricalFace, None)

    def setPoints(self, p1, p2):
        """Modify points attribute of the feature.

        See __init__.
        """
        self.__clear()
        self._fill_attribute(self._CreationMethod, "AxisByPointsCase")
        self._fill_attribute(self._FirstPoint, p1)
        self._fill_attribute(self._SecondPoint, p2)
        pass

    def setCylindricalFace(self, face):
        """Modify CylindricalFace attribute of the feature.

        See __init__.
        """
        self.__clear()
        self._fill_attribute(self._CreationMethod, "AxisByCylindricalFaceCase")
        self._fill_attribute(self._CylindricalFace, face)
        pass
