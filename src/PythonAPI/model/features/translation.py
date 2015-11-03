"""Translation Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addTranslation(part, *args):
    """Add an Translation feature to the Part and return Translation.

    Pass all args to Translation __init__ function.
    """
    assert(args)
    feature = part.addFeature("Translation")
    return Translation(feature, *args)


class Translation(Interface):
    """Interface class for Translation features.

    Translation(feature) -> feature interface without initialization
    Translation(feature, main_objects, axis_object, distance) ->
        feature interface initialized from arguments:
        - main_objects
        - axis_object
        - distance
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
