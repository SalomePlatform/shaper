"""Extrusion Interface
Author: Daniel Brunier-Coulin with contribution by Mikhail Ponikarov
        and Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface
from model import Selection

def addExtrusion(part, *args):
    """Add an Extrusion feature to the Part and return Extrusion.

    Pass all args to Extrusion __init__ function.
    """
    assert(args)
    feature = part.addFeature("Extrusion")
    return Extrusion(feature, *args)


class Extrusion(Interface):
    """Interface on an Extrusion feature."""

    def __init__(self, feature, *args):
        """Initialize an Extrusion feature with given parameters.

        Expected arguments for all modes:
        feature -- an Extrusion feature

        Expected arguments for initializing the feature:
        base -- name, sketch or list of names and sketches.
        If base is None then don't change the feature.

        For BySize mode (expect 2 arguments):
        to_size -- upper size
        from_size -- lower size

        For ByPlanesAndOffsets mode (expect 4 arguments):
        to_object -- upper object (plane)
        to_offset -- offset from upper object
        from_object -- lower object (plane)
        from_offset -- offset from lower object
        """
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Extrusion")

        self._base = self._feature.data().selectionList("base")
        self._CreationMethod = self._feature.string("CreationMethod")
        self._to_size = self._feature.data().real("to_size")
        self._from_size = self._feature.data().real("from_size")
        self._to_object = self._feature.data().selection("to_object")
        self._to_offset = self._feature.data().real("to_offset")
        self._from_object = self._feature.data().selection("from_object")
        self._from_offset = self._feature.data().real("from_offset")

        assert(self._base)
        assert(self._CreationMethod)
        assert(self._to_size)
        assert(self._from_size)
        assert(self._to_object)
        assert(self._to_offset)
        assert(self._from_object)
        assert(self._from_offset)

        if not args:
            return

        assert(len(args) in (2, 3, 5))
        base = args[0]
        args = args[1:]

        self.setBase(base)

        if len(args) == 4:
            self.setPlanesAndOffsets(*args)
        elif len(args) == 2:
            self.setSizes(*args)
        elif len(args) == 1:
            self.setSize(args[0])

        self._execute()
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

    def setBase(self, base):
        """Modify base attribute of the feature.

        See __init__.
        """
        self._fill_attribute(self._base, base)
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

    def setSize(self, size):
        """Modify the size of the feature.

        If size is positive then initialize to_size with size.
        If size is negative then initialize from_size with -size.
        """
        to_size, from_size = 0, 0
        if size >= 0:
            to_size = size
        else:
            from_size = -size

        self.setSizes(to_size, from_size)
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

    def result(self):
        return [Selection(result, result.shape()) for result in (self.firstResult(),)]
