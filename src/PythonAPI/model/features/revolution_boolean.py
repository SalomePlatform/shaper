"""RevolutionCut and RevolutionFuse Interface
Author: Sergey Pokhodenko
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

from .roots import CompositeBoolean


def addRevolutionCut(part, *args):
    """Add an RevolutionCut feature to the Part and return RevolutionBoolean.

    Pass all args to RevolutionCut __init__ function.
    """
    assert(args)
    feature = part.addFeature("RevolutionCut")
    return RevolutionBoolean(feature, *args)

def addRevolutionFuse(part, *args):
    """Add an RevolutionFuse feature to the Part and return RevolutionBoolean.

    Pass all args to RevolutionFuse __init__ function.
    """
    assert(args)
    feature = part.addFeature("RevolutionFuse")
    return RevolutionBoolean(feature, *args)


class RevolutionBoolean(CompositeBoolean):

    def __init__(self, feature, *args):
        CompositeBoolean.__init__(self, feature, *args)

        self._axis_object = self._feature.data().selection("axis_object")
        self._CreationMethod = self._feature.string("CreationMethod")
        self._to_angle = self._feature.data().real("to_angle")
        self._from_angle = self._feature.data().real("from_angle")
        self._to_object = self._feature.data().selection("to_object")
        self._to_offset = self._feature.data().real("to_offset")
        self._from_object = self._feature.data().selection("from_object")
        self._from_offset = self._feature.data().real("from_offset")

        if not args:
            return

        self.setAxisObject(args[0])

        if len(args) == 4:
            self.setPlanesAndOffsets(*args[1:])
        elif len(args) == 2:
            self.setAngles(*args[1:])
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

