"""Partition Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addPartition(part, *args):
    """Add an Partition feature to the Part and return Partition.

    Pass all args to Partition __init__ function.
    """
    assert(len(args) > 0 and args[0] is not None)
    feature = part.addFeature("Partition")
    return Partition(feature, *args)


class Partition(Interface):
    """Interface on an Partition feature."""

    def __init__(self, feature, main_objects=None,
                 tool_objects=None, partition_combine=None):
        """Initialize an Partition feature with given parameters.

        Expected arguments:
        feature -- an Partition feature

        Expected arguments for initializing the feature:
        main_objects -- list of solids.
        tool_objects -- list of solids.
        partition_combine -- boolean value.
        """
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Partition")

        self._main_objects = self._feature.data().selectionList("main_objects")
        self._tool_objects = self._feature.data().selectionList("tool_objects")
        self._partition_combine = self._feature.data().boolean("partition_combine")

        assert(self._main_objects)
        assert(self._tool_objects)
        assert(self._partition_combine)

        if main_objects is None:
            return

        self._fill_attribute(self._main_objects, main_objects)
        self._fill_attribute(self._tool_objects, tool_objects)
        self._fill_attribute(self._partition_combine, partition_combine)
        pass

    def setMainObjects(self, main_objects):
        """Modify base attribute of the feature.

        See __init__.
        """
        self._fill_attribute(self._main_objects, main_objects)
        pass

    def setToolObjects(self, tool_objects):
        """Modify the to_size, from_size attributes of the feature.

        See __init__.
        """
        self._fill_attribute(self._tool_objects, tool_objects)
        pass

    def setPartitionCombine(self, partition_combine):
        """Modify planes and offsets attributes of the feature.

        See __init__.
        """
        self._fill_attribute(self._partition_combine, partition_combine)
        pass
