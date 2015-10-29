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

    def __init__(self, feature, *args):
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Translation")

        self._main_objects = self._feature.data().selectionList("main_objects")
        self._axis_object = self._feature.data().selection("axis_object")
        self._distance = self._feature.data().real("distance")

        assert(self._main_objects)
        assert(self._axis_object)
        assert(self._distance)

        assert(len(args) == 3)
        self.setMainObjects(args[0])
        self.setAxisObject(args[1])
        self.setDistance(args[2])
        pass

    def setMainObjects(self, main_objects):
        """Modify main_objects attribute of the feature.

        See __init__.
        """
        self._fill_attribute(self._main_objects, main_objects)
        pass

    def setAxisObject(self, axis_object):
        """Modify axis_object attribute of the feature.

        See __init__.
        """
        self._fill_attribute(self._axis_object, axis_object)
        pass

    def setDistance(self, distance):
        """Modify distance attribute of the feature.

        See __init__.
        """
        self._fill_attribute(self._distance, distance)
        pass
