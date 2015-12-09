"""Partition Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addPartition(part, *args):
    """Add a Partition feature to the Part.

    .. function:: addPartition(part, main_objects, tool_objects, partition_combine)

    Args:
        part (ModelAPI_Document): part document
        main_objects (list of Selection): main objects
        tool_objects (list of Selection): tool objects
        partition_combine (boolean):
            If True combines all results to one. If False builds separate result for each object.

    Returns:
        Partition: partition object
    """
    assert(len(args) > 0 and args[0] is not None)
    feature = part.addFeature("Partition")
    return Partition(feature, *args)


class Partition(Interface):
    """Interface class for Partition feature.

    .. function:: Partition(feature)

        Create interface for the feature without initialization.

    .. function:: Partition(feature, main_objects, tool_objects, partition_combine)

        Create interface for the feature and initialize the feature with arguments.
    """

    def __init__(self, feature, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Partition")

        self._main_objects = self._feature.data().selectionList("main_objects")
        self._tool_objects = self._feature.data().selectionList("tool_objects")
        self._partition_combine = self._feature.data().boolean("partition_combine")

        assert(self._main_objects)
        assert(self._tool_objects)
        assert(self._partition_combine)

        if not args:
            return

        assert(len(args) == 3)
        main_objects, tool_objects, partition_combine = args

        self.setMainObjects(main_objects)
        self.setToolObjects(tool_objects)
        self.setPartitionCombine(partition_combine)

        self.execute()
        pass

    def setMainObjects(self, main_objects):
        """Modify base attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._main_objects, main_objects)
        pass

    def setToolObjects(self, tool_objects):
        """Modify the to_size, from_size attributes of the feature.

        See __init__.
        """
        self._fillAttribute(self._tool_objects, tool_objects)
        pass

    def setPartitionCombine(self, partition_combine):
        """Modify planes and offsets attributes of the feature.

        See __init__.
        """
        self._fillAttribute(self._partition_combine, partition_combine)
        pass
