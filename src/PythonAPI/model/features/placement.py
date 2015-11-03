"""Placement Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addPlacement(part, *args):
    """Add an Placement feature to the Part and return Placement.

    Pass all args to Placement __init__ function.
    """
    assert(args)
    feature = part.addFeature("Placement")
    return Placement(feature, *args)


class Placement(Interface):
    """Interface class for Placement feature.

    Placement(feature) -> feature interface without initialization
    Placement(feature, objects_list, start_shape, end_shape,
              reverse_direction, centering) ->
        feature interface initialized from arguments:
        - objects_list
        - start_shape
        - end_shape
        - reverse_direction
        - centering
    """

    def __init__(self, feature, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Placement")

        self._objects_list = self._feature.data().selectionList("placement_objects_list")
        self._start_shape = self._feature.data().selection("placement_start_shape")
        self._end_shape = self._feature.data().selection("placement_end_shape")
        self._reverse_direction = self._feature.data().boolean("placement_reverse_direction")
        self._centering = self._feature.data().boolean("placement_centering")

        assert(self._objects_list)
        assert(self._start_shape)
        assert(self._end_shape)
        assert(self._reverse_direction)
        assert(self._centering)

        if not args:
            return

        assert(len(args) == 5)
        self.setObjectList(args[0])
        self.setStartShape(args[1])
        self.setEndShape(args[2])
        self.setReverseDirection(args[3])
        self.setCentering(args[4])

        self._execute()
        pass

    def setObjectList(self, objects_list):
        """Modify placement_objects_list attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._objects_list, objects_list)
        pass

    def setStartShape(self, start_shape):
        """Modify start_shape attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._start_shape, start_shape)
        pass

    def setEndShape(self, end_shape):
        """Modify end_shape attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._end_shape, end_shape)
        pass

    def setReverseDirection(self, reverse_direction):
        """Modify reverse_direction attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._reverse_direction, reverse_direction)
        pass

    def setCentering(self, centering):
        """Modify centering attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._centering, centering)
        pass
