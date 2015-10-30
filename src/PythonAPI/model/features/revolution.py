"""Revolution Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface


def addRevolution(part, *args):
    """Add an Revolution feature to the Part and return Revolution.

    Pass all args to Revolution __init__ function.
    """
    assert(len(args) > 0 and args[0] is not None)
    feature = part.addFeature("Revolution")
    return Revolution(feature, *args)


class Revolution(Interface):
    """Interface on an Revolution feature."""

    def __init__(self, feature, *args):
        """Initialize an Revolution feature with given parameters.

        Expected arguments for all modes:
        feature -- an Revolution feature

        Expected arguments for initializing the feature:
        base -- name, sketch or list of names and sketches.
        If base is None then don't change the feature.
        axis_object -- name, edge for axis

        For ByAngles mode (expect 2 arguments):
        to_angle -- upper angle
        from_angle -- lower angle

        For ByPlanesAndOffsets mode (expect 4 arguments):
        to_object -- upper object (plane)
        to_offset -- offset from upper object
        from_object -- lower object (plane)
        from_offset -- offset from lower object
        """
        Interface.__init__(self, feature)
        assert(self._feature.getKind() == "Revolution")

        self._base = self._feature.data().selectionList("base")
        self._axis_object = self._feature.data().selection("axis_object")
        self._CreationMethod = self._feature.string("CreationMethod")
        self._to_angle = self._feature.data().real("to_angle")
        self._from_angle = self._feature.data().real("from_angle")
        self._to_object = self._feature.data().selection("to_object")
        self._to_offset = self._feature.data().real("to_offset")
        self._from_object = self._feature.data().selection("from_object")
        self._from_offset = self._feature.data().real("from_offset")

        assert(self._base)
        assert(self._axis_object)
        assert(self._CreationMethod)
        assert(self._to_angle)
        assert(self._from_angle)
        assert(self._to_object)
        assert(self._to_offset)
        assert(self._from_object)
        assert(self._from_offset)

        if not args:
            return

        assert(len(args) in (4, 6))

        base, axis_object = args[:2]
        args = args[2:]

        self.setBase(base)
        self.setAxisObject(axis_object)

        if len(args) == 4:
            self.setPlanesAndOffsets(*args)
        elif len(args) == 2:
            self.setAngles(*args)

        self._execute()
        pass

    def __clear(self):
        self._CreationMethod.setValue("ByAngles")
        self._fill_attribute(self._to_angle, 0)
        self._fill_attribute(self._from_angle, 0)
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

    def setAxisObject(self, axis_object):
        """Modify axis_object attribute of the feature.

        See __init__.
        """
        self._fill_attribute(self._axis_object, axis_object)
        pass

    def setAngles(self, to_angle, from_angle):
        """Modify the to_angle, from_angle attributes of the feature.

        See __init__.
        """
        self.__clear()
        self._CreationMethod.setValue("ByAngles")
        self._fill_attribute(self._to_angle, to_angle)
        self._fill_attribute(self._from_angle, from_angle)
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
