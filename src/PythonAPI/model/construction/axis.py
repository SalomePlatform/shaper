"""Axis Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addAxis(part, *args):
    """Add an Axis feature to the Part and return Axis.

    Pass all args to Axis __init__ function.
    """
    assert(args)
    feature = part.addFeature("Axis")
    return Axis(feature, *args)


class Axis(Interface):
    """Interface class for Axis feature.

    Axis(feature) -> feature interface without initialization
    Axis(feature, p1, p2) ->
        feature interface initialized from arguments:
        - p1 -- FirstPoint
        - p2 -- SecondPoint
    Axis(feature, face) ->
        feature interface initialized from arguments:
        - face -- CylindricalFace
    """

    def __init__(self, feature, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
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
        self._fillAttribute(self._CreationMethod, "AxisByPointsCase")
        self._fillAttribute(self._FirstPoint, None)
        self._fillAttribute(self._SecondPoint, None)
        self._fillAttribute(self._CylindricalFace, None)

    def setPoints(self, p1, p2):
        """Modify points attribute of the feature.

        See __init__.
        """
        self.__clear()
        self._fillAttribute(self._CreationMethod, "AxisByPointsCase")
        self._fillAttribute(self._FirstPoint, p1)
        self._fillAttribute(self._SecondPoint, p2)
        pass

    def setCylindricalFace(self, face):
        """Modify CylindricalFace attribute of the feature.

        See __init__.
        """
        self.__clear()
        self._fillAttribute(self._CreationMethod, "AxisByCylindricalFaceCase")
        self._fillAttribute(self._CylindricalFace, face)
        pass
