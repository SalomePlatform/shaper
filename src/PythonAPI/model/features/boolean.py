"""Boolean operations Interface
Author: Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface

def addAddition(part, *args):
    """Inserts an addition to the given Part and executes the operation.
    This operation adds tool to the given object.
    """
    assert(args)
    object, tool = args
    feature = part.addFeature("Boolean")
    return Boolean(feature, object, tool, GeomAlgoAPI_Boolean.BOOL_FUSE)


def addSubtraction(part, *args):
    """Inserts a subtraction to the given Part and executes the operation.
    This operation subtracts tool to the given object.
    """
    assert(args)
    object, tool = args
    feature = part.addFeature("Boolean")
    return Boolean(feature, object, tool, GeomAlgoAPI_Boolean.BOOL_CUT)


def addIntersection(part, *args):
    """Inserts an intersection to the given Part and executes the operation.
    This operation intersects tool to the given object.
    """
    assert(args)
    object, tool = args
    feature = part.addFeature("Boolean")
    return Boolean(feature, object, tool, GeomAlgoAPI_Boolean.BOOL_COMMON)


class Boolean(Interface):
    """Interface class for Boolean features.

    Boolean(feature) -> feature interface without initialization
    Boolean(feature, main_objects, tool_objects, bool_type) ->
        feature interface initialized from arguments
    """

    def __init__(self, feature, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Boolean")

        self._main_objects = self._feature.selectionList("main_objects")
        self._tool_objects = self._feature.selectionList("tool_objects")
        self._bool_type = self._feature.integer("bool_type")

        assert(self._main_objects)
        assert(self._tool_objects)
        assert(self._bool_type)

        if not args:
            return

        assert(len(args) == 3)
        main_objects, tool_objects, bool_type = args

        self.setMainObjects(main_objects)
        self.setToolObjects(tool_objects)
        self.setBoolType(bool_type)

        self.execute()
        pass

    def setMainObjects(self, main_objects):
        """F.setMainObjects(iterable) -- modify main_objects attribute"""
        self._fillAttribute(self._main_objects, main_objects)
        pass

    def setToolObjects(self, tool_objects):
        """F.setToolObjects(iterable) -- modify tool_objects attribute"""
        self._fillAttribute(self._tool_objects, tool_objects)
        pass

    def setBoolType(self, bool_type):
        """F.setBoolType(integer) -- modify bool_type attribute.

        Available types:
        - GeomAlgoAPI_Boolean.BOOL_FUSE
        - GeomAlgoAPI_Boolean.BOOL_CUT
        - GeomAlgoAPI_Boolean.BOOL_COMMON
        """
        self._fillAttribute(self._bool_type, bool_type)
        pass
