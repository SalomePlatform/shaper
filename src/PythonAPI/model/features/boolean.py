"""Boolean operations Interface
Author: Daniel Brunier-Coulin with contribution by Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from GeomAlgoAPI import GeomAlgoAPI_Boolean

from model.roots import Interface
from model import Selection

def addAddition(part, *args):
    """Perform addition in the Part.

    .. function:: addAddition(part, main_objects, tool_objects)

        This operation adds tools to the given objects.

    Args:
        part (ModelAPI_Document): part document
        main_objects (list of :class:`model.Selection`): main objects
        tool_objects (list of :class:`model.Selection`): tool_objects objects

    Returns:
        Boolean: boolean object
    """
    assert(args)
    main_objects, tool_objects = args
    feature = part.addFeature("Boolean")
    return Boolean(
        feature, main_objects, tool_objects, GeomAlgoAPI_Boolean.BOOL_FUSE)


def addSubtraction(part, *args):
    """Perform subtraction in the Part.

    .. function:: addSubtraction(part, main_objects, tool_objects)

        This operation subtracts tools from the given objects.

    Args:
        part (ModelAPI_Document): part document
        main_objects (list of :class:`model.Selection`): main objects
        tool_objects (list of :class:`model.Selection`): tool_objects objects

    Returns:
        Boolean: boolean object
    """
    assert(args)
    main_objects, tool_objects = args
    feature = part.addFeature("Boolean")
    return Boolean(
        feature, main_objects, tool_objects, GeomAlgoAPI_Boolean.BOOL_CUT)


def addIntersection(part, *args):
    """Perform intersection in the Part.

    .. function:: addIntersection(part, main_objects, tool_objects)

        This operation intersects tools with the given objects.

    Args:
        part (ModelAPI_Document): part document
        main_objects (list of :class:`model.Selection`): main objects
        tool_objects (list of :class:`model.Selection`): tool_objects objects

    Returns:
        Boolean: boolean object
    """
    assert(args)
    main_objects, tool_objects = args
    feature = part.addFeature("Boolean")
    return Boolean(
        feature, main_objects, tool_objects, GeomAlgoAPI_Boolean.BOOL_COMMON)


class Boolean(Interface):
    """Interface class for Boolean features.

    .. function:: Boolean(feature)

        Create interface for the feature without initialization.

    .. function:: Boolean(feature, main_objects, tool_objects, bool_type)

        Create interface for the feature and initialize the feature with arguments.
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
        """Modify main_objects attribute of the feature.

        Args:
            main_objects (list of :class:`model.Selection`): main objects
        """
        self._fillAttribute(self._main_objects, main_objects)
        pass

    def setToolObjects(self, tool_objects):
        """Modify tool_objects attribute of the feature.

        Args:
            tool_objects (list of :class:`model.Selection`): tool objects
        """
        self._fillAttribute(self._tool_objects, tool_objects)
        pass

    def setBoolType(self, bool_type):
        """Modify bool_type attribute of the feature.

        Args:
            bool_type (integer): type of operation

        Available types:

        * GeomAlgoAPI_Boolean.BOOL_FUSE
        * GeomAlgoAPI_Boolean.BOOL_CUT
        * GeomAlgoAPI_Boolean.BOOL_COMMON
        """
        self._fillAttribute(self._bool_type, bool_type)
        pass

    def result(self):
        """F.result() -> list of Selection objects"""
        return [Selection(result, result.shape()) for result in (self.firstResult(),)]
