"""ExtrusionCut and ExtrusionFuse  Interfaces
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from .roots import CompositeBoolean


def addExtrusionCut(part, *args):
    """Add an ExtrusionCut feature to the Part.

    .. function:: addExtrusionCut(part, sketch, sketch_selection, boolean_objects, to_size, from_size)

    Args:
        part (ModelAPI_Document): part document
        sketch (ModelAPI_Object): sketch feature
        sketch_selection (Selection): sketch objects
        boolean_objects (list of Selection): boolean objects
        to_size (double): upper size of the extrusion
        from_size (double): lower size of the extrusion

    .. function:: addExtrusionCut(part, sketch, sketch_selection, boolean_objects, to_object, to_offset, from_object, from_offset)

    Args:
        part (ModelAPI_Document): part document
        sketch (ModelAPI_Object): sketch feature
        sketch_selection (Selection): sketch objects
        boolean_objects (list of Selection): boolean objects
        to_object (Selection): upper plane
        to_offset (double): offset from upper plane
        from_object (Selection): lower plane
        from_offset (double): offset from lower plane

    Returns:
        ExtrusionBoolean: extrusion boolean object
    """
    assert(args)
    feature = part.addFeature("ExtrusionCut")
    return ExtrusionBoolean(feature, *args)

def addExtrusionFuse(part, *args):
    """Add an ExtrusionFuse feature to the Part and return ExtrusionBoolean.

    .. function:: addExtrusionFuse(part, sketch, sketch_selection, boolean_objects, to_size, from_size)

    Args:
        part (ModelAPI_Document): part document
        sketch (ModelAPI_Object): sketch feature
        sketch_selection (Selection): sketch objects
        boolean_objects (list of Selection): boolean objects
        to_size (double): upper size of the extrusion
        from_size (double): lower size of the extrusion

    .. function:: addExtrusionFuse(part, sketch, sketch_selection, boolean_objects, to_object, to_offset, from_object, from_offset)

    Args:
        part (ModelAPI_Document): part document
        sketch (ModelAPI_Object): sketch feature
        sketch_selection (Selection): sketch objects
        boolean_objects (list of Selection): boolean objects
        to_object (Selection): upper plane
        to_offset (double): offset from upper plane
        from_object (Selection): lower plane
        from_offset (double): offset from lower plane

    Returns:
        ExtrusionBoolean: extrusion boolean object
    """
    assert(args)
    feature = part.addFeature("ExtrusionFuse")
    return ExtrusionBoolean(feature, *args)


class ExtrusionBoolean(CompositeBoolean):
    """Interface class for ExtrusionBoolean features.

    Supported features:

    * ExtrusionCut
    * ExtrusionFuse

    .. function:: ExtrusionBoolean(feature)

        Create interface for the feature without initialization.

    .. function:: ExtrusionBoolean(feature, sketch, sketch_selection, boolean_objects, to_size, from_size)

        Create interface for the feature and initialize the feature with arguments.

    .. function:: ExtrusionBoolean(feature, sketch, sketch_selection, boolean_objects, to_object, to_offset, from_object, from_offset)

        Create interface for the feature and initialize the feature with arguments.
    """

    def __init__(self, feature, *args):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        CompositeBoolean.__init__(self, feature, *args[:3])
        args = args[3:]

        self._CreationMethod = self._feature.string("CreationMethod")
        self._to_size = self._feature.data().real("to_size")
        self._from_size = self._feature.data().real("from_size")
        self._to_object = self._feature.data().selection("to_object")
        self._to_offset = self._feature.data().real("to_offset")
        self._from_object = self._feature.data().selection("from_object")
        self._from_offset = self._feature.data().real("from_offset")

        assert(self._CreationMethod)
        assert(self._to_size)
        assert(self._from_size)
        assert(self._to_object)
        assert(self._to_offset)
        assert(self._from_object)
        assert(self._from_offset)

        if not args:
            return

        assert(len(args) in (2, 4))
        if len(args) == 4:
            self.setPlanesAndOffsets(*args)
        elif len(args) == 2:
            self.setSizes(*args)

        self.execute()
        pass

    def __clear(self):
        self._CreationMethod.setValue("BySizes")
        self._fillAttribute(self._to_size, 0)
        self._fillAttribute(self._from_size, 0)
        self._fillAttribute(self._to_object, None)
        self._fillAttribute(self._to_offset, 0)
        self._fillAttribute(self._from_object, None)
        self._fillAttribute(self._from_offset, 0)
        pass

    def setSizes(self, to_size, from_size):
        """Modify the to_size, from_size attributes of the feature.

        See __init__.
        """
        self.__clear()
        self._CreationMethod.setValue("BySizes")
        self._fillAttribute(self._to_size, to_size)
        self._fillAttribute(self._from_size, from_size)
        pass

    def setPlanesAndOffsets(self, to_object, to_offset,
                            from_object, from_offset):
        """Modify planes and offsets attributes of the feature.

        See __init__.
        """
        self.__clear()
        self._CreationMethod.setValue("ByPlanesAndOffsets")
        self._fillAttribute(self._to_object, to_object)
        self._fillAttribute(self._to_offset, to_offset)
        self._fillAttribute(self._from_object, from_object)
        self._fillAttribute(self._from_offset, from_offset)
        pass

