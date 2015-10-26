"""Extrusion Interface
Author: Daniel Brunier-Coulin with contribution by Mikhail Ponikarov
        and Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from .roots import Interface


def addExtrusion(part, *args):
    """Add an Extrusion feature to the Part and return Extrusion.

    Pass all args to Extrusion __init__ function.
    """
    assert(len(args) > 0 and args[0] is not None)
    feature = part.addFeature("Extrusion")
    return Extrusion(feature, *args)


class Extrusion(Interface):
    """Interface on an Extrusion feature."""

    def __init__(self, feature, base=None, *args):
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

        if base is None:
            return

        self.__setBase(base)

        if len(args) == 4:
            self.__createByPlanesAndOffsets(*args)
        elif len(args) == 2:
            self.__createBySizes(*args)
        else:
            raise AssertionError(
                "Extrusion takes 4 or 6 arguments (%s given)." % (len(args) + 2)
                )

        self.__execute()
        pass

    def __setBase(self, base):
        self._fill_attribute(self._base, base)
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

    def __createBySizes(self, to_size, from_size):
        self.__clear()
        self._CreationMethod.setValue("BySizes")
        self._fill_attribute(self._to_size, to_size)
        self._fill_attribute(self._from_size, from_size)
        pass

    def __createByPlanesAndOffsets(self, to_object, to_offset,
                                   from_object, from_offset):
        self.__clear()
        self._CreationMethod.setValue("ByPlanesAndOffsets")
        self._fill_attribute(self._to_object, to_object)
        self._fill_attribute(self._to_offset, to_offset)
        self._fill_attribute(self._from_object, from_object)
        self._fill_attribute(self._from_offset, from_offset)
        pass

    def __execute(self):
        if self.areInputValid():
            self.execute()
        else:
            raise Exception("Cannot execute Extrusion: %s" %
                            self._feature.error())

    def setBase(self, base):
        """Modify base attribute of the feature.

        See __init__.
        """
        self.__setBase(base)
        self.__execute()
        pass

    def setSizes(self, *args):
        """Modify the to_size, from_size attributes of the feature.

        See __init__.
        """
        self.__createBySizes(*args)
        self.__execute()
        pass

    def setPlanesAndOffsets(self, *args):
        """Modify planes and offsets attributes of the feature.

        See __init__.
        """
        self.__createByPlanesAndOffsets(*args)
        self.__execute()
        pass
