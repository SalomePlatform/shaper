"""Extrusion Interface
Author: Daniel Brunier-Coulin with contribution by Mikhail Ponikarov
        and Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from .roots import Interface
from .sketcher.sketch import Sketch


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
            self.__createBySize(*args)
        else:
            raise Exception("cannot create the Extrusion")

        self.__execute()
        pass

    def __setBase(self, base):
        """Initialize base attribute of the feature.

        Expected arguments:
        base -- could be object of this types:
        - string - topology name of object
        - Sketch - sketch object
        - iterable - list or tuple with string and/or Sketch objects
        """
        self._base.clear()
        if isinstance(base, basestring):
            self._base.append(base)
        elif isinstance(base, Sketch):
            self._base.append(base.result(), base.buildShape())
        else:
            for item in base:
                if isinstance(item, basestring):
                    self._base.append(item)
                else:
                    self._base.append(item.result(), item.buildShape())
        pass

    def __clear(self):
        self._CreationMethod.setValue("BySizes")
        self._to_size.setValue(0)
        self._from_size.setValue(0)
        self._to_object.setValue(None, None)
        self._to_offset.setValue(0)
        self._from_object.setValue(None, None)
        self._from_offset.setValue(0)
        pass

    def __createBySize(self, to_size, from_size):
        self.__clear()
        self._CreationMethod.setValue("BySizes")
        self._to_size.setValue(to_size)
        self._from_size.setValue(from_size)
        pass

    def __createByPlanesAndOffsets(self, to_object, to_offset,
                                   from_object, from_offset):
        self.__clear()
        self._CreationMethod.setValue("ByPlanesAndOffsets")
        self._to_object.setValue(to_object.result(), to_object.buildShape())
        self._to_offset.setValue(to_offset)
        self._from_object.setValue(from_object.result(), from_object.buildShape())
        self._from_offset.setValue(from_offset)
        pass

    def __execute(self):
        if self.areInputValid():
          self.execute()
        else:
          raise Exception("cannot execute Extrusion")

    def setBase(self, base):
        """Modify base attribute of the feature.

        See __init__.
        """
        self.__setBase(base)
        self.__execute()
        pass

    def setSize(self, *args):
        """Modify the to_size, from_size attributes of the feature.

        See __init__.
        """
        self.__createBySize(*args)
        self.__execute()
        pass

    def setPlanesAndOffsets(self, *args):
        """Modify planes and offsets attributes of the feature.

        See __init__.
        """
        self.__createByPlanesAndOffsets(*args)
        self.__execute()
        pass
