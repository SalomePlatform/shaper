"""ExtrusionCut and ExtrusionFuse  Interfaces
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from .roots import CompositeBoolean


def addExtrusionCut(part, *args):
    """Add an ExtrusionCut feature to the Part and return ExtrusionBoolean.

    Pass all args to Extrusion __init__ function.
    """
    assert(args)
    feature = part.addFeature("ExtrusionCut")
    return ExtrusionBoolean(feature, *args)

def addExtrusionFuse(part, *args):
    """Add an ExtrusionFuse feature to the Part and return ExtrusionBoolean.

    Pass all args to Extrusion __init__ function.
    """
    assert(args)
    feature = part.addFeature("ExtrusionFuse")
    return ExtrusionBoolean(feature, *args)


class ExtrusionBoolean(CompositeBoolean):

    def __init__(self, feature, *args):
        CompositeBoolean.__init__(self, feature, *args)

        self._CreationMethod = self._feature.string("CreationMethod")
        self._to_size = self._feature.data().real("to_size")
        self._from_size = self._feature.data().real("from_size")
        self._to_object = self._feature.data().selection("to_object")
        self._to_offset = self._feature.data().real("to_offset")
        self._from_object = self._feature.data().selection("from_object")
        self._from_offset = self._feature.data().real("from_offset")

        if not args:
            return

        assert(len(args) == 4 or len(args) == 2)
        if len(args) == 4:
            self.setPlanesAndOffsets(*args)
        elif len(args) == 2:
            self.setSizes(*args)
        pass

    def __clear(self):
        self._CreationMethod.setValue("BySizes")
        self._fill_attribute(self._to_size, 0)
        self._fill_attribute(self._from_size, 0)
        self._fill_attribute(self._to_object, None)
        self._fill_attribute(self._to_offset, 0)
        self._fill_attribute(self._from_object, None)
        self._fill_attribute(self._from_offset, 0)
        pass

    def setSizes(self, to_size, from_size):
        """Modify the to_size, from_size attributes of the feature.

        See __init__.
        """
        self.__clear()
        self._CreationMethod.setValue("BySizes")
        self._fill_attribute(self._to_size, to_size)
        self._fill_attribute(self._from_size, from_size)
        pass

    def setPlanesAndOffsets(self, to_object, to_offset,
                            from_object, from_offset):
        """Modify planes and offsets attributes of the feature.

        See __init__.
        """
        self.__clear()
        self._CreationMethod.setValue("ByPlanesAndOffsets")
        self._fill_attribute(self._to_object, to_object)
        self._fill_attribute(self._to_offset, to_offset)
        self._fill_attribute(self._from_object, from_object)
        self._fill_attribute(self._from_offset, from_offset)
        pass

