"""Rotation Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addRotation(part, *args):
    """Add an Rotation feature to the Part and return Rotation.

    Pass all args to Rotation __init__ function.
    """
    assert(args)
    feature = part.addFeature("Rotation")
    return Rotation(feature, *args)


class Rotation(Interface):

    def __init__(self, feature, *args):
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Rotation")

        self._main_objects = self._feature.data().selectionList("main_objects")
        self._axis_object = self._feature.data().selection("axis_object")
        self._angle = self._feature.data().real("angle")

        assert(self._main_objects)
        assert(self._axis_object)
        assert(self._angle)

        if not args:
            return

        assert(len(args) == 3)
        self.setMainObjects(args[0])
        self.setAxisObject(args[1])
        self.setAngle(args[2])

        self._execute()
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

    def setAngle(self, angle):
        """Modify angle attribute of the feature.

        See __init__.
        """
        self._fill_attribute(self._angle, angle)
        pass
