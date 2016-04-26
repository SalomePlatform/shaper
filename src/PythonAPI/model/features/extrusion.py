"""Extrusion Interface
Author: Daniel Brunier-Coulin with contribution by Mikhail Ponikarov
        and Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from model.roots import Interface
from model import Selection

def addExtrusion(part, *args):
    """Add an Extrusion feature to the Part and return Extrusion.

    This function has *3 signatures*:

    .. function:: addExtrusion(base, size)

    Arguments:
        base(str, Sketch or list): base object(s)
        size(double): size of the extrusion, the side is decided by the sign

    .. function:: addExtrusion(base, to_size, from_size)

    Arguments:
        base(str, Sketch or list): base object(s)
        to_size(double): upper size of the extrusion
        from_size(double): lower size of the extrusion

    .. function:: addExtrusion(base, to_object, to_offset, from_object, from_offset)

    Arguments:
        base(str, Sketch or list): base object(s)
        to_object(plane): upper plane
        to_offset(double): offset from upper object
        from_object(plane): lower plane
        from_offset(double): offset from lower plane

    In all three cases the function returns an extrusion object

    Returns:
        Extrusion: extrusion object
    """
    assert(args)
    feature = part.addFeature("Extrusion")
    return Extrusion(feature, *args)


class Extrusion(Interface):
    """Interface class for Extrusion feature.
    """

    def __init__(self, feature, *args):
        """
        Extrusion(feature) -> feature interface without initialization
        Extrusion(feature, base, size) ->
            feature interface initialized from arguments:
            - base -- name, sketch or list of names and sketches
            - size -- if positive -> to_size, if negative -> from_size
        Extrusion(feature, base, to_size, from_size) ->
            feature interface initialized from arguments:
            - base -- name, sketch or list of names and sketches
            - to_size -- upper size
            - from_size -- lower size
        Extrusion(feature, base, to_object, to_offset, from_object, from_offset) ->
            feature interface initialized from arguments:
            - base -- name, sketch or list of names and sketches
            - to_object -- upper object (plane)
            - to_offset -- offset from upper object
            - from_object -- lower object (plane)
            - from_offset -- offset from lower object
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

    def setBase(self, base):
        """Modify base attribute of the feature.

        See __init__.
        """
        self._fillAttribute(self._base, base)
        pass

    def setSizes(self, to_size, from_size, to_size_text="", from_size_text=""):
        """Modify the to_size, from_size attributes of the feature.

        See __init__.
        """
        # MPV: with "clear" calling here the extrusion all the time becomes modificed (height is set to 
        # zero and then to actual value, but this function is used in macro Bax, that causes "modified"
        # values without changes that causes cyclic dependency
        #self.__clear()
        self._fillAttribute(self._CreationMethod, "BySizes")
        if to_size_text == "":
            self._fillAttribute(self._to_size, to_size)
        else:
            self._fillAttribute(self._to_size, to_size_text)

        if from_size_text == "":
            self._fillAttribute(self._from_size, from_size)
        else:
            self._fillAttribute(self._to_size, from_size_text)
        pass

    def setSize(self, size, text=""):
        """Modify the size of the feature.

        If size is positive then initialize to_size with size.
        If size is negative then initialize from_size with -size.
        """
        to_size, from_size = 0, 0
        to_size_text, from_size_text = "", ""
        if size >= 0:
            to_size = size
            to_size_text = text
        else:
            from_size = -size
            from_size_text = text

        self.setSizes(to_size, from_size, to_size_text, from_size_text)

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

    def result(self):
        """F.result() -> list of Selection objects"""
        return [Selection(result, result.shape()) for result in (self.firstResult(),)]
