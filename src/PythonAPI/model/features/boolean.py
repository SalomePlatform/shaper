"""Boolean operations Interface
Author: Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from ModelAPI    import *
from GeomAlgoAPI import *


from model.roots import Interface


def addAddition(part, object, tool):
    """Inserts an addition to the given Part and executes the operation.
    This operation adds tool to the given object.
    """
    feature = part.addFeature("Boolean")
    return Boolean(feature, object, tool, GeomAlgoAPI_Boolean.BOOL_FUSE)


def addSubtraction(part, object, tool):
    """Inserts a subtraction to the given Part and executes the operation.
    This operation subtracts tool to the given object.
    """
    feature = part.addFeature("Boolean")
    return Boolean(feature, object, tool, GeomAlgoAPI_Boolean.BOOL_CUT)


def addIntersection(part, object, tool):
    """Inserts an intersection to the given Part and executes the operation.
    This operation intersects tool to the given object.
    """
    feature = part.addFeature("Boolean")
    return Boolean(feature, object, tool, GeomAlgoAPI_Boolean.BOOL_COMMON)


class Boolean(Interface):
    """Abstract root class of Boolean Features."""
    def __init__(self, feature, main_objects, tool_objects, bool_type):
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Boolean")

        self._main_objects = self._feature.selectionList("main_objects")
        self._tool_objects = self._feature.selectionList("tool_objects")
        self._bool_type = self._feature.integer("bool_type")

        assert(self._main_objects)
        assert(self._tool_objects)
        assert(self._bool_type)

        self.setMainObjects(main_objects)
        self.setToolObjects(tool_objects)
        self.setBoolType(bool_type)
        pass

    def setMainObjects(self, main_objects):
        self._fill_attribute(self._main_objects, main_objects)
        pass

    def setToolObjects(self, tool_objects):
        self._fill_attribute(self._tool_objects, tool_objects)
        pass

    def setBoolType(self, bool_type):
        self._fill_attribute(self._bool_type, bool_type)
        pass
