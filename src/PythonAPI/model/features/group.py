"""Group Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addGroup(part, *args):
    """Add a Group feature to the Part.

    .. function:: addGroup(part, group_list)

    Args:
        part (ModelAPI_Document): part document
        group_list (list of Selection): list of objects

    Returns:
        Group: group object
    """
    assert(args)
    feature = part.addFeature("Group")
    return Group(feature, *args)


class Group(Interface):
    """Interface class for Group feature.

    .. function:: Group(feature)

        Create interface for the feature without initialization.

    .. function:: Group(feature, group_list)

        Create interface for the feature and initialize the feature with arguments.
    """

    def __init__(self, feature, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Group")

        self._group_list = self._feature.data().selectionList("group_list")

        assert(self._group_list)

        if not args:
            return

        assert(len(args) == 1)
        self.setGroupList(args[0])

        self.execute()
        pass

    def setGroupList(self, main_objects):
        """Modify group_list attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._group_list, group_list)
        pass
