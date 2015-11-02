"""Plane Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addPlane(part, *args):
    """Add a Plane feature to the Part and return Plane.

    Pass all args to Plane __init__ function.
    """
    feature = part.addFeature("Plane")
    return Plane(feature, *args)


class Plane(Interface):
    """Interface class for Plane feature.

    Plane(feature) -> feature interface without initialization
    Plane(feature, face, distance) ->
        feature interface initialized from arguments:
        - face -- planeFace
        - distance -- distance
    Plane(feature, a, b, c, d) ->
        feature interface initialized from arguments:
        - A, B, C, D -- GeneralEquation parameters
    """

    def __init__(self, feature, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Plane")

        self._CreationMethod = self._feature.data().string("CreationMethod")
        self._plane_face = self._feature.data().selection("planeFace")
        self._distance = self._feature.data().real("distance")
        self._a = self._feature.data().real("A")
        self._b = self._feature.data().real("B")
        self._c = self._feature.data().real("C")
        self._d = self._feature.data().real("D")

        assert(self._CreationMethod)
        assert(self._plane_face)
        assert(self._distance)
        assert(self._a)
        assert(self._b)
        assert(self._c)
        assert(self._d)

        if not args:
            return

        assert(len(args) in (2, 4))
        if len(args) == 2:
            self.setFaceAndDistance(*args)
        elif len(args) == 4:
            self.setGeneralEquation(*args)

        self._execute()
        pass

    def __clear(self):
        self._fill_attribute(self._CreationMethod, "PlaneByFaceAndDistance")
        self._fill_attribute(self._plane_face, None)
        self._fill_attribute(self._distance, 0)
        self._fill_attribute(self._a, 0)
        self._fill_attribute(self._b, 0)
        self._fill_attribute(self._c, 0)
        self._fill_attribute(self._d, 0)

    def setFaceAndDistance(self, face, distance):
        """Modify face and distance attribute of the feature.

        See __init__.
        """
        self.__clear()
        self._fill_attribute(self._CreationMethod, "PlaneByFaceAndDistance")
        self._fill_attribute(self._plane_face, face)
        self._fill_attribute(self._distance, distance)
        pass

    def setGeneralEquation(self, a, b, c, d):
        """Modify GeneralEquation parameters of the feature.

        See __init__.
        """
        self.__clear()
        self._fill_attribute(self._CreationMethod, "PlaneByGeneralEquation")
        self._fill_attribute(self._a, a)
        self._fill_attribute(self._b, b)
        self._fill_attribute(self._c, c)
        self._fill_attribute(self._d, d)
        pass
