"""Translation Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addTranslation(part, *args):
    """Add a Translation feature to the Part.

    .. function:: addTranslation(part, main_objects, axis_object, distance)

    Args:
        part (ModelAPI_Document): part document
        main_objects (list of Selection): main objects
        axis_object (Selection): axis objects
        distance (double): distance

    Returns:
        Translation: translation object
    """
    assert(args)
    feature = part.addFeature("Translation")
    return Translation(feature, *args)


class Translation(Interface):
    """Interface class for Translation features.

    .. function:: Translation(feature)

        Create interface for the feature without initialization.

    .. function:: Translation(feature, main_objects, axis_object, distance)

        Create interface for the feature and initialize the feature with arguments.
    """

    def __init__(self, feature, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Translation")

        self._main_objects = self._feature.data().selectionList("main_objects")
        self._axis_object = self._feature.data().selection("axis_object")
        self._distance = self._feature.data().real("distance")

        assert(self._main_objects)
        assert(self._axis_object)
        assert(self._distance)

        if not args:
            return

        assert(len(args) == 3)
        self.setMainObjects(args[0])
        self.setAxisObject(args[1])
        self.setDistance(args[2])

        self.execute()
        pass

    def setMainObjects(self, main_objects):
        """Modify main_objects attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._main_objects, main_objects)
        pass

    def setAxisObject(self, axis_object):
        """Modify axis_object attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._axis_object, axis_object)
        pass

    def setDistance(self, distance):
        """Modify distance attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._distance, distance)
        pass
