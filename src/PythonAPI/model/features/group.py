"""Group Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addGroup(part, *args):
    """Add an Group feature to the Part and return Group.

    Pass all args to Group __init__ function.
    """
    assert(args)
    feature = part.addFeature("Group")
    return Group(feature, *args)


class Group(Interface):

    def __init__(self, feature, *args):
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Group")

        self._group_list = self._feature.data().selectionList("group_list")

        assert(self._group_list)

        if not args:
            return

        assert(len(args) == 1)
        self.setGroupList(args[0])

        self._execute()
        pass

    def setGroupList(self, main_objects):
        """Modify group_list attribute of the feature.

        See __init__.
        """
        self._fill_attribute(self._group_list, group_list)
        pass
