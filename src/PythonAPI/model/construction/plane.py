"""Plane Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addPlane(part, *args):
    """Add a Plane feature to the Part and return Plane.

    .. function:: addPartition(part, face, distance)

    Args:
        part (ModelAPI_Document): part document
        face (Selection): plane face
        distance (double): distance

    .. function:: addPartition(part, a, b, c, d)

    Args:
        part (ModelAPI_Document): part document
        a (double): general equation parameter
        b (double): general equation parameter
        c (double): general equation parameter
        d (double): general equation parameter

    Returns:
        Plane: plane object
    """
    assert(args)
    feature = part.addFeature("Plane")
    return Plane(feature, *args)


class Plane(Interface):
    """Interface class for Plane feature.

    .. function:: Plane(feature)

        Create interface for the feature without initialization.

    .. function:: Plane(feature, face, distance)

        Create interface for the feature and initialize the feature with arguments.

    .. function:: Plane(feature, a, b, c, d)

        Create interface for the feature and initialize the feature with arguments.
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

        self.execute()
        pass

    def __clear(self):
        self._fillAttribute(self._CreationMethod, "PlaneByFaceAndDistance")
        self._fillAttribute(self._plane_face, None)
        self._fillAttribute(self._distance, 0)
        self._fillAttribute(self._a, 0)
        self._fillAttribute(self._b, 0)
        self._fillAttribute(self._c, 0)
        self._fillAttribute(self._d, 0)

    def setFaceAndDistance(self, face, distance):
        """Modify face and distance attribute of the feature.

        See __init__.
        """
        self.__clear()
        self._fillAttribute(self._CreationMethod, "PlaneByFaceAndDistance")
        self._fillAttribute(self._plane_face, face)
        self._fillAttribute(self._distance, distance)
        pass

    def setGeneralEquation(self, a, b, c, d):
        """Modify GeneralEquation parameters of the feature.

        See __init__.
        """
        self.__clear()
        self._fillAttribute(self._CreationMethod, "PlaneByGeneralEquation")
        self._fillAttribute(self._a, a)
        self._fillAttribute(self._b, b)
        self._fillAttribute(self._c, c)
        self._fillAttribute(self._d, d)
        pass
